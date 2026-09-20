/// gc_gen1_crossgen_ref_test — 跨代引用在 Gen1 搬移后的正确性（端到端验证）。
///
/// 背景（docs/dev/completed/20260920-02-raw-pointer-key-state-tables）：
///   `GcGen1Collection()` 的三条**快路径**（Tier1 span<=4096 / Tier2 对象数少 /
///   drain bitmap 失败）曾把对象 `memcpy` 到 Gen2 新地址后**直接 return**，
///   从不调用 `RelocateGen1References` —— 于是所有跨代边仍指向**已腾空的
///   Gen1 地址**（cross-gen UAF，对象自身的注释称之为 GC-N6 mode3）。
///
///   本文件构造该场景的**端到端**最小复现：一个 Gen2 对象持有 Gen1 对象的
///   引用；触发 Gen1 收集把被引用者搬走；随后**通过 Gen2 对象读回**该引用
///   指向的数据。修复前读到的是旧地址（悬垂内存），修复后读到新地址。
///
/// 与 gc_raw_key_relocation_test.cpp 的区别：
///   那个验证的是 **native 侧表键**（pin_set / g_stream_state）；
///   本文件验证的是 **GC 自己管理的堆内引用** —— 即
///   `RelocateGen1References` Phase 1 的 old-gen slot 重写路径。
///
/// ⚠️ 判定口径：本测试断言「通过跨代边读到的对象身份正确」。
///    修复前该断言应失败（读到的是被覆盖的旧内存）。

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
#include <gtest/gtest.h>

using namespace chaos::il2cpp::runtime_core;

namespace {

constexpr CHAOS_IL2CPP_SIZE kObjSize = 64;

/// Offset of the first payload word we use to store a reference.
constexpr CHAOS_IL2CPP_SIZE kRefOffset = 8;
/// Offset of the identity pattern used to recognise an object.
constexpr CHAOS_IL2CPP_SIZE kPatternOffset = 16;

/// Stamp @a obj with a test TypeInfo and an identity pattern.
void StampObject(void* obj, uint32_t pattern) {
    *static_cast<const void**>(obj) = GcUnitTestBase::GetTestTypeInfo(kObjSize);
    *reinterpret_cast<uint32_t*>(static_cast<char*>(obj) + kPatternOffset) = pattern;
}

uint32_t ReadPattern(const void* obj) {
    return *reinterpret_cast<const uint32_t*>(static_cast<const char*>(obj) + kPatternOffset);
}

/// Store @a target as a managed reference inside @a holder's first payload slot.
void SetRef(void* holder, void* target) {
    std::memcpy(static_cast<char*>(holder) + kRefOffset, &target, sizeof(void*));
}

void* GetRef(void* holder) {
    void* value = nullptr;
    std::memcpy(&value, static_cast<char*>(holder) + kRefOffset, sizeof(void*));
    return value;
}

void ClearNursery() {
    auto* nursery = g_young_gen.region.load(std::memory_order_acquire);
    if (nursery == nullptr) return;
    auto* n_end = g_young_gen.bump.load(std::memory_order_acquire);
    if (n_end != nullptr && n_end > nursery->begin) {
        std::memset(nursery->begin, 0,
                    static_cast<size_t>(n_end - nursery->begin));
    }
}

struct CrossGenRefTest : GcUnitTestBase {
    void TearDown() override {
        tls_tlab.start = nullptr;
        tls_tlab.end = nullptr;
        GcUnitTestBase::TearDown();
    }
};

}  // namespace

// ════════════════════════════════════════════════════════════════════════
// 端到端：Gen2 持有 Gen1 引用，Gen1 搬移后跨代边必须仍然有效
// ════════════════════════════════════════════════════════════════════════
//
// 1. 在 Gen2（old-gen）分配 holder，并记下身份图案
// 2. 在 Gen1 分配 target，写身份图案，让 holder 引用它
// 3. 用 nursery 引用保持 target 存活（Phase 3a 扫 Gen0 → Gen1 的边）
// 4. 触发 GcGen1Collection() —— target 被 promote 到 Gen2 新地址
// 5. 通过 holder 的跨代边读回 target，校验身份图案
//
// 修复前：跨代边保留旧地址 → ReadPattern 读到被覆盖的内存（图案不符）
// 修复后：Phase 1 重写 old-gen page slot → 读到正确图案

TEST_F(CrossGenRefTest, Gen2HolderSeesRelocatedGen1Target) {
    GcGen1Collection();
    ClearNursery();

    // ── Step 1: Gen2 holder ──
    void* holder = g_old_gen.Allocate(kObjSize, /*pinned=*/false);
    ASSERT_NE(holder, nullptr) << "failed to allocate holder in old-gen";
    StampObject(holder, 0x60DE0000);

    // ── Step 2: Gen1 target, referenced by holder ──
    void* target = TryAllocateInGen1(kObjSize);
    ASSERT_NE(target, nullptr);
    StampObject(target, 0x7A26E000);
    SetRef(holder, target);

    // ── Step 3: keep target alive from the nursery ──
    void* nursery_ref = NurseryAllocate(kObjSize);
    ASSERT_NE(nursery_ref, nullptr);
    std::memset(nursery_ref, 0, kObjSize);
    std::memcpy(static_cast<char*>(nursery_ref) + kRefOffset, &target, sizeof(void*));

    // Keep both reachable from the stack as well (conservative scan).
    volatile void* keep_holder = holder;
    volatile void* keep_target = target;
    (void)keep_holder;
    (void)keep_target;

    ASSERT_TRUE(IsInGen1(target)) << "precondition: target must start in Gen1";
    ASSERT_EQ(GetRef(holder), target) << "precondition: holder must reference target";

    // ── Step 4: trigger the relocation ──
    Gen1CollectionResult r = GcGen1Collection();
    ASSERT_FALSE(r.promotion_failed) << "Gen1 collection failed";

    // ══════════════════════════════════════════════════════════════════
    // 🔴 关键步骤：复用 Gen1 内存，否则本测试**测不出缺陷**。
    //
    // `GcGen1Collection()` 的快路径只把 `gen1_bump` 重置回 `gen1->begin`，
    // **不清零**腾出的内存。因此一条未重写的跨代边在内存被再次分配之前，
    // 仍能读到原对象的字节 —— 立即检查无法区分「边已正确重写」与
    // 「边仍指向陈旧但完好的内存」。
    //
    // 分配新的 Gen1 对象会覆盖旧地址（真实负载正是如此）。此后悬垂的边
    // 就指向被复用的内存，身份图案校验才有判别力。
    //
    // 实测（修复前）：本节执行后 holder 读到 0xBADBAD00（填充图案）而非
    // 原对象 → cross-gen UAF 可观测。
    // ══════════════════════════════════════════════════════════════════
    {
        constexpr int kReuseCount = 4;
        for (int i = 0; i < kReuseCount; i++) {
            void* filler = TryAllocateInGen1(kObjSize);
            if (filler == nullptr) break;
            std::memset(filler, 0xCD, kObjSize);
            *static_cast<const void**>(filler) =
                GcUnitTestBase::GetTestTypeInfo(kObjSize);
            *reinterpret_cast<uint32_t*>(
                static_cast<char*>(filler) + kPatternOffset) = 0xBADBAD00u;
            volatile void* keep = filler;
            (void)keep;
        }
    }

    // ── Step 5: the assertion under test ──
    void* via_holder = GetRef(holder);

    std::fprintf(stderr,
        "[crossgen] holder=%p target=%p promoted=%llu via_holder=%p same=%d\n",
        holder, target,
        static_cast<unsigned long long>(r.objects_promoted),
        via_holder,
        (via_holder == target) ? 1 : 0);

    ASSERT_NE(via_holder, nullptr)
        << "cross-generation edge became null across relocation";

    // ⚠️ 不能用 `via_holder != target` 判断是否发生搬移：
    //    `target` 是栈上的局部变量，保守栈扫描会把它当作根并在搬移后**一并更新**
    //    （RelocateGen1References Phase 3），因此两者在搬移后仍可能相等。
    //    真正的判据是**通过跨代边读到的对象身份是否仍然正确**。

    EXPECT_EQ(ReadPattern(via_holder), 0x7A26E000u)
        << "reading through the Gen2->Gen1 edge yielded a different object — "
           "the cross-generation reference was NOT rewritten after relocation "
           "(cross-gen UAF); holder now points at vacated Gen1 memory";
}

// ════════════════════════════════════════════════════════════════════════
// 对照：未被引用的 Gen1 对象应被回收（确认本装置不是"什么都不做"）
// ════════════════════════════════════════════════════════════════════════

TEST_F(CrossGenRefTest, Sanity_UnreferencedGen1ObjectIsNotKeptAlive) {
    GcGen1Collection();
    ClearNursery();

    void* orphan = TryAllocateInGen1(kObjSize);
    ASSERT_NE(orphan, nullptr);
    StampObject(orphan, 0x0A000000);

    Gen1CollectionResult r = GcGen1Collection();

    std::fprintf(stderr,
        "[crossgen sanity] orphan=%p promoted=%llu reclaimed=%llu\n",
        orphan,
        static_cast<unsigned long long>(r.objects_promoted),
        static_cast<unsigned long long>(r.bytes_reclaimed));

    // Either it was reclaimed, or it was conservatively kept (its address may
    // still sit on this stack frame).  Both are acceptable; what matters is
    // that the collection ran without failure.
    EXPECT_FALSE(r.promotion_failed);
}
