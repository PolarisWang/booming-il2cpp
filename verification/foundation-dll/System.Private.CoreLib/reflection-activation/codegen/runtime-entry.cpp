// Auto-generated runtime entry point for chaos-il2cpp full-assembly AOT output.
// No single entry point — all methods are available via dispatch/hotpatch tables.
#include "codegen_bridge.h"
#include "runtime_abi.h"

int main(int argc, char** argv) {
    auto* bridge = chaos_codegen_get_bridge_v0();
    if (!bridge) return -1;
    bridge->bootstrap_runtime();
    return 0;
}
