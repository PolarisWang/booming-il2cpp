# STATUS — IL2CPP Closure 精度重构

## 元信息

```yaml
task_type: roadmap
phase: completed
roadmap_or_plan: docs/dev/in-progress/closure-precision-roadmap/roadmap-v1-01.md
```

## 阶段完成状态

| Phase | Status | 结果 |
|-------|--------|------|
| D5 | ✅ 完成 | closure 49,862 → 352 (−99.3%), 构建 6min → 37s |
| D1 | ✅ 完成 | pipeline-config.yaml + assemblyDirs 配置体系 |
| D2 | ⏹️ 暂停 | 直接加载 runtime assembly 方案不可行（closure 校验失败） |
| Wiki | ✅ 已完成 | LCAC 架构文档已更新 + 已知限制 + 回退方案 |

## D2 结论

D2 的 "subject-rooted assembly loading" 方案经过验证不可行：将 subjects wrapper 调用的 runtime assembly 直接作为 `--assembly` 加载会导致 pipeline closure 校验失败（missing entry method）。D5 的默认方案（不加载额外 assembly，由 BRIDGE-AOT 处理跨程序集调用）在当时是正确的。

**注**: BridgeAOT 已被 [LCAC](wiki/03-功能模块/06-il2cpp核心架构/LCAC.md) 取代。LCAC 使用三层分层架构（Gold Direct Link + Hephaestus Lib + Demeter Table），彻底解决 BridgeAOT 的 IL 解码异常和增量编译问题。所有 chunks 的构建已验证通过。

完整 D2（Loader 级 Subject-Rooted Call-Graph Closure）需 2-4 周改造 Loader，但 ROI 低——不影响 fact 通过率。

## 最终结果

- AOT IR: 49,862 → **352**
- Fact 通过率: 112/121（9 个 Vector SIMD 失败独立于 closure）
- 构建时间: 6min → **37s**
- BRIDGE-AOT: 72 OK, 11 FAIL（无害）
