// Auto-generated runtime-entry.cpp for project-test [runtime-self-test]
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <chrono>
#include <excpt.h>
#include <chaos/log.h>
#include "codegen_bridge.h"
#include "runtime_abi.h"
#include "runtime_core.h"
#include "exception_helpers.h"
#include "patch_loader.h"
#include "hotpatch_table.h"
#include "runtime_stubs/misc_stubs.h"
#include "runtime_stubs/stub_common.h"
#include "string_table.h"
#include "support.h"
#include "jit_registration.h"

extern "C" void* kChaosExternalRuntimeFnTable[];
extern "C" const char* kChaosExternalRuntimeSubjects[];
extern "C" int32_t kChaosExternalRuntimeCount;
extern "C" void ChaosJitRegisterAll();
extern "C" std::int32_t RunNativeAot(std::int32_t);
extern "C" double RunBenchmark(int, int);

extern const uint8_t kPatchData[];
extern const size_t kPatchDataSize;
extern const char* const kPatchDataHostClassName;

extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;
extern "C" const int kAotMethodCount;
extern "C" void SetExceptionFallback(void (*fn)());
extern "C" void SetUnhandledExceptionHook(void (*fn)(CHAOS_IL2CPP_INTPTR));

static const int kProjectEntryIndex = 24;

static void FillExternalRuntimeStubs() {
    for (int32_t i = 0; i < kChaosExternalRuntimeCount; i++) {
        if (kChaosExternalRuntimeFnTable[i] != nullptr) continue;
        const char* sub = kChaosExternalRuntimeSubjects[i];
        if (sub == nullptr || sub[0] == '\0') continue;
        if (std::strstr(sub, "::") == nullptr) continue;
        if (std::strstr(sub, "Console::WriteLine:")) {
            kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(
                +[](CHAOS_IL2CPP_INTPTR str_ptr) -> CHAOS_IL2CPP_INT32 {
                    const char* utf8 = nullptr;
                    CHAOS_IL2CPP_UINT32 byte_count = 0;
                    if (str_ptr != 0) {
                        if (chaos_is_string_id(str_ptr)) {
                            auto id = chaos_extract_string_id(str_ptr);
                            auto sv = chaos::il2cpp::string_table::Resolve(id);
                            if (sv.utf8_data != nullptr) {
                                utf8 = sv.utf8_data;
                                byte_count = sv.byte_count;
                            }
                        } else {
                            auto* hdr = reinterpret_cast<const StubStringHeader*>(str_ptr);
                            utf8 = stub_string_data(reinterpret_cast<const void*>(str_ptr));
                            byte_count = static_cast<CHAOS_IL2CPP_UINT32>(hdr->byte_count);
                        }
                    }
                    if (utf8 != nullptr && byte_count > 0) {
                        std::fwrite(utf8, 1, byte_count, stdout);
                    }
                    std::fputc('\n', stdout);
                    return 0;
                });
            continue;
        }
        kChaosExternalRuntimeFnTable[i] = reinterpret_cast<void*>(
            +[](CHAOS_IL2CPP_INTPTR) -> CHAOS_IL2CPP_INT32 { return 0; });
    }
}

int main(int argc, char* argv[]) {
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) { std::fprintf(stderr, "get_bridge failed\n"); return -1; }
    bridge->register_codegen(
        &chaos_codegen_code_registration,
        &chaos_codegen_metadata_registration,
        &chaos_codegen_options);
    bridge->bootstrap_runtime();

    auto* abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0();
    if (abi == nullptr) { std::fprintf(stderr, "GetRuntimeAbiV0 failed\n"); return -1; }

    RuntimeInitParams init_params = {};
    init_params.struct_size = sizeof(init_params);
    init_params.init_flags = 0;
    init_params.host_name_utf8 = "runtime-entry";

    RuntimeConfig config = {};
    config.struct_size = sizeof(config);
    config.allocator = nullptr;
    config.deallocator = nullptr;

    RuntimeState* runtime_state = nullptr;
    RuntimeStatus status = abi->runtime_init(&init_params, &config, &runtime_state);
    if (status != CHAOS_RUNTIME_STATUS_OK || runtime_state == nullptr) {
        std::fprintf(stderr, "runtime_init failed (status=%d)\n", static_cast<int>(status));
        return -1;
    }
    chaos::il2cpp::runtime_core::SetCurrentRuntimeState(runtime_state);

    ThreadState* thread_state = nullptr;
    status = abi->thread_attach(runtime_state, &thread_state);
    if (status != CHAOS_RUNTIME_STATUS_OK || thread_state == nullptr) {
        std::fprintf(stderr, "thread_attach failed (status=%d)\n", static_cast<int>(status));
        return -1;
    }

    FillExternalRuntimeStubs();
    ChaosJitRegisterAll();

    chaos::il2cpp::common::g_chaos_fail_hook = []() { chaos::il2cpp::runtime_core::chaos_raise_exception(0); };

    // -- Run with crash catching --
    __try {
        RunNativeAot(kProjectEntryIndex);
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        std::fprintf(stderr, "FATAL: SEH code=0x%08X\n",
            static_cast<unsigned>(GetExceptionCode()));
        _Exit(1);
    }

    chaos::il2cpp::common::g_chaos_fail_hook = nullptr;
    std::fflush(stdout);
    _Exit(0);
}
