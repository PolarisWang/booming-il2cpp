#include "host_main.h"

#include <vector>

int main(int argc, char** argv)
{
    std::vector<const char*> forwarded_arguments;
    forwarded_arguments.reserve(static_cast<size_t>((argc > 0 ? argc : 0) + 1));
    forwarded_arguments.push_back("MobileHelloWorldProof");
    for (int index = 1; index < argc; ++index)
    {
        if (argv != nullptr && argv[index] != nullptr)
        {
            forwarded_arguments.push_back(argv[index]);
        }
    }

    argc = static_cast<int>(forwarded_arguments.size());
    const char** forwarded_argv = forwarded_arguments.data();
    return il2cpp_host_run(argc, forwarded_argv);
}
