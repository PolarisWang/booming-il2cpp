# P0: Call-Site Slot 间接化 + Version + ReverseSlotMap 实现计划

> **面向执行 Agent：** 必须使用 `dev-subagent-driven-development`（如果可用）或 `dev-executing-plans` 执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。完成时必须先归档子任务目录，再同步父任务。

**目标：** 将 JIT 调用从嵌入绝对地址改造为 slot 间接调用，并建立方法版本失效机制，为后续内联和 hot-update 安全打下基础。

**架构：** JIT 编译时对每个 call 分配 slot 并发射 `call [rip + offset]`，slot 指针在 hotpatch 时由 ReverseSlotMap 更新；内联的方法通过 version mismatch 触发调用者失效重编译。

**技术栈：** x64 call encoding (`FF 15`), C++ atomics, unordered_dense map, callback registration

**架构审核模式：** critical（修改 contracts/native/v0 合约文件 + JIT 引擎核心）

**结构告警重点：** slot_map.h 职责需聚焦，不混入 JitPrecode 逻辑；jit_engine.cpp 的 call 发射改造需控制修改范围

**权责图审核主题：** HotpatchEntryV0 新增 version 字段——确认不会破坏现有 ABI（字段追加在 struct 末尾，现有代码不读取该字段，向前兼容）

**设计文档：** `../../design-v1-01.md`（父任务）

**问题清零来源：** parent-roadmap（inherit from 20260524-jit-perf-optimization）

**计划来源：** roadmap-child-auto（父 roadmap P0 子任务）

**预期知识沉淀：** `n/a`（slot_map 实现细节不构成长期 wiki 知识）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 同步父任务"固定链路。

---

## 修改文件清单

| 文件 | 操作 | 职责 |
|------|------|------|
| `contracts/native/v0/codegen_bridge.h` | 修改 | HotpatchEntryV0 追加 uint32_t version |
| `src/native/jit/jit_method.h` | 修改 | JitMethod 新增 call_site_slots/call_site_count/inlined_method_tokens/compile_version |
| `src/native/jit/jit_engine.h` | 修改 | NativeCodeGenerator 新增 EmitSlotCall / alloc_slot |
| `src/native/jit/jit_engine.cpp` | 修改 | Call/Bridge/CallVirt 发射改为 slot 间接；新增 slot 分配 |
| `src/native/jit/slot_map.h` | **新增** | ReverseSlotMap 实现 |
| `src/native/jit/jit_precode.h` | 修改 | 前置声明或 include slot_map.h |
| `src/native/jit/jit_precode.cpp` | 修改 | 编译成功后注册 slot；hotpatch 回调；释放时清理 |
| `src/native/runtime-core/hotpatch_table.h` | 修改 | 新增 SlotUpdateCallback 类型 |
| `src/native/runtime-core/hotpatch_table.cpp` | 修改 | SetPatchedBySlot 成功后触发回调 |
| `src/native/runtime-core/jit_registration.h` | 修改 | 新增 RegisterReverseSlot / UnregisterMethodSlots / OnSlotUpdate 声明 |
| `src/native/runtime-core/jit_registration.cpp` | 修改（可能新建） | RegisterReverseSlot / UnregisterMethodSlots / OnSlotUpdate 实现 |
| `src/native/CMakeLists.txt` | 修改 | 添加 slot_map.h 到 include 路径或源文件列表（如需要） |

## 任务分解

### 任务 1: x64_encoder.h — 新增 `EmitCallRipRelative`

新增一个函数发射 `call [rip + rel32]`（`FF 15` + rel32）：

```cpp
/// call [rip + offset] — RIP-relative indirect call
inline void EmitCallRipRelative(CodeBuffer& buf, int32_t offset) noexcept {
    buf.EmitByte(0xFF);    // FF opcode
    buf.EmitByte(0x15);    // ModRM: Mod=00, Reg=2, R/M=101 (RIP-relative)
    buf.Emit32(offset);    // rel32 offset
}
```

### 任务 2: codegen_bridge.h — HotpatchEntryV0 追加 version

```cpp
typedef struct HotpatchEntryV0 {
    void*       direct_ptr;          /* AOT function pointer (set by jit)   */
    void*       interrupt_ptr;       /* = &InterpreterEntryDirect              */
    uintptr_t   method_key;          /* = PatchMethod* (0 when not patched)   */
    uint32_t    flags;               /* bit 0: kHotpatchActive                 */
    uint32_t    version;             /* ← NEW: bumped on every hotpatch        */
} HotpatchEntryV0;
```

**约束**：追加到 struct 末尾，不改变前 4 个字段的偏移量。旧的 codegen 输出不初始化 `version`，默认为 0（首次 JIT 编译时将 0 视为 version=1，或 codegen 在 JIT 模式下显式初始化）。

设计决策：`version` 从 1 开始，0 表示"未被 JIT 系统初始化"。首次 Call 编译时如果 entry->version == 0，认为 version = 1。

### 任务 3: jit_method.h — JitMethod 新增 slot/version 字段

```cpp
struct JitMethod {
    // ... 现有字段不变 ...

    // NEW: call-site slot table (array of void*, appending to code buffer or separately allocated)
    // Each slot holds the direct_ptr of the callee at compile time.
    // Hotpatch updates these pointers without requiring caller recompile.
    void**      call_site_slots      = nullptr;
    uint32_t    call_site_count      = 0;
    uint32_t    call_site_capacity   = 0;   // allocated slots (may exceed count)

    // NEW: method tokens of methods inlined into this JitMethod
    // Used by the invalidation system — when an inlined method is hotpatched,
    // this list identifies all JitMethods that need recompilation.
    uint32_t*   inlined_method_tokens = nullptr;
    uint32_t    inlined_count         = 0;

    // NEW: compilation version snapshot
    // Records the versions of all called callees at compile time.
    // Used by the invalidation system to detect stale inlined code.
    uint32_t*   callee_versions       = nullptr;   // parallel to call_site_slots / inlined
    uint32_t    callee_version_count  = 0;

    // NEW: this method's version at compile time
    uint32_t    compile_version       = 0;
};
```

**布局决策**：`call_site_slots` 有两种方案：
- **A) 接在 code 缓冲区后面**：和 code 在同一 RX 页面内，无法直接写入。需要在 compilation 阶段分配 RW 拷贝，生成完成后拷贝到 RX 区域。更新时需要 `VirtualProtect(PAGE_READWRITE)` 临时修改保护。
- **B) 独立 RW 分配**：用 `malloc` 或 `VirtualAlloc` 单独分配 RW 页面，可以直接写入。

**选择 B**（独立 RW 分配）：hotpatch 时不需要切换页面保护，减少并发风险。每个 JitMethod 的 slot 表在 ~4-16 个 slot 范围内，独立分配的开销可忽略。

### 任务 4: slot_map.h — ReverseSlotMap 实现

```cpp
// slot_map.h — Reverse mapping from callee token to caller slot entries.
// Used by the hotpatch system to update JIT call-site slot pointers.

#pragma once
#include <cstdint>
#include <chaos/unordered_dense.h>

namespace chaos::il2cpp::jit {

struct SlotEntry {
    JitMethod* caller;       // JitMethod that has a call to the callee
    uint32_t   slot_index;   // index into caller->call_site_slots[]
    uint32_t   callee_token; // metadata token of the callee (for verification)
};

// ReverseSlotMap: key = callee metadata token → vector<SlotEntry>
// Each entry records a call site that must be updated when the callee's direct_ptr changes.
//
// Thread safety: all accesses happen under the global hotpatch lock (held by
// HotpatchNameRegistry when SetPatchedBySlot is called).
class ReverseSlotMap {
public:
    void Add(uint32_t callee_token, JitMethod* caller, uint32_t slot_index) noexcept;
    void RemoveAll(JitMethod* caller) noexcept;  // cleanup when JitMethod is freed
    void UpdateAll(uint32_t callee_token, void* new_direct_ptr) noexcept;  // called on hotpatch

private:
    using Map = ankerl::unordered_dense::map<uint32_t, std::vector<SlotEntry>>;
    Map map_;
};

// Global singleton — declared in jit_registration.h, defined in jit_registration.cpp
extern ReverseSlotMap g_reverse_slot_map;

}  // namespace chaos::il2cpp::jit
```

**线程安全**：hotpatch 操作 `SetPatchedBySlot` 在 `HotpatchNameRegistry` 内部已经持有全局锁（从 `hotpatch_table.cpp` 代码可以看到 `g_hotpatch_name_registry` 是 static 单例，实际操作受锁保护）。ReverseSlotMap 的 `UpdateAll` 在同一锁保护下调用，不需要独立加锁。

### 任务 5: jit_engine.cpp — Call 发射改造

**核心改动**：在 `NativeCodeGenerator` 中新增 slot 分配函数，在 `EmitInstruction()` 的 `Call/CallBridge` case 中使用。

```cpp
// 在 NativeCodeGenerator 中新增:
uint32_t AllocateSlot(void* target_fn, uint32_t callee_token) {
    if (jit_method_->call_site_count >= jit_method_->call_site_capacity) {
        // Grow: double capacity, min 4
        uint32_t new_cap = jit_method_->call_site_capacity ? jit_method_->call_site_capacity * 2 : 4;
        auto* new_slots = new void*[new_cap];
        if (jit_method_->call_site_slots) {
            memcpy(new_slots, jit_method_->call_site_slots, jit_method_->call_site_count * sizeof(void*));
            delete[] jit_method_->call_site_slots;
        }
        jit_method_->call_site_slots = new_slots;
        jit_method_->call_site_capacity = new_cap;
    }
    uint32_t idx = jit_method_->call_site_count++;
    jit_method_->call_site_slots[idx] = target_fn;

    // Register reverse mapping
    if (callee_token != 0) {
        g_reverse_slot_map.Add(callee_token, jit_method_, idx);
    }
    return idx;
}
```

**Call emit 改造**（在 case IROpCode::Call / CallBridge 中）：

```cpp
// 当前:
// enc_.EmitMovImm64(kRAX, reinterpret_cast<uint64_t>(target_fn));
// EmitCallWithSpill(kRAX);  // 内部调用 enc_.EmitCallReg(kRAX) → FF D0

// 改成:
uint32_t slot_idx = AllocateSlot(target_fn, callee_token);
// slot 表地址需要放在生成代码之前就知道的位置
// 方案: slot 表在 Compile() 中分配，slot_idx 决定 call [rip + slot_base + idx*8] 的偏移
int32_t rel32 = CalculateSlotRipOffset(slot_idx, buf_.pos() + call_size);
enc_.EmitCallRipRelative(rel32);  // FF 15 + rel32
// 不需要 EmitCallWithSpill 了 — RIP-relative call 不占用寄存器，不需要 spill
```

**关键设计**：slot 表指针需要在代码生成之前就知道（因为 `call [rip + offset]` 需要编译时计算偏移）。方案：

1. `Compile()` 中预先估算 call 数量（扫描 RegisterMethod 的 Call/CallBridge/CallVirt 指令），分配 slot 表
2. 每个 call 在发射时分配 slot_index，计算 `offset = slot_table_addr - (call_instruction_addr + 6)`
3. slot 表可以接在 code 缓冲区末尾（已知偏移），或者独立分配（但独立分配后无法使用 RIP-relative，因为 slot 表不在 code 页面内）

**修正方案**：slot 表作为 code 缓冲区的一部分（末尾追加），生成完成后通过 `VirtualProtect` 保护整个区域为 RX。更新 slot 时再 `VirtualProtect` 改为 RW，写入后改回 RX。

```cpp
// slot 表偏移 = code_size (对齐到 8 字节)
uint32_t slot_table_offset = (code_size + 7) & ~7;
// 每个 call 的 RIP-relative 偏移:
// call 指令地址 = code_base + cur_pos
// slot_addr = code_base + slot_table_offset + slot_idx * 8
// rel32 = slot_addr - (call_addr + 6) = (slot_table_offset + slot_idx * 8) - (cur_pos + 6)
int32_t rel32 = (slot_table_offset + slot_idx * 8) - static_cast<int32_t>(buf_.pos() + 6);
enc_.EmitCallRipRelative(rel32);
```

**Hotpatch 时更新 slot**：
```cpp
void OnSlotUpdate(uint32_t token, void* new_ptr) {
    g_reverse_slot_map.UpdateAll(token, new_ptr);
    // UpdateAll 内部对每个 SlotEntry:
    //   DWORD old;
    //   VirtualProtect(caller->call_site_slots, page_size, PAGE_READWRITE, &old);
    //   caller->call_site_slots[slot_index] = new_ptr;
    //   VirtualProtect(caller->call_site_slots, page_size, PAGE_READONLY, &old);
    //   或者如果 slot 表是独立 RW 分配的，直接写入
}
```

### 任务 6: jit_precode.cpp — 编译后注册 slot + 失效逻辑

在 `JitStubDispatchImpl` 编译成功后，增加的逻辑：

```cpp
// Compile 成功后的 slot 注册（在 direct_ptr 替换之前）
if (jit->call_site_count > 0) {
    for (uint32_t i = 0; i < jit->call_site_count; i++) {
        uint32_t token = jit->call_sites[i].method_token;  // 从 call_sites 获取
        if (token != 0) {
            g_reverse_slot_map.Add(token, jit, i);
        }
    }
}
```

释放时清理 `UnregisterMethodSlots`：
```cpp
JitMethod::~JitMethod() noexcept {
    g_reverse_slot_map.RemoveAll(this);
    // ... 现有释放逻辑 ...
}
```

### 任务 7: hotpatch_table.cpp — Hotpatch 触发 slot 更新

新增回调注册机制：

```cpp
// hotpatch_table.h
using SlotUpdateCallback = void (*)(uint32_t method_token, void* new_direct_ptr);
void RegisterSlotUpdateCallback(SlotUpdateCallback cb);

// hotpatch_table.cpp
static SlotUpdateCallback g_slot_update_cb = nullptr;
void RegisterSlotUpdateCallback(SlotUpdateCallback cb) {
    g_slot_update_cb = cb;
}

// 在 SetPatchedBySlot 中（patch 成功后）
void HotpatchNameRegistry::SetPatchedBySlot(uint32_t module_id, uint32_t slot, 
                                            bool patched, void* method_key) noexcept {
    HotpatchEntryV0* entry = GetDispatchEntryBySlot(module_id, slot);
    if (entry == nullptr) return;

    if (patched) {
        entry->method_key = reinterpret_cast<uintptr_t>(method_key);
        entry->version++;  // ← NEW: bump version
        _InterlockedOr(&entry->flags, kHotpatchActive);

        // Notify JIT slot system
        // 用 module 的 token_slot_entries 从 slot 反查 token
        uint32_t token = SlotToToken(module_id, slot);  // ← NEW: reverse lookup
        if (token != 0 && g_slot_update_cb) {
            g_slot_update_cb(token, entry->direct_ptr);
        }
    } else {
        _InterlockedAnd(&entry->flags, ~kHotpatchActive);
        entry->method_key = 0;
        entry->version++;  // ← NEW: bump version even on unpatch
    }
}
```

**SlotToToken 实现**：反向查找 `token_slot_entries` 数组（codegen 发射的 `HotpatchSlotEntryV0` 结构包含 `token` 和 `slot` 字段），找到匹配 slot 的 token。

### 任务 8: jit_registration.cpp — ReverseSlotMap 全局实例

```cpp
// jit_registration.cpp (可能新增)
#include "slot_map.h"
namespace chaos::il2cpp::jit {
    ReverseSlotMap g_reverse_slot_map;
}
```

### 任务 9: CMakeLists.txt — 注册新文件

在 `src/native/jit/CMakeLists.txt` 或 `src/native/CMakeLists.txt` 中添加 `slot_map.h`。

## 验证方法

### 编译验证
```bash
cd build && cmake --build . --target chaos_jit
```

### Fact 验证（convert-char）
```bash
# 确保 entry-jit.exe 已用 JIT 模式重建
python build/check_foundation_dll_pipeline.py --family convert-char --stage jit-build
# 验证 fact
python build/check_foundation_dll_pipeline.py --family convert-char --stage fact_jit
```

### Slot 指令验证
```bash
# 反汇编验证 call 指令格式
objdump -d entry-jit.exe | grep -A2 "call.*\*0x"
# 应该看到 FF 15 开头的指令，而不是 FF D0/D1/E0
```

### Hotpatch 验证
1. 正常跑一轮 entry-jit.exe fact（所有方法代码生成）
2. 手动模拟一次 hotpatch（调用 `SetPatchedBySlot`）
3. 验证 slot 表中的指针被更新（`ReverseSlotMap::UpdateAll` 被触发）

## 执行顺序

```
任务 1: x64_encoder.h → EmitCallRipRelative       (独立，可先做)
任务 2: codegen_bridge.h → version 字段            (独立，可先做)
任务 3: jit_method.h → 新增字段                     (独立，可先做)
任务 4: slot_map.h → ReverseSlotMap                (独立，可先做)
任务 5: jit_engine.cpp → call 发射改造              (依赖任务 3)
任务 6: jit_precode.cpp → 注册/失效/清理            (依赖任务 4, 5)
任务 7: hotpatch_table.cpp → 回调 + version bump    (依赖任务 2, 4)
任务 8: jit_registration.cpp → 全局实例             (依赖任务 4)
任务 9: CMakeLists.txt → 注册文件                   (依赖任务 4)
验证: 编译 → fact → slot 指令确认 → hotpatch 测试
```

建议按 **1 → 2 → 3 → 4 → 5(→6+7+8+9 可并行) → 验证** 的顺序执行。
