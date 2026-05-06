#include "result_reporter.h"

#include <chaos/common.h>

void report_host_result(const HostRunResult& result)
{
    const char* stage = result.stage != nullptr ? result.stage : "unknown-stage";
    const char* detail = result.detail != nullptr ? result.detail : "unknown-detail";
    CHAOS_IL2CPP_LOG_WRITE_RAW_M("mobile-host|stage={0}|detail={1}|exitCode={2}\n",
                                 stage, detail, result.exitCode);
    CHAOS_IL2CPP_LOG_FLUSH_STDOUT();
}
