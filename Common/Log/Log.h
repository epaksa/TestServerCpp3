#pragma once

#include <concurrent_queue.h>
#include <string>
#include <format>

// todo
// 1. file size별
// 2. 시간별로 나뉘게 하는 기능
// 3. color
// 4. 한글 나오는건지 모르겠음

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