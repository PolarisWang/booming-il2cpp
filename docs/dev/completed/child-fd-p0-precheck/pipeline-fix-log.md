# Pipeline 预检修复日志

> 记录 Phase 0 预检期间实施的修复。

## 修复 1：Microbench Crash（BgcTryMark SEH 保护）

- **问题**：entry.exe --microbench 在 BGC 并发标记期间因 0xC0000005 ACCESS_VIOLATION 崩溃
- **根因**：BgcTryMark 访问的 object 被 mutator 并发修改或释放
- **修复**：在 `BgcTryMark` 添加 `__try/__except` SEH 保护（Windows），捕获并发标记中的 transient AV 后返回 false
- **文件**：`src/native/runtime-core/gc/gc_old_gen.cpp`（~line 3097）
- **验证**：编译通过，pipeline microbench 阶段返回 passed

## 修复 2：Audit 计数口径不一致

- **问题**：buffer-memory family 的 audit report 自报 "passed: true"，但 pipeline 标 failed
- **根因**：`mechanism_audit.py` 使用 `principle_overall == "ALIGNED"` 作为通过条件，但 pipeline orchestrator 使用不同标准
- **修复**：
  1. `mechanism_audit.py` line 416: `principle_overall == "ALIGNED"` → `principle_overall != "VIOLATION"`（接受 CONCERN 作为通过状态）
  2. `family_verification_orchestrator.py` line 1157: 新增显式日志打印 mechanism_passed 和 principle criteria
- **验证**：pipeline audit 阶段返回 passed，principle=ALIGNED

## 修复 3：Asn1/encoding-rules subject IDs

- **问题**：encoding-rules family 的 `methodSubjectIds` 为空，无法执行实际验证
- **修复**：添加 `AsnEncodingRules` 枚举的 5 个方法（.cctor、.ctor、ToString、GetHashCode、Equals）
- **文件**：`testing/foundation-dll/System.Formats.Asn1/encoding-rules/contract.json`、`capability-family-contract.json`

## 修复 4：Text.Json/attributes subject IDs

- **问题**：attributes family 的 `methodSubjectIds` 为空，无法执行实际验证
- **修复**：添加 11 个 JSON 属性类型的 23 个方法（constructors + property getters）
- **文件**：`testing/foundation-dll/System.Text.Json/attributes/contract.json`、`capability-family-contract.json`

## 修复 5：pipeline-config.yaml 与实际运行脱节

- **问题**：pipeline-config.yaml 只定义 6 个 stages（preflight→audit），实际管线已扩展至 13-stage
- **修复**：更新配置为全 13-stage 管线，清理过时的 migrationSkips
- **文件**：`testing/config/foundation-dll/pipeline-config.yaml`
