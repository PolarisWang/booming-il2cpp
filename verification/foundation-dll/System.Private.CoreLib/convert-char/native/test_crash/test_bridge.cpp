#include <cstdio>
#include "codegen_bridge.h"
#include "runtime_abi.h"
#include "runtime_core.h"

// Generated code symbols
extern "C" const int kAotMethodCount;
extern "C" const CodeRegistrationV0 chaos_codegen_code_registration;
extern "C" const MetadataRegistrationV0 chaos_codegen_metadata_registration;
extern "C" const CodegenRegistrationOptionsV0 chaos_codegen_options;

int main() {
    printf("Step 1: argc/argv OK\n"); fflush(stdout);

    auto* bridge = chaos_codegen_get_bridge_v0();
    printf("Step 2: bridge=%p\n", (void*)bridge); fflush(stdout);

    if (!bridge) return -1;

    bridge->register_codegen(
        &chaos_codegen_code_registration,
        &chaos_codegen_metadata_registration,
        &chaos_codegen_options);
    printf("Step 3: registered\n"); fflush(stdout);

    bridge->bootstrap_runtime();
    printf("Step 4: bootstrapped\n"); fflush(stdout);

    printf("kAotMethodCount=%d\n", kAotMethodCount);
    fflush(stdout);
    return 0;
}