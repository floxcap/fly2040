#ifndef _MEMLOG_HPP
#define _MEMLOG_HPP

#include <string>
#include "context.hpp"

class MemLog
{
public:
    MemLog(MemLog const&) = delete;

    MemLog& operator=(MemLog const&) = delete;

    ~MemLog()
    {}

    static std::string get()
    {
        std::ostringstream ss;
        /*
        Context& context = *Context::get_instance();
        ss << "FS: " << (context.get().fsEnabled?"true":"false") << "\n";
        ss << "LOG: " << (context.get().logEnabled?"true":"false") << "\n";
        ss << "cpumon: " << (context.get().cpumonRunning?"true":"false") << " val:" << context.get()._cpumonValue << "\n";
        ss << "genmon: " << (context.get().genmonRunning?"true":"false") << " val:" << context.get()._genmonValue << "\n";
        while(!context.eof())
        {
            std::string s;
            *Context::get_instance() >> s;
            ss << s << std::endl;
        }
        */
        ss << get_instance()->_log;
        return ss.str();
    }

    static void log(const char* text)
    {
        get_instance()->_log.append(text);
    }

    static void log(std::string text)
    {
        get_instance()->_log.append(text);
    }

private:
    static MemLog* get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static MemLog instance{};
        return &instance;
    }

    explicit MemLog() : _log()
    {}

    std::string _log;
};

#endif // _MEMLOG_HPP