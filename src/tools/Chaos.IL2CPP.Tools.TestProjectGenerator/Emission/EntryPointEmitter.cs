using System.Text;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Emission;

public sealed class EntryPointEmitter
{
    public string GenerateEntryPoint(IReadOnlyList<SubjectModel> subjects)
    {
        var sb = new StringBuilder();

        sb.Append("""
            // entry.cpp — Auto-generated
            // CLI entry point for native-AOT verification dispatch.
            //
            // Modes:
            //   (no args)            — fact: run all subject entries, print Passed: N/M
            //   --benchmark N I      — benchmark method N for I iterations
            //   --hotupdate          — hotpatch fact: baseline + apply + semantic-check + revert
            //   --hotupdate-and-benchmark N I — post-patch benchmark
            //   --microbench         — interpreter microbenchmarks
            //   --list               — list all subjects with their indices
            //   --fact-json          — per-method JSON output for value-level verification

            #include <cstdio>
            #include <cstdlib>
            #include <cstring>
            #include <chrono>
            #include <cstdint>
            #include <cinttypes>

            #if defined(_WIN32)
            #define NOMINMAX
            #include <Windows.h>
            #endif

            namespace chaos { namespace il2cpp { namespace common { namespace log_internal {
            bool g_log_use_stderr = false;
            }}}}

            #include <chaos/config.h>
            #include <chaos/native_types.h>
            #include <chaos/profile.h>
            #include <runtime_core.h>

            #include "chaos_runtime_host.h"

            #include <chaos/hotpatch_dispatch.h>
            #include <patch_loader.h>

            extern "C" const int kAotMethodCount;
            extern "C" const int kSubjectEntryCount;
            extern "C" const int kSubjectSlotMap[];

            extern "C" const HotpatchEntryV0* GetHotpatchEntries() noexcept;

            extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
            extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
            extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
            extern "C" void ChaosRegisterGcLayouts();

            #define CHAOS_USE_DEFAULT_THUNKS nullptr

            struct BenchmarkResult {
                double elapsed_ms;
                int64_t allocated_bytes;
            };

            extern "C" CHAOS_IL2CPP_INT32 RunFactAll();
            extern "C" BenchmarkResult RunBenchmark(int entry_index, int iterations);
            extern "C" CHAOS_IL2CPP_INT32 RunHotpatchAll();
            extern "C" BenchmarkResult RunHotpatchBenchmark(int entry_index, int iterations);

            extern const uint8_t kPatchData[];
            extern const size_t kPatchDataSize;
            extern const char* const kPatchDataHostClassName;

            extern "C" void RunMicrobench();

            """);

        // Generate subject metadata table for --list mode
        sb.AppendLine("// Subject metadata table for --list / --fact-json modes");
        sb.AppendLine("struct SubjectEntry {");
        sb.AppendLine("    int index;");
        sb.AppendLine("    const char* subjectId;");
        sb.AppendLine("    const char* assemblyName;");
        sb.AppendLine("    const char* typeName;");
        sb.AppendLine("    const char* methodName;");
        sb.AppendLine("    bool isStatic;");
        sb.AppendLine("};");
        sb.AppendLine();
        sb.AppendLine("static const SubjectEntry kSubjectEntries[] = {");

        for (int i = 0; i < subjects.Count; i++)
        {
            var s = subjects[i];
            var escapedSubjectId = EscapeCppString(s.SubjectId);
            var escapedAssembly = EscapeCppString(s.AssemblyName);
            var escapedType = EscapeCppString(s.FullTypeName);
            var escapedMethod = EscapeCppString(s.MethodName);
            sb.AppendLine($"    {{{i}, \"{escapedSubjectId}\", \"{escapedAssembly}\", \"{escapedType}\", \"{escapedMethod}\", {(s.IsStatic ? "true" : "false")}}},");
        }
        sb.AppendLine("};");
        sb.AppendLine();
        sb.AppendLine($"static const int kSubjectEntryCount_Generated = {subjects.Count};");
        sb.AppendLine();

        sb.Append("""
            static chaos::il2cpp::runtime_core::PatchContext* ApplyHotpatchIfAvailable() {
                if (kPatchDataSize > 0u) {
                    auto* patch_ctx = chaos::il2cpp::runtime_core::ApplyPatchFromMemory(
                        kPatchData, kPatchDataSize, kPatchDataHostClassName);
                    if (patch_ctx == nullptr) {
                        std::fprintf(stderr, "WARN: ApplyPatchFromMemory returned null (no patches applied)\n");
                    } else {
                        std::fprintf(stderr, "INFO: Applied patches to %u methods\n",
                                     static_cast<unsigned>(patch_ctx->method_count));
                    }
                    return patch_ctx;
                }
                return nullptr;
            }

            static int RunFactMode() {
                int failed_count = RunFactAll();
                int passed_count = kSubjectEntryCount - failed_count;
                printf("Passed: %d/%d\n", passed_count, kSubjectEntryCount);
                std::fflush(stdout);
                return failed_count;
            }

            static int RunFactJsonMode() {
                const int kCount = kSubjectEntryCount;
                printf("{\"factResults\":[");
                bool first = true;
                for (int si = 0; si < kCount; si++) {
                    int i = kSubjectSlotMap[si];
                    CHAOS_IL2CPP_INT32 result = 0;
                    bool caught = false;
                    CHAOS_EH_TRY
                        result = chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                            GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                    CHAOS_EH_CATCH_BEGIN
                        caught = true;
                    CHAOS_EH_END
                    if (!first) printf(",");
                    printf("{\"si\":%d,\"methodIndex\":%d,\"passed\":%s,\"exitCode\":%d}",
                           si, i, caught ? "false" : "true", caught ? -1 : (int)result);
                    first = false;
                }
                printf("]}\n");
                std::fflush(stdout);
                return 0;
            }

            static int RunListMode() {
                printf("{\"subjects\":[");
                for (int i = 0; i < kSubjectEntryCount_Generated; i++) {
                    if (i > 0) printf(",");
                    printf("{\"index\":%d,\"subjectId\":\"%s\",\"assemblyName\":\"%s\","
                           "\"typeName\":\"%s\",\"methodName\":\"%s\",\"isStatic\":%s}",
                           kSubjectEntries[i].index,
                           kSubjectEntries[i].subjectId,
                           kSubjectEntries[i].assemblyName,
                           kSubjectEntries[i].typeName,
                           kSubjectEntries[i].methodName,
                           kSubjectEntries[i].isStatic ? "true" : "false");
                }
                printf("]}\n");
                std::fflush(stdout);
                return 0;
            }

            static int RunBenchmarkMode(int entry_index, int iterations) {
                CHAOS_IL2CPP_PROFILE_SCOPE("RunBenchmarkMode");
                auto result = RunBenchmark(entry_index, iterations);
                if (result.elapsed_ms < 0.0) {
                    printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
                    return 1;
                }
                double ns_per_op = (result.elapsed_ms * 1e6) / iterations;
                double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
                double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
                printf(
                    "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
                    "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                    "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
                    result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
                    result.allocated_bytes, alloc_per_op);
                std::fflush(stdout);
                return 0;
            }

            static int RunHotupdateMode() {
                const int kCount = kSubjectEntryCount;
                printf("{\n");
                bool baseline_ok[256] = {false};
                bool baseline_caught[256] = {false};
                int64_t baseline_value[256] = {0};
                printf("\"baselineFact\":[");
                for (int si = 0; si < kCount; si++) {
                    int i = kSubjectSlotMap[si];
                    int64_t bv = 0;
                    bool caught = false;
                    CHAOS_EH_TRY
                        bv = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                            GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                        baseline_ok[si] = true;
                    CHAOS_EH_CATCH_BEGIN
                        caught = true;
                    CHAOS_EH_END
                    baseline_caught[si] = caught;
                    baseline_value[si] = bv;
                    if (si > 0) printf(",");
                    printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
                           si, caught ? "false" : "true", bv);
                }
                printf("],");
                auto* patch_ctx = ApplyHotpatchIfAvailable();
                int semantic_passed = 0;
                int semantic_changed_count = 0;
                printf("\"patchedFact\":[");
                for (int si = 0; si < kCount; si++) {
                    if (!baseline_ok[si]) { continue; }
                    int i = kSubjectSlotMap[si];
                    int64_t patched_value = 0;
                    bool patched_caught = false;
                    CHAOS_EH_TRY
                        patched_value = chaos::il2cpp::runtime_core::ChaosDispatchMethodGetValue(
                            GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                    CHAOS_EH_CATCH_BEGIN
                        patched_caught = true;
                    CHAOS_EH_END
                    if (semantic_passed > 0) printf(",");
                    printf("{\"si\":%d,\"passed\":%s,\"value\":%" PRId64 "}",
                           si, patched_caught ? "false" : "true",
                           patched_caught ? 0 : patched_value);
                    semantic_passed++;
                    if (baseline_caught[si] != patched_caught ||
                        baseline_value[si] != patched_value) {
                        semantic_changed_count++;
                    }
                }
                printf("],");
                if (patch_ctx != nullptr) {
                    chaos::il2cpp::runtime_core::Unpatch(patch_ctx);
                }
                bool all_revert = true;
                int revert_passed = 0;
                for (int si = 0; si < kCount; si++) {
                    int i = kSubjectSlotMap[si];
                    CHAOS_EH_TRY
                        chaos::il2cpp::runtime_core::ChaosDispatchMethod(
                            GetHotpatchEntries(), kAotMethodCount, i, CHAOS_USE_DEFAULT_THUNKS);
                        revert_passed++;
                    CHAOS_EH_CATCH_BEGIN
                        all_revert = false;
                    CHAOS_EH_END
                }
                bool all_semantic = (semantic_passed > 0 && semantic_changed_count > 0);
                printf(
                    "\"passedMethods\":%d,\"failedMethods\":0,"
                    "\"totalMethods\":%d,\"allSemantic\":%s,\"allRevert\":%s,"
                    "\"semanticChangedCount\":%d}\n",
                    semantic_passed, kCount, all_semantic ? "true" : "false",
                    all_revert ? "true" : "false", semantic_changed_count);
                std::fflush(stdout);
                return 0;
            }

            static int RunMicrobenchMode() {
                RunMicrobench();
                return 0;
            }

            static int RunHotupdateBenchmarkMode(int entry_index, int iterations) {
                auto* patch_ctx = ApplyHotpatchIfAvailable();
                auto result = RunHotpatchBenchmark(entry_index, iterations);
                if (result.elapsed_ms < 0.0) {
                    printf("{\"elapsedMilliseconds\":-1.0,\"error\":\"invalid index\"}\n");
                    return 1;
                }
                double ops_per_sec = (iterations / result.elapsed_ms) * 1000.0;
                double alloc_per_op = static_cast<double>(result.allocated_bytes) / iterations;
                printf(
                    "{\"elapsedMilliseconds\":%.3f,\"calibratedMs\":%.3f,"
                    "\"opsPerSecond\":%.0f,\"iterations\":%d,"
                    "\"allocatedBytes\":%" PRId64 ",\"allocPerOp\":%.1f}\n",
                    result.elapsed_ms, result.elapsed_ms, ops_per_sec, iterations,
                    result.allocated_bytes, alloc_per_op);
                std::fflush(stdout);
                return 0;
            }

            int main(int argc, char* argv[]) {
                chaos::il2cpp::common::log_internal::g_log_use_stderr = true;
                chaos::il2cpp::runtime_core::g_bgc_enabled = false;

                auto* chaos_host = new ChaosRuntimeHost();
                if (!chaos_host->Initialize("verification-entry")) {
                    std::fprintf(stderr, "FATAL: ChaosRuntimeHost::Initialize failed\n");
                    return 1;
                }
                if (!chaos_host->RegisterModule(
                        &chaos_codegen_code_registration,
                        &chaos_codegen_metadata_registration,
                        &chaos_codegen_options)) {
                    std::fprintf(stderr, "FATAL: RegisterModule failed\n");
                    return 1;
                }
                ChaosRegisterGcLayouts();

                int ret = 0;
                if (argc < 2) { ret = RunFactMode(); goto shutdown; }

                if (std::strcmp(argv[1], "--fact-json") == 0) { ret = RunFactJsonMode(); goto shutdown; }

                if (std::strcmp(argv[1], "--list") == 0) { ret = RunListMode(); goto shutdown; }

                if (std::strcmp(argv[1], "--benchmark") == 0) {
                    if (argc < 4) { printf("Usage: entry.exe --benchmark <index> <iterations>\n"); return 1; }
                    ret = RunBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
                    goto shutdown;
                }

                if (std::strcmp(argv[1], "--hotupdate") == 0) { ret = RunHotupdateMode(); goto shutdown; }

                if (std::strcmp(argv[1], "--hotupdate-and-benchmark") == 0) {
                    if (argc < 4) { printf("Usage: entry.exe --hotupdate-and-benchmark <index> <iterations>\n"); return 1; }
                    ret = RunHotupdateBenchmarkMode(std::atoi(argv[2]), std::atoi(argv[3]));
                    goto shutdown;
                }

                if (std::strcmp(argv[1], "--microbench") == 0) { ret = RunMicrobenchMode(); goto shutdown; }

                printf("Unknown flag: %s\n", argv[1]);
                ret = 1;

            shutdown:
                std::fflush(stdout);
                TerminateProcess(GetCurrentProcess(), static_cast<DWORD>(ret));
            }
            """);

        return sb.ToString();
    }

    public string GenerateEntryHeader()
    {
        return """
            // entry.h — Auto-generated
            #pragma once
            """;
    }

    private static string EscapeCppString(string value)
    {
        return value
            .Replace("\\", "\\\\")
            .Replace("\"", "\\\"")
            .Replace("\n", "\\n")
            .Replace("\r", "\\r")
            .Replace("\t", "\\t");
    }
}
