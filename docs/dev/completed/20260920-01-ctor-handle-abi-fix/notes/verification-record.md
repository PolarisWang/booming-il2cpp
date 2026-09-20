# 验证记录 — ctor 句柄 ABI 修复

## 1. 基线（干净树, 2026-09-20 11:39）

fact.json: passed=570 failed=100 realTotal=235 realVerified=135 stubGap=185 unassertable=155 preAssertionRaise=164 assertionFailed=12 gatePassed=385 total=746

## 2. 修复后（2026-09-20 12:51）

passed=590 failed=96 realTotal=232 realVerified=136 stubGap=185 unassertable=173 preAssertionRaise=145 assertionFailed=11 gatePassed=405 total=746

## 3. 逐 subject 迁移（cmp.py）

- 改善 20：realDefect/失败 → unassertable(169) + real(1) + smoke(1)
- 回退 0（最终态）
- only-baseline=0, only-after=0（subject 集合完整）

## 4. 目标 subject（4 个 WriteEnd*）

修复前：均 realDefect(pass=False, caught=True)
修复后：均 unassertable(pass=True, caught=False) —— 不再把「正常执行/静默」错记为 caught

## 5. 产物逐字节 diff（门槛，gen-baseline vs gen-after）

差异文件：native-aot.generated.cpp + page-0014/0015/0016.cpp
实质差异行：全部为 `ctor() → const CHAOS_IL2CPP_INTPTR chaos_handle = ctor(); _s1 = chaos_handle`。
涉及符号：仅 XmlTextReader/XmlTextWriter 两个 `.ctor`（唯一置位者）。
其余差异均为 `chaos_type_id_` FNV 非确定性噪声（GetHashCode 按进程随机化，见记忆 codegen-nondeterminism）。
未标记 ctor（如 StringWriter `___ctor_System_Void__`）差异为 0。

## 6. 跨 chunk 回归

threading：codegen 产物中 0 处 XML ctor、0 处标记生效点（chaos_handle 均为既有 reflection 符号）。
✅ 我的改动在 threading 中不生效（全部标记=true 只落在 XML 句柄 ctor）。threading fact 阶段 entry-jit 挂死与本次改动无关（并行 agent 的 PARK/TP debug fprintf 钩子）。

## 7. 判据

- 无「修好 N 弄坏 M」：最终回退=0
- 未标记路径：产物 diff 除 FNV 噪声外为 0
- 全量对比：20 改善 / 0 回退

## 8. 诚实标注

- 4 个目标 subject 未转 real，而是转 unassertable —— 这些是 `void` 返回方法，ATG 无法断言（结构性），但从「错误地 caught」修成「正确无断言」。
- XML chunk 剩余 failed 96 未逐一对账（不在本任务范围）。
- threading 回归被并行 agent 的挂死阻断，只验证了「改动在其中不生效」。
