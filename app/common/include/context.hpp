#ifndef _CONTEXT_HPP
#define _CONTEXT_HPP

#include "switch/result.h"
#include "sysrgb.h"
#include "sysrgb/client/ipc.h"
#include "memstream.hpp"
#include <iomanip>
#include <ios>
#include <sys/_stdint.h>

#if defined(ATMOSPHERE_RELEASE_VERSION)
using amsResult = ams::Result;
#else
using amsResult = Result;
#endif

class Context
{
public:
    Context(Context const&) = delete;

    Context& operator=(Context const&) = delete;

    ~Context() = default;

    static SysRgbContext& get()
    {
        return get_instance()._context;
    }

    static void set(SysRgbContext& ctx)
    {
        get_instance()._context = ctx;
    }

    static bool r_ok(const char* id, amsResult rc)
    {
        if (R_FAILED(rc))
        {
            loge(id, rc);
            return false;
        }

        return true;
    }

    static bool r_err(const char* id, amsResult rc)
    {
        return !r_ok(id, rc);
    }

    static void get_mark()
    {
#if MAX_LOG > 0
        get_instance()._context.put = stream().put_pos();
        get_instance()._context.get = stream().get_pos();
#endif
    }

    static void set_mark()
    {
#if MAX_LOG > 0
        stream().set_pos(get_instance()._context.put, get_instance()._context.get);
#endif
    }

    static void logs(const char* s)
    {
#if MAX_LOG > 0
        stream() << s << std::endl;
#endif
    }

    static void loge(const char* id, amsResult rc)
    {
#if MAX_LOG > 0
    #if defined(ATMOSPHERE_RELEASE_VERSION)
        stream() << "ERR:"  << id << ":M:" << rc.GetModule() << ":D:" << rc.GetDescription() << std::endl;
    #else
        stream() << "ERR:" << id << ":" << std::setw(8) << std::setfill('0') << std::hex << rc << std::endl;
    #endif
#endif
    }

    static void logd(const char* header, const void* pData, size_t len)
    {
#if MAX_LOG > 0
        Context::stream() << header;
        for (size_t i=0; i<len; i++)
        {
            Context::stream() << " " << std::setw(2) << std::setfill('0') << std::hex << (int)(((u8*)pData)[i]);
        }
        Context::stream() << std::endl;
#endif
    }

#if MAX_LOG > 0
    static memstream& stream()
    {
        return get_instance()._stream;
    }
#endif

    static Context& get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static Context instance{};
        return instance;
    }

private:

    explicit Context()
    #if MAX_LOG > 0
    : _stream(_context.log, sizeof(_context.log))
    #endif
    {}

    SysRgbContext _context = {};
#if MAX_LOG > 0
    memstream _stream;
#endif
};

#endif // _CONTEXT_HPP