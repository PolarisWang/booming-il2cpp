#include "method_replacement.h"

#include <iostream>

namespace {

void* ReplacementThunk() {
    return reinterpret_cast<void*>(0x42u);
}

}  // namespace

int main() {
    using chaos::il2cpp::method_replacement::ActiveCount;
    using chaos::il2cpp::method_replacement::Register;
    using chaos::il2cpp::method_replacement::Resolve;
    using chaos::il2cpp::method_replacement::Revert;
    using chaos::il2cpp::method_replacement::RevertAll;

    RevertAll();
    if (!Register(0x06000001u, reinterpret_cast<void*>(&ReplacementThunk))) {
        std::cerr << "method-replacement-register=unexpected" << std::endl;
        return 1;
    }

    if (Resolve(0x06000001u) == nullptr) {
        std::cerr << "method-replacement-resolve=unexpected" << std::endl;
        return 1;
    }

    std::cout << "method-replacement-active=" << ActiveCount() << std::endl;
    std::cout << "method-replacement-resolve=ok" << std::endl;

    if (!Revert(0x06000001u)) {
        std::cerr << "method-replacement-revert=unexpected" << std::endl;
        return 1;
    }

    std::cout << "method-replacement-after-revert=" << ActiveCount() << std::endl;
    return 0;
}
