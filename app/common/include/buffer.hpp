#ifndef _BUFFER_HPP
#define _BUFFER_HPP

#include <stdint.h>
#include <mutex>

class Buffer
{
public:
    Buffer(Buffer const&) = delete;

    Buffer& operator=(Buffer const&) = delete;

    virtual ~Buffer()
    {}

    static uint8_t* reserve()
    {
        get_instance()._mutex.lock();
        return get_instance()._buffer;
    }

    static void release()
    {
        get_instance()._mutex.unlock();
    }

    static size_t size()
    {
        return sizeof(get_instance()._buffer);
    }

    static Buffer& get_instance()
    {
        // Static local variable initialization is thread-safe
        // and will be initialized only once.
        static Buffer instance{};
        return instance;
    }

private:
    Buffer() : _mutex()
    {

    }

    alignas(4*1024) uint8_t _buffer[2048] = {};
    std::mutex _mutex;
};

#endif // _BUFFER_HPP