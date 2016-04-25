#pragma once
#include <iostream>
#include <string>
#include "PluginI.h"


#define DLL_EXPORT 


extern "C"
{
    DLL_EXPORT IPlugin* Init();
    DLL_EXPORT void Release(IPlugin*);
}
