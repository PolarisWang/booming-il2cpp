# Foundation DLL 新增验证接入流程

> 面向 `foundation-dll-audit` 主线的操作手册。用于新增一个 DLL 的 denominator、提升为正式 authority，并刷新 formal projection/docs/archive。

## 适用范围

- `System.Private.CoreLib + 13 DLL` 范围内，新增或重做某个 DLL 的验证接入
- 新增一个新的 foundation DLL 进入审计范围
- 重算某个 DLL 的 `capability families`

## 最小流程

### 1. 准备 authority 输入

至少确认以下输入已经就位：

- `testing/verification-catalog/catalog/programs/foundation-dll-translation-audit.program.json`
- `public-surface-manifest-v1-01.json`
- `semantic-family-plan-v1-01.json`
- `nativeization plan` / `lowering plan`
- 对应 DLL 的 task `STATUS.md`

### 2. 生成候选分母

```powershell
run foundation-dll derive --scope <assembly>
```

输出：

- `testing/verification-catalog/projections/foundation-dll-audit/capability-family-ledger.candidate.json`

此时产物是 `candidate-derived`，不能直接当正式分母 authority。

### 3. 审查 candidate

重点检查：

- family 粒度是否过粗或过细
- 是否出现 `new-candidate`
- 是否出现 `orphan-confirmed`
- 对于更复杂情况，是否出现 `merge-suspect` / `split-suspect`

### 4. 提升为正式 authority

```powershell
run foundation-dll promote --scope <assembly> --promote-approved true
```

输出：

- `testing/verification-catalog/projections/foundation-dll-audit/capability-family-ledger.json`

### 5. 刷新报告和 formal 数据

推荐直接使用一条总命令：

```powershell
run foundation-dll onboard --scope <assembly> --promote-approved true --mode full
```

它会串行执行：

1. `derive`
2. `promote`
3. `refresh`
4. `verify-consistency`
5. `verify verification-v1`

## 常用命令

### 单独派生 candidate

```powershell
run foundation-dll derive --scope System.Text.Json
```

### 单独提升正式 ledger

```powershell
run foundation-dll promote --scope System.Text.Json --promote-approved true
```

### 单独刷新 projection/docs

```powershell
run foundation-dll refresh
```

### 单独校验一致性

```powershell
run foundation-dll verify-consistency --mode full
```

### 一次性完整执行

```powershell
run foundation-dll full --scope all --promote-approved true --mode full
```

## 新增完全新的 DLL

如果 DLL 之前不在当前 14 个 scope 里，步骤是：

1. 先把 DLL 加进 `foundation-dll-translation-audit.program.json`
2. 确认 surface / semantic / nativeization 输入覆盖到它
3. 运行 `derive`
4. 审核 candidate
5. 运行 `promote`
6. 运行 `onboard`

## 规则提醒

- `Capability Closure` 才是 DLL 真实进度主轴
- `Workflow Progress` 只是辅助流程条
- `workspace` 只算 proof container，不是正式分母
- `promote` 必须显式带 `--promote-approved true`
- `run verify verification-v1 --json` 仍然是 formal refresh canonical 入口
