#ifndef _CONFIG_HPP
#define _CONFIG_HPP

#include "cfg_item.h"
#include "json.hpp"
#include "memstream.hpp"

#define CFG_BUF_MAX 1024

class Config
{
public:
    Config() : _config(), _stream(_buf, sizeof(_buf)) {}

    Config(Config const&) = delete;

    Config& operator=(Config const&) = delete;

    virtual ~Config()
    {}

    CfgSet& get()
    {
        return _config;
    }

    std::string toString(const int indent = -1)
    {
        nlohmann::json json = _config;
        return json.dump(indent);
    }

//    std::string toString(const int indent = -1)
//    {
//        toStream(_stream, indent);
//        return _stream.str();
//    }

    void toStream(memstream& stream, const int indent = -1)
    {
        stream.clear();
        stream.set_pos(0, 0);
        nlohmann::json json = _config;
        if (indent > 0)
        {
            stream << std::setw(indent) << json << std::endl;
        }
        else
        {
            stream << json << std::endl;
        }
    }

    static nlohmann::json fromText(const char* configJson, size_t len)
    {
        try
        {
            return nlohmann::json::parse(configJson, configJson+len);
        }
        catch (nlohmann::json::exception& exception)
        {
        }

        return {};
    }

    static CfgSet parse(const char* configJson, size_t len)
    {
        try
        {
            return nlohmann::json::parse(configJson, configJson+len).template get<CfgSet>();
        }
        catch (nlohmann::json::exception& exception)
        {
        }

        return {};
    }

protected:

    CfgSet _config;
    memstream _stream;
    char _buf[CFG_BUF_MAX];
};

#endif // _CONFIG_HPP