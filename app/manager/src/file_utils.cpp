#include <cstdio>
#include <sys/stat.h>
#include "file_utils.hpp"

void FileUtils::Initialize()
{
    //if (R_FAILED(fsOpenSdCardFileSystem(&_fsSdmc)))
    //    return;
    _has_initialized = true;
    refreshFlags(true);
}

bool FileUtils::IsInitialized()
{
    return getInstance()->_has_initialized;
}

bool FileUtils::IsLoggingEnabled()
{
    return getInstance()->_log_enabled;
}

void FileUtils::LogLine(const char* format, ...)
{
    std::va_list args;
    va_start(args, format);
    getInstance()->log(format, args);
    va_end(args);
}

void FileUtils::refreshFlags(bool force)
{
    if (!_has_initialized)
    {
        return;
    }

    uint64_t now = armTicksToNs(armGetSystemTick());
    if (!force && (now - _last_flag_check) < FILE_FLAG_CHECK_INTERVAL_NS)
    {
        return;
    }

    _last_flag_check = now;

    struct stat st;
    if (stat(FILE_LOG_FLAG_PATH, &st) != 0)
    {
        if (_log_enabled)
        {
            brls::Logger::getLogEvent()->unsubscribe(_logSub);
            _log_enabled = false;
        }
        return;
    }

#if 0
    FsTimeStampRaw timestamp;
    if (R_FAILED(fsFsGetFileTimeStampRaw(&_fsSdmc, FILE_LOG_FLAG_PATH, &timestamp)))
    {
        if (_log_enabled)
        {
            brls::Logger::getLogEvent()->unsubscribe(_logSub);
            _log_enabled = false;
        }
        return;
    }
#endif

    if (!_log_enabled)
    {
        _log_enabled = true;
        _logSub = brls::Logger::getLogEvent()->subscribe([&](std::string text)
                                                         {
                                                             log(text.c_str());
                                                         });
    }
}

void FileUtils::log(const char* text)
{
    if (!_has_initialized)
    {
        return;
    }

    refreshFlags(false);

    if (_log_enabled)
    {
        FILE* f = fopen(FILE_LOG_FILE_PATH, "a");
        if (nullptr == f)
        {
            return;
        }

        fseek(f, 0, SEEK_END);
        fputs(text, f);
        fputc('\n', f);
        fclose(f);
    }
}

void FileUtils::log(const char* format, std::va_list vl)
{
    if (!_has_initialized)
    {
        return;
    }

    refreshFlags(false);

    if(_log_enabled)
    {
        FILE* f = fopen(FILE_LOG_FILE_PATH, "a");
        if (nullptr == f)
        {
            return;
        }

        fseek(f, 0, SEEK_END);
        vfprintf(f, format, vl);
        fputc('\n', f);
        fclose(f);
#if 0
        //std::scoped_lock lk(_mutex);
        //hlp::ScopeGuard fsGuard([&] { fsFsClose(&fsSdmc); });

        FsFile fileConfig;
        if (R_FAILED(fsFsOpenFile(&fsSdmc, FILE_LOG_FILE_PATH, FsOpenMode_Write | FsOpenMode_Append, &fileConfig)))
            return;

        //hlp::ScopeGuard fileGuard([&] { fsFileClose(&fileConfig); });

        s64 size;
        if (R_FAILED(fsFileGetSize(&fileConfig, &size)))
            return;

        //fsFileWrite(&fileConfig, size, const void* buf, u64 write_size, FsWriteOption_Flush);
#endif
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
