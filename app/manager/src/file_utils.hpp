#pragma once

#include <borealis.hpp>
#include <cstddef>
#include <switch.h>
#include <time.h>
#include <vector>
#include <string>
#include <atomic>
#include <cstdarg>


#define FILE_CONFIG_DIR "/config/sys-rgb"
#define FILE_LOG_FLAG_PATH FILE_CONFIG_DIR "/log.flag"
#define FILE_LOG_FILE_PATH FILE_CONFIG_DIR "/mgrlog.txt"
#define FILE_FLAG_CHECK_INTERVAL_NS 5000000000ULL

class FileUtils
{
public:
    static FileUtils* getInstance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static FileUtils instance{};
        return &instance;
    }

    FileUtils(FileUtils const&) = delete;

    FileUtils& operator=(FileUtils const&) = delete;

    ~FileUtils();

    static bool IsInitialized();

    static bool IsLoggingEnabled();

    static bool IsLogEnabled();

    static void Exit();

    static void LogLine(const char* format, ...);

protected:
    void Initialize();
    void refreshFlags(bool force);

    void log(const char* format, std::va_list vl);
    void log(const char* text);

private:
    explicit FileUtils()
    {Initialize();}

    char _buf[0x200] = {};
    bool _log_enabled = false;
    std::atomic_bool _has_initialized = false;
    uint64_t _last_flag_check = 0;
    FsFileSystem _fsSdmc = {};
    brls::Event<std::string>::Subscription _logSub;
};
