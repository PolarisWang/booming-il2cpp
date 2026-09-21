# reflection-final-sprint — 开发计划（R-Final）

> **日期**: 2026-09-21
> **上游**: COMPLETENESS-ASSESSMENT-2026-09-21.md（本目录）
> **决策记录**: S1+S2 合并一个 worktree；S2 验收 = real ≥ 300/440；worktree+workflow 模式；按阶段性目标交付
> **brainstorm**: 2026-09-21 用户确认（轻量级，父评估已清零边界问题）

## 0. 目标与验收总口径

**目标**：解决评估报告 P0-P5 全部缺口，达到生产级标准：
- 运行时 `real ≥ 300/440`，`stubGap ≤ 50`（S2 验收）
- AOT/JIT 判定逐条一致（每阶段验证）
- 三档判定制维持：`real` / `not-supported`（显式抛）/ `not-applicable`（论证）
- REF-RISK-1/2 已闭（保持），RISK-7 闭合（S4）
- 全部诊断 fprintf 清除后 push

## 1. 阶段计划

### S1 receiver 供给复用 + 普查（P0 前半）
- **worktree**: `reflection-final`（从 main 拉起，含 T7 keystone 提交）
- **workflow**: 3-4 个并行调查员普查 390 stubGap 的 receiver 构成
  （Create<T> 泛型体 / GetUninitializedObject 零化 / typeof 折叠 / ATG fixture），输出 file:line + 修法
- **实施**: T7 四段修复链移植（stable_id 特判 / 双形态解析 / no-op stub / 身份敏感共享禁用）
- **交付物**: ① receiver 构成普查报告 ② 共享禁用+特判提交 ③ stubGap 初步下降实测
- **合并条件**: 编译绿 + chunk 全量管线无回归 + 交付物入库

### S2 真实断言接入（P0 后半，同一 worktree）
- **实施**: ATG fixture 真实实例表扩展（复用 c61ba23d6 的 19 类型真实实例工厂模式）+ 逐族激活断言
- **workflow**: 逐族并行接入（MemberInfo/PropertyInfo/FieldInfo/MethodInfo/Assembly/Module/TypeInfo 族）
- **交付物**: **real ≥ 300/440，stubGap ≤ 50**（S2 验收硬指标）+ 逐族提交记录
- **合并条件**: fact 分布达标 + AOT/JIT 一致 + push

### S3 typeof coerce（P1，新 worktree `reflection-typeof`）
- **实施**: 反射查询入口双形态解析推广（T7 GetUninitializedObject 模式：raw TypeInfoHot* / 托管 Type 对象 +16）
- **边界对齐**: 与 generic-instantiation-materialization L2 设计交汇处开一次架构对齐（短会）
- **交付物**: C 组 8 项（typeof 折叠 NRE）翻绿 + typeof 链闭合文档
- **合并条件**: C 组绿 + 全量无回归

### S4 RISK-7 codegen 接线（P2，新 worktree `reflection-risk7`）
- **实施**: dev-architecture-first-development 先行（三次盲试已禁）→ 定位 push/pop 发射点 → 实现
- **交付物**: `tls_executing_image` 非 null 实测 + push/pop 对打印
- **合并条件**: 反射 chunk + ReferenceProof 管线双通过

### S5 收尾三件（P3+P4+P5，收尾 worktree）
- 字段偏移：GcTypeLayout offsetof 表扩展语义到普通字段（设计 + 实现）
- not-supported 111 项：ReferenceProof 抽样验证显式抛
- GetPublicKey/GetSetMethod/标记指针 3 项合并修
- **交付物**: 全部缺口清零或转 needs-design（有论证）+ 诊断 fprintf 全部清除
- **合并条件**: 全量管线 + push

## 2. worktree/workflow 使用规范

| 阶段 | worktree | workflow |
|---|---|---|
| S1+S2 | `reflection-final` 一个 | S1 普查 3-4 员；S2 逐族 3-4 员 |
| S3 | `reflection-typeof` | 不需要（单点架构修复） |
| S4 | `reflection-risk7` | 不需要（架构查询用 dev-architecture-first-development） |
| S5 | `reflection-finalize` | 抽样验证可并行 2 员 |

worktree 纪律（沿用 T7 实证）：SDK --force 重建；捆绑 DLL/模板三副本核对（UTF-16 字节验证）；heredoc 反斜杠折叠 → tmp_*.py 脚本文件；每阶段合并后删 worktree。

## 3. 里程碑与总量

| 里程碑 | 预计会话 | 累计 |
|---|---|---|
| M1: S1+S2 完成（real ≥ 300） | 1.5-2 | 1.5-2 |
| M2: S3 typeof 闭合 | 0.5-1 | 2-3 |
| M3: S4 RISK-7 闭合 | 1 | 3-4 |
| M4: S5 收尾 + 反射生产级达标 | 0.5 | **3.5-4.5** |

## 4. 风险

| 风险 | 缓解 |
|---|---|
| 并行会话复活撞车 | 每阶段开工前查 .classified + mtime |
| T7 keystone 提交未 push（本地 main） | S1 worktree 从本地 main 拉起 ✓ |
| stubGap 中部分判 not-supported 更诚实 | 三档判定制兜底，不强凑 300 |
| ATG fixture 重写波及 | 逐族提交，每族独立可回退 |
