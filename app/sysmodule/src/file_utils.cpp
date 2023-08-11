#include "file_utils.h"
#include "context.hpp"
#include "memstream.hpp"
#include <cstdio>
#include <iomanip>
#include <ios>

void FileUtils::Initialize()
{
    getInstance()->_has_initialized = true;
    getInstance()->refreshFlags(true);
}

bool FileUtils::IsInitialized()
{
    return getInstance()->_has_initialized;
}

bool FileUtils::IsLoggingEnabled()
{
    return getInstance()->_log_enabled;
}

void FileUtils::LogIfError(const char* header, uint32_t err)
{
    if (0 != err)
    {
        LogLine("%s %08X (%u)", header, err, err);
    }
}

void FileUtils::LogLine(const char* format, ...)
{
    if (!getInstance()->refreshFlags())
    {
        return;
    }

    std::va_list args;
    va_start(args, format);
    getInstance()->log(format, args);
    va_end(args);
}

bool FileUtils::refreshFlags(bool force)
{
    if (!_has_initialized)
    {
        return false;
    }

    uint64_t now = armTicksToNs(armGetSystemTick());
    if (!force && (now - _last_flag_check) < FILE_FLAG_CHECK_INTERVAL_NS)
    {
        return _log_enabled;
    }

    bool bFound = false;
    if (R_FAILED(ams::fs::HasDirectory(&bFound, FILE_CONFIG_DIR)) || !bFound)
    {
        Context::r_ok("log mkd", ams::fs::EnsureDirectory(FILE_CONFIG_DIR));
    }

    if (R_SUCCEEDED(ams::fs::HasFile(&bFound, FILE_LOG_FLAG_PATH)) && bFound)
    {
        if (!_log_enabled)
        {
            std::lock_guard lk(_mutex);
            _log_enabled = true;
            ams::fs::DeleteFile(FILE_LOG_FILE_PATH);
            ams::fs::CreateFile(FILE_LOG_FILE_PATH, 0);
        }
    }
    else
    {
        _log_enabled = false;
    }

    _last_flag_check = now;

    return _log_enabled;
}

void FileUtils::log(const char* format, std::va_list vl)
{
    if (!_has_initialized)
    {
        return;
    }

    if(_log_enabled)
    {
        std::lock_guard lk(_mutex);

        ams::fs::FileHandle logFile;
        if (Context::r_ok("log open", ams::fs::OpenFile(&logFile, FILE_LOG_FILE_PATH, ams::fs::OpenMode_All)))
        {
            ON_SCOPE_EXIT { ams::fs::CloseFile(logFile); };

            ams::TimeSpan ts = ams::os::ConvertToTimeSpan(ams::os::GetSystemTick());
            // Print time
            int len = ams::util::TSNPrintf(_buf, sizeof(_buf), "%02lid %02li:%02li:%02li: ",
                                           ts.GetDays(),
                                           ts.GetHours() % 24,
                                           ts.GetMinutes() % 60,
                                           ts.GetSeconds() % 60);
            // Print the actual text
            len += ams::util::TVSNPrintf(&_buf[len], sizeof(_buf) - len, format, vl);
            len += ams::util::TSNPrintf(&_buf[len], sizeof(_buf) - len, "\n");

            s64 logOffset;
            ams::fs::GetFileSize(&logOffset, logFile);

            ams::fs::WriteFile(logFile, logOffset, _buf, len, ams::fs::WriteOption::Flush);
        }
    }
}

void FileUtils::Exit()
{
    getInstance()->_log_enabled = false;
    getInstance()->_has_initialized = false;
}

FileUtils::~FileUtils()
{
    Exit();
}
