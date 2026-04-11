# Interpreter IR Decision v0

## 决策

Phase 0 选择：

- `IL -> Internal IR -> Interpreter Execute`

而不是：

- 直接解释原始 IL
- 直接把现有 `TypedIlIrArtifact` 当作执行格式

## 原因

- 原始 IL opcode 面过大，直接解释会把 Phase 7 复杂度推高。
- 现有 `TypedIlIrArtifact` 更偏 artifact / 序列化表示，不是面向执行的 VM IR。
- 单独的 interpreter IR 可以与 shared identity 对齐，同时保留更紧凑的执行格式。

## 与 TypedIL 的关系

- 二者共享同一套 `SubjectId` / token / metadata lookup 规则。
- TypedIL 继续承担分析与 artifact 边界角色。
- interpreter IR 只承担运行时执行角色。

## v0 最小 IR 类别

- 常量：`ldc`, `ldnull`, `ldstr`
- 参数 / 局部：`ldarg`, `starg`, `ldloc`, `stloc`
- 字段：`ldfld`, `stfld`, `ldsfld`, `stsfld`
- 调用：`call`, `callvirt`, `call_bridge`
- 分支：`br`, `brtrue`, `brfalse`, `beq`, `blt`
- 对象：`newobj`, `box`, `unbox`, `castclass`, `isinst`
- 数组：`newarr`, `ldelem`, `stelem`, `ldlen`
- 异常：`throw`, `rethrow`, `leave`, `endfinally`
- 返回：`ret`

## 范围边界

- v0 只冻结决策和最小指令族，不冻结完整 opcode 枚举编号。
- 具体 VM、basic block、EH stack 细节留给 Phase 7 实现。
