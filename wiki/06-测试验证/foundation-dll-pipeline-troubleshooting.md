# Foundation-DLL Pipeline 故障排除指南

## 按阶段的常见失败原因

### Stage 0: preflight (Contract 完整性检测)

**症状：** `StageResult.status == "failed"`，摘要包含 "Contract not found" 或 "No methodSubjectIds"

**常见原因：**
1. `contract.json` 不存在 — 检查 `testing/foundation-dll/<Assembly>/<slug>/contract.json`
2. `contract.json` JSON 格式错误 — 用 `python -m json.tool contract.json` 验证
3. `methodSubjectIds` 为空数组 — 至少需要 1 个 method subject
4. `methodSubjectIds` key 缺失 — schema 要求必须包含该字段

**修复：** 确保 contract.json 符合 `_contracts/family-schema.json`

### Stage 1: codegen (AOT 构建)

**症状：** codegen 阶段失败，摘要包含 "Build error" 或 "Codegen failed"

**常见原因：**
1. MSVC 编译错误（C++ 语法、模板实例化失败）
2. Linker 错误（LNK2019/LNK2001 未解析符号）
3. `chaos-il2cpp` 转换失败（C# 编译错误、闭包扫描异常）
4. CMake 配置错误（找不到 chaos SDK）

**调试：**
- 检查 `testing/results/foundation-dll/<Assembly>/<slug>/codegen/build.log`
- 手动进入 `native/` 目录执行 `cmake --build .` 查看详细输出
- 检查 `chaos-il2cpp` 输出中的 managed 编译错误

### Stage 2: jit_codegen (JIT 构建)

**症状：** JIT codegen 失败，摘要包含 "Build error"

**常见原因：** 同 Stage 1，但使用 JIT 模式 codegen

**注意：** JIT codegen 失败**不阻塞**后续 AOT 阶段。JIT 失败会在最终报告中标记为 failed。

### Stage 3-4: fact / fact_jit (事实验证)

**症状：** fact 失败，摘要显示 "Passed: N/M" 且 N < M，或 exit code != 0

**常见原因：**
1. 测试 assertion 失败 — 被测方法行为与预期不符
2. entry.exe 崩溃（SEH 异常、访问越界）
3. 超时（默认 120s）

**调试：**
```bash
# 直接运行 entry.exe 查看详细输出
./native/entry-aot.exe
./native/entry-jit.exe
```

### Stage 5: audit (审计)

**症状：** audit failed，摘要包含 "VIOLATION" 或压线警告

**常见原因：**
1. `p1_benchmark` VIOLATION — 方法未实现 benchmark，但 benchmark 是 required obligation
2. `p2_gc_slot_map` VIOLATION — 生成的 GC slot map 覆盖不足
3. 机制审计 stub 检测 — 方法解析为 stub

**覆盖规则：** 当 benchmark 或 JIT 模式被 skip 时，对应原则自动降级为 NOT_APPLICABLE

### Stage 6: asm_compare (指令对比)

**症状：** stage 失败，摘要包含 asm compare 错误

**常见原因：**
1. JIT 与 AOT 生成的指令序列差异超出预期
2. IR 扩展比异常（过高说明 codegen 效率问题）

### Stage 7: microbench (微基准)

**症状：** microbench 失败，exit code = 0xC0000005（访问越界）

**常见原因：**
1. GC 并发问题 — 已知在 GC 活动下可能出现 access violation
2. 该 exit code 有特殊处理逻辑，会自动记录但继续执行

### Stage 8: benchmark (性能对比)

**症状：** benchmark 失败或超时

**常见原因：**
1. 部分方法 benchmark 超时（默认 100000 次迭代）
2. JIT benchmark 需要 entry-jit.exe 不存在时自动跳过

### Stage 9-12: HotUpdate 阶段

**症状：** hotupdate 失败，摘要包含 "failedMethods > 0"

**常见原因：**
1. `entry.exe --hotupdate` 执行时 patch 未正确应用
2. AOT dispatch 与 interpreter 路径结果不一致
3. HotUpdate benchmark 部分方法失败

## 报告路径

| 报告 | 路径 |
|------|------|
| 统一验证报告 | `testing/results/foundation-dll/<Assembly>/<slug>/unified-verification-report.json` |
| 性能历史 | `testing/results/foundation-dll/<Assembly>/<slug>/perf/benchmark-history.jsonl` |
| 微基准历史 | `testing/results/foundation-dll/<Assembly>/<slug>/perf/microbench-history.jsonl` |
| AsmCompare 历史 | `testing/results/foundation-dll/<Assembly>/<slug>/perf/asmcompare-history.jsonl` |
| 构建日志 | `testing/results/foundation-dll/<Assembly>/<slug>/codegen/build.log` |
| 批量报告 | `testing/results/batch-report.json` |
| 回归基线 | `testing/results/verification-history/<Assembly>/<slug>/baseline-benchmark.json` |

## 调试命令

```bash
# 运行单个 family（标准模式）
python -m verification.entry_points.cli --slug convert-char --assembly System.Private.CoreLib

# 跳过特定阶段（快速调试）
python -m verification.entry_points.cli --slug convert-char --assembly System.Private.CoreLib \
  --skip benchmark hotupdate

# 严格模式
python -m verification.entry_points.cli --slug convert-char --assembly System.Private.CoreLib --mode strict

# 全量运行
python -m verification.entry_points.batch

# CI smoke（4 个 families, 6 阶段快速模式）
python -m verification.entry_points.ci_smoke

# 运行单元测试
pytest tests/ -v
```

## 已知问题

1. **windows-identity (System.Security.Principal.Windows)** — Codegen 生成 C2440 类型不匹配（abi_manifest 字段类型），需 codegen Scriban 模板同步
2. **guid-random-hashcode hotupdate AOT fact** — 返回 "returned 0 total"，使用错误 binary 入口
3. **空 contract families** — `--family/` 和 `Xfamily/` 等特殊 families 的 methodSubjectIds 为空，属故意设计
4. **GC 并发访问越界** — `--microbench` 在 GC 活动下的 exit code 0xC0000005 有特殊跳过逻辑
