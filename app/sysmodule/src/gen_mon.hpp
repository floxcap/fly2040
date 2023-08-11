//
// Created by adam on 13/07/23.
//

#ifndef _GEN_MON_HPP
#define _GEN_MON_HPP

#include <switch.h>
#include <stratosphere.hpp>

enum GenMonMode
{
    GenMonMode_GPU,
    GenMonMode_SOCTemperature,
    GenMonMode_PCBTemperature,
    GenMonMode_SKINTemperature
};

class GenMon
{
    volatile bool _running = false;
    GenMonMode _mode = GenMonMode_GPU;
    uint32_t _GPULoad = 0;
    int32_t _temperature = 0;
    uint16_t _refreshRate = 100;

    Thread _threadh;
    uint32_t _nvh;

    bool _hosGt14 = false;
    bool _nvCheck = false;
    bool _tsCheck = false;
    bool _tcCheck = false;

public:
    ams::Result start();
    void stop();
    bool running();

    void setMode(GenMonMode);
    float get();

    void thread();

protected:
    static void _thread(void*);
};

#endif //_GEN_MON_HPP
