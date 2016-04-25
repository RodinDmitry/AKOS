#pragma once
#include <iostream>
#include <cstdio>
#include <string>
#include <vector>
#include <cstdlib>
#include <cstring>
#include "PluginI.h"
#include <dlfcn.h>
#include <dirent.h>

using init_dll = IPlugin*(*)(void);
using release_plugin = void(*)(IPlugin*);

typedef void* Dllhandle;
#define DllExt string(".so")
#define buildName(name) (string("./") + name + DllExt)

#define loadDll(name) dlopen(name, RTLD_LAZY)
#define getFunction(lib, name) dlsym(lib, name)
#define closeDll(lib) dlclose(lib)


std::vector<std::string> ListOfPlugins()
{
    std::vector<std::string> res;
    DIR *dir = opendir("Plugins");
    dirent *end;
    
    if (dir == NULL)
        return res;

    int len;
    while ((end = readdir(dir)) != NULL)
    {
        len = strlen(end->d_name);
        if (len > 2 && end->d_name[len - 3] == '.' 
		&& end->d_name[len - 2] == 's' 
		&& end->d_name[len - 1] == 'o')
        {
            res.push_back("Plugins/" + std::string(end->d_name));
        }
    }
    return res;
}
