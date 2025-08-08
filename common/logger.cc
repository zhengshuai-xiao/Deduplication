#include "logger.h"
#include <algorithm>



namespace common {

std::unordered_map<std::string, LogLevel> LogLevelMap = {
    {"TRACE", LogLevel::TRACE},
    {"DBG", LogLevel::DBG},
    {"INFO", LogLevel::INFO},
    {"WARN", LogLevel::WARN},
    {"ERROR", LogLevel::ERROR},
    {"FATAL", LogLevel::FATAL}
};

Logger& Logger::Instance() {
    static Logger inst;
    return inst;
}

Logger::Logger() {}

Logger::~Logger() {
    stop();
}

void Logger::setLogFile(const std::string& filename, bool async) {
    std::lock_guard<std::mutex> lock(mtx_);
    if (file_.is_open()) file_.close();
    if(!filename.empty()){
        file_.open(filename, std::ofstream::out | std::ofstream::app);
    }
    async_ = async;
    if (async_ && !running_) {
        running_ = true;
        thread_ = std::thread(&Logger::asyncThreadFunc, this);
    }
}

void Logger::setLevel(std::string level_str){
    std::transform(level_str.begin(), level_str.end(), level_str.begin(), ::toupper);
    auto it = LogLevelMap.find(level_str);
    if (it != LogLevelMap.end()) {
        setLevel(it->second);
    } else {
        // Handle invalid log level
        setLevel(LogLevel::INFO);
        LOG_WARN() << "Invalid log level: " << level_str;
    }
}
void Logger::setLevel(LogLevel level) {
    min_level_ = level;
}

void Logger::stop() {
    if (async_) {
        running_ = false;
        cv_.notify_one();
        if (thread_.joinable()) thread_.join();
    }
    if (file_.is_open()) file_.close();
}

void Logger::log(LogLevel level, const std::string& msg, std::thread::id tid, const char* func) {
    if (level < min_level_) return;
    std::string formatted = format(level, tid, msg, func);
    if (async_) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            queue_.push(formatted);
        }
        cv_.notify_one();
    } else {
        std::lock_guard<std::mutex> lock(mtx_);
        if (file_.is_open())
            file_ << formatted << std::endl;
        else
            std::cout << formatted << std::endl;
    }
}

void Logger::asyncThreadFunc() {
    while (running_) {
        std::unique_lock<std::mutex> lock(mtx_);
        cv_.wait(lock, [this] { return !queue_.empty() || !running_; });
        while (!queue_.empty()) {
            if (file_.is_open())
                file_ << queue_.front() << std::endl;
            else
                std::cout << queue_.front() << std::endl;
            queue_.pop();
        }
        if (file_.is_open()) file_.flush();
    }
}

std::string Logger::format(LogLevel level, std::thread::id tid, const std::string& msg, const char* func) {
    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    std::tm tm;
    localtime_r(&t, &tm);

    std::ostringstream oss;
    oss << "[" << std::put_time(&tm, "%Y-%m-%d %H:%M:%S")
        << "." << std::setfill('0') << std::setw(3) << ms.count()
        << "][" << LogLevelNames[static_cast<int>(level)] << "][" << tid
        << "] " << func << ": "
         << msg;
    return oss.str();
}

// LogStream实现
Logger::LogStream::LogStream(LogLevel level, std::thread::id tid, const char* func)
    : level_(level), tid_(tid), func_(func) {}

Logger::LogStream::~LogStream() {
    Logger::Instance().log(level_, oss_.str(), tid_, func_);
}

} // namespace common