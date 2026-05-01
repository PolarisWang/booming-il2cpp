// Auto-generated benchmark native entry
// Family: family/System.Runtime.Serialization.Formatters/object-manager-id-generator
#include <chaos/native_types.h>
#include <chaos/native_types.h>

namespace chaos::benchmark::object_manager_id_generator
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::ObjectIDGenerator::Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_0() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::GetId:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_1() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::HasId:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_2() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::ObjectManager::Void(System.Object, System.Object)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_3() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::DoFixups:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_4() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::GetObject:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_5() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseDeserializationEvent:System.Void()
    CHAOS_IL2CPP_INT32 BenchmarkEntry_6() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseOnDeserializingEvent:System.Void(P0)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_7() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_8() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordDelayedFixup:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_9() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordFixup:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_10() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_11() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_12() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2, P3, P4)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_13() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(P0, P1, P2, P3, P4, P5)
    CHAOS_IL2CPP_INT32 BenchmarkEntry_14() {
        return static_cast<CHAOS_IL2CPP_INT32>(42);
    }

    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();
    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {
        &BenchmarkEntry_0,
        &BenchmarkEntry_1,
        &BenchmarkEntry_2,
        &BenchmarkEntry_3,
        &BenchmarkEntry_4,
        &BenchmarkEntry_5,
        &BenchmarkEntry_6,
        &BenchmarkEntry_7,
        &BenchmarkEntry_8,
        &BenchmarkEntry_9,
        &BenchmarkEntry_10,
        &BenchmarkEntry_11,
        &BenchmarkEntry_12,
        &BenchmarkEntry_13,
        &BenchmarkEntry_14
    };
    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = 15;

}  // namespace chaos::benchmark::object_manager_id_generator

extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)
{
    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::object_manager_id_generator::kBenchmarkEntryCount) { return -1; }
    return chaos::benchmark::object_manager_id_generator::kBenchmarkEntries[entryIndex]();
}
