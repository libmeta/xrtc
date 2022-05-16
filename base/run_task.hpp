#pragma once

#include <chrono>
#include <functional>
#include <optional>

namespace xrtc {

class RunTask {
public:
    using RunFuncType = std::function<void(void)>;

    explicit RunTask(int64_t run_max_count = 1);

    explicit RunTask(const std::chrono::nanoseconds &run_max_time,int64_t run_max_count = 1);

    ~RunTask();

    bool doByTimeAndCount(const RunFuncType& yet);

    bool doByTimeOrCount(const RunFuncType& yet);

    void reset();

private:
    bool checkCountDone() const;

    bool checkTimeDone() const;

    void selfIncForCount();

    void selfIncForTime();

private:
    std::optional<std::chrono::nanoseconds> run_time_ = std::nullopt;
    std::optional<int64_t> run_count_ = std::nullopt;

    std::optional<std::chrono::nanoseconds> run_max_time_ = std::nullopt;
    std::optional<int64_t> run_max_count_ = std::nullopt;
    std::optional<std::chrono::steady_clock::time_point> start_run_time_point_ = std::nullopt;
};

}
