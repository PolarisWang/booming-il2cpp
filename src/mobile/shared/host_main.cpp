#include "host_main.h"
#include "result_reporter.h"

namespace
{
const char* resolve_detail(int argc, const char** argv)
{
    if (argc > 0 && argv != nullptr && argv[0] != nullptr)
    {
        return argv[0];
    }

    return "mobile-host-skeleton";
}
}

extern "C" int il2cpp_host_run(int argc, const char** argv)
{
    HostRunResult result{
        0,
        "shared-host-bootstrap",
        resolve_detail(argc, argv),
    };

    report_host_result(result);
    return result.exitCode;
}
