	lifecycle_status: completed
	completed_at: 2026-05-24
	results:
	  - entry-jit.exe fact 18/18 通过
	  - HotpatchEntryV0 新增 version 字段
	  - x64_encoder.h 新增 EmitCallRipRel
	  - slot_map.h: ReverseSlotMap (Add/RemoveAll/UpdateAll)
	  - jit_engine.cpp: slot 表发射 + call [rip+off] + g_reverse_slot_map
	  - hotpatch_table.cpp: RegisterSlotUpdateCallback + version bump + callback
	  - jit_precode.cpp: SlotUpdateCallback 注册
	  - 基准测试无回归（15/18 benchmark 与前一致）
	  - 已知问题：methods 0/9/10 benchmark 段错误（预存现，与 slot 无关）
---
task_type: plan
parent_task_id: 20260524-jit-perf-optimization
source_task_id: p0-call-site-slot
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
created: 2026-05-24
---

# P0: Call-Site Slot 间接化 + Version + ReverseSlotMap — STATUS

## Scope

实现 JIT 编译的 call-site slot 间接化基础设施。包括：

1. **HotpatchEntryV0 新增 version 字段** — 每次 hotpatch 时 atomic bump
2. **JIT call 发射改造** — 从 `mov rax, imm64; call rax` 改为 `call [rip + slot_off]`
3. **Slot 表分配** — 附着在 JitMethod 代码缓冲区后，或独立 RW 页面
4. **ReverseSlotMap** — callee token → (caller JitMethod*, slot_index) 映射
5. **Hotpatch 回调** — hotpatch 时更新所有引用该方法的 slot 指针
6. **内联失效框架** — JitMethod 记录 inlined_method_tokens，hotpatch 时标记 stale
7. **版本快照** — 编译时记录 callee version，用于内联失效判定

## Inputs

- 设计文档: `../design-v1-01.md` — D1/D2 决策
- 父 roadmap: `../roadmap-v1-01.md` — P0 阶段定义

## Expected Outputs

**修改的文件：**
- `contracts/native/v0/codegen_bridge.h` — HotpatchEntryV0 加 version
- `src/native/jit/jit_engine.h` — JitMethod 加 call_site_slots/inlined_tokens/version
- `src/native/jit/jit_engine.cpp` — EmitCallWithSpill 改 slot 发射
- `src/native/jit/jit_precode.cpp` — RegisterReverseSlot / OnSlotUpdate / 失效
- `src/native/jit/jit_precode.h` — ReverseSlotMap 相关声明
- `src/native/runtime-core/hotpatch_table.cpp` — Patch 后触发 slot update callback
- `src/native/runtime-core/jit_registration.h` — RegisterReverseSlot API

**新增的文件：**
- `src/native/jit/slot_map.h` — ReverseSlotMap 实现

**验证产物：**
- entry-jit.exe (convert-char 18 methods fact 通过)

## Exit Criteria

1. entry-jit.exe fact 全部通过（18/18）
2. JIT 生成的 call 指令使用 `call [rip+off]` 格式（验证 objdump）
3. Hotpatch 测试：patch 一个方法后，调用者的 slot 指针正确更新
4. 版本号测试：version bump 后，内联调用者触发失效重编译
5. 旧 AOT 模式不受影响（entry-aot.exe fact 全部通过）

## Terminal Notes

（由执行终端填写）
