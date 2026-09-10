# Source Links Rendering Rules v1.01

Date: 2026-04-26 00:30:00 +08:00
Status: draft
Phase: phase-3 (20260425-07)
Parent: foundation-dll-capability-closure-reporting-roadmap

## 1. 目标

定义 foundation DLL audit 报告中 **Source Links** 的渲染规则，使审核者可以从报告页面直接跳转到源码、验证代码、生成代码和证据入口。

## 2. Source Links 类型

### 2.1 五种 Source Link 类型

从 `projection-schema-v1-01.md` 定义的 `sourceLinks` 字段：

| 字段 | 显示名 | 用途 | 目标类型 |
|------|--------|------|----------|
| `subjectSource` | Subject Source | 该 DLL 的 subject `.sln` / 源码入口 | directory |
| `verificationSource` | Verification Source | verification program manifest 等 | file |
| `generatedCode` | Generated Code | 生成的 native code 目录 | directory |
| `evidence` | Evidence | proof / benchmark 运行产物目录 | directory |
| `authorityDocs` | Authority Docs | ledger / schema / contract 设计文档 | directory |

### 2.2 优先级顺序

在 DLL Detail 页中，Source Links 按以下顺序渲染：

```
1. Subject Source        ← 最优先：审核者需要知道源码在哪
2. Generated Code        ← 第二优先：审核者需要看到生成的 native code
3. Evidence              ← 第三优先：审核者需要查看 proof 结果
4. Verification Source   ← 第四优先：program manifest
5. Authority Docs        ← 最后：设计文档（可选）
```

## 3. 渲染规则

### 3.1 通用规则

每个 source link 必须渲染为可点击的 HTML 链接。

```html
<a href="{root_prefix}{path}" target="_blank" rel="noreferrer">
  <span class="source-link-icon">{icon}</span>
  <span class="source-link-label">{displayName}</span>
  <span class="source-link-path">{shortPath}</span>
</a>
```

其中：
- `root_prefix`: dashboard 到 repo root 的相对路径前缀（如 `../../../`）
- `icon`: 类型图标（见 §3.2）
- `displayName`: 显示名（见 §2.1 表格）
- `shortPath`: path 的最后 1-2 段（如 `runs/20260425-.../generated/`）

### 3.2 图标规则

| 类型 | 图标 (Unicode) | CSS Class |
|------|----------------|-----------|
| Subject Source | `📁` | `source-link-icon subject` |
| Generated Code | `⚙️` | `source-link-icon generated` |
| Evidence | `📊` | `source-link-icon evidence` |
| Verification Source | `📋` | `source-link-icon verification` |
| Authority Docs | `📄` | `source-link-icon authority` |

### 3.3 空值规则

当某个 source link 的值为空字符串或 null 时：
- **不渲染**该链接
- 不显示占位符
- 不显示 "n/a"
- 直接跳过

### 3.4 长路径截断

当 path 超过 60 个字符时，shortPath 截断规则：
1. 保留最后 3 个路径段
2. 中间用 `...` 代替
3. 全路径在 title attribute 中显示（hover tooltip）

```
原始: artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/

显示: .../system-private-corelib/native-reference/generated/runtime/
```

### 3.5 Link Target

- `target="_blank"`: 所有 source link 在新标签页打开
- `rel="noreferrer"`: 安全属性
- 不阻止用户右键 "Open in new tab"

## 4. 显示位置

### 4.1 DLL Detail 页 Header

Source Links 在 DLL Detail 页的 header 中作为 row 显示：

```
┌──────────────────────────────────────────────────────┐
│ State  │ Current │ Phase │ Families │ Evidence       │
├──────────────────────────────────────────────────────┤
│ Capability Closure:  [████░░] 90%                    │
│ Workflow Progress:   [████░░] 75%                    │
├──────────────────────────────────────────────────────┤
│ [📁 Subject Source]  [⚙️ Generated Code]             │
│ [📊 Evidence]        [📋 Verification]  [📄 Docs]    │
└──────────────────────────────────────────────────────┘
```

### 4.2 Dashboard 上每个 DLL 卡片

Dashboard 上的精简 DLL 卡片也包含 Source Links，但只显示 3 个最常用的：

```
┌─ System.Private.CoreLib ──────────────────────────┐
│ ● In Progress │ Closure: 90% │ Progress: 75%      │
│ [📁 Source] [⚙️ Generated Code] [📊 Evidence]     │
│ [Detail Page →]                                    │
└────────────────────────────────────────────────────┘
```

### 4.3 Program Overview

Program Overview section 不显示每个 DLL 的 source link。只显示 Program 级别的 source links（program manifest 等）。

## 5. 路径解析规则

### 5.1 相对路径转绝对链接

Source link 的 `path` 值存储为相对于 repo root 的路径。渲染时必须转换为可点击的 URL。

转换规则：
```
path  = "subjects/SolutionCorePack/source/..."
href  = "{root_prefix}{path}"
```

`root_prefix` 由 generator 的函数 `_root_relative_prefix(repo_root, output_path)` 计算。

### 5.2 Directory vs File 的渲染区别

| linkTargetType | 渲染方式 | 用户行为 |
|---------------|----------|----------|
| `directory` | 链接到目录 URL | 文件浏览器打开目录 |
| `file` | 链接到文件 URL | 浏览器显示文件内容 |

### 5.3 存在性检查

渲染前必须检查 `exists` 字段：
- `exists = true`: 正常渲染为蓝色链接
- `exists = false`: 渲染为灰色文字 + 删除线 + "missing" 标签

```html
<span class="source-link missing">
  <span class="source-link-label">{displayName}</span>
  <span class="missing-badge">(missing)</span>
</span>
```

## 6. Source Links 在 Family 级别的展开

### 6.1 Implementation Family → Source mapping

每个 semantic family 的 `implementationFamilies[]` 中的方法 ID 可以直接链接到源码：
- 点击 method subject ID → 跳转到 `audit/<assembly>/input-manifest.json` 对应条目
- 或者跳转到 subject 源码文件中的对应类型（如果路径可推导）

### 6.2 Source Group → Type mapping

每个 semantic family 的 `sourceGroups[]` 中的类型名可以链接到：
- `audit/<assembly>/surface-ledger.json` 中对应类型的条目
- subject 源码目录中的对应 `.cs` 文件（如果类型到文件路径的映射可推导）

### 6.3 方法级链接规则

当渲染 method subject ID 时，优先提供以下链接：
1. 如果 `subjectSource` 可用，构造 `{subjectSource}/{TypeName}.cs` 链接（最佳情况）
2. 如果 1 不可用，链接到 `audit/<assembly>/input-manifest.json`（回退方案）
3. 如果 2 也不可用，纯文本显示（最差情况）

## 7. 示例

### 7.1 完整的 Source Links Section

```html
<div class="source-links">
  <h3>Source Links</h3>
  <div class="source-links-grid">
    <a href="../../../subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/"
       target="_blank" rel="noreferrer"
       class="source-link"
       title="subjects/SolutionCorePack/source/EngineeringScenarios/FoundationDllTranslationSolution/">
      <span class="source-link-icon">📁</span>
      <span class="source-link-label">Subject Source</span>
      <span class="source-link-path">FoundationDllTranslationSolution/</span>
    </a>
    <a href="../../../artifacts/subjects/SolutionCorePack/runs/20260425-153952-windows-ff29/analysis/generated/supplemental-full-closures/system-private-corelib/native-reference/generated/runtime/"
       target="_blank" rel="noreferrer"
       class="source-link">
      <span class="source-link-icon">⚙️</span>
      <span class="source-link-label">Generated Code</span>
      <span class="source-link-path">.../native-reference/generated/runtime/</span>
    </a>
    ...
  </div>
</div>
```

### 7.2 Dashboard 精简版 Source Links

```html
<div class="dll-card-links">
  <a class="pill-link" href="..." target="_blank">📁 Source</a>
  <a class="pill-link" href="..." target="_blank">⚙️ Generated Code</a>
  <a class="pill-link" href="..." target="_blank">📊 Evidence</a>
  <a class="pill-link" href="./dlls/System.Private.CoreLib.html">Detail Page →</a>
</div>
```

## 8. 与现有 generator 的集成

### 8.1 新增渲染函数

| 函数 | 职责 |
|------|------|
| `_render_source_links(dll, root_prefix)` | 渲染 DLL detail 页的 source links section |
| `_render_dashboard_source_links(dll, root_prefix)` | 渲染 dashboard 卡片中的精简 source links |
| `_render_method_source_link(method_id, source_links)` | 渲染单个 method subject ID 的 source link |

### 8.2 在现有渲染流程中的插入点

- `_render_dll_detail_page`: 在 header 的 summary cards 后插入 source links section
- `_render_dashboard`: 在 per-DLL card 的 header 后插入精简 source links

## 9. 变更日志

| 日期 | 版本 | 变更 | 原因 |
|------|------|------|------|
| 2026-04-26 | 1.0.0 | 初始版本 | Phase 3 初始创建 |
