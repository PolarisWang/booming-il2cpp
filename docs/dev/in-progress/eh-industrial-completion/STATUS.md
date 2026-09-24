# eh-industrial-completion — STATUS

> **task_id**: eh-industrial-completion
> **task_type**: roadmap
> **phase**: P4 收口（P1/P2/P3/P4 主体已完成，见下）
> **更新日期**: 2026-09-24
> **entry_skill**: dev-brainstorm → dev-roadmap → executing-plans
> **roadmap_or_plan**: `roadmap-v1-01.md`
> **关键文档**: `roadmap-v1-01.md`、`KNOWN-ISSUES.md`、`P4-PLATFORM-REPORT.md`

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

---

## 架构映射

| 层 | 本轮变更 | 位置 |
|---|---|---|
| **L1 运行时投递** | 无变更（已成熟） | `src/native/runtime-core/exception_jmp.h` |
| **L2 codegen 包装** | ✅ 接入 `CHAOS_EH_*` 宏 | `NativeAotLoweringPlanner.MethodEmission.cs`、`LinearEmission.Advanced.cs`、`ExternalRuntimeHelpers.cs` |
| **L3 异常翻译** | ✅ 补验证网（类型匹配三模式套件） | `tests/unit/runtime-native/runtime-core/eh/eh_type_matching_test.cpp` |
| **L4 类型表** | 无变更（P2-0 修复了跨程序集重复 SubjectId） | `AotCoreIrLowering.cs` |

---

## 各 Phase 状态

| Phase | 内容 | 状态 | commit |
|---|---|---|---|
| **P1** | L2 包装层接入 `CHAOS_EH_*` 宏 | ✅ 完成 | `c7a33bcbf` |
| **P2-0** | 跨程序集重复 SubjectId 致 ToDictionary 抛异常 | ✅ 完成 | `5c823d47a` |
| **P2-L3** | 类型匹配三模式套件 + CI 门扩展 | ✅ 完成 | `55b0e2947` |
| **P3** | 三模式等价性套件 + CI 门 | ✅ 完成 | `e7198771f` / `cab384258` |
| **P4** | 铁律检查器 + pre-commit + 跨平台门 + 报告 | ✅ 主体完成 | `d01756e29` / `bd802a1ab` / `7af5b9416` |
| — | EH Expert + 11 条铁律 + roadmap | ✅ 完成 | `4fc1d8738` |

---

## 已验证结论（一手证据）

### P1：硬编码 EH 机制清零
产物 `native-aot.generated.cpp` 实测：
```
硬编码 `try {`                            58 → 0
硬编码 `catch (const chaos_managed_exception`  58 → 0
CHAOS_EH_TRY                              0 → 59
CHAOS_EH_CATCH_BEGIN / END                 0 → 57 / 57
```

### P3+P2：三模式与 L3 类型匹配
```
eh_mode_equivalence_{cpp_throw,win32_seh,setjmp} → PASS 12/12 ×3
eh_type_matching_{cpp_throw,win32_seh,setjmp}    → PASS  9/9 ×3
ctest 端到端：6/6 passed，5.08 秒
```
两套均经**负控验证会红**（破坏 filter / 反转匹配极性 → FAIL + exit 1）。

### P4：铁律静态检查
`build/check_eh_iron_rules.py` 接入 `.githooks/pre-commit`：
- 真实源码全量扫描 **0 违规**
- 负控 3 类历史形态（硬编码 try / catch / throw）**全部检出**
- 合法代码（宏 / 注释 / 日志）**不误报**

---

## 关键认知修正（避免后人重蹈）

| 起初判断 | 实测结论 |
|---|---|
| L3 异常翻译未实现 | **错** —— L3 六要素（throw/rethrow/match_type/endfilter/finally/MethodTable）全部存在，缺的是**验证网** |
| 需新建 `CHAOS_EH_*` 抽象头 | **错** —— `src/native/common/chaos/eh.h` 早已三模式完备，缺的是**接线** |
| 残片是「参数列表丢失」 | **错** —— 是**行中删除 78 字节**（公共前缀恰 93 字节） |
| 残片来自 `MethodDeclarations` | **错** —— 渲染前后该符号均完整 |
| 模板 CRLF 导致残片 | **已排除** —— 强制 LF 后缺陷形态不变 |

---

## 未完成项

1. **KNOWN-ISSUE-1**（`KNOWN-ISSUES.md`）：`native-aot.generated.cpp` 行中删除
   78 字节，阻断 `System.ObjectModel` / `System.Collections.NonGeneric`。
   已排除 10 个候选，含两次 Workflow 调查（7 agent）与 11+ 轮手动追查。
   下一步入手点已写在文档中（方法体区组装处的 `methodSections` + subject 过滤路径分叉）。

2. **Linux CI 首次实跑**：门已跨平台化，但本机无法实跑
   （无 WSL 发行版 / 无 docker / 无 Linux 工具链）。
   `enforce_gate` 保持 false，待 CI 收集基线后翻 true。

3. **iOS/Android**：SETJMP 路径已在 Windows 上实跑验证；iOS/Android 构建链
   不存在，**未验证**，不声称已验证。

---

## 下一步入口

- KNOWN-ISSUE-1 收口 → 按 `KNOWN-ISSUES.md` 「建议的下一步（修订版）」
- Linux CI 基线 → 合并后在 ubuntu runner 跑 `eh-mode-gate`
