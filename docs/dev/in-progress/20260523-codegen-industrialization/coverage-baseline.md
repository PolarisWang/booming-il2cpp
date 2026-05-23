# Codegen Coverage Baseline (C-P2-3 / V2 Coverage Gate)

> 创建日期: 2026-05-23
> 覆盖范围: Chaos.IL2CPP.Generator 托管 C# codegen
> 排除项: ReferenceProof, PatchData, Scriban

---

## 当前覆盖率基线

| 类别 | 覆盖率 | 覆盖行/总行数 |
|------|--------|--------------|
| **总体** | **95.4%** | 1606/1684 |
| Pluginization | 100.0% | 172/172 |
| Root (未分类) | 94.8% | 1434/1512 |

> 注: `final_coverage.py` 脚本仅将文件归类到 Emission/Planning/RuntimeSupport/Pluginization/BuildSystem/Validation/Lowering/Templating 等子目录。未被上述目录匹配的文件归入 Root。更细粒度覆盖率分析见 CoverageResults/ 下的完整 cobertura XML。

## CI 集成

- 覆盖收集通过 `coverlet.runsettings` + CI workflow 的 Coverage 步骤完成
- 产物上传为 `codegen-coverage-report` artifact
- 使用 `final_coverage.py` 脚本解析 Cobertura XML

## 门禁策略

- 当前为信息性收集阶段，不设硬性阈值
- Phase 2 完成时目标: ≥ 90% 总体覆盖率
- Phase 3 完成时目标: ≥ 95% 总体覆盖率
- 回归检测: PR 覆盖率不可低于当前基线超过 2 个百分点

## 运行方式

```bash
# 本地收集
dotnet test tests/managed/Chaos.IL2CPP.CodeGen.Tests/Chaos.IL2CPP.CodeGen.Tests.csproj -c Release --settings tests/managed/Chaos.IL2CPP.CodeGen.Tests/coverlet.runsettings --collect "XPlat Code Coverage"

# 解析结果
python tests/managed/Chaos.IL2CPP.CodeGen.Tests/final_coverage.py tests/managed/Chaos.IL2CPP.CodeGen.Tests/CoverageResults/coverage.cobertura.xml
```
