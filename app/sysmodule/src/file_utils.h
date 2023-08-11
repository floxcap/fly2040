/*
 * --------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <p-sam@d3vs.net>, <natinusala@gmail.com>, <m4x@m4xw.net>
 * wrote this file. As long as you retain this notice you can do whatever you
 * want with this stuff. If you meet any of us some day, and you think this
 * stuff is worth it, you can buy us a beer in return.  - The sys-clk authors
 * --------------------------------------------------------------------------
 */

#pragma once

#include <cstddef>
#include <stdint.h>
#include <switch.h>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <atomic>
#include <cstdarg>
//#include <sysrgb.h>
#include <queue>
#include <stratosphere.hpp>
//#include "init.hpp"

//#if FS_USE
#define FILE_CONFIG_DEV "sdmc:"
//#else
//#define FILE_CONFIG_DEV
//#endif


#define FILE_CONFIG_DIR FILE_CONFIG_DEV "/config/" TARGET
#define FILE_FLAG_CHECK_INTERVAL_NS 5000000000ULL
#define FILE_CONTEXT_CSV_PATH FILE_CONFIG_DIR "/context.csv"
#define FILE_LOG_FLAG_PATH FILE_CONFIG_DIR "/log.flag"
#define FILE_LOG_FILE_PATH FILE_CONFIG_DIR "/log.txt"

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

    static void Initialize();

    static bool IsInitialized();

    static bool IsLoggingEnabled();

    static bool IsLogEnabled();

    static void Exit();

    static void LogIfError(const char* header, uint32_t err);
    static void LogLine(const char* format, ...);

protected:

    bool refreshFlags(bool force = false);
    void log(const char* format, std::va_list vl);

private:
    explicit FileUtils() : _mutex(false)
    {}

    ams::os::Mutex _mutex;
    char _buf[0x100] = {};
    bool _log_enabled = false;
    std::atomic_bool _has_initialized = false;
    uint64_t _last_flag_check = 0;
};
