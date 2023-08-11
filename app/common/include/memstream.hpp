#ifndef _MEMSTREAM_HPP
#define _MEMSTREAM_HPP

#include <cassert>
#include <cstring>
#include <array>
#include <iostream>
#include <streambuf>

class memstream: virtual public std::streambuf, virtual public std::iostream
{
    using traits_type = typename std::basic_streambuf<char>::traits_type;
    using char_type = typename traits_type::char_type;
    using int_type = typename traits_type::int_type;

    char_type _tmp = '\0';
    char_type* _buf = nullptr;
    ::std::streamsize _len = 0;

protected:
    ::std::streamsize _put = 0;
    ::std::streamsize _get = 0;

public:
    memstream(char_type* buf, ::std::streamsize len)
    : std::streambuf(), std::iostream(this), _buf(buf), _len(len)
    {
        setbuf(&_tmp, 1);
    }

    ::std::streamsize inline available() const
    {
        return (_put >= _get ? _put - _get : _len - std::abs(_put - _get));
    }

    void set_pos(::std::streamsize put, ::std::streamsize get)
    {
        _put = put;
        _get = get;
    }

    ::std::streamsize put_pos()
    {
        return _put;
    }

    ::std::streamsize get_pos()
    {
        return _get;
    }

    int_type overflow(int_type c) override
    {
        setp(&_tmp, &_tmp);

        if (available() < _len)
        {
            _buf[_put] = (char_type)c;
            _put = (_put + 1) % _len;
            return traits_type::to_int_type(c);
        }
        return traits_type::eof();
    }

    int_type underflow() override
    {
        setg(&_tmp, &_tmp, &_tmp);

        int_type ret = traits_type::eof();
        if (available())
        {
            _tmp = _buf[_get];
            ret = traits_type::to_int_type(_tmp);
        }
        return ret;
    }

    int_type uflow() override
    {
        setg(&_tmp, &_tmp, &_tmp);

        int_type ret = traits_type::eof();
        if (available())
        {
            _tmp = _buf[_get];
            _get = (_get + 1) % _len;
            ret = traits_type::to_int_type(_tmp);
        }
        return ret;
    }

    ::std::streamsize showmanyc() override
    {
        return available();
    }

    ::std::streambuf* setbuf(char_type* const s, ::std::streamsize const n) override
    {
        setg(s, s, s);
        setp(s, s);

        return this;
    }

    ::std::streamsize xsgetn(char_type* const s, ::std::streamsize const size) override
    {
        ::std::streamsize count = size;
        char_type* dst = s;

        while (available() && count)
        {
            *dst++ = _buf[_get];
            _get = (_get + 1) % _len;
            count --;
        }
        return size-count;
    }

    ::std::streamsize xsputn(char_type const* s, ::std::streamsize const size) override
    {
        ::std::streamsize count = size;
        const char_type* src = s;
        while (available() < _len && count)
        {
            _buf[_put] = *src++;
            _put = (_put + 1) % _len;
            count--;
        }

        return size - count;
    }

    std::string str()
    {
        std::ostringstream result;
        while (available())
        {
            result << _buf[_get];
            _get = (_get + 1) % _len;
        }
        return result.str();
//        std::istreambuf_iterator<char> it(*this), end;
//        std::string result(it, end);
//        return result;
    }
};

#endif //_MEMSTREAM_HPP
