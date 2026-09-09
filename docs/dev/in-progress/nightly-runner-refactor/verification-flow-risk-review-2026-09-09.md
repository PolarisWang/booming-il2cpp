# Verification 全流程验证 — 深度 Review 风险报告

> 生成: 2026-09-09 | 范围: chaos-il2cpp foundation-dll 验证管线(chunk_pipeline 的 stage 集)
> 方法: 实测数据(honest-snapshot 2026-09-09)+ 源码逐 stage 读 + 本会话多轮跑测观察
> 状态: 反映"事实绿已修(S1/S2 gate)"后的残余风险清单与优先级

---

## 〇 验证管线拓扑(DAG)

```
build
  ├─→ fact(权威语义门, 现带 real-vs-smoke)
  │       ├─→ benchmark(aot/jit)   [S2 已 gate: fact fail 则 skipped_fact_gate]
  │       │       └─→ benchmark_report
  │       ├─→ managed_benchmark(net8/net10) [⚠ 未 gate — 见 R2.4]
  │       │       └─→ benchmark_report 依赖它
  │       ├─→ hotupdate             [S2 已 gate]
  │       └─→ coverage-audit        [覆盖门禁, 只依赖 build 非 fact]
  ├─→ profile
aggregate(读所有 chunk .json)
  └─→ reporting / allure_report
```

---

## 一 已确认真实缺陷（实测 + 已修 S1/S2 门）

| 风险 | 实测证据(2026-09-09 snapshot) | 状态 |
|------|------------------------------|------|
| **R0 假绿 fact**(UNVERIFIED smoke 计真实 passed) | Immutable 32/32 nominal 但 real=0(全 42); Net.Http 143 nominal 22 real; TOTAL nom 66492 vs real 20097(smoke 46395) | S1 聚合层已按 factGate 排除 gated 的 bench/hu; 报告带 real/smoke; S2 源头 gate 已上 |
| R2.4 managed_benchmark 未 gate on fact | 见源码: run_managed_benchmark@348 无 classify_gate/factGate 读, 只跑 techs; on a smoke chunk它会产出 net8/10 perf 数 | ⚠ **缺口-未修** |

---

## 二 stage 级风险清单一

### 🔴 高(影响结果真实性)

- **R1 fact realTotal 可被 JIT 掩埋的 old path**:_write_fact_results 用 `stdout` 解析, runner 把 42/exception 计入 passed。**即使 S1/S2 在已跑到事实多的 host 生效; 对纯 smoke 的 chunk(Immutable)整 chunk 是 GATED(skip)— 正确**。残余: 对"部分 smoke + 部分真" boundary chunk 会 gate 出 real 为 pass 的方法也一并 skip(过度死亡)。属保守 not 破坏真数据。
- **R2 managed_benchmark / benchmark_report 未做 real-gate** (见 2.4): managed .NET+JIT perf on smoke chunk 仍会混进 bench.summary。需把 S2 gate 同样加上(规模约 2 处)。
- **R3 gating 以 fact.json 为权威但 stage-source 与 aggregate 读同文件依赖其被写**; 若 fact stage `skipped(no subject)` 后无 fact.json,gate=skip → 不 gate,benchmark 照跑(nominal 缺失下合理但可能空跑 stub 却不算绿? aggreg中 fact=no_results → total 0 → 不进绿)。

### 🟠 中
- R4 build→fact 之间缺"fact 结果一致性"预检:fact only 取决于(entry)+ stdout JSON;若 entry 崩溃/截断(rc!=0 但 pass==total to partial) → 不当作 fail 而是 partial 一种。需确认 partial 是否让下游 gate 判定。 — 现行 classify_gate 用 fact.json 的 total/real, 若 fact status=partial 但 json 有 real>0 ⇒ 视通过。
- R5 coverage-audit 只 gate on build(skipped)不 gate on fact 真实;coverage-%-gap(未 dispatch 方法)与 fact real 无关联。同 fact-gate 层, 不参与 semantics。
- R6 双层 gate 判定门槛 GATE_REAL_RATIO=0.10 magic; 放 lambda 不可见; test 只测 unit 不测 e2e(未跑全 family)。
- R7 honest report 工具(_report/_summarize/tmp)未收编成正式可复用命令; 我们手动打的 snapshot 脚本还散在 e2e(已被我 rm)。欠一个正式 `verification/…/reports_honest.py`。

### 🔵 低 / 未来
- R8 profile / profile_range: 与 correctness gating 无关, 性能专用不能 gate。
- R9 reporting cross-family vs per-chunk 一致性; aggregate read path 与 S1 gate 同源。
- R10 allure_report 为报告外围,信息来自已 gate 数据。

---

## 三 残余最大单一风险(会给推进假象) — 排序

1. **R6**: 我未跑"全 family 端到端 gate"—— 只跑了 Immutable/N2 两个。真实覆盖率/阈值是否 gate 掉太多 healthy chunks 未量化。
2. **R2**: managed_benchmark＋reporting 的 gate 空缺仍会让 smoke chunk 的 .NET perf 混入汇总。
3. **R0 residual**: 即使 S2 gate, `_tech_status` 仍把 nominal pass 当绿给下游 consuming report? — aggregate 是唯一绿汇总, S1 已 gate。

---

## 四 fix 优先级建议
P0: (a) 修 R2 给 run_managed_benchmark 加同样 fact head gate (S2 patch 一个 function)。
P0: (b) 把 honest snapshot 工具 formal(收成 verification/nightly 或 verification/tools/honest_report.py) 让操作者有定式报告而非临时脚本。
P1: 补 e2e 验证一个高-real health chunk 跨 build→fact→bench(un-gate 路径)+ 一个全 smokeg chunk(gate 路径) 当回归。
P1: gating.py 的 GATE_REAL_RATIO 从 hardcode 提为 config(from_env 可读 CHAOS_GATE_REAL_RATIO)。
P2: fact real 的 JIT 提升合并/tech 差异不改 total 语义(现有)。
剩余 R4/R5/R8-R10 属低影响, 视 iteration 再定。

## 五 结论
验证流程在"语义真实性"已大幅加固(S0/S1/S2), 但仍有三处形成假绿的残余漏洞:
1. managed_benchmark 未 real-gate(R2) — 会让 smoke 域 .NET perf 混绿;
2. 端到端全 family gate 回归未跑(R6),
3. 无正式 honest-report 工具约束(操作者可回退手工看旧绿)。
修复顺序: R2 → 正式 tool → e2e gate 回归 → GATE 阈值 configable。
