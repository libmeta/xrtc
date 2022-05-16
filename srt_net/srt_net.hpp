#pragma once

#include <memory>
#include <optional>
#include <string>

#include <srt.h>

class SrtNet {
public:
    explicit SrtNet();

    ~SrtNet();

    bool startOrUpdate(const std::string& ip, uint16_t port);

    void stop();

    std::optional<bool> sendData(const uint8_t* data, int len, SRT_MSGCTRL* msg_ctrl = nullptr);

    int getLastError() const;

    bool setSockOption(SRT_SOCKOPT optname, const void* optval, int optlen);

private:
    void setSockOptionsPre();

private:
    std::optional<int> srtNetSocket = std::nullopt;
    std::optional<int> epollid = std::nullopt;
};
