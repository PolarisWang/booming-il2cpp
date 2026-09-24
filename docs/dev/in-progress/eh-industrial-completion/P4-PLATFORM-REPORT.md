# EH 工业化 P4 — 平台验证报告

> **task_id**: eh-industrial-completion
> **phase**: P4
> **创建日期**: 2026-09-24

---

## 一、交付摘要

| P4 exit_criteria | 状态 | 证据 |
|---|---|---|
| ① Windows/Linux 实跑通过 | **部分** | Windows 6/6 ✅；Linux 门已就绪但**未在 CI 实跑**（本机无 Linux/无 WSL/无 docker，诚实标注） |
| ② iOS/Android 设计验证 | 部分 | 设计验证文档见下；未实跑（无构建链） |
| ③ EH 铁律进入 skill + pre-commit 门 | ✅ | `dev-il2cpp-eh-expert` SKILL + `build/check_eh_iron_rules.py` 接入 `.githooks/pre-commit` |
| ④ 全 chunk EH 逃逸指标下降 | 部分 | P2-0 后 `System.Linq` 转通过；`System.ObjectModel`/`System.Collections.NonGeneric` 被 **KNOWN-ISSUE-1** 阻断（见该文档） |

## 二、已验证（本机 Windows）

### 三模式等价性套件（P3）
```
eh_mode_equivalence_cpp_throw → PASS 12/12 (mode=CPP_THROW)
eh_mode_equivalence_win32_seh → PASS 12/12 (mode=WIN32_SEH)
eh_mode_equivalence_setjmp    → PASS 12/12 (mode=SETJMP)
```

### L3 类型匹配套件（P2）
```
eh_type_matching_cpp_throw → PASS 9/9 (mode=CPP_THROW)
eh_type_matching_win32_seh → PASS 9/9 (mode=WIN32_SEH)
eh_type_matching_setjmp    → PASS 9/9 (mode=SETJMP)
```

ctest 端到端：**6/6 passed，5.08 秒**。

### 铁律静态检查（pre-commit）
- RULE-1：真实源码 `src/managed/Chaos.IL2CPP.Generator` 全量扫描 **0 违规**
- RULE-1 负控：3 类历史形态（硬编码 try/catch/throw）**全部检出**
- RULE-3：advisory，权威在 `tools/scan_raised_exception_types.py --check`

## 三、未实跑的部分（诚实标注）

### Linux (CPP_THROW + SETJMP)
- 门已跨平台化（`eh-mode-gate.yml` 支持 `os: ubuntu-latest`，Linux 期望 4 target）
- **未实跑**：本机无 WSL 发行版、无 docker、无 Linux 工具链
- 验收口径：**Linux 首次实跑确认前，`enforce_gate` 保持 false**

### iOS/Android (SETJMP)
- SETJMP 路径已在本机 Windows 上实跑验证（`eh_type_matching_setjmp` / `eh_mode_equivalence_setjmp` 均绿）
- **设计验证**：SETJMP 路径的可移植性由 setjmp/longjmp 标准库保证；`config.h` 的平台路由
  （iOS/Android → `CHAOS_IL2CPP_EH_SETJMP`）已代码级核对
- **未实跑**：无 iOS/Android 构建链，不声称已验证

## 四、关于 KNOWN-ISSUE-1 对 P4-④ 的影响

P4 的「全 chunk EH 逃逸指标下降」被 KNOWN-ISSUE-1 阻断：
- `System.Linq` / `System.Text.Json`：P2-0 修复后通过
- `System.ObjectModel` / `System.Collections.NonGeneric`：codegen 产物出现
  行中删除 78 字节的残缺声明（C2182/C2144），与 EH 无关但阻断这两个 chunk
- 详见 `KNOWN-ISSUES.md`

## 五、下一步

1. **CI 首次实跑**：合并本报告后，在目标平台（ubuntu-latest）跑一次 `eh-mode-gate`
   收集真实基线，确认 4/4 后把 `enforce_gate` 翻 true
2. **KNOWN-ISSUE-1 收口**：按 `KNOWN-ISSUES.md` 的修订下一步，从方法体区组装
   处入手
3. **P4 结项**：前两项完成后更新本报告并归档
