#include "srt_net.hpp"

SrtNet::SrtNet()
{
}

SrtNet::~SrtNet()
{
    stop();
}

bool SrtNet::startOrUpdate(const std::string& ip, uint16_t port)
{
    stop();

    srt_startup();
    srtNetSocket = srt_create_socket();
    if (srtNetSocket < 0) {
        return false;
    }

    setSockOptionsPre();

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);
    if (inet_pton(AF_INET, ip.c_str(), &sa.sin_addr) != 1) {
        return false;
    }

    epollid = srt_epoll_create();
    if (epollid.value() == -1) {
        return false;
    }

    // When a caller is connected, a write-readiness event is triggered.
    const int modes = SRT_EPOLL_OUT | SRT_EPOLL_ERR;
    if (SRT_ERROR == srt_epoll_add_usock(epollid.value(), srtNetSocket.value(), &modes)) {
        return false;
    }

    const auto srt_connect_st = srt_connect(srtNetSocket.value(), (struct sockaddr*)&sa, sizeof sa);
    if (srt_connect_st == SRT_ERROR) {
        return false;
    }

    // We had subscribed for write-readiness or error.
    // Write readiness comes in wready array,
    // error is notified via rready in this case.
    int rlen = 1;
    SRTSOCKET rready;
    int wlen = 1;
    SRTSOCKET wready;
    if (srt_epoll_wait(epollid.value(), &rready, &rlen, &wready, &wlen, -1, 0, 0, 0, 0) == -1) {
        return false;
    }

    const SRT_SOCKSTATUS state = srt_getsockstate(srtNetSocket.value());
    if (state != SRTS_CONNECTED || rlen > 0) // rlen > 0 - an error notification
    {
        //            fprintf(stderr, "srt_epoll_wait: reject reason %s\n", srt_rejectreason_str(srt_getrejectreason(rready)));
        return false;
    }

    if (wlen != 1 || wready != srtNetSocket.value()) {
        return false;
    }

    return true;
}

void SrtNet::stop()
{
    if (srtNetSocket.has_value()) {
        const auto srt_close_st = srt_close(srtNetSocket.value());
        srtNetSocket = std::nullopt;
        if (srt_close_st == SRT_ERROR) {
            return;
        }

        srt_cleanup();
    }
}

std::optional<bool> SrtNet::sendData(const uint8_t* data, int len, SRT_MSGCTRL* msg_ctrl)
{
    SRTSOCKET rready = SRT_INVALID_SOCK;
    int rlen = 1;
    SRTSOCKET wready = SRT_INVALID_SOCK;
    int wlen = 1;

    const int timeout_ms = 0; // ms
    const auto srt_epoll_wait_res = srt_epoll_wait(epollid.value(), &rready, &rlen, &wready, &wlen, timeout_ms, 0, 0, 0, 0);
    if (srt_epoll_wait_res < 0) {
        if (srt_getlasterror(nullptr) == SRT_ETIMEOUT) {
            return std::nullopt;
        }

        return false;
    }

    const auto srt_sendmsg2_st = srt_sendmsg2(srtNetSocket.value(), (const char*)(data), len, msg_ctrl);
    if (srt_sendmsg2_st < 0) {
        return false;
    }

    return true;
}

int SrtNet::getLastError() const
{
    return srt_getlasterror(nullptr);
}

bool SrtNet::setSockOption(SRT_SOCKOPT optname, const void* optval, int optlen)
{
    if (srt_setsockopt(srtNetSocket.value(), 0, optname, optval, optlen) < 0) {
        //        fprintf(stderr, "failed to set option %s on socket: %s\n", optnamestr, srt_getlasterror_str());
        return false;
    }

    return true;
}

void SrtNet::setSockOptionsPre()
{
    const bool no = false;
    setSockOption(SRTO_SNDSYN, &no, sizeof(no));
    setSockOption(SRTO_RCVSYN, &no, sizeof(no));

    const int tlpktdrop = 0;
    setSockOption(SRTO_TLPKTDROP, &tlpktdrop, sizeof(tlpktdrop));

    const int64_t maxbw = -1;
    setSockOption(SRTO_MAXBW, &maxbw, sizeof(maxbw));

    const int payload_size = 188 * 7;
    setSockOption(SRTO_PAYLOADSIZE, &payload_size, sizeof(payload_size));
}
