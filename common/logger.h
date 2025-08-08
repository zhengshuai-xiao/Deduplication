#pragma once
#include <string>
#include <sstream>
#include <fstream>
#include <mutex>
#include <thread>
#include <condition_variable>
#include <queue>
#include <atomic>
#include <chrono>
#include <iomanip>
#include <iostream>

namespace common {

enum class LogLevel { TRACE, DBG, INFO, WARN, ERROR, FATAL };
const std::array<std::string, 6> LogLevelNames = {
    "TRACE", "DBG", "INFO", "WARN", "ERROR", "FATAL"
};

class Logger {
public:
    static Logger& Instance();

    void setLogFile(const std::string& filename, bool async = false);
    void setLevel(LogLevel level);
    void stop();

    // 支持 << 操作符
    class LogStream {
    public:
        LogStream(LogLevel level, std::thread::id tid, const char* func);
        ~LogStream();

        template<typename T>
        LogStream& operator<<(const T& val) {
            oss_ << val;
            return *this;
        }
    private:
        LogLevel level_;
        std::thread::id tid_;
        const char* func_;
        std::ostringstream oss_;
    };

    void log(LogLevel level, const std::string& msg, std::thread::id tid, const char* func);
    //TODO:
    /*setMaxLogSize()*/

private:
    Logger();
    ~Logger();

    void asyncThreadFunc();
    std::string format(LogLevel level, std::thread::id tid, const std::string& msg, const char* func);

    std::ofstream file_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::queue<std::string> queue_;
    std::thread thread_;
    std::atomic<bool> running_{false};
    bool async_{false};
    //TODO: put the default value to config file
    LogLevel min_level_{LogLevel::INFO};
};

// 宏定义，自动带上文件、行号、函数名
#define LOG_TRACE()   common::Logger::LogStream(common::LogLevel::TRACE, std::this_thread::get_id(), __FUNCTION__)
#define LOG_DEBUG()   common::Logger::LogStream(common::LogLevel::DBG, std::this_thread::get_id(), __FUNCTION__)
#define LOG_INFO()    common::Logger::LogStream(common::LogLevel::INFO, std::this_thread::get_id(),  __FUNCTION__)
#define LOG_WARN()    common::Logger::LogStream(common::LogLevel::WARN, std::this_thread::get_id(),  __FUNCTION__)
#define LOG_ERROR()   common::Logger::LogStream(common::LogLevel::ERROR, std::this_thread::get_id(), __FUNCTION__)
#define LOG_FATAL()   common::Logger::LogStream(common::LogLevel::FATAL, std::this_thread::get_id(), __FUNCTION__)

#define FUNC_TRACE() common::FuncLog logfunc(__FUNCTION__)
    class FuncLog{
    public:
        FuncLog(std::string str):str_(str){
            LOG_TRACE() << str_ <<": enter";
        }
        ~FuncLog(){
            LOG_TRACE() << str_ <<": exit";
        }
    private:
        std::string str_;
    };
} // namespace common