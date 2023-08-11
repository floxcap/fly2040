//
// Created by adam on 13/07/23.
//

#ifndef _CPU_MON_HPP
#define _CPU_MON_HPP

#include <switch.h>
#include <stratosphere.hpp>
#include <sys/_stdint.h>

#define NUM_CORES       4

class CpuMon;

struct CpuMonCtx
{
    volatile float _idle;
    volatile float _idleAvg;
    uint64_t _idleA;
    uint64_t _sysA;
    uint64_t _idleB;
    uint64_t _sysB;
    Thread _thread;
    uint8_t _id;
    CpuMon* _mon;
};

class CpuMon
{
    volatile bool _running = false;
    CpuMonCtx _ctx[NUM_CORES] = {};
    uint16_t _refreshRate = 200;

public:
    ams::Result start();
    void stop();
    float get(int id=-1);
    float avg(int id=-1);
    bool running();

    void thread(CpuMonCtx&);

protected:
    static void _thread(void*);
};


#endif //_CPU_MON_HPP
