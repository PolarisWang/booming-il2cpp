#include "host_main.h"
#include "result_reporter.h"

#include <chaos/native_types.h>
#include <fmt/format.h>

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>

namespace
{
constexpr const char* kDefaultDetail = "mobile-host-skeleton";
constexpr const char* kBootstrapStage = "shared-host-bootstrap";
constexpr const char* kHeartbeatStage = "shared-host-heartbeat";
constexpr const char* kSoakCompleteStage = "shared-host-soak-complete";
constexpr const char* kSoakDurationPrefix = "--soak-duration-seconds=";
constexpr const char* kHeartbeatIntervalPrefix = "--heartbeat-interval-seconds=";
constexpr const char* kSubjectIdPrefix = "--subject-id=";
constexpr const char* kSoakDurationEnvironmentVariable = "CHAOS_MOBILE_HOST_SOAK_DURATION_SECONDS";
constexpr const char* kHeartbeatIntervalEnvironmentVariable = "CHAOS_MOBILE_HOST_HEARTBEAT_INTERVAL_SECONDS";
constexpr const char* kSubjectIdEnvironmentVariable = "CHAOS_MOBILE_HOST_SUBJECT_ID";
constexpr double kDefaultHeartbeatIntervalSeconds = 30.0;

struct HostRunOptions
{
    const char* subjectId;
    double soakDurationSeconds;
    double heartbeatIntervalSeconds;
};

bool starts_with(const char* value, const char* prefix)
{
    if (value == nullptr || prefix == nullptr)
    {
        return false;
    }

    const size_t prefix_length = std::strlen(prefix);
    return std::strncmp(value, prefix, prefix_length) == 0;
}

double parse_positive_seconds(const char* value, double fallback)
{
    if (value == nullptr || *value == '\0')
    {
        return fallback;
    }

    char* end = nullptr;
    const double parsed = std::strtod(value, &end);
    if (end == value || end == nullptr || *end != '\0' || parsed <= 0.0)
    {
        return fallback;
    }

    return parsed;
}

const char* resolve_detail(int argc, const char** argv)
{
    if (argc > 0 && argv != nullptr && argv[0] != nullptr && argv[0][0] != '\0')
    {
        return argv[0];
    }

    return kDefaultDetail;
}

HostRunOptions parse_options(int argc, const char** argv)
{
    HostRunOptions options{
        resolve_detail(argc, argv),
        0.0,
        kDefaultHeartbeatIntervalSeconds,
    };
    const char* environment_subject_id = std::getenv(kSubjectIdEnvironmentVariable);
    if (environment_subject_id != nullptr && environment_subject_id[0] != '\0')
    {
        options.subjectId = environment_subject_id;
    }

    options.soakDurationSeconds = parse_positive_seconds(
        std::getenv(kSoakDurationEnvironmentVariable),
        options.soakDurationSeconds
    );
    options.heartbeatIntervalSeconds = parse_positive_seconds(
        std::getenv(kHeartbeatIntervalEnvironmentVariable),
        options.heartbeatIntervalSeconds
    );

    for (int index = 1; index < argc; ++index)
    {
        const char* argument = argv != nullptr ? argv[index] : nullptr;
        if (argument == nullptr)
        {
            continue;
        }

        if (starts_with(argument, kSoakDurationPrefix))
        {
            options.soakDurationSeconds = parse_positive_seconds(
                argument + std::strlen(kSoakDurationPrefix),
                options.soakDurationSeconds
            );
            continue;
        }

        if (starts_with(argument, kHeartbeatIntervalPrefix))
        {
            options.heartbeatIntervalSeconds = parse_positive_seconds(
                argument + std::strlen(kHeartbeatIntervalPrefix),
                options.heartbeatIntervalSeconds
            );
            continue;
        }

        if (starts_with(argument, kSubjectIdPrefix))
        {
            const char* explicit_subject_id = argument + std::strlen(kSubjectIdPrefix);
            if (explicit_subject_id[0] != '\0')
            {
                options.subjectId = explicit_subject_id;
            }
        }
    }

    return options;
}

double elapsed_seconds_since(const std::chrono::steady_clock::time_point& started_at)
{
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - started_at).count();
}

void report_success_stage(const char* stage, const std::string& detail)
{
    HostRunResult result{
        0,
        stage,
        detail.c_str(),
    };

    report_host_result(result);
}

std::string format_heartbeat_detail(const HostRunOptions& options, int heartbeat_index, double elapsed_seconds)
{
    return fmt::format("{}|heartbeat={}|elapsedSeconds={:.3f}|targetSeconds={:.3f}",
                       options.subjectId,
                       heartbeat_index,
                       elapsed_seconds,
                       options.soakDurationSeconds);
}

std::string format_completion_detail(const HostRunOptions& options, int heartbeat_count, double elapsed_seconds)
{
    return fmt::format("{}|elapsedSeconds={:.3f}|heartbeats={}",
                       options.subjectId,
                       elapsed_seconds,
                       heartbeat_count);
}
    return std::string(buffer);
}
}

extern "C" int il2cpp_host_run(int argc, const char** argv)
{
    const HostRunOptions options = parse_options(argc, argv);
    HostRunResult result{
        0,
        kBootstrapStage,
        options.subjectId,
    };
    report_host_result(result);

    if (options.soakDurationSeconds <= 0.0)
    {
        return result.exitCode;
    }

    const auto started_at = std::chrono::steady_clock::now();
    int heartbeat_count = 0;
    double elapsed_seconds = 0.0;

    while (elapsed_seconds < options.soakDurationSeconds)
    {
        ++heartbeat_count;
        report_success_stage(
            kHeartbeatStage,
            format_heartbeat_detail(options, heartbeat_count, elapsed_seconds)
        );

        const double remaining_seconds = options.soakDurationSeconds - elapsed_seconds;
        const double sleep_seconds = remaining_seconds < options.heartbeatIntervalSeconds
            ? remaining_seconds
            : options.heartbeatIntervalSeconds;
        if (sleep_seconds > 0.0)
        {
            std::this_thread::sleep_for(std::chrono::duration<double>(sleep_seconds));
        }
        elapsed_seconds = elapsed_seconds_since(started_at);
    }

    const std::string completion_detail = format_completion_detail(
        options,
        heartbeat_count,
        elapsed_seconds_since(started_at)
    );
    result.stage = kSoakCompleteStage;
    result.detail = completion_detail.c_str();
    report_host_result(result);
    return result.exitCode;
}
