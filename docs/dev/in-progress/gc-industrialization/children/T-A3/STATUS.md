# T-A3 — TSAN 并发测试 CI

## Scope

将 TSAN（ThreadSanitizer，数据竞争检测）接入 GC 并发测试的 per-PR 门禁。TSAN 是本计划「验证先行」战略中检测数据竞争中位数最有价值的手段——它补 ASAN（检测 UAF/越界）检测不到的**数据竞争**（正是 A2b 类 bug 的根源）。

## ⚠️ 现状调查结论（范围调整确认点）

**TSAN 在 Windows 上不可用**。ThreadSanitizer 是 GCC/Clang 特性，MSVC 不支持。

CRAG 的 GC CI 现状：
- `gc-ci.yml` 三个 job 全为 `windows-latest`（Windows + MSVC）
- **没有任何 Linux GC 原生单测的 CI 消费者**（`arm64-jit-ci.yml` 是 JIT 不是 GC；`aot-regression.yml` 是 AOT）
- 存在 `linux-x64.cmake` toolchain + `linux-x64-packaging` preset（Linux 构建基础设施存在）
- 但 **Linux x64 GC 原生单测从未在 CI 验证过**，可能有不绿风险

## 范围调整必要

原 T-A3 设计假设「在现有 CI 加 TSAN flag」。现实是 TSAN 需要**新增一整个 Linux x64 + Clang + TSAN GC 构建 job**：

1. 新增 `ubuntu-latest` + Clang 构建 job（用 `linux-x64` preset）
2. `-fsanitize=thread` 构建整个 GC native 单测树
3. 跑 GC 并发单测（general stress / young_collector 等）

这是一个**实质性的新平台 CI 基建**，超出原「加 flag」范围。且 Linux GC 单测可能从未绿过——需要先修绿 Linux 基线才能谈 TSAN。

## 决策请求

需要用户确认 T-A3 的执行方式：

- **A) 完整落地**：新增 Linux x64 GC CI + TSAN per-PR（最彻底，但工作量大，需先修 Linux GC 基线）
- **B) 拆分**：先新增 Linux x64 GC CI（无 TSAN），验证 Linux GC 单测绿；绿后再叠 TSAN（渐进，风险低）
- **C) 延后**：TSAN 延到 P4（平台覆盖阶段），先做 Windows 上能做的 T-A4/A5/A6

## Inputs

- `CMakePresets.json` `linux-x64-packaging` preset
- `build/toolchains/linux-x64.cmake` toolchain
- `arm64-jit-ci.yml`（Linux CI 构建参考，但注意那是 JIT 非 GC）
- `gc-ci.yml`（现行 GC CI）

## Expected Outputs

待决策后确定。

## Exit Criteria

待决策后确定。

## 元信息

- `parent_task_id`: gc-industrialization
- `source_relation: roadmap-child`
- `clearance_source: parent-roadmap`
- `entry_skill`: `dev-writing-plans`
- **blocked**: 等待用户确认范围调整（Linux platform 基建超出原子任务原始范围）