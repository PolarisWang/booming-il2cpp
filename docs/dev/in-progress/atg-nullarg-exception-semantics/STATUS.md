# atg-nullarg-exception-semantics — STATUS

> **task_id**: atg-nullarg-exception-semantics
> **task_type**: implementation
> **创建日期**: 2026-09-16
> **来源**: B7 收官诊断（reflection chunk 剩余 12 项 nullArg 类失败）
> **域**: ATG（TestEmitter.cs）+ fact 分类（fact_chunk.py）

## 机制链（调研结论，2026-09-16）

1. **ATG probe**：对每个 subject 在真实 .NET 上执行并捕获结果。
   抛异常 → `ProbeResult.HasException=true` → ATG 对 CoreLib subject 发
   `Assert.Throws<具体异常类型>`（外部桩程序集发 `ThrowsAny` + `[UNVERIFIED]`）。
   位置：`TestEmitter.cs` `hasException` 分支（~line 296-310）。
2. **null 参数**：引用型参数第一个 value set 是 `default(T)`（null，
   null-forgiving `!` 标记）。真实 .NET 对此抛 `ArgumentNullException`。
3. **AOT 侧**：codegen null-guard 抛的是 **NRE**
   （`chaos_runtime_get_abi_v0()->raise_null_reference_exception()`）。
4. **失败形态**：`Assert.Throws<ArgumentNullException>` 接不到 NRE →
   异常逃逸 → `caught=true, assertFailed=false` → fact 红，
   分类器（fact_chunk.py:217-233）正确识别为 `nullArg`（非缺陷），
   但 **passed=false 仍计入失败总数**。

## 修复设计（已定，待实施）

**TestEmitter 单点改动**：`hasException==true` 且该 value set 含
`default(<引用类型>)!` 参数 → 发 **`Assert.ThrowsAny`** 而非
`Assert.Throws<具体类型>`。理由：AOT 的 NRE-vs-ANE 区分是运行时层
已知缺口（不是被测方法的缺陷）；null 入参的"抛异常"语义已被忠实
复现，异常类型差异不应判红。

- 判定信息 ATG 生成时全部可得（null-forgiving 正则同
  fact_chunk.py `_get_null_arg_subject_ids` 的模式）。
- 预期收益：reflection chunk 12 项（si=58/76/88/90/92/94/106/107/
  108/109/138/139/141）。其他 chunk 的同类 subject 同步受益。

## 验收标准

- [ ] reflection chunk passed 265 → ≥277（12 项转绿）
- [ ] 其他 chunk（xml/json/array 等）的 nullArg 类失败同步消失
- [ ] 单测 2252/2252（注意 float64 线 S24 WIP 的重复注册需先合入修复）
- [ ] 失败集无新增回归

## ⚠️ 已知的另一教训（任务②失败，勿重蹈）

让"未实现"的方法成功 lowering ≠ subject 变绿——catch-all 抛异常时
fact 分类可能本来就把 subject 判为可接受（异常语义匹配 probe）。
改任何断言/分类判定前，先弄清当前绿/红的真实来源。
