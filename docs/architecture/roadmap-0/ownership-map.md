# Ownership Map (Roadmap 0)

## Track Ownership

- Frontend track: `frontend/`，负责输入接入与前端集成边界。
- Analysis track: `analysis/`，负责语义分析能力与分析产物。
- Runtime track: `runtime/`，负责运行时执行语义与桥接实现。
- Codegen track: `codegen/`，负责生成策略落地与输出结构。
- Build/Release track: `build/`，负责跨平台编译、链接、打包门禁。
- Verification track: `tests/`，负责跨平台验证与 smoke 回归。
- Contract track: `contracts/`，负责 schema/native/trace 约束冻结。
- Dependency stewardship: `third_party/`，负责 vendored 快照登记与审阅。
- Artifact stewardship: `artifacts/`，负责运行输出集中收口与清理策略。

## Future Workstream Guidance

- 每条工作轨必须在其所有目录内完成实现与维护，不跨目录引入未声明职责。
- 跨轨改动需要同时更新 `contracts/` 或架构文档中的边界定义。
- 若职责变更，先更新本文件与 `repo-layout.md` 再推进实现。

