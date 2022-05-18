#include "srt_net.hpp"
#include "logger/logger.hpp"

namespace xrtc {

SRTNet::SRTNet()
{
    xlogi("SRTNet constructed");
}

SRTNet::~SRTNet()
{
    stop();
    xlogi("SRTNet destruct");
}

void SRTNet::closeAllClientSockets()
{
    std::lock_guard<std::mutex> lock(client_list_mtx_);
    for (auto& client : client_list_) {
        SRTSOCKET socket = client.first;
        int result = srt_close(socket);
        if (clientDisconnected) {
            clientDisconnected(client.second, socket);
        }
        if (result == SRT_ERROR) {
            xloge("srt_close failed: {}", srt_getlasterror_str());
        }
    }

    client_list_.clear();
}

bool SRTNet::isIPv4(const std::string& str)
{
    struct sockaddr_in sa = { 0 };
    return inet_pton(AF_INET, str.c_str(), &sa.sin_addr) != 0;
}

bool SRTNet::isIPv6(const std::string& str)
{
    struct sockaddr_in6 sa = { 0 };
    return inet_pton(AF_INET6, str.c_str(), &sa.sin6_addr) != 0;
}

bool SRTNet::startServer(const std::string& ip, uint16_t port, int reorder, int32_t latency, int overhead, int mtu,
    const std::string& psk, std::shared_ptr<NetworkConnection> ctx)
{

    struct sockaddr_in saV4 = { 0 };
    struct sockaddr_in6 saV6 = { 0 };

    int ipType = AF_INET;
    if (isIPv4(ip)) {
        ipType = AF_INET;
    } else if (isIPv6(ip)) {
        ipType = AF_INET6;
    } else {
        xloge("Provided IP-Address not valid.");
    }

    std::lock_guard<std::mutex> lock(net_mtx_);

    if (current_mode_ != Mode::unknown) {
        xloge("SRTNet mode is already set");
        return false;
    }

    if (!clientConnected) {
        xlogt("waitForSRTClient needs clientConnected callback method terminating server!");
        return false;
    }

    connection_context_ = ctx; // retain the optional context

    context_ = srt_create_socket();
    if (context_ == SRT_ERROR) {
        xlogi("srt_socket: {}", srt_getlasterror_str());
        return false;
    }

    if (ipType == AF_INET) {
        saV4.sin_family = AF_INET;
        saV4.sin_port = htons(port);
        if (inet_pton(AF_INET, ip.c_str(), &saV4.sin_addr) != 1) {
            xloge("inet_pton failed");
            srt_close(context_);
            return false;
        }
    }

    if (ipType == AF_INET6) {
        saV6.sin6_family = AF_INET6;
        saV6.sin6_port = htons(port);
        if (inet_pton(AF_INET6, ip.c_str(), &saV6.sin6_addr) != 1) {
            xlogt("inet_pton failed ");
            srt_close(context_);
            return false;
        }
    }

    int32_t yes = 1;
    int result = srt_setsockflag(context_, SRTO_RCVSYN, &yes, sizeof(yes));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_RCVSYN: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_LATENCY, &latency, sizeof(latency));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_LATENCY: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_LOSSMAXTTL, &reorder, sizeof(reorder));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_LOSSMAXTTL: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_OHEADBW, &overhead, sizeof(overhead));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_OHEADBW: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_PAYLOADSIZE, &mtu, sizeof(mtu));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_PAYLOADSIZE: {}", srt_getlasterror_str());
        return false;
    }

    if (psk.length()) {
        int32_t aes128 = 16;
        result = srt_setsockflag(context_, SRTO_PBKEYLEN, &aes128, sizeof(aes128));
        if (result == SRT_ERROR) {
            xlogt("srt_setsockflag SRTO_PBKEYLEN: {}", srt_getlasterror_str());
            return false;
        }

        result = srt_setsockflag(context_, SRTO_PASSPHRASE, psk.c_str(), psk.length());
        if (result == SRT_ERROR) {
            xlogt("srt_setsockflag SRTO_PASSPHRASE: {}", srt_getlasterror_str());
            return false;
        }
    }

    if (ipType == AF_INET) {
        result = srt_bind(context_, reinterpret_cast<sockaddr*>(&saV4), sizeof(saV4));
        if (result == SRT_ERROR) {
            xlogt("srt_bind: {}", srt_getlasterror_str());
            srt_close(context_);
            return false;
        }
    }

    if (ipType == AF_INET6) {
        result = srt_bind(context_, reinterpret_cast<sockaddr*>(&saV6), sizeof(saV6));
        if (result == SRT_ERROR) {
            xlogt("srt_bind: {}", srt_getlasterror_str());
            srt_close(context_);
            return false;
        }
    }

    result = srt_listen(context_, 2);
    if (result == SRT_ERROR) {
        xlogt("srt_listen: {}", srt_getlasterror_str());
        srt_close(context_);
        return false;
    }
    server_active_ = true;
    current_mode_ = Mode::server;
    worker_thread_ = std::thread(&SRTNet::waitForSRTClient, this);
    return true;
}

void SRTNet::serverEventHandler()
{
    SRT_EPOLL_EVENT ready[MAX_WORKERS];
    while (server_active_) {
        int ret = srt_epoll_uwait(poll_id_, &ready[0], 5, 1000);
        if (ret == MAX_WORKERS + 1) {
            ret--;
        }

        if (ret > 0) {
            for (size_t i = 0; i < ret; i++) {
                uint8_t msg[2048];
                SRT_MSGCTRL thisMSGCTRL = srt_msgctrl_default;
                SRTSOCKET thisSocket = ready[i].fd;
                int result = srt_recvmsg2(thisSocket, reinterpret_cast<char*>(msg), sizeof(msg), &thisMSGCTRL);

                std::lock_guard<std::mutex> lock(client_list_mtx_);
                auto iterator = client_list_.find(thisSocket);
                if (result == SRT_ERROR) {
                    xloge("srt_recvmsg error: {}", srt_getlasterror_str());
                    if (iterator == client_list_.end()) {
                        continue; // This client has already been removed by closeAllClientSockets()
                    }

                    auto ctx = iterator->second;
                    client_list_.erase(iterator->first);
                    srt_epoll_remove_usock(poll_id_, thisSocket);
                    srt_close(thisSocket);
                    if (clientDisconnected) {
                        clientDisconnected(ctx, thisSocket);
                    }
                } else if (result > 0 && receivedData) {
                    auto pointer = std::make_unique<std::vector<uint8_t>>(msg, msg + result);
                    receivedData(pointer, thisMSGCTRL, iterator->second, thisSocket);
                } else if (result > 0 && receivedDataNoCopy) {
                    receivedDataNoCopy(msg, result, thisMSGCTRL, iterator->second, thisSocket);
                }
            }
        } else if (ret == -1) {
            xloge("epoll error: {}", srt_getlasterror_str());
        }
    }

    xlogt("serverEventHandler");

    srt_epoll_release(poll_id_);
}

void SRTNet::waitForSRTClient()
{
    int result = SRT_ERROR;
    poll_id_ = srt_epoll_create();
    srt_epoll_set(poll_id_, SRT_EPOLL_ENABLE_EMPTY);
    event_thread_ = std::thread(&SRTNet::serverEventHandler, this);

    closeAllClientSockets();

    while (server_active_) {
        struct sockaddr_storage theirAddr = { 0 };
        xlogt("SRT Server wait for client");
        int addrSize = sizeof(theirAddr);
        SRTSOCKET newSocketCandidate = srt_accept(context_, reinterpret_cast<sockaddr*>(&theirAddr), &addrSize);
        if (newSocketCandidate == -1) {
            continue;
        }

        xlogt("Client connected:{}", newSocketCandidate);
        auto ctx = clientConnected(*reinterpret_cast<sockaddr*>(&theirAddr), newSocketCandidate, connection_context_);

        if (ctx) {
            const int events = SRT_EPOLL_IN | SRT_EPOLL_ERR;
            std::lock_guard<std::mutex> lock(client_list_mtx_);
            client_list_[newSocketCandidate] = ctx;
            result = srt_epoll_add_usock(poll_id_, newSocketCandidate, &events);
            if (result == SRT_ERROR) {
                xlogt("srt_epoll_add_usock error:{}", srt_getlasterror_str());
            }
        } else {
            close(newSocketCandidate);
        }
    }
}

void SRTNet::getActiveClients(const std::function<void(std::map<SRTSOCKET, std::shared_ptr<NetworkConnection>>&)>& function)
{
    std::lock_guard<std::mutex> lock(client_list_mtx_);
    function(client_list_);
}

// Host can provide a IP or name meaning any IPv4 or IPv6 address or name type www.google.com
// There is no IP-Version preference if a name is given. the first IP-version found will be used
bool SRTNet::startClient(const std::string& host,
    uint16_t port,
    int reorder,
    int32_t latency,
    int overhead,
    std::shared_ptr<NetworkConnection>& ctx,
    int mtu,
    const std::string& psk)
{
    std::lock_guard<std::mutex> lock(net_mtx_);
    if (current_mode_ != Mode::unknown) {
        xloge("SRTNet mode is already set");
        return false;
    }

    client_context_ = ctx;

    int result = 0;
    int32_t yes = 1;
    xlogt("SRT client startup");

    context_ = srt_create_socket();
    if (context_ == SRT_ERROR) {
        xlogt("srt_socket: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_SENDER, &yes, sizeof(yes));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_SENDER:: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_LATENCY, &latency, sizeof(latency));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_LATENCY: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_LOSSMAXTTL, &reorder, sizeof(reorder));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_LOSSMAXTTL: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_OHEADBW, &overhead, sizeof(overhead));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_OHEADBW: {}", srt_getlasterror_str());
        return false;
    }

    result = srt_setsockflag(context_, SRTO_PAYLOADSIZE, &mtu, sizeof(mtu));
    if (result == SRT_ERROR) {
        xlogt("srt_setsockflag SRTO_PAYLOADSIZE: {}", srt_getlasterror_str());
        return false;
    }

    if (psk.length()) {
        int32_t aes128 = 16;
        result = srt_setsockflag(context_, SRTO_PBKEYLEN, &aes128, sizeof(aes128));
        if (result == SRT_ERROR) {
            xlogt("srt_setsockflag SRTO_PBKEYLEN: {}", srt_getlasterror_str());
            return false;
        }

        result = srt_setsockflag(context_, SRTO_PASSPHRASE, psk.c_str(), psk.length());
        if (result == SRT_ERROR) {
            xlogt("srt_setsockflag SRTO_PASSPHRASE: {}", srt_getlasterror_str());
            return false;
        }
    }

    // get all addresses for connection
    struct addrinfo hints = { 0 };
    struct addrinfo* svr;
    struct addrinfo* hld;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;
    hints.ai_family = AF_UNSPEC;
    result = getaddrinfo(host.c_str(), std::to_string(port).c_str(), &hints, &svr);
    if (result) {
        xlogt("Failed getting the IP target for {}: {} Errno:{}", host, port, result);
        return false;
    }

    xlogt("SRT connect");
    for (hld = svr; hld; hld = hld->ai_next) {
        result = srt_connect(context_, reinterpret_cast<sockaddr*>(hld->ai_addr), hld->ai_addrlen);
        if (result != SRT_ERROR) {
            xlogt("Connected to SRT Server");
            break;
        }
    }
    if (result == SRT_ERROR) {
        srt_close(context_);
        freeaddrinfo(svr);
        xlogt("srt_connect failed");
        return false;
    }
    freeaddrinfo(svr);
    current_mode_ = Mode::client;
    client_active_ = true;
    worker_thread_ = std::thread(&SRTNet::clientWorker, this);
    return true;
}

void SRTNet::clientWorker()
{
    while (client_active_) {
        uint8_t msg[2048];
        SRT_MSGCTRL thisMSGCTRL = srt_msgctrl_default;
        int result = srt_recvmsg2(context_, reinterpret_cast<char*>(msg), sizeof(msg), &thisMSGCTRL);
        if (result == SRT_ERROR) {
            if (client_active_) {
                xloge("srt_recvmsg error: {}", srt_getlasterror_str());
            }

            if (clientDisconnected) {
                clientDisconnected(client_context_, context_);
            }
            break;
        } else if (result > 0 && receivedData) {
            auto data = std::make_unique<std::vector<uint8_t>>(msg, msg + result);
            receivedData(data, thisMSGCTRL, client_context_, context_);
        } else if (result > 0 && receivedDataNoCopy) {
            receivedDataNoCopy(msg, result, thisMSGCTRL, client_context_, context_);
        }
    }
    client_active_ = false;
}

std::pair<SRTSOCKET, std::shared_ptr<SRTNet::NetworkConnection>> SRTNet::getConnectedServer()
{
    if (current_mode_ == Mode::client) {
        return { context_, client_context_ };
    }
    return { 0, nullptr };
}

bool SRTNet::sendData(const uint8_t* data, size_t len, SRT_MSGCTRL* msgCtrl, SRTSOCKET targetSystem)
{
    int result;

    if (current_mode_ == Mode::client && context_ && client_active_) {
        result = srt_sendmsg2(context_, reinterpret_cast<const char*>(data), len, msgCtrl);
    } else if (current_mode_ == Mode::server && targetSystem && server_active_) {
        result = srt_sendmsg2(targetSystem, reinterpret_cast<const char*>(data), len, msgCtrl);
    } else {
        xlogw("Can't send data, the client is not active.");
        return false;
    }

    if (result == SRT_ERROR) {
        xloge("srt_sendmsg2 failed:{}", srt_getlasterror_str());
        return false;
    }

    if (result != len) {
        xloge("Failed sending all data");
        return false;
    }

    return true;
}

bool SRTNet::stop()
{
    std::lock_guard<std::mutex> lock(net_mtx_);
    if (current_mode_ == Mode::server) {
        server_active_ = false;
        if (context_) {
            int result = srt_close(context_);
            if (result == SRT_ERROR) {
                xloge("srt_close failed: {}", srt_getlasterror_str());
                return false;
            }
        }
        closeAllClientSockets();
        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }
        if (event_thread_.joinable()) {
            event_thread_.join();
        }

        xlogt("Server stopped");
        current_mode_ = Mode::unknown;
        return true;
    } else if (current_mode_ == Mode::client) {
        client_active_ = false;
        if (context_) {
            int result = srt_close(context_);
            if (result == SRT_ERROR) {
                xloge("srt_close failed:{}", srt_getlasterror_str());
                return false;
            }
        }

        if (worker_thread_.joinable()) {
            worker_thread_.join();
        }

        xlogt("Client stopped");
        current_mode_ = Mode::unknown;
        return true;
    }
    return true;
}

bool SRTNet::getStatistics(SRT_TRACEBSTATS* currentStats, int clear, int instantaneous, SRTSOCKET targetSystem)
{
    std::lock_guard<std::mutex> lock(net_mtx_);
    if (current_mode_ == Mode::client && client_active_ &&context_) {
        int result = srt_bistats(context_, currentStats, clear, instantaneous);
        if (result == SRT_ERROR) {
            xloge("srt_bistats failed: {}", srt_getlasterror_str());
            return false;
        }
    } else if (current_mode_ == Mode::server && server_active_ && targetSystem) {
        int result = srt_bistats(targetSystem, currentStats, clear, instantaneous);
        if (result == SRT_ERROR) {
            xloge("srt_bistats failed: {}", srt_getlasterror_str());
            return false;
        }
    } else {
        xloge("Statistics not available");
        return false;
    }
    return true;
}

}
