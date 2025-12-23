#pragma once

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <memory>
#include <mutex>
#include <ostream>
#include <sstream>
#include <string>
#include <condition_variable>
#include <thread>
#include <utility>
#include "lfree.h"

// ANSI 颜色代码
#define RESET "\033[0m"
#define GREEN "\033[32m"
#define CYAN "\033[36m"
#define YELLOW "\033[33m"
#define RED "\033[31m"
#define MAGENTA "\033[35m"
namespace ltx_log{
class __Logger__ {
public:
    
    enum Level { INFO = 0, DEBUG, WARNING, ERROR, FATAL };

    // 设置最低打印等级
    static inline Level log_level = INFO;
    // 决定了日志输出的目标，""默认是cout， "out.txt"表示输出到文件，
    // #define default_out_stream ""
    static inline std::string default_out_stream = "";
    static void set_out_put(const std::string& file){
        default_out_stream = file;
    }
    static void set_out_level(Level level){
        log_level = level;
    }

    class OutStream{
    public:
        using ptr = std::shared_ptr<OutStream>;
        static OutStream::ptr get(){
            static std::mutex mtx;
            static OutStream::ptr stance;
            if (stance.get() == nullptr) {
                std::unique_lock<std::mutex> lock(mtx);
                if (stance.get() == nullptr) {
                    stance = std::make_shared<OutStream>();
                }
            }
            return stance;
        }

        std::ostream& outfile() {
            if (out.is_open()) return out;
            return std::cout;
        }
        
        OutStream() {
            std::string file = default_out_stream;
            if (file != "") {
                out.open(file);
                if (!out.is_open()) {
                    std::cout << "failed to open file of log. please check file name of log";
                    exit(1);
                }
            }

            log_thread = new std::thread([this](){
                while (1) {
                    if (quit && task.readable() == false) {
                        break;
                    }
                    std::string task_str;
                    task.get(task_str);
                    outfile() << task_str;
                }
            });
        }
        ~OutStream() {
            quit = true;
            task.quit();
            log_thread->join();
        }

        void push(const std::string& mas) {
            task.put(mas);
        }
        
    private:
        bool quit = false;
        std::ofstream out;
        std::thread* log_thread = nullptr;
        lfree::ring_queue<std::string> task { lfree::queue_size::K05 };
    };

    class LogStream {
        friend class __Logger__;

    private:
        std::stringstream ss;
        const char       *file;
        int               line;
        Level             lvl;

        std::string getColor() const {
            switch (lvl) {
            case INFO:
                return GREEN;
            case DEBUG:
                return CYAN;
            case WARNING:
                return YELLOW;
            case ERROR:
                return RED;
            case FATAL:
                return MAGENTA;
            }
            return RESET;
        }
        void format(std::stringstream& s) {
            char c;
            while(s.get(c)){
                ss << c;
            }
            return ;
        }
        template<class K, class... Args>
        void format(std::stringstream& s,K&& arg, Args&&... args) {
            char c = 0;
            s.get(c);
            while(c != '{'){
                ss << c;
                s.get(c);
            }
            while(c != '}'){
                s.get(c);
            }
            ss << std::forward<K>(arg);
            format(s,std::forward<Args>(args)...);
            return ;
        }

    public:
        LogStream(const char *file, int line, Level level)
            : file(file), line(line), lvl(level) {
        }
        LogStream(const LogStream &log)
            : file(log.file), line(log.line), lvl(log.lvl) {
        }

        template <typename T> LogStream &operator<<(const T &value) {
            ss << value;
            return *this;
        }
        LogStream &operator<<(std::ostream& (*manip)(std::ostream&)) {
            manip(ss);
            return *this;
        }
        
        template<class... Args>
        LogStream & operator()(const std::string& fmt,Args&&... args) {
            std::stringstream s(fmt);
            format(s,std::forward<Args>(args)...);
            return *this;
        }


        ~LogStream() {
            if (lvl >= log_level) {
                std::stringstream out;
                std::time_t now = std::chrono::system_clock::to_time_t(
                    std::chrono::system_clock::now());
                out << getColor() << "["
                          << std::put_time(std::localtime(&now),
                                           "%Y-%m-%d %H:%M:%S")
                          << "] [";

                switch (lvl) {
                case INFO:
                    out << "INFO";
                    break;
                case DEBUG:
                    out << "DEBUG";
                    break;
                case WARNING:
                    out << "WARNING";
                    break;
                case ERROR:
                    out << "ERROR";
                    break;
                case FATAL:
                    out << "FATAL";
                    break;
                }

                out << "] [" << file << ":" << line << "] " << ss.str()
                          << RESET << std::endl;
                OutStream::get()->push(out.str());
            }
        }
    };

    static LogStream info(const char *file, int line) {
        return LogStream(file, line, INFO);
    }
    static LogStream debug(const char *file, int line) {
        return LogStream(file, line, DEBUG);
    }
    static LogStream warning(const char *file, int line) {
        return LogStream(file, line, WARNING);
    }
    static LogStream error(const char *file, int line) {
        return LogStream(file, line, ERROR);
    }
    static LogStream fatal(const char *file, int line) {
        return LogStream(file, line, FATAL);
    }
}; // class __Logger__
} // namespace ltx_log

// 宏定义，用于简化使用
#define infolog ltx_log::__Logger__::info(__FILE__, __LINE__)
#define debuglog ltx_log::__Logger__::debug(__FILE__, __LINE__)
#define warninglog ltx_log::__Logger__::warning(__FILE__, __LINE__)
#define errorlog ltx_log::__Logger__::error(__FILE__, __LINE__)
#define fatallog ltx_log::__Logger__::fatal(__FILE__, __LINE__)
