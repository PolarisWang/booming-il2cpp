# Async 状态机翻译引擎 — 交接文档（2026-09-07 会话 2）

> 交接日期：2026-09-07 | 状态：段 A 结构发射完成，段 B(续列)+段 C(entry)+R2(编译)待续
> 权威入口：`docs/dev/in-progress/async-task-industrialization/STATUS.md`（§执行进度+§2026-09-07 会话末再校准）
> 上一交接：`docs/dev/in-progress/async-task-industrialization/HANDOFF-2026-09-07.md`

---

## 一、一句话现状

**codegen 层已把 async MoveNext 从 C++20 coroutine 占位改成真结构化发射**（产出真实 `extern "C"` 状态机函数，含 switch(state)+字段读写+EH+builder/awaiter call），但**真正 async/await 运行语义未闭环**：resume 续列注册未发、async entry 未建 box+Start、emitted C++ 未编译验证。

---

## 二、已提交成果（main，按时间序）

| Commit | 内容 | 域 |
|--------|------|----|
| Phase 1 基础设施（前会话） | | |
| `a5f6ec141` | **P1-1** ThreadPool 生命周期接入 RuntimeInit | runtime |
| `ef440e3de` | **P1-2** AsyncTask 续列(单槽 box resumption + 原子完成) + 5 测试 | runtime |
| `a46d4be49` | **P1-3** 续列经 ThreadPool 派发 + 桥接注册 + 2 测试 | runtime |
| `074775d4f` | **P2-1 spike** 手写 `async Task<int> One()` 的 native C++ 状态机，证明翻译模式端到端 | runtime+test |
| 本会话 Phase 2 翻译引擎 | | |
| `f04d4af34` | **Step 0+1** 真实 async subject (AsyncTestAssembly.dll) + 全管线提取测试 | codegen+test |
| `60b1c8b06` | **段 A** 替换 MethodEmission async 分支：移除 GenPromise/GenCoro 占位 → fall-through 到普通结构化发射 | codegen |
| `d5342d7e6` | **并行 agent** async.h 竞态修复（finish_async_task 顺序）+ smoke test 线程身份断言 | runtime |
| `23af99875` | **会话校准** STATUS.md 诚实评估段 A 边界 | docs |

---

## 三、架构决策（已锁定，不要推翻）

1. **手工状态机翻译，不做 C++20 coroutine** — hotupdate (P3) 不可丢
2. **MoveNext 当普通 value-this 方法发射** — 复用结构化发射路径，不内联自造 builder
3. **不反推 await 链** — 直接翻译 IL 状态机 switch + goto（Roslyn 已扁平化）
4. **段 A 结构发射 → 段 B 语义 → 段 C entry → R2 编译** 的顺序

---

## 四、剩余工作（下一会话入口）

### 段 C（推荐优先）— async entry wrapper emission
真正的 `async Task<int> GetOne()`（非 MoveNext）需翻译为：
```
builder.Create() → 初始化 >d__ struct → builder.Start(ref d__) → 返回 builder.Task
```
当前 codegen 产出 `GetOne` 是 Process 且走 interpreter stub / 0-return（因为它是 async 方法，IL 内 `builder.Start` 调用等尚未被真发射）。

**文件**：`MethodEmission.cs` 主路径（`GetOne` 不命中 `IsAsyncStateMachineMoveNext`，走正常发射 → 其 IL 含 `builder.Start` 调用、`builder.Task` getter、`stfld d__` 字段）。

### 段 B/R1 — resume 续列注册（codegen 侧）
`call AwaitUnsafeOnCompleted` 当前落成 `chaos_external_runtime_...` 外部解析 stub，未真注册 continuation。需要：
- 在 emission 的 awaiter 挂起分支（`is_completed==0`）补 `async_task_on_completed(task_handle, continuation_cb=MoveNext, ctx=this)` 调用
- `async_task_on_completed`/`finish_async_task` 已在 async.h 实现且已修复竞态（d5342d7e6）

### R2 — emitted C++ 编译验证
把 AsyncTestAssembly 整个 codegen 产出接进 foundation-dll 或 native smoke 测试：
- 确认 `chaos_valuetype_...d__` struct 声明在 GeneratedModule 存在
- 确认 `async_yield_*` / `async_task_builder_*` extern 声明齐全（无 C3861/LNK2019）
- 跑通 spike 等价往返：One→yield→ThreadPool→set_result(1)→completed=1

---

## 五、关键文件索引

| 文件 | 角色 |
|------|------|
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs` | async 分支（段 A 已改, 206-231） |
| `src/managed/Chaos.IL2CPP.Generator/Emission/AsyncCoroutineEmitter.cs` | 遗留: GenPromise/GenCoro 已不被非-Complex 路径调, 但 `IsAsyncStateMachineMoveNext`/`ClassifyAsyncMethod` 仍在用 |
| `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ObjectModelEmission.cs` | struct 声明（1277-1306 行 `>d__` 进 struct 集） |
| `src/native/common/chaos/async.h` | async_task_on_completed/finish_async_task/builder API（已修复竞态） |
| `tests/unit/managed/codegen/AsyncTestAssembly/AsyncMethods.cs` | 真实 async subject（GetOne/DoVoid） |
| `tests/unit/managed/codegen/AsyncPipelineTests.cs` | 全管线提取 + MoveNext 发射断言（3 测试） |
| `tests/unit/runtime-native/runtime-core/threading/async_integration_smoke_test.cpp` | Phase1 spike + 集成测试 |
| `docs/dev/in-progress/async-task-industrialization/STATUS.md` | 权威进度跟踪 |
| `docs/dev/in-progress/async-task-industrialization/roadmap-v1-01.md` | 6 阶段 roadmap |

---

## 六、并行注意

- 仓库多 agent 共享，`index.lock` 频繁争用。commit 前必 `git diff --cached --name-only` 核对暂存区
- `async.h` / `async_integration_smoke_test.cpp` 有并行 agent 的工作已在 d5342d7e6 push 到 main（竞态修复 + 线程身份断言），rebase 后再依赖
- 未提交的并行 agent 工作已 stash 保留（`stash@{0}`），不影响工作树

---

## 七、推荐顺序（下一 clean session）

```
1. 段 C：async entry wrapper（GetOne 本体 → builder.Start(box) → return Task）
   └──────────── codegen 单元测试验证文本
2. 段 B/R1：resume 续列注册（AwaitUnsafeOnCompleted → async_task_on_completed）
   └──────────── codegen 单元测试验证文本
3. R2：编译验证（emitted C++ 进 native smoke）
   └──────────── async_integration_smoke 新测试
4. 端到端往返（spike 等价：One→yield→ThreadPool→set_result(1)→completed=1）
```

段 C 推荐优先的理由：它建 >d__ box + 调 builder.Start → MoveNext 才可达。没有段 C，MoveNext 真跑不了。段 B/R1 补续列注册后，往返才真正异步。