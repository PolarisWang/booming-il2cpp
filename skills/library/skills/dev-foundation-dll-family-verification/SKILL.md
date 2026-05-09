---
name: dev-foundation-dll-family-verification
description: Per-family verification standards — principle alignment, mechanism audit rules, skip governance, handwrite integration. NOT an execution guide; execution is done by verify_family() orchestrator.
---

# Foundation DLL Family Verification Standards

## 概述

定义单 family 验证的标准和约束。**本技能不包含执行步骤**——执行由 `verify_family()` 7 阶段管线统一完成（`family_verification_orchestrator.py`）。本技能仅定义 AI 在做 codegen 和验证时必须遵守的原则、规则和质量标准。

## 核心原则

**项目全局优先级**（源自 CLAUDE.md，本技能所有验证门必须与之对齐）：

```
P1 — 性能最优（第一优先级）：生成的 C++ 代码运行时性能为第一考量
P2 — 方案完美性（第二优先级）：架构完整性、正确性、可维护性
P3 — 热更适配（第三优先级）：最大化支持热更新
```

**裁决规则**：低优先级与高优先级冲突时，低优先级让位于高优先级。但高优先级方案不应故意破坏低优先级。

**本技能的原则对齐要求**：每个 codegen/验证结果必须附带原则对齐评估。测试通过但违反原则 == 不通过。

1. **No-Skip Policy** — 测试用例除以下情况外不允许 skip：
   - Unity IL2CPP 本身也不支持该行为（需提供证据）
   - 有已批准的修复计划 + ticket 跟踪
   - 在 skip registry 中登记 + 定期审查
2. **完备机制优先** — 每个问题必须用完备的 codegen/runtime 机制解决，不允许通过特例（hardcoded override、方法级豁免、`if(family==X)` 分支）让测试通过。
3. **Handwrite 注入** — AI review 过程中允许注入 handwritten entrypoint / test 用例，但必须与 auto-generated 用例使用同一验证管道，不可绕过 assert 机制。
4. **Mechanism Audit** — 每个 "passed" 结果必须经过机制审计：确认通过原因是真实 C++ lowering（`chaos_eval_stack`），而非桩代码或 stub 的偶然正确。

## 原则评估矩阵（自动化执行）

每个 codegen/验证结果必须附带原则对齐评估。以下 7 项检查由 `principle_auto_checks.py` **全自动执行**。

> 另有 4 项不可自动化的检查（p2_layer_correctness、p2_no_code_duplication、p2_abstraction_balance、p3_no_hardcoded_state）降级为 dashboard 趋势监控，不参与门禁阻塞。

### P1 — 性能最优评估（4 项自动化检查）

```yaml
principle: 性能最优
priority: 1
checks:
  - id: p1_lowering                                   # 自动: 检查 chaos_eval_stack
    pass_condition: "generated C++ 包含 chaos_eval_stack，无 SimpleForward"
    automation: "principle_auto_checks.py check_p1_lowering()"

  - id: p1_no_redundant_branch                         # 自动: 检查 if(family==X) / switch(methodIndex)
    pass_condition: "生成代码中无运行时 family/method 条件分支"
    automation: "principle_auto_checks.py check_p1_no_redundant_branch()"

  - id: p1_no_unnecessary_indirection                  # 自动: 检查 runtime_stubs 调用
    pass_condition: "生成代码不经过 runtime_stubs.cpp 的已知桩函数"
    automation: "principle_auto_checks.py check_p1_no_unnecessary_indirection()"

  - id: p1_benchmark                                   # 自动: benchmark_comparator.py 输出
    pass_condition: "avg speedup > -50%，无 managed_faster 退化"
    automation: "principle_auto_checks.py check_p1_benchmark()"
```

### P2 — 方案完美性评估（1 项自动化检查 + 3 项监控）

```yaml
principle: 方案完美性
priority: 2
checks:
  - id: p2_template_hygiene                           # 自动: 扫描 Scriban 模板
    pass_condition: "Scriban 模板不含语义 if/else 决策"
    automation: "principle_auto_checks.py check_p2_template_hygiene()"

  # 以下 3 项不可自动阻塞，仅 dashboard 监控趋势：
  - id: p2_layer_correctness                          # 监控: 文件路径分布
    monitoring: "记录变更文件所属目录与预期架构层的匹配度"
  - id: p2_no_code_duplication                        # 监控: 跨 family 相似度
    monitoring: "跨 family 生成代码 fingerprint 相似度分析"
  - id: p2_abstraction_balance                        # 监控: 调用链深度
    monitoring: "函数调用链深度与同类 family 中位数偏差"
```

### P3 — 热更适配评估（2 项自动化检查 + 1 项监控）

```yaml
principle: 热更适配
priority: 3
checks:
  - id: p3_patch_entry                                # 自动: dispatch_table interrupt_ptr
    pass_condition: "dispatch_table 包含该方法的 interrupt_ptr 条目"
    automation: "principle_auto_checks.py check_p3_patch_entry()"

  - id: p3_patchdata_generation                       # 自动: .patchdata 文件存在
    pass_condition: ".patchdata 文件存在且非空"
    automation: "principle_auto_checks.py check_p3_patchdata()"

  - id: p3_no_hardcoded_state                         # 监控: 静态变量声明扫描
    monitoring: "扫描 global/static 变量声明，黑名单模式匹配"
```

### 原则冲突裁决

当 P1/P2/P3 之间出现冲突时，按以下规则裁决：

| 冲突类型 | 裁决规则 | 示例 |
|---------|---------|------|
| P1 vs P2 | 性能优先，但必须记录架构妥协 | SimpleForward 比真实 lowering 快但架构上不正确 → 选真实 lowering 并优化性能 |
| P1 vs P3 | 性能优先，但不应故意破坏热更 | 内联函数调用提升性能但绕过 dispatch 表 → 用 dispatch 表 + 内联优化 |
| P2 vs P3 | P2 优先于 P3 | 为热更引入架构 hack → 不应接受，需找干净的方案 |

### 原则对齐输出格式

```json
{
  "principleAlignment": {
    "performance": {
      "status": "ALIGNED" | "CONCERN" | "VIOLATION",
      "evidence": {
        "p1_lowering": "chaos_eval_stack present ✓",
        "p1_no_redundant_branch": "no runtime dispatch branches ✓",
        "p1_no_unnecessary_indirection": "direct lowering ✓"
      },
      "concerns": []
    },
    "architecturalPerfection": {
      "status": "ALIGNED",
      "evidence": {
        "p2_layer_correctness": "fix in emitter layer, consistent with arch table ✓",
        "p2_no_code_duplication": "no duplicated patterns ✓"
      },
      "concerns": []
    },
    "hotupdateCompatibility": {
      "status": "NOT_APPLICABLE",
      "evidence": {},
      "concerns": ["family has no hotupdate path"]
    }
  },
  "overallAlignment": "ALIGNED" | "CONCERN" | "VIOLATION"
}
```

## Skip 治理

### 禁止的 skip 类型

| 类型 | 示例 | 替代方案 |
|------|------|---------|
| out/ref 参数导致跳过 | `DateTime.TryParse` | 修复 `test_code_generator.py` 的 ref 参数解析，自动生成临时变量 |
| 泛型 T 无法解析 | `RuntimeHelpers.GetSubArray` | 实现泛型参数推导，默认用 `object` 或 `int` |
| Parse 方法跳过 | `Int32.Parse`, `Guid.Parse` | 使用有效输入字符串，无需 skip |
| 集合方法跳过 | `List.Remove`, `Dictionary.ContainsKey` | 先用 Add 填充集合再调用 |
| 内部 intrinsic | `Buffer.Memmove` | 在 codegen 中标记为 intrinsic 并生成 direct call |
| 线程操作 | `Thread.Start`, `Monitor.Enter` | 使用轻量同步替代，不依赖真正多线程 |

### 允许的例外

所有例外必须在 skip registry 中登记，包含：
- 方法 ID
- skip 原因（必须引用 Unity IL2CPP 也不支持的证据）
- 关联 issue/修复计划链接
- 下次审查日期

```json
{
  "methodSubjectId": "System.Private.CoreLib/System.Buffer::Memmove(...)",
  "skipReason": "internal runtime intrinsic, not in .NET 8 public API",
  "unityReference": "Unity IL2CPP also does not expose Buffer.Memmove as public API",
  "eliminationPlan": "Phase 4 codegen intrinsic support — track in issue #xxx",
  "reviewDate": "2026-08-01"
}
```

## Mechanism Audit 规则

对每个声称 `"passed"` 的 native-proof claim，审计规则：

```python
AUDIT_RULES = {
    "not_a_stub": "生成的 C++ 代码不能是桩（无 NativeReferenceStub_ 模式）",
    "has_real_lowering": "包含 chaos_eval_stack 真实 IL 降级",
    "no_skip_override": "方法不在 _METHOD_OVERRIDES skip 列表中",
    "assert_triggered": "assert intrinsic 至少执行了一次（证明有实际比较）",
    "stub_detector_clean": "stub_detector 报告 is_stub=False",
}
```

审计由 `mechanism_audit.py:audit_family()` 自动执行：
1. **Stub detection** — `stub_detector.scan_family()` 扫描 C++ 代码
2. **加载生成 C++** — 优先命名空间路径，回退扁平路径
3. **解析 _METHOD_OVERRIDES** — 从 `test_code_generator.py` AST 提取 skip 条目
4. **Per-method 审计** — 标记 false-passing、missing lowering、missing assert

### 审计输出

#### 单方法审计记录

```json
{
  "family": "convert-char",
  "method": "System.Convert::ToChar(int)",
  "claimed": "passed",
  "auditResult": "PASS",
  "evidence": {
    "has_real_lowering": true,
    "stub_detector": {"is_stub": false, "pattern_matched": null},
    "no_skip_override": true,
    "assert_count": 1
  }
}
```

#### Family 级汇总

```json
{
  "family": "convert-char",
  "total_methods": 18,
  "audited": 18,
  "false_passing": 0,
  "stubs_found": 0,
  "skips_found": 0,
  "methods_without_lowering": 0,
  "passed": true
}
```

### 审计失败处理

如果审计发现 false-passing（声称通过但实际是 stub/特例）：
1. 该 family 标记为 `audit:FAILED`
2. 定位根因：stub_detector 误判？skip 隐藏未实现？assert 未触发？
3. 修复后重新运行完整验证

## Handwrite 测试集成

### 触发条件

AI review 过程中发现以下情况时，应注入 handwritten 测试：
1. Auto-generated entrypoint 因方法签名复杂（ref/pointer/generic）生成无效 C#
2. 方法的行为无法通过简单的 checksum 验证（例如返回 void 但需要验证副作用）
3. 需要验证边界条件（空参数、null 输入、越界索引）
4. Auto-generated 用例无法覆盖的 codegen 路径

### 规范

1. Handwrite entrypoint 放在 `handwritten/<Type>.Handwritten.cs`
2. 必须继承同一入口点接口，与 auto-generated 使用相同发现机制
3. 必须参与 L2 checksum 验证，不可绕过 assert 机制
4. 必须在验证报告中标记为 `source: handwritten`

### 验证管道

```
handwritten entrypoint 与 auto-generated entrypoint 使用同一管道：
  ✓ chaos-il2cpp convert-to-cpp
  ✓ CMake build entry.exe
  ✓ entry.exe 运行验证

禁止的绕过方式：
  ✗ 跳过 convert-to-cpp 直接提供手写 C++
  ✗ 自定义 assert 逻辑绕过 Mechanism Audit
  ✗ 只跑 managed 测试不跑 native 验证
```

## 关联

- 上游执行: `dev-foundation-dll-verification-pipeline` — 实际调用 verify_family() 7 阶段管线
- 审计代码: `mechanism_audit.py` — audit 逻辑实现
- 原则检查: `principle_auto_checks.py` — 7 项自动化原则检查
- Skip Registry: `wiki/06-测试验证/foundation-dll-skip-registry.md`
- Handwrite 规范: `wiki/06-测试验证/handwrite-test-guidelines.md`