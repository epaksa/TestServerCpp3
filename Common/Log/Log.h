#pragma once

#include <concurrent_queue.h>
#include <string>
#include <format>

class Log
{
public:
    static void Start(const std::string& file_name);
    static void Write(const std::string& message);

private:
    static void Run(const std::string& file_name);

private:
    static Concurrency::concurrent_queue<std::string> _queue;
};