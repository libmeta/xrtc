#include "run_task.hpp"

namespace xrtc {

RunTask::RunTask(int64_t run_max_count)
    : run_max_count_(run_max_count)
{
}

RunTask::RunTask(const std::chrono::nanoseconds& run_max_time, int64_t run_max_count)
    : run_max_time_(run_max_time)
    , run_max_count_(run_max_count)

{
}

RunTask::~RunTask()
{
    reset();
}

bool RunTask::doByTimeAndCount(const RunFuncType& yet)
{
    if (checkCountDone() && checkTimeDone()) {
        return false;
    }

    selfIncForCount();
    selfIncForTime();

    if (!(checkCountDone() && checkTimeDone())) {
        return false;
    }

    yet();
    return true;
}

bool RunTask::doByTimeOrCount(const RunFuncType& yet)
{
    if (checkCountDone() || checkTimeDone()) {
        return false;
    }

    selfIncForCount();
    selfIncForTime();

    if (!(checkCountDone() || checkTimeDone())) {
        return false;
    }

    yet();
    return true;
}

void RunTask::reset()
{
    run_count_ = std::nullopt;
    run_time_ = std::nullopt;
}

bool RunTask::checkCountDone() const
{
    return !run_max_count_.has_value() || run_count_.value_or(0) >= run_max_count_.value();
}

bool RunTask::checkTimeDone() const
{
    return !run_max_time_.has_value() || run_time_.value_or(std::chrono::nanoseconds::zero()) >= run_max_time_.value();
}

void RunTask::selfIncForCount()
{
    if (!run_max_count_.has_value()) {
        return;
    }

    run_count_ = run_count_.value_or(0) + 1;
}

void RunTask::selfIncForTime()
{
    if (!run_max_time_.has_value()) {
        return;
    }

    if (!start_run_time_point_.has_value()) {
        start_run_time_point_ = std::chrono::steady_clock::now();
    }

    run_time_ = std::chrono::steady_clock::now() - start_run_time_point_.value();
}

}
