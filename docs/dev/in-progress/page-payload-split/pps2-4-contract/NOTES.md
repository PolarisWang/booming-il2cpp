# pps2-4 执行笔记 — 契约头与接线

## 已定位/已修的跨 TU 符号类别（逐轮实测推进）

| 轮 | 症状 | 根因 | 处置 |
|---|---|---|---|
| 1 | C2146/C2059 语法错 | payload TU 零 include | ✅ preamble（`WrapPayloadSectionInTranslationUnit`） |
| 2 | 19× C2065 `_generic` | 缺 AOT 方法声明 | ✅ preamble 发 `MethodDeclarations` |
| 3 | C3861 reflection helper | 定义在 object model、被 module reg 调用 | ✅ preamble 硬编码声明（待登记器取代） |
| 4 | **97× C2065/C2737** | `kSlots_*` static 内部链接 + 无声明 | ✅ **登记器 + 改 extern** |
| 5 | 13× C2065 + 2× C2737 | `kGeneric*`/`s_method_aot*` 无声明 | ✅ 登记器补登记 |
| 6 | 1× C2065 `s_abi_manifest` | **pps-3b 静默丢弃 ABI manifest 段** | ✅ 补 `AddSection("abimanifest")` |

## 🔴 第 6 轮发现：pps-3b 静默丢内容（严重）

`abiManifestCode` 在 `Methods.cs` 中 `moduleRegSb.Insert(0, ...)` 插入，
**但没有对应的 `AddSection`** —— 它进入了最终 `moduleRegistrationCode` 字符串，
却不属于任何 section。**emitter 是按 section 渲染 payload TU 的 → 整段被静默丢弃。**

**一手证据（拆分前 vs 拆分后）**：
- 旧产物 `tmp_verify/.../native-aot.generated.cpp`：`s_abi_manifest` 出现 **6 次**（含完整定义
  `s_abi_manifest_prefix_sum[1855]` + `s_abi_manifest_storage`）
- 新产物 `payload.page-0001.cpp`：出现 **1 次**（仅 module descriptor 的引用，
  **无定义**）→ `C2065: 's_abi_manifest': undeclared identifier`

**教训**：pps-3b 的 `AddSection` 是**手工登记**的，漏登记 = 静默丢内容，
且**不在同 TU 内看不出**（原巨型 TU 里文本仍在）。这类缺陷必须靠
「拆分前 vs 拆分后产物对照」才能发现。

## 登记器落地状态（D1）

`RegisterCrossSectionSymbol(name, declaration, needsExternalLinkage)` —
planner 级字典，各发射点生成时登记，emitter 从
`templateModel.CrossSectionSymbols` 发射到每个 payload TU 的 preamble。

**性质**：登记器是**防再发**机制 —— 漏登记者，下轮构建会以 C2065 暴露（不再静默）。
