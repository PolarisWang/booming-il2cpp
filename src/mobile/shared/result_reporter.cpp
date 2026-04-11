#include "result_reporter.h"

#include <cstdio>

void report_host_result(const HostRunResult& result)
{
    const char* stage = result.stage != nullptr ? result.stage : "unknown-stage";
    const char* detail = result.detail != nullptr ? result.detail : "unknown-detail";
    std::fprintf(stdout, "mobile-host|stage=%s|detail=%s|exitCode=%d\n", stage, detail, result.exitCode);
    std::fflush(stdout);
}
