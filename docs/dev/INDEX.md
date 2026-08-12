# docs/dev 开发阶段记录

按生命周期组织：完整（archive）→ 进行中（in-progress）→ 挂起（hanging）。

- [`designs/`](designs/) — 当前活跃设计方案（GC 分带、work plan 等）
- [`tests/`](tests/) — 测试架构文档
- [`assessments/`](assessments/) — 能力评估 / 限制说明（内存、multi-assembly roadmap、T4、gap-analysis、[**il2cpp 核心能力指南**](assessments/il2cpp-core-capability-guide.md)、[**JIT/寄存器分配续路**](assessments/jit-regalloc-a2-continuation-roadmap.md)）
- [`in-progress/`](in-progress/INDEX.md) — 当前进行中的阶段
- [`hanging/`](hanging/INDEX.md) — 挂起的阶段（等待恢复或废弃）
- [`scripts/`](scripts/) — 活跃验证 / 工具脚本
- 已完成的阶段已归档至 [`docs/archive/dev-completed/`](../archive/dev-completed/INDEX.md)（只读，保历史）

每个阶段目录包含 `STATUS.md` 和对应的设计/规划文档。
