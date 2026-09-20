/// gc_raw_key_relocation_test — 裸托管指针键在 GC 搬移后的失效复现。
///
/// 背景（docs/dev/in-progress/raw-pointer-key-state-tables）：
///   native 侧状态表以**裸托管对象地址**为键。GC 的 Gen1 收集会把对象
///   物理搬移到新地址（gc_gen1.cpp: 批量 promote 到 Gen2 / Gen1 内部紧凑），
///   但只重写 old-gen slot / 静态根 / 线程栈 / GCHandle（见 gc_gen1.h
///   RelocateGen1References 注释）—— 裸指针键不在清单内。
///
/// 本文件验证两个受影响目标的**修复前红色 / 修复后绿色**：
///   A. pin_set        —— GcCreatePinnedHandle 记的键在搬移后失配
///   B. g_stream_state —— ChaosStringWriterCtor 记的键在搬移后失配
///   Sanity            —— 证明 Gen1 收集确实搬移对象（两实验的前提）
///
/// ⚠️ A 与 B 在修复前**必须失败**。若直接通过，先看 Sanity：
///    若 Sanity 也通过而 A/B 通过，说明修复已生效；
///    若 Sanity 失败，说明搬移未发生，装置无效，不得据其判断修复效果。

#include <cstdint>
#include <cstdio>
#include <cstring>

#include <chaos/native_types.h>
#include "gc_gen1.h"
#include "gc_heap.h"
#include "gc_scheduler.h"
#include "gc_stats.h"
#include "gc_young_gen.h"
#include "gc_region.h"
#include "gc_old_gen.h"
#include "gc_layout.h"
#include "thread_state.h"
#include "gc_test_base.h"
#include "gc_events.h"
#include "core/engine_lifecycle.h"
#include "runtime_stubs/stream_state.h"
#include <gtest/gtest.h>

// ⚠️ stream_stubs.h 的声明**没有** extern "C" 包裹，但 stream_stubs.cpp 的定义
// 在 extern "C" { }（:96-428）内 —— 头文件直接 include 会得到 C++ mangled 声明，
// 而库中实际是未修饰的 C 符号，链接报 LNK2019。
// 同族坑见记忆 stub-header-extern-c-subset-block。
extern "C" {
#include "runtime_stubs/stream_stubs.h"
}

using namespace chaos::il2cpp::runtime_core;

namespace {

constexpr CHAOS_IL2CPP_SIZE kObjSize = 64;

/// Allocate an object directly in Gen1 and stamp it with a test TypeInfo.
void* MakeGen1Object(uint32_t pattern) {
    void* obj = TryAllocateInGen1(kObjSize);
    if (obj == nullptr) return nullptr;
    *static_cast<const void**>(obj) = GcUnitTestBase::GetTestTypeInfo(kObjSize);
    *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + 8) = pattern;
    return obj;
}

/// Keep @a obj reachable from the nursery so the Gen1 collector marks it live
/// (Phase 3a scans Gen0 objects for Gen1 pointers).
void* PublishFromNursery(void* obj) {
    void* ref = NurseryAllocate(kObjSize);
    if (ref == nullptr) return nullptr;
    std::memset(ref, 0, kObjSize);
    std::memcpy(static_cast<char*>(ref) + 8, &obj, sizeof(void*));
    return ref;
}

/// Clear the nursery so previously-published references do not keep stale
/// objects alive across tests (mirrors gc_gen1_test.cpp's ClearNursery).
void ClearNursery() {
    auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) return;
    auto* n_end = g_young_gen.bump.load(std::memory_order_acquire);
    if (n_end != nullptr && n_end > nursery->begin) {
        std::memset(nursery->begin, 0,
                    static_cast<size_t>(n_end - nursery->begin));
    }
}

struct RawKeyRelocationTest : GcUnitTestBase {
    void TearDown() override {
        tls_tlab.start = nullptr;
        tls_tlab.end = nullptr;
        GcUnitTestBase::TearDown();
    }
};

}  // namespace

// ════════════════════════════════════════════════════════════════════════
// Sanity — 证明 Gen1 收集确实搬移对象（A/B 的前提）
// ════════════════════════════════════════════════════════════════════════
//
// 观测方式：不可用 pinned handle 观测搬移结果 —— pin 的语义正是「对象不被
// 搬移」，两者互斥（首轮实测：建了 pinned handle 后 old == new，测试假绿）。
// 改用 IsInGen1(obj)：搬移后旧地址不再落于 Gen1 区间。

TEST_F(RawKeyRelocationTest, Sanity_Gen1CollectionMovesObjects) {
    GcGen1Collection();
    ClearNursery();

    void* obj = MakeGen1Object(0xDEAD0000);
    ASSERT_NE(obj, nullptr);
    ASSERT_NE(PublishFromNursery(obj), nullptr);
    volatile void* stack_ref = obj;
    (void)stack_ref;

    ASSERT_TRUE(IsInGen1(obj)) << "precondition: object must start in Gen1";

    // 单次收集即触发 promote（实测走 gen1_early_exit 快路径，
    // 对象被 G_OldGen().Allocate + memcpy 搬到 Gen2 新地址）。
    Gen1CollectionResult r = GcGen1Collection();
    ASSERT_FALSE(r.promotion_failed) << "Gen1 collection failed";

    std::fprintf(stderr, "[raw-key sanity] obj=%p promoted=%llu in_gen1_after=%d\n",
                 obj, static_cast<unsigned long long>(r.objects_promoted),
                 IsInGen1(obj) ? 1 : 0);

    EXPECT_GE(r.objects_promoted, 1u)
        << "nothing promoted — the relocation premise does not hold";
    EXPECT_FALSE(IsInGen1(obj))
        << "object still resides in Gen1 after promotion — no physical move";
}

// ════════════════════════════════════════════════════════════════════════
// 实验 A — pin_set 的键在搬移后失配
// ════════════════════════════════════════════════════════════════════════
//
//   1. GcCreatePinnedHandle(obj)  → pin_set[obj] = true
//   2. 两次 Gen1 收集把 obj 从 Gen1 promote 到 Gen2（物理搬移 → new_addr）
//   3. GcRelocateHandles 更新 handle 表（GcGetHandleTarget 可见 new_addr）
//      但 **不碰 pin_set**（GcSetHandleTarget 零调用者）
//   4. 断言 GcIsPinnedObject(new_addr) 应为 true —— 修复前为 false

TEST_F(RawKeyRelocationTest, A_PinSetKeySurvivesRelocation) {
    GcGen1Collection();
    ClearNursery();

    void* obj = MakeGen1Object(0x0000A001);
    ASSERT_NE(obj, nullptr);
    ASSERT_NE(PublishFromNursery(obj), nullptr);

    // ⚠️ 必须把 obj 保存在**不会被后续调用覆盖**的位置。
    // 首版用局部变量，post-collect 打印时其值已被改写（...EC0048 → ...1E0F68），
    // 使 old/new 比较失去意义。用 static 强制稳定存储。
    static void* s_obj_a = nullptr;
    s_obj_a = obj;
    volatile void* stack_ref = s_obj_a;
    (void)stack_ref;

    const CHAOS_IL2CPP_UINT64 handle = GcCreatePinnedHandle(s_obj_a);
    ASSERT_NE(handle, 0u);
    ASSERT_TRUE(GcIsPinnedObject(s_obj_a))
        << "precondition: a fresh pinned handle must register in pin_set";

    ASSERT_TRUE(IsInGen1(s_obj_a)) << "precondition: object must start in Gen1";

    std::fprintf(stderr, "[raw-key A] pre-collect: obj=%p handle_target=%p in_gen1=%d pinned=%d\n",
                 s_obj_a, GcGetHandleTarget(handle), IsInGen1(s_obj_a) ? 1 : 0,
                 GcIsPinnedObject(s_obj_a) ? 1 : 0);

    // ── 单次收集即 promote（实测走 gen1_early_exit 快路径，物理搬移）──
    // 注：GC 的搬移路径**不查询 pin 状态**（gc_gen1.cpp 无 pin 判定），
    // 故即使是 pinned 对象也会被搬走 —— 这正是本缺陷的可观测条件。
    Gen1CollectionResult r = GcGen1Collection();
    ASSERT_FALSE(r.promotion_failed) << "Gen1 collection failed";
    ASSERT_GE(r.objects_promoted, 1u)
        << "object was not promoted — test is vacuous (see Sanity test)";

    // handle 表由 GC 重定位（GcRelocateHandles），据此读出搬移后的真实地址
    void* new_addr = GcGetHandleTarget(handle);
    ASSERT_NE(new_addr, nullptr);

    std::fprintf(stderr, "[raw-key A] post-collect: old=%p new=%p in_gen1_old=%d pinned_new=%d\n",
                 s_obj_a, new_addr, IsInGen1(s_obj_a) ? 1 : 0,
                 GcIsPinnedObject(new_addr) ? 1 : 0);

    // 若地址未变则本实验没有观测到搬移，是 vacuous 的
    ASSERT_NE(s_obj_a, new_addr)
        << "object did not move — cannot exercise the relocation path";

    // ── 被测断言 ──
    // 对象搬移后仍然存活、仍然是 pinned，故 GcIsPinnedObject 对**当前地址**
    // 必须回答 true。修复前 pin_set 的键停留在旧地址 → 返回 false。
    EXPECT_TRUE(GcIsPinnedObject(new_addr))
        << "pin_set key was not updated across relocation";

    GcFreeHandle(handle);
}

// ════════════════════════════════════════════════════════════════════════
// 实验 B — g_stream_state 的键不随对象搬移而更新
// ════════════════════════════════════════════════════════════════════════
//
// 观测方式：与实验 A 同构 —— 用 pinned handle 读出搬移后的新地址
// （handle 表由 GcRelocateHandles 重定位），再用新地址查 g_stream_state。
//
//   1. ChaosStringWriterCtor(obj) → g_stream_state[obj].kind = StringWriter
//   2. 触发 Gen1 收集，obj 被物理搬移到 new_addr
//   3. 断言：以 new_addr 查表应命中原 state
//      修复前：键停留在旧地址 → 落空 → 之后 ctor 会新建默认 state（内容全丢）

TEST_F(RawKeyRelocationTest, B_StreamStateKeyFollowsObjectAddress) {
    GcGen1Collection();
    ClearNursery();

    void* obj = MakeGen1Object(0x0000B001);
    ASSERT_NE(obj, nullptr);
    ASSERT_NE(PublishFromNursery(obj), nullptr);

    // 与 A 同理：局部变量会被后续调用覆盖，必须用稳定存储
    static void* s_obj_b = nullptr;
    s_obj_b = obj;
    volatile void* stack_ref = s_obj_b;
    (void)stack_ref;

    // 建流状态：以 obj 地址为键
    ChaosStringWriterCtor(reinterpret_cast<CHAOS_IL2CPP_INTPTR>(s_obj_b));

    {
        std::lock_guard<std::mutex> lock(g_stream_state_mutex);
        auto it = g_stream_state.find(s_obj_b);
        ASSERT_NE(it, g_stream_state.end())
            << "precondition: stream state must be keyed by the object address";
        ASSERT_EQ(it->second.kind, StreamKind::StringWriter);
    }

    ASSERT_TRUE(IsInGen1(s_obj_b)) << "precondition: object must start in Gen1";

    const CHAOS_IL2CPP_UINT64 handle = GcCreatePinnedHandle(s_obj_b);
    ASSERT_NE(handle, 0u);

    Gen1CollectionResult r = GcGen1Collection();
    ASSERT_FALSE(r.promotion_failed) << "Gen1 collection failed";
    ASSERT_GE(r.objects_promoted, 1u)
        << "object was not promoted — test is vacuous (see Sanity test)";

    void* relocated = GcGetHandleTarget(handle);
    ASSERT_NE(relocated, nullptr);
    ASSERT_NE(s_obj_b, relocated) << "object did not move";

    std::fprintf(stderr, "[raw-key B] old=%p relocated=%p\n", s_obj_b, relocated);

    // ── 被测断言 ──
    // 以搬移后的地址查表，应当能解析到该对象的流状态。
    // 修复前：表键停留在旧地址 → 落空（get_or_create_state 会新建默认 state，
    //         导致原状态内容全部丢失）。
    {
        std::lock_guard<std::mutex> lock(g_stream_state_mutex);
        auto by_new = g_stream_state.find(relocated);
        EXPECT_NE(by_new, g_stream_state.end())
            << "stream state is not keyed by the object's current address — "
               "after a GC relocation the state becomes unreachable";
        if (by_new != g_stream_state.end()) {
            EXPECT_EQ(by_new->second.kind, StreamKind::StringWriter)
                << "state found at the new address is not the original entry";
        }
    }

    GcFreeHandle(handle);
}
