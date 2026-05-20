/// gc_test_base.cpp — Shared test implementations for GC test fixtures.

#include "gc_test_base.h"

#include <cstring>
#include <random>

#include <chaos/native_types.h>
#include "gc_gen1.h"
#include "gc_layout.h"
#include "gc_loh.h"
#include "gc_old_gen.h"
#include "gc_region.h"
#include "gc_young_gen.h"

#if defined(_MSC_VER)
#include <intrin.h>
#include <windows.h>
#endif

namespace chaos::il2cpp::runtime_core {

// ═══════════════════════════════════════════════════════════════════════════
// GcUnitTestBase statics
// ═══════════════════════════════════════════════════════════════════════════

GcTestTypeRegistry* GcUnitTestBase::s_test_types_ = nullptr;
void*              GcUnitTestBase::s_warmup_      = nullptr;

// ═══════════════════════════════════════════════════════════════════════════
// SetupDefaultTestTypes
// ═══════════════════════════════════════════════════════════════════════════

GcTestTypeRegistry& SetupDefaultTestTypes() {
    static GcTestTypeRegistry s_registry;
    static bool s_initialised = false;
    if (s_initialised) return s_registry;

    constexpr int kCount = sizeof(kTestTypeSizes) / sizeof(kTestTypeSizes[0]);

    for (int i = 0; i < kCount; i++) {
        CHAOS_IL2CPP_SIZE size = kTestTypeSizes[i];
        uint64_t sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(
            static_cast<uint32_t>(size));
        s_registry.infos[i].stable_id = sid;
    }

    auto* reg = &GcLayoutRegistry::Instance();
    s_registry.range_begin = reinterpret_cast<uintptr_t>(&s_registry.infos[0]);
    s_registry.range_end =
        s_registry.range_begin + sizeof(s_registry.infos);
    reg->RegisterTypeInfoRange(s_registry.range_begin, s_registry.range_end);

    s_initialised = true;
    return s_registry;
}

const void* GcUnitTestBase::GetTestTypeInfo(CHAOS_IL2CPP_SIZE instance_size) {
    auto& reg = SetupDefaultTestTypes();
    constexpr int kCount = sizeof(kTestTypeSizes) / sizeof(kTestTypeSizes[0]);
    for (int i = 0; i < kCount; i++) {
        if (kTestTypeSizes[i] == instance_size) {
            return &reg.infos[i];
        }
    }
    return nullptr;
}

// ═══════════════════════════════════════════════════════════════════════════
// GcStressTestBase
// ═══════════════════════════════════════════════════════════════════════════

namespace {

/// Global array of surviving stress-test objects.
/// Written by worker threads under relaxed ordering; read-only after join.
static constexpr int kMaxGlobalStressObjects = 100000;
static std::atomic<void*> g_stress_globals[kMaxGlobalStressObjects];
static std::atomic<int> g_stress_global_count{0};

/// Pick an index from a probability-weighted distribution.
/// Returns the index into the distribution array.
template <typename T>
int PickWeighted(const std::vector<T>& dist, std::mt19937& rng) {
    float r = std::uniform_real_distribution<float>(0.0f, 1.0f)(rng);
    float cum = 0.0f;
    for (size_t i = 0; i < dist.size(); i++) {
        cum += dist[i].probability;
        if (r <= cum) return static_cast<int>(i);
    }
    return static_cast<int>(dist.size()) - 1;
}

}  // anonymous namespace

int GcStressTestBase::RunConcurrentAlloc(const StressConfig& cfg) {
    std::atomic<int> ok{1};
    g_stress_global_count.store(0, std::memory_order_relaxed);

    std::vector<std::thread> threads;
    threads.reserve(cfg.thread_count);

    for (int t = 0; t < cfg.thread_count; t++) {
        threads.emplace_back([&ok, &cfg, t]() {
            // Each thread registers itself and enters cooperative mode.
            uint32_t tid = threading::AllocateThreadId();
            threading::RegisterThread(tid, nullptr);
            threading::EnterCooperativeMode();

            // TLS survival array (thread-local references prevent GC from
            // collecting these objects).
            constexpr int kMaxTlsObjects = 1000;
            void* tls_objects[kMaxTlsObjects];
            int tls_count = 0;

            // Per-thread RNG seeded by thread id.
            std::mt19937 rng(static_cast<unsigned>(t + 1) * 0x9E3779B9u);

            for (int i = 0; i < cfg.allocs_per_thread; i++) {
                // Pick size class.
                int si = PickWeighted(cfg.size_distribution, rng);
                CHAOS_IL2CPP_SIZE size = cfg.size_distribution[si].size;

                // Pick survival strategy.
                int surv_idx = PickWeighted(cfg.survival_strategy, rng);
                SurvivalLevel surv = cfg.survival_strategy[surv_idx].level;

                // Allocate.
                void* obj;
                if (size >= 85 * 1024) {
                    obj = g_loh.Allocate(size);
                } else {
                    obj = NurseryAllocate(static_cast<uint32_t>(size));
                }

                if (!obj) {
                    ok.store(0, std::memory_order_relaxed);
                    break;
                }

                // Write TypeInfo header (for non-LOH objects).
                if (size < 85 * 1024 && size >= sizeof(void*)) {
                    *static_cast<const void**>(obj) = test_type_info_64();
                }
                // Write magic at fixed offset 12 for corruption detection.
                // This offset is past the TypeInfo header (8 bytes) and
                // the object must be at least 16 bytes for this to work.
                if (size >= 16) {
                    *reinterpret_cast<uint32_t*>(
                        static_cast<char*>(obj) + 12) = cfg.magic_pattern;
                }

                // Apply survival strategy.
                switch (surv) {
                case SurvivalLevel::GLOBAL: {
                    int idx = g_stress_global_count.fetch_add(1, std::memory_order_relaxed);
                    if (idx < kMaxGlobalStressObjects) {
                        g_stress_globals[idx].store(obj, std::memory_order_relaxed);
                    }
                    break;
                }
                case SurvivalLevel::THREAD_LOCAL: {
                    if (tls_count < kMaxTlsObjects) {
                        tls_objects[tls_count++] = obj;
                    }
                    break;
                }
                case SurvivalLevel::EPHEMERAL:
                default:
                    // No external reference — object dies at next GC.
                    break;
                }
            }

            // Keep TLS references alive by "using" them (prevents optimiser
            // from discarding).
            volatile void* sink;
            for (int j = 0; j < tls_count; j++) {
                sink = tls_objects[j];
                (void)sink;
            }

            threading::UnregisterThread();
        });
    }

    for (auto& th : threads) th.join();

    return g_stress_global_count.load(std::memory_order_relaxed);
}

void GcStressTestBase::VerifyNoCorruption(const StressConfig& cfg) {
    int count = g_stress_global_count.load(std::memory_order_acquire);
    for (int i = 0; i < count && i < kMaxGlobalStressObjects; i++) {
        void* obj = g_stress_globals[i].load(std::memory_order_relaxed);
        if (obj == nullptr) continue;

        // Check magic at the fixed offset 12.
        uint32_t val;
        std::memcpy(&val, static_cast<const uint8_t*>(obj) + 12, sizeof(val));
        EXPECT_EQ(val, cfg.magic_pattern)
            << "Global object " << i << " at " << obj << " corrupted";
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// GcUnitTestBase — Gen1 benchmark + timing utilities
// ═══════════════════════════════════════════════════════════════════════════

uint64_t GcUnitTestBase::RdtscToNs(uint64_t ticks) {
    static double s_ns_per_tick = 0.0;
    if (s_ns_per_tick == 0.0) {
        // Calibrate using QPC.
        LARGE_INTEGER freq, start_tsc, end_tsc, start_qpc, end_qpc;
        QueryPerformanceFrequency(&freq);
        QueryPerformanceCounter(&start_qpc);
        start_tsc.QuadPart = static_cast<LONGLONG>(Rdtsc());
        // Busy-wait ~1 ms.
        do {
            QueryPerformanceCounter(&end_qpc);
        } while (static_cast<double>(end_qpc.QuadPart - start_qpc.QuadPart) /
                     static_cast<double>(freq.QuadPart) < 0.001);
        end_tsc.QuadPart = static_cast<LONGLONG>(Rdtsc());
        uint64_t elapsed_tsc = static_cast<uint64_t>(
            end_tsc.QuadPart - start_tsc.QuadPart);
        double elapsed_s = static_cast<double>(
            end_qpc.QuadPart - start_qpc.QuadPart) /
            static_cast<double>(freq.QuadPart);
        s_ns_per_tick = (elapsed_s * 1e9) / static_cast<double>(elapsed_tsc);
    }
    return static_cast<uint64_t>(static_cast<double>(ticks) * s_ns_per_tick);
}

void GcBenchTestBase::RecordMetric(const char* name, uint64_t value_ns) {
    // Output as JSON-lite for potential post-processing.
    printf("[BENCH] %s: %llu ns\n", name, static_cast<unsigned long long>(value_ns));
    fflush(stdout);
}

void GcUnitTestBase::FillSurvivorTo(float occupancy) {
    // Calculate how many bytes to allocate in Gen1.
    char* survivor_start = g_young_gen.survivor_begin;
    char* survivor_end   = g_young_gen.survivor_end;
    CHAOS_IL2CPP_SIZE total_size = static_cast<CHAOS_IL2CPP_SIZE>(
        survivor_end - survivor_start);
    CHAOS_IL2CPP_SIZE target = static_cast<CHAOS_IL2CPP_SIZE>(
        static_cast<float>(total_size) * occupancy);

    // Allocate 1024-byte objects until target is reached.
    constexpr CHAOS_IL2CPP_SIZE kObjSize = 1024;
    // Need TypeInfo for 1024-byte objects.
    uint64_t sid = GcLayoutRegistry::Instance().RegisterOrGetRawAllocType(kObjSize);
    static GcTestTypeInfo s_big_ti{};
    s_big_ti.stable_id = sid;
    {
        auto* reg = &GcLayoutRegistry::Instance();
        uintptr_t ti_addr = reinterpret_cast<uintptr_t>(&s_big_ti);
        reg->RegisterTypeInfoRange(ti_addr, ti_addr + sizeof(GcTestTypeInfo));
    }

    // First do a Gen1 collection to clear any existing objects.
    GcGen1Collection();

    CHAOS_IL2CPP_SIZE allocated = 0;
    while (allocated + kObjSize <= target) {
        void* obj = TryAllocateInGen1(kObjSize);
        if (!obj) break;
        *static_cast<const void**>(obj) = &s_big_ti;
        // Write magic pattern at offset 8 for verification.
        *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) = 0xF00DCAFE;
        // Keep alive via the stack (volatile sink).
        volatile void* sink = obj;
        (void)sink;
        allocated += kObjSize;
    }
}

uint64_t GcUnitTestBase::MeasureGen1Collection() {
    uint64_t start = Rdtsc();
    Gen1CollectionResult r = GcGen1Collection();
    uint64_t end = Rdtsc();
    uint64_t ns = RdtscToNs(end - start);
    printf("[BENCH] gen1_pause_ns: %llu ns\n",
           static_cast<unsigned long long>(ns));
    printf("  [Gen1] objects=%llu promoted=%llu reclaimed=%llu\n",
           static_cast<unsigned long long>(r.objects_in_gen1),
           static_cast<unsigned long long>(r.objects_promoted),
           static_cast<unsigned long long>(r.bytes_reclaimed));
    return ns;
}

}  // namespace chaos::il2cpp::runtime_core
