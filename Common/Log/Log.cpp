#include "Log.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#define OUT

Concurrency::concurrent_queue<std::string> Log::_queue;

void Log::Run(const std::string& file_name)
{
    std::ofstream fout(file_name);

    std::string message;

    while (true)
    {
        if (_queue.try_pop(OUT message))
        {
            std::cout << message << std::endl;
            fout << message << std::endl;
        }
    }

    fout.close();
}

void Log::Start(const std::string& file_name)
{
    std::thread log_thread(&Log::Run, file_name);
    log_thread.detach();
}

void Log::Write(const std::string& message)
{
    const std::chrono::system_clock::time_point now = std::chrono::system_clock::now();

    const int64_t time = std::chrono::system_clock::to_time_t(now);

    struct tm timeInfo;
    localtime_s(&timeInfo, &time);

    const int now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() % 1000;

    std::stringstream ss;
    ss << std::this_thread::get_id();
    const uint64_t thread_id = std::stoull(ss.str());

    const std::string time_string = std::format("{}-{:02d}-{:02d} {:02d}:{:02d}:{:02d}.{:03d}",
        (timeInfo.tm_year % 100), (timeInfo.tm_mon + 1), timeInfo.tm_mday, timeInfo.tm_hour, timeInfo.tm_min, timeInfo.tm_sec, now_ms);

    const std::string final_message = std::format("[{}][{}] {}", time_string, thread_id, message);

    _queue.push(final_message);
}
