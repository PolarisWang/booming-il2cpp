#include "gc_stress.h"

namespace chaos::il2cpp::runtime_core {

GcStressState g_gc_stress;
thread_local bool tls_in_gc_stress = false;

}  // namespace chaos::il2cpp::runtime_core
