//
// Created by patteliu on 2018/10/23.
//
#include "PortTunnel.h"
#include "Common/InstanceManager.h"
#include "Plugin/LibPluginExport.h"

extern "C" IPlugin * NewPortTunnel()
{
    return new InstanceManager<CPortTunnel>;
}
