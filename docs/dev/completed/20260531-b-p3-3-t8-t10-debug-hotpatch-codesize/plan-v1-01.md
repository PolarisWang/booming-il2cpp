# B-P3-3: T8-T10 Debug Info, Hotpatch Metadata, Code Size — 实现计划

> **面向执行 Agent：** 使用 dev-executing-plans 执行本计划。三项增强共享 code_generator.cpp 文件，必须串行按 T8 → T9 → T10 顺序实现。当前任务带有 `source_relation: roadmap-child`，执行完成时必须先归档子任务目录，再同步父 roadmap STATUS.md。

**目标：** 在 T4 native codegen 中实现三项收官增强：T8 debug info（行号映射）、T9 hotpatch metadata（可替换 call 标记）、T10 code size optimization（冷路径分离 + 紧凑帧）

**架构：** 三项增强均为 code_generator.cpp 的局部修改。T8/T9 在 NativeMethod/CallSiteInfo 中增加元数据并在 Scriban 输出注释；T10 激活既有冷分支 trampoline 机制并增加紧凑帧模式。

**技术栈：** C++20 (native codegen), Scriban (托管模板)

**架构审核模式：** critical（codegen 主线）

**结构告警重点：** codegen 与 Scriban 分层 — 所有调试和 hotpatch 元数据在 native 侧收集，Scriban 模板只负责输出注释

**权责图审核主题：** NativeMethod 的数据流（codegen 填充 → 序列化 → Scriban 读取 → 输出）是否正确分层

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: codegen-industrialization
- capabilityItem: B-P3-3-T8-T10
- ownerSubjectId: chaos::il2cpp::codegen
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: Smoke（T9 直接支撑 hotupdate call 替换，T10 减小补丁尺寸）
- formalVerificationObjects: [codegen_native_test.cpp all tests pass]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（设计摘要已写入 STATUS.md）

**问题清零来源：** parent-roadmap（父 roadmap B-P3-3 设计已批准，blocking_questions = []）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## Preflight Self-Review

| 检查项 | 结果 | 说明 |
|--------|------|------|
| 父 roadmap 边界 | pass | B-P3-3 边界已定义：不修改 IR 格式、不改 ABI |
| Files 冲突 | pass | T8/T9/T10 修改 code_generator.cpp，串行执行无冲突 |
| 潜在 blocker | pass | 冷分支 trampoline 基础设施已就绪（`cold_branch_patches_`），仅需激活 |
| 退出条件 | pass | codegen_native_test 全通过 = build pass |

Preflight 结论: **pass** — 所有检查项通过，可以继续自动执行。

---

## 阶段一: T8 Debug Info — 最小行号映射

### 修改文件

1. **`src/native/codegen/native_method.h`** — NativeMethod 新增 `DebugInfo` 嵌套结构
2. **`src/native/codegen/code_generator.cpp`** — EmitInstruction 循环中收集行号、NativeMethod 填充时序列化 debug info
3. **`src/managed/Chaos.IL2CPP.Generator/Templates/NativeAot.Method.cpp.scriban`** — 输出 `// line N` 注释

### 具体实现

#### A. NativeMethod.h: 新增 DebugInfo

```cpp
// ── Debug info: maps instruction offset to source line number ──
struct DebugEntry {
    uint32_t instr_offset;  // byte offset from code entry
    uint32_t source_line;   // source line number (0 = unknown)
};
struct DebugInfo {
    DebugEntry* entries    = nullptr;
    uint32_t    entry_count = 0;
    char*       debug_string = nullptr;  // optional method-level debug string
    uint32_t    debug_string_len = 0;

    ~DebugInfo() noexcept;
    DebugInfo() = default;
    // no copy, only move
    DebugInfo(const DebugInfo&) = delete;
    DebugInfo& operator=(const DebugInfo&) = delete;
    DebugInfo(DebugInfo&& other) noexcept;
    DebugInfo& operator=(DebugInfo&& other) noexcept;
};
```

NativeMethod 新增成员:
```cpp
DebugInfo debug_info_;
```

#### B. code_generator.cpp: 收集行号

在 `NativeCodeGenerator` 类中新增：
```cpp
// debug info state
std::vector<std::pair<uint32_t, uint32_t>> debug_entries_;  // (native_offset, source_line)
std::string debug_string_;
```

在 `EmitInstruction` switch 的 **call 类指令** 和 **branch 类指令** 的处理分支中，记录：

```cpp
// T8: record debug line info
if (instr.imm.debug_source_line != 0) {
    debug_entries_.push_back({buf_.pos(), instr.imm.debug_source_line});
}
```

注意：需要确认 `RegisterInstruction.imm` 是否有 `debug_source_line` 字段。
- 若有：直接读取
- 若没有：在 `RegisterInstruction` 中新增 `uint32_t source_line = 0` 字段

#### C. code_generator.cpp: 填充 NativeMethod

在 Generate() 末尾，NativeMethod 填充区新增：

```cpp
// T8: persist debug info
if (!debug_entries_.empty()) {
    nm->debug_info_.entry_count = static_cast<uint32_t>(debug_entries_.size());
    nm->debug_info_.entries = static_cast<DebugEntry*>(
        CHAOS_IL2CPP_MALLOC(nm->debug_info_.entry_count * sizeof(DebugEntry)));
    if (nm->debug_info_.entries) {
        for (uint32_t i = 0; i < nm->debug_info_.entry_count; ++i) {
            nm->debug_info_.entries[i] = {debug_entries_[i].first, debug_entries_[i].second};
        }
    }
}
if (!debug_string_.empty()) {
    nm->debug_info_.debug_string_len = static_cast<uint32_t>(debug_string_.size());
    nm->debug_info_.debug_string = static_cast<char*>(
        CHAOS_IL2CPP_MALLOC(debug_string_.size() + 1));
    if (nm->debug_info_.debug_string) {
        std::memcpy(nm->debug_info_.debug_string, debug_string_.data(), debug_string_.size());
        nm->debug_info_.debug_string[debug_string_.size()] = '\0';
    }
}
```

#### D. NativeMethod 析构/移动

在 `~NativeMethod()` 中添加 `debug_info_` 的清理，在 move 构造/赋值中添加转移。

#### E. Scriban 模板

在 `NativeAot.Method.cpp.scriban` 中，在方法体开始处添加：

```
{{~ if method.debug_info ~}}
// debug: {{ method.debug_info.debug_string }}
{{~ for entry in method.debug_info.entries ~}}
// line {{ entry.source_line }}: offset=0x{{ entry.instr_offset | x 8 }}
{{~ end ~}}
{{~ end ~}}
```

### 验证方式

- **构建**: `cmake --build build/native --target chaos_codegen_native_test --config RelWithDebInfo`
- **单元测试**: 在 codegen_native_test.cpp 新增 `Test_T8_DebugInfo` — 生成一个含多行指令的方法，验证 debug_entries_ 非空

---

## 阶段二: T9 Hotpatch Metadata — 可替换 Call 标记

### 修改文件

1. **`src/native/codegen/native_method.h`** — `CallSiteInfo` 新增 `is_hotpatchable` 字段
2. **`src/native/codegen/code_generator.cpp`** — `EmitCall` 对 hotpatchable call 插入 NOP sled
3. **`src/managed/Chaos.IL2CPP.Generator/Templates/NativeAot.Method.cpp.scriban`** — 输出 hotpatch 注释

### 具体实现

#### A. CallSiteInfo 新增字段

```cpp
struct CallSiteInfo {
    uint32_t    instr_pc;
    uint32_t    native_offset;
    uint32_t    method_token = 0;
    uint32_t    module_id    = 0;
    bool        is_hotpatchable = false;  // T9: true if this call site can be hotpatched
};
```

#### B. code_generator.cpp: 插入 NOP sled

在 call 类指令的 call_sites_.push_back 处，新增 `is_hotpatchable` 判断：

```cpp
// T9: check if call is hotpatchable (method has [ReplaceableCall] attribute)
bool hotpatchable = false;
if (config_.call_cache != nullptr && current_instr_index_ < config_.call_cache_count) {
    const auto& cached = static_cast<const runtime_instantiation::CachedCallInfo*>(config_.call_cache)[current_instr_index_];
    hotpatchable = cached.is_hotpatchable;  // requires CachedCallInfo to expose this flag
}
if (hotpatchable) {
    // Emit 5-byte NOP sled before the call for future inline patching
    // NOP sled: 0x66, 0x66, 0x66, 0x66, 0x90 (5 single-byte NOPs)
    buf_.EmitByte(0x66); buf_.EmitByte(0x66); buf_.EmitByte(0x66); buf_.EmitByte(0x66); buf_.EmitByte(0x90);
}
uint32_t call_pos = buf_.pos();
// ... emit call ...
call_sites_.push_back({current_instr_index_, call_pos, call_token, call_module, hotpatchable});
```

注意：`CachedCallInfo` 需要额外暴露 `is_hotpatchable`。这需要在 `patch_loader.h` 中确认或添加。

#### C. Scriban 模板

在 `NativeAot.Method.cpp.scriban` 中，遍历 call_sites:

```
{{~ for cs in method.call_sites ~}}
{{~ if cs.is_hotpatchable ~}}
// replaceable_call site={{ for.index }}, target=0x{{ cs.method_token | x 8 }}, nop_offset=0x{{ cs.native_offset | x 8 }}
{{~ end ~}}
{{~ end ~}}
```

### 验证方式

- **构建**: `cmake --build build/native --target chaos_codegen_native_test --config RelWithDebInfo`
- **单元测试**: 在 codegen_native_test.cpp 新增 `Test_T9_HotpatchCall` — 生成一个含 hotpatchable call 的方法，验证 call_site.is_hotpatchable == true

---

## 阶段三: T10 Code Size — 冷路径分离 + 紧凑帧

### 修改文件

1. **`src/native/codegen/code_generator.h`** — `CodeGenConfig` 新增 `optimize_for_size`
2. **`src/native/codegen/code_generator.cpp`** — 激活冷分支 trampoline + 紧凑帧模式

### 具体实现

#### A. CodeGenConfig 新增开关

```cpp
// If true, prefer smaller code over faster code (compact frame, cold-path splitting).
bool optimize_for_size = false;
```

#### B. 激活冷分支 trampoline

在 `EmitInstruction` 的 branch 处理分支（~line 1157），当前状态为：

```cpp
// PGO: profiles available via GetBranchTakenPct() for future
// branch hint or cold-path trampoline optimization.
```

修改为：

```cpp
// T10: cold-path trampoline for PGO cold branches (<10% taken)
if (config_.optimize_for_size && config_.branch_profiles != nullptr) {
    int32_t taken_pct = GetBranchTakenPct(current_instr_index_);
    if (taken_pct >= 0 && taken_pct < 10) {
        // Invert branch condition and emit JMP to cold section
        EmitConditionalJump(instr, /*invert=*/true, /*cold_trampoline=*/true);
        cold_branch_patches_.push_back({current_instr_index_, buf_.pos()});
        continue;  // fall-through now represents "not taken" (hot path)
    }
}
```

更新 `cold_branch_patches_` 填充段（~line 3399-3409）的注释，移除 "Currently unused"：

```cpp
// ── Cold-branch trampolines (PGO cold-path optimization) ────────────
// For branches with <10% taken probability, the Jcc was inverted and
// redirected here.  Activated when optimize_for_size is enabled.
```

#### C. 紧凑帧

在 `Generate()` 中，计算 `kFrameSize` 后，根据 `optimize_for_size` 和方法大小选择帧大小：

```cpp
// T10: compact frame for tiny methods
uint32_t effective_frame_size = kFrameSize;
if (config_.optimize_for_size && rm_.instructions.size() <= 4) {
    // Compact frame: only shadow space + minimal register file
    // No CallVirtArgs needed for tiny methods (no virtual dispatch)
    // FPR file kept but unused for non-FPR methods
    effective_frame_size = kCompactFrameSize;  // shadow(32) + GPR(512) = 544
    // Check if FPRs are actually used
    bool has_fpr = false;
    for (const auto& instr : rm_.instructions) {
        if (instr.has_dst() && instr.dst_reg() >= kGprCount) { has_fpr = true; break; }
        if (instr.has_src1() && instr.src1_reg() >= kGprCount) { has_fpr = true; break; }
    }
    if (!has_fpr) effective_frame_size = kCompactFrameSizeNoFpr;  // 544
}
```

新增常量：
```cpp
static constexpr uint32_t kCompactFrameSize = kCallVirtArgsOff;  // 800 bytes (no CallVirtArgs)
static constexpr uint32_t kCompactFrameSizeNoFpr = kGprFileOff + kGprFileSize;  // 544 bytes
```

**约束**: OSR entry 方法始终使用完整帧（`osr_entry_offset != 0` 时跳过紧凑帧）。

修改 prologue 中 `prologue_sub_rsp_size_` 的计算，使用 `effective_frame_size` 替代 `kFrameSize`。

修改 "Seal code buffer" 段的 `sm->frame_size`（~line 3661），使用 `effective_frame_size`。

### 验证方式

- **构建**: `cmake --build build/native --target chaos_codegen_native_test --config RelWithDebInfo`
- **单元测试**: 在 codegen_native_test.cpp 新增 `Test_T10_CompactFrame` — 生成 ≤4 指令的方法，验证生成的 native code 帧大小 < kFrameSize

---

## 验证命令

```bash
# 构建
cmake --build build/native --target chaos_codegen_native_test --config RelWithDebInfo

# 运行所有 codegen 测试
build/native/RelWithDebInfo/chaos_codegen_native_test.exe

# 仅运行 T8-T10 新增测试
build/native/RelWithDebInfo/chaos_codegen_native_test.exe --gtest_filter="*T8*:*T9*:*T10*"
```

## 退出标准

1. ✅ `chaos_codegen_native_test.exe` 所有测试通过（含新增的 T8/T9/T10 测试）
2. ✅ 无异于 B-P3-2 之前的回归（pre-existing OSR crash 不变）
3. ✅ 构建无警告

## 结构告警与架构审视

- **NativeMethod 扩展**：新增 `debug_info_` 字段增加了 NativeMethod 的大小，但 DebugInfo 使用堆分配 entries，NativeMethod 本体只增加一个指针大小
- **codegen/Scriban 分层**：所有元数据在 native 侧收集，模板只做输出拼接，没有决策逻辑混入 Scriban
- **紧凑帧与 OSR 兼容**：明确跳过 OSR entry 方法，不存在帧不兼容问题
- **冷分支激活安全性**：PGO counter 验证（taken_pct < 10）确保只对极冷分支生效；误判时只会增大码体积（额外的 JMP），不影响正确性

## 归档流程

执行完成后:
1. 更新 `docs/dev/in-progress/20260531-b-p3-3-t8-t10-debug-hotpatch-codesize/STATUS.md` → phase: `completed`
2. 物理移动目录 → `docs/dev/completed/20260531-b-p3-3-t8-t10-debug-hotpatch-codesize/`
3. 更新父 roadmap `docs/dev/in-progress/20260523-codegen-industrialization/STATUS.md` → B-P3-3: `completed`
4. 标记下一步为 B-P3-3 的下一个可用子任务或进入 P3-merge
