//
// Created by hackerl on 10/14/18.
//

#ifndef SHADOWSOCKSR_CPP_IPIPE_H
#define SHADOWSOCKSR_CPP_IPIPE_H

#include <iostream>

class IPipe
{
public:
    virtual bool PipeIn(const char * Buffer, size_t Length) = 0;
    virtual bool PipeOut(const char * Buffer, size_t Length) = 0;
    virtual void PipeConnect(IPipe * Pipe) = 0;
    virtual void PipeClose() = 0;
};

class IDelayInstance
{
public:
    virtual bool InitDelay(void * arg) = 0;
};

#endif //SHADOWSOCKSR_CPP_IPIPE_H