// Auto-generated runtime entry point for chaos-il2cpp AOT output.
#include <cstdint>
#include <cstdlib>
#include "codegen_bridge.h"
#include "runtime_abi.h"

extern "C" std::int32_t RunNativeAot(std::int32_t);

int main(int argc, char** argv) {
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) return -1;
    bridge->bootstrap_runtime();
    int result = RunNativeAot(argc > 1 ? std::atoi(argv[1]) : 0);
    return result;
}
