# Managed C# Scriban 模板审查规范 (M-E)

> 日期：2026-09-02
> 域：codegen(4) / Scriban 模板
> 状态：审计基线完成, 规范生效
> 生成器：NativeAotLoweringPlanner (Generator) 使用 Scriban 渲染 C++ 输出

---

## 1. 审计范围

`src/managed/Chaos.IL2CPP.Generator/Templates/*.scriban` (160 个):
- 45 个核心 codegen 模板 (`NativeAot.*.cpp/h`) — ABI/hot-path 关键
- 115 个 NativeReferenceProof.* 模板 — verification/reference 框架

## 2. 审计基线 (2026-09-02)

### 通过项 (无问题)
- 无 AI 贷款 stub / `return 0 TEMP` / 未完成占位符残留
- 核心模板无中文/非 ASCII 硬编码 (grep [^x00-x7F] 空)
- 无硬编码 magic 字符串注入 (注释中的 "hardcodes" 是说明文字)
- snapshot 测试 108/108 全绿 → codegen 输出正确性由快照守护

### 已知 gap (记录, 非本次引入)
- `NativeAot.DispatchEntryCode.cpp.scriban` L41: `kSubjectContractMap` 无下游
  C++ 消费者 (fact-runner JSON hardcode `contractIndex:-1`, 自 2026-06-18)。
  数组保留供未来 slot→Subject_N 映射。低风险 (无崩溃路径)。

## 3. 审查规范 (新增 codegen 模板必查)

| # | 检查 | 否决条件 |
|---|------|---------|
| 1 | **无 stub 残留** | 禁止 `return 0; // TEMP` / `placeholder` / 未实现分支 fallthrough |
| 2 | **无中文硬编码** | 输出 C++ 必须 ASCII (注释可英文) |
| 3 | **ABI 宏一致性** | 函数指针/导出必须用 `CHAOS_RUNTIME_ABI_CALL/EXPORT`, 类型必须 `CHAOS_IL2CPP_*` |
| 4 | **内存宏** | 分配走 `CHAOS_IL2CPP_NEW/ALLOC`, 禁裸 new/malloc |
| 5 | **契约同步** | 改动 ABI 布局/签名必须同步 snapshot baseline (SNAPSHOT_UPDATE) |
| 6 | **无跨层越界** | 模板不得写回 managed/codegen 层文件 (仅输出 C++) |

## 4. 执行入口 (新增/修改模板后的验证)

```bash
# 改动模板后, 用 SNAPSHOT_UPDATE=1 更新 codegen snapshot 基线并审核 diff:
dotnet test tests/unit/managed/snapshot/ --configuration Release \
  --settings tests/unit/managed/snapshot/snapshot-update.runsettings
# 审核 git diff tests/unit/managed/snapshot/Baselines/ 确认输出正确稳定

# 验证 (非更新模式) snapshot 108 全绿 + driver 29 全绿:
python tests/runner/test_driver.py --layer unit   # codegen+driver+snapshot 应 OVERALL OK
```

## 5. 结论

Scriban 模板基础设施**健康** — 无 stub/硬编码/编码残留, snapshot 守护输出正确性。
规范已固化 (第 3 节检查清单) 供后续 codegen 修改遵守。
已知 gap (DisptachEntryCode contractIndex 无消费者) 已记录, 交由 codegen 域判断,
不构成工业化阻断。
