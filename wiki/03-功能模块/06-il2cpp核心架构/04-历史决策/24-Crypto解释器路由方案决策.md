# 24 — Crypto 解释器路由方案决策

> 来源：`MEMORY.md > crypto-interpreter-routing.md`、`interpreter-routing-design.md`

## 决策

360 个 crypto 方法通过 **IL 嵌入 + 运行时 Interpreter 路由** 执行，不作为 AOT 编译目标。

## 背景

System.Security.Cryptography.dll 中的 360 个方法在 kChaosExternalRuntimeFnTable 调度时执行 ChaosExternalRuntimeFallback，返回 0 导致调用方空指针解引用 (value=-1)。尝试将 crypto DLL 加入 codegen 的 assemblyDir 导致 closure 膨胀 1000+ 方法、SIMD stub 重载冲突、大量 codegen 修复需求。

## 方案对比

| 方案 | 结论 |
|------|------|
| A: Crypto DLL 作为附加程序集 | 失败 — closure 膨胀 + codegen 不兼容 |
| B: Codegen 内联 IL→AOT IR 转换 | **推荐** — 优先尝试 |
| C: 运行时加载 Crypto DLL | 不可行 — 需要的 PE 解析器/IL 解码器不存在 |

## 推荐方案 (B) 详细设计

- Codegen 检测 crypto 方法时，从 loadedWorld.Methods 获取 IL bytecodes
- 优先尝试通过 AotCoreIrLowering 生成 AOT IR JSON 嵌入 C++ 代码
- 失败时降级到原始 IL 嵌入 + 运行时微 IL→IR 转换器
- 运行时 ChaosExternalRuntimeFallback 在表中查找 subjectId，通过 InterpreterVM::Execute 执行

## 实施计划

- Phase 1: 基础设施 (codegen IL 收集 + C++ 数据表发射) — 3-4 天
- Phase 2: 运行时 IL→IR 转换器 — 5-7 天
- Phase 3: 集成与验证 — 3-4 天

## TRIGGER

涉及 crypto 方法执行路径、ExternalRuntimeFallback 行为、或 interpreter routing 选择时。
