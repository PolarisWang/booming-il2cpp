#pragma once

struct HostRunResult
{
    int exitCode;
    const char* stage;
    const char* detail;
};

extern "C" int il2cpp_host_run(int argc, const char** argv);
