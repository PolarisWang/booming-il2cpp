# Phase 1 交付 — 假绿清零（口径修复）

> 日期：2026-09-20
> 提交：`d4f24f0bd`

## 1. 做了什么

### 1.1 核实三个口径陷阱的真实状态

Phase 0 报告列出三个陷阱。**逐一核实后发现只有一个是真问题** ——
另两个是**陈旧快照**导致的误判：

| 陷阱 | Phase 0 判断 | **核实结论** |
|:-----|:-------------|:-------------|
| `passed` 含 stubGap | 存在 | ✅ **确认存在**，已修 |
| `gateTotal` 分子>分母 | 存在 | ❌ **已修复**（`fact_chunk.py:853-871` 有注释记录历史 bug 与修法） |
| AOT/JIT 记录混淆 | 存在 | ❌ **已分离**（`jitPassed`/`jitTotal` 独立字段 + `aot`/`jit` 独立数组） |

**教训**：Phase 0 看到的是 9/10 的陈旧快照。**引用数据前必须核实新鲜度。**

### 1.2 修复 `passed` 的语义歧义

**问题量化**（本日实测）：

| chunk | passed | realVerified | 虚高比 |
|:------|:------:|:------------:|:------:|
| text-json | 209 | **2** | **104.5×** |
| system-xml-xsl | 251 | **4** | **62.8×** |
| json-converters | 94 | 8 | 11.8× |
| xml | 590 | 136 | 4.3× |
| serialization | 134 | 63 | 2.1× |

**根因**（`fact_chunk.py:139`）：
```python
passed = sum(1 for fr in fact_results if fr.get("passed"))
```
`passed` 是**原生 runner 的运行结果**，与 `resultKind` 无关 ——
`stubGap`（ATG 写死 42L、从不调 native）也返回 `passed=True`。

**修复**（`fact_chunk.py`）：新增机器可读警示字段
```python
"verifiedPassed": real_ct,            # 真正验证通过的数量
"passedIsNotVerification": True,      # 显式标注 passed 的语义边界
```
**既有字段全部保持不变**（不破坏下游消费方）。

**修复**（`aggregate.py`）：修正失实措辞
```
原: "all {passed}/{total} that ran passed"     ← total 含未执行的 stubGap
新: "{realPassed}/{total} real-verified (nominal {passed}/{total} passed,
     incl. non-executing stubs)"
```

### 1.3 端到端验证

跑 converters chunk：
```
passed                  = 94
verifiedPassed          = 8      ← 新字段
passedIsNotVerification = True   ← 新字段
realVerified            = 8
stubGap                 = 80
```

## 2. 未纳入本阶段（移交后续）

矩阵识别出 **4 项 `fake-green`**（`realDefect` —— 执行了但与期望不符）：

```
System.Xml.XmlDocument::GetElementsByTagName
System.Xml.XmlNodeReader::ResolveEntity
System.Xml.XmlTextReader::ResolveEntity
System.Xml.XmlValidatingReader::ResolveEntity
```

**这属于 Phase 4（failed 对账）的范围**，不在 Phase 1（口径修复）内。
已记录，避免遗漏。

## 3. 诚实标注

- 本阶段**只改口径，未改任何测试执行逻辑** —— 真实覆盖率数字**不变**
  （`realVerified` 前后一致），只是让 `passed` 不再误导
- 矩阵生成器仍放在任务目录（`tools/`），**未工程化到常驻工具链** ——
  留待 Phase 5 收口时决定
- Phase 0 报告中「三个陷阱」的说法**已修正为「一个真问题 + 两个陈旧快照」**
