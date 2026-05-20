#include <cstdint>

// Declare the function
namespace chaos::il2cpp::runtime_core::threading {
    uint32_t RequestGlobalSafepoint() noexcept;
    void ReleaseGlobalSafepoint(uint32_t) noexcept;
}

int main() {
    auto gen = chaos::il2cpp::runtime_core::threading::RequestGlobalSafepoint();
    chaos::il2cpp::runtime_core::threading::ReleaseGlobalSafepoint(gen);
    return 0;
}
