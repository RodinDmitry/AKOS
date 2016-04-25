#include <iostream>
#include <stdio.h>
#include <string>
#include <vector>
#include <stdlib.h>
#include "PluginI.h"
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>

using init_dll = IPlugin*(*)(void);
using release_plugin = void(*)(IPlugin*);

typedef void* Dllhandle;


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


int main()
{
    auto plugins = ListOfPlugins();
    
    for (auto &name : plugins)
    {
        Dllhandle handle = dlopen(name.c_str(), RTLD_LAZY);
        IPlugin *plugin;
        
        auto initFunc = (init_dll)dlsym(handle, "Init");
        plugin = initFunc();
        
        plugin->PrintString();

        auto releaseFunc = (release_plugin)dlsym(handle, "Release");
        releaseFunc(plugin);
        dlclose(handle);
    }
    
    getchar();
    return 0;
}
