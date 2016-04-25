#include "Dll.h"

class Plugin : public IPlugin
{
public:
    void PrintString() override
    {
        std::cout << "DLL 1 respond" << std::endl;
    }
    ~Plugin() { }
};

extern "C"
{
    DLL_EXPORT IPlugin* Init()
    {
        return new Plugin();
    }

    DLL_EXPORT void Release(IPlugin* obj)
    {
        delete obj;
    }
}
