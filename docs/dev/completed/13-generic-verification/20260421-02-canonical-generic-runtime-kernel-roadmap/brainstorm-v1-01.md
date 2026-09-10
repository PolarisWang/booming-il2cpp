# Canonical Generic Runtime Kernel Roadmap Brainstorm v1.01

Date: 2026-04-21 23:02:19 +08:00
Status: approved-for-roadmap

## 1. 背景

当前 generic codegen 虽然已经具备：

- 闭合实例 `genericContext`
- 一批 proof-driven 的 family 支持
- `Task<T>` / collection / dispatch / constraint 的局部可运行路径

但主路径仍然是：

- loader 复制闭合实例
- planner/emitter 按字符串前缀做 family 特判
- generated native code 以闭合 body 为核心

这意味着系统“能过 proof”，但不具备足够稳固的泛化能力，也无法自然承接更多 BCL generic family。

## 2. 约束

- 必须遵守 `managed solution -> collector -> collection -> managed/native/hotupdate hosts` 主线，不允许绕开 pipeline。
- 不能把 generic 新能力只停留在 metadata 描述层，必须最终进入 native runtime 执行语义。
- 不接受长期双轨 generic 主路径；最终必须清掉 legacy clone/materialization。
- 必须把 hotupdate generic 边界纳入正式验证，而不是只跑 managed/native proof。

## 3. 备选路线

### 路线 A：验证先行硬切

- 先补全 generic matrix，再进入 contracts/loader/IR/runtime 改造。
- 优点：风险暴露最早。
- 缺点：前期进度观感较慢，架构推进节奏偏保守。

### 路线 B：runtime kernel 先行硬切

- 直接先改 planner/emitter/runtime，再倒推 loader/IR。
- 优点：最早看到 shared-generic 执行效果。
- 缺点：最容易在后期发现 contract/IR 要返工。

### 路线 C：Loader/IR 先行硬切

- 先统一 contracts、naming、loader instantiation graph、AOT IR，再接 planner/emitter/runtime。
- 优点：先把系统“如何理解 generic”统一，再切“如何执行 generic”，架构最稳。
- 缺点：中间态会持续一段时间，IR 先变、runtime 后变，必须用测试和 gates 把住边界。

## 4. 选型结论

选择路线 C，并进一步收敛为 `C-A`：

- contracts/naming 先收口
- loader 改成 instantiation graph
- AOT IR 先切到 shared-generic 语义
- matrix/gates 跟进
- planner/emitter/runtime 再统一接入

这是一个明确的 `roadmap` 级任务，不适合继续停留在单一 `Task<T>` family plan。

## 5. 成功标准

- 闭合 generic method/type 不再默认等价于“一整份独立 body”。
- loader 主路径不再依赖 `SubstituteText` / `SubstituteMethodBody` 风格的文本替换。
- AOT IR 能显式表达 `shared body + runtime generic context + thin stub`。
- planner/emitter/runtime 可以消费统一 generic IR，而不是持续扩张字符串前缀特判。
- `Task<T>`、generic dispatch、reflection、hotupdate generic boundary 都进入正式验证矩阵。

## 6. 非目标

- 本轮不承诺一次性收口全部 BCL generic family。
- 本轮不把动态代码生成、完全开放式 runtime generic synthesis 一起纳入。
- 本轮不接受“只把旧 special-case 重命名成 generic helper”作为完成口径。
