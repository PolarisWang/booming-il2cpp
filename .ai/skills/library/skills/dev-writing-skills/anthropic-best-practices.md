# 技能编写最佳实践

> 学习如何编写 Claude 能够发现并成功使用的有效技能。

好的技能是简洁的、结构良好的，并经过真实使用的测试。本指南提供实用的编写决策，帮助你编写 Claude 能够有效发现和使用的技能。

关于技能工作原理的概念性背景，请参见[技能概述](/en/docs/agents-and-tools/agent-skills/overview)。

## 核心原则

### 简洁为王

[上下文窗口](https://platform.claude.com/docs/en/build-with-claude/context-windows)是公共资源。你的技能与 Claude 需要了解的所有其他内容共享上下文窗口，包括：

* 系统提示
* 对话历史
* 其他技能的元数据
* 你的实际请求

并非技能中的每个 token 都有即时成本。启动时，只有来自所有技能的元数据（名称和描述）会预加载。Claude 只有在技能变得相关时才读取 SKILL.md，只有在需要时才读取额外文件。然而，SKILL.md 中的简洁性仍然重要：一旦 Claude 加载它，每个 token 都与对话历史和其他上下文竞争。

**默认假设**：Claude 已经非常聪明

只添加 Claude 还没有的上下文。质疑每条信息：

* "Claude 真的需要这个解释吗？"
* "我可以假设 Claude 知道这个吗？"
* "这段话值得消耗 token 吗？"

**好例子：简洁**（约 50 个 token）：

````markdown  theme={null}
## 提取 PDF 文本

使用 pdfplumber 进行文本提取：

```python
import pdfplumber

with pdfplumber.open("file.pdf") as pdf:
    text = pdf.pages[0].extract_text()
```
````

**坏例子：过于冗长**（约 150 个 token）：

```markdown  theme={null}
## 提取 PDF 文本

PDF（便携式文档格式）文件是一种常见的文件格式，包含
文本、图像和其他内容。要从 PDF 中提取文本，你需要
使用一个库。有很多 PDF 处理库可用，但我们
推荐 pdfplumber，因为它易于使用且能处理大多数情况。
首先，你需要用 pip 安装它。然后可以使用下面的代码...
```

简洁版本假设 Claude 知道 PDF 是什么以及库如何工作。

### 设置适当的自由度

将具体程度与任务的脆弱性和可变性相匹配。

**高自由度**（基于文本的指令）：

使用时机：

* 多种方法都有效
* 决策取决于上下文
* 启发式方法指导处理方式

示例：

```markdown  theme={null}
## 代码审查流程

1. 分析代码结构和组织
2. 检查潜在的 bug 或边缘情况
3. 建议可读性和可维护性的改进
4. 验证是否遵守项目惯例
```

**中等自由度**（带参数的伪代码或脚本）：

使用时机：

* 存在首选模式
* 可以接受一些变化
* 配置影响行为

示例：

````markdown  theme={null}
## 生成报告

使用此模板并根据需要自定义：

```python
def generate_report(data, format="markdown", include_charts=True):
    # 处理数据
    # 生成指定格式的输出
    # 可选地包含可视化
```
````

**低自由度**（具体脚本，少量或无参数）：

使用时机：

* 操作脆弱且容易出错
* 一致性至关重要
* 必须遵循特定顺序

示例：

````markdown  theme={null}
## 数据库迁移

精确运行此脚本：

```bash
python scripts/migrate.py --verify --backup
```

不要修改命令或添加额外标志。
````

**类比**：将 Claude 想象为探索路径的机器人：

* **两侧有悬崖的窄桥**：只有一条安全前进的路。提供具体的护栏和精确指令（低自由度）。示例：必须按精确顺序运行的数据库迁移。
* **没有危险的开阔地**：许多路径通向成功。给出大致方向，信任 Claude 找到最佳路线（高自由度）。示例：上下文决定最佳方法的代码审查。

### 使用所有计划使用的模型进行测试

技能作为模型的补充，因此有效性取决于底层模型。使用你计划使用的所有模型测试你的技能。

**按模型的测试注意事项**：

* **Claude Haiku**（快速、经济）：技能是否提供了足够的指导？
* **Claude Sonnet**（平衡）：技能是否清晰高效？
* **Claude Opus**（强大推理）：技能是否避免了过度解释？

对 Opus 完美的内容对 Haiku 可能需要更多细节。如果你计划跨多个模型使用技能，目标是能与所有模型良好配合的指令。

## 技能结构

<Note>
  **YAML 前置元数据**：SKILL.md 前置元数据支持两个字段：

  * `name` - 技能的人类可读名称（最多 64 个字符）
  * `description` - 技能功能和使用时机的一行描述（最多 1024 个字符）

  有关完整的技能结构详情，请参见[技能概述](/en/docs/agents-and-tools/agent-skills/overview#skill-structure)。
</Note>

### 命名惯例

使用一致的命名模式使技能更容易引用和讨论。我们推荐使用**动名词形式**（动词 + -ing）为技能命名，因为这清楚地描述了技能提供的活动或能力。

**好的命名示例（动名词形式）**：

* "处理 PDF"（Processing PDFs）
* "分析电子表格"（Analyzing spreadsheets）
* "管理数据库"（Managing databases）
* "测试代码"（Testing code）
* "编写文档"（Writing documentation）

**可接受的替代方案**：

* 名词短语："PDF 处理"、"电子表格分析"
* 动作导向："处理 PDF"、"分析电子表格"

**避免**：

* 模糊名称："Helper"、"Utils"、"Tools"
* 过于通用："Documents"、"Data"、"Files"
* 技能集合中的不一致模式

一致命名使以下事项更容易：

* 在文档和对话中引用技能
* 一眼看出技能的功能
* 组织和搜索多个技能
* 维护专业、有凝聚力的技能库

### 编写有效的描述

`description` 字段启用技能发现，应该同时包括技能的功能和使用时机。

<Warning>
  **始终以第三人称书写**。描述被注入到系统提示中，不一致的视角会导致发现问题。

  * **好的：** "处理 Excel 文件并生成报告"
  * **避免：** "我可以帮你处理 Excel 文件"
  * **避免：** "你可以用这个处理 Excel 文件"
</Warning>

**要具体并包含关键术语**。同时包含技能的功能以及使用时机的具体触发器/上下文。

每个技能恰好有一个描述字段。描述对技能选择至关重要：Claude 用它从可能 100+ 个可用技能中选择合适的技能。你的描述必须提供足够的细节让 Claude 知道何时选择这个技能，而 SKILL.md 的其余部分提供实现细节。

有效示例：

**PDF 处理技能：**

```yaml  theme={null}
description: 从 PDF 文件中提取文本和表格，填写表单，合并文档。当处理 PDF 文件或用户提到 PDF、表单或文档提取时使用。
```

**Excel 分析技能：**

```yaml  theme={null}
description: 分析 Excel 电子表格，创建数据透视表，生成图表。当分析 Excel 文件、电子表格、表格数据或 .xlsx 文件时使用。
```

**Git 提交助手技能：**

```yaml  theme={null}
description: 通过分析 git diff 生成描述性提交消息。当用户请求帮助编写提交消息或审查暂存的更改时使用。
```

避免像这样的模糊描述：

```yaml  theme={null}
description: 帮助处理文档
```

```yaml  theme={null}
description: 处理数据
```

```yaml  theme={null}
description: 对文件做各种事情
```

### 渐进式披露模式

SKILL.md 作为概述，根据需要将 Claude 指向详细材料，类似于入职指南中的目录。有关渐进式披露工作原理的解释，请参见概述中的[技能工作原理](/en/docs/agents-and-tools/agent-skills/overview#how-skills-work)。

**实用指导：**

* 保持 SKILL.md 正文在 500 行以下以获得最佳性能
* 接近此限制时将内容拆分到单独文件
* 使用以下模式有效组织指令、代码和资源

#### 视觉概述：从简单到复杂

基本技能从只有 SKILL.md 文件（包含元数据和指令）开始。

随着技能增长，你可以打包 Claude 只在需要时加载的额外内容。

完整的技能目录结构可能如下所示：

```
pdf/
├── SKILL.md              # 主指令（触发时加载）
├── FORMS.md              # 表单填写指南（按需加载）
├── reference.md          # API 参考（按需加载）
├── examples.md           # 使用示例（按需加载）
└── scripts/
    ├── analyze_form.py   # 工具脚本（执行，不加载）
    ├── fill_form.py      # 表单填写脚本
    └── validate.py       # 验证脚本
```

#### 模式 1：带引用的高层指南

````markdown  theme={null}
---
name: PDF Processing
description: 从 PDF 文件提取文本和表格，填写表单，合并文档。当处理 PDF 文件或用户提到 PDF、表单或文档提取时使用。
---

# PDF 处理

## 快速开始

用 pdfplumber 提取文本：
```python
import pdfplumber
with pdfplumber.open("file.pdf") as pdf:
    text = pdf.pages[0].extract_text()
```

## 高级功能

**表单填写**：完整指南见 [FORMS.md](FORMS.md)
**API 参考**：所有方法见 [REFERENCE.md](REFERENCE.md)
**示例**：常见模式见 [EXAMPLES.md](EXAMPLES.md)
````

Claude 只在需要时加载 FORMS.md、REFERENCE.md 或 EXAMPLES.md。

#### 模式 2：领域特定组织

对于有多个领域的技能，按领域组织内容以避免加载不相关的上下文。当用户询问销售指标时，Claude 只需要读取销售相关模式，而非财务或营销数据。

```
bigquery-skill/
├── SKILL.md（概述和导航）
└── reference/
    ├── finance.md（营收、账单指标）
    ├── sales.md（商机、销售管道）
    ├── product.md（API 使用、功能）
    └── marketing.md（活动、归因）
```

#### 模式 3：条件式详细内容

显示基本内容，链接到高级内容：

```markdown  theme={null}
# DOCX 处理

## 创建文档

使用 docx-js 创建新文档。见 [DOCX-JS.md](DOCX-JS.md)。

## 编辑文档

简单编辑时，直接修改 XML。

**追踪修改**：见 [REDLINING.md](REDLINING.md)
**OOXML 详情**：见 [OOXML.md](OOXML.md)
```

Claude 只在用户需要这些功能时才读取 REDLINING.md 或 OOXML.md。

### 避免深层嵌套引用

Claude 可能在从其他引用文件中引用的文件时部分读取。遇到嵌套引用时，Claude 可能使用 `head -100` 等命令来预览内容而非读取整个文件，导致信息不完整。

**保持引用在 SKILL.md 的一层深度**。所有引用文件应直接从 SKILL.md 链接，以确保 Claude 在需要时读取完整文件。

**坏例子：太深**：

```markdown  theme={null}
# SKILL.md
见 [advanced.md](advanced.md)...

# advanced.md
见 [details.md](details.md)...

# details.md
这里是实际信息...
```

**好例子：一层深度**：

```markdown  theme={null}
# SKILL.md

**基本用法**：[SKILL.md 中的指令]
**高级功能**：见 [advanced.md](advanced.md)
**API 参考**：见 [reference.md](reference.md)
**示例**：见 [examples.md](examples.md)
```

### 为较长的引用文件添加目录

对于超过 100 行的引用文件，在顶部包含目录。这确保 Claude 即使在部分读取时也能看到可用信息的完整范围。

**示例**：

```markdown  theme={null}
# API 参考

## 目录
- 认证和设置
- 核心方法（创建、读取、更新、删除）
- 高级功能（批量操作、webhooks）
- 错误处理模式
- 代码示例

## 认证和设置
...

## 核心方法
...
```

Claude 可以读取完整文件或根据需要跳转到特定部分。

## 工作流和反馈循环

### 对复杂任务使用工作流

将复杂操作分解为清晰的顺序步骤。对于特别复杂的工作流，提供一个 Claude 可以复制到其响应并随进展勾选的检查清单。

**示例 1：研究综合工作流**（无代码技能）：

````markdown  theme={null}
## 研究综合工作流

复制此检查清单并跟踪进度：

```
研究进度：
- [ ] 步骤 1：读取所有源文档
- [ ] 步骤 2：识别关键主题
- [ ] 步骤 3：交叉引用声明
- [ ] 步骤 4：创建结构化摘要
- [ ] 步骤 5：验证引用
```

**步骤 1：读取所有源文档**

查看 `sources/` 目录中的每个文档。注意主要论点和支持证据。

**步骤 2：识别关键主题**

寻找跨来源的模式。哪些主题反复出现？来源在哪些方面一致或不一致？

...
````

**示例 2：PDF 表单填写工作流**（有代码技能）：

````markdown  theme={null}
## PDF 表单填写工作流

复制此检查清单并随完成勾选：

```
任务进度：
- [ ] 步骤 1：分析表单（运行 analyze_form.py）
- [ ] 步骤 2：创建字段映射（编辑 fields.json）
- [ ] 步骤 3：验证映射（运行 validate_fields.py）
- [ ] 步骤 4：填写表单（运行 fill_form.py）
- [ ] 步骤 5：验证输出（运行 verify_output.py）
```

**步骤 1：分析表单**

运行：`python scripts/analyze_form.py input.pdf`

这提取表单字段及其位置，保存到 `fields.json`。

...
````

清晰的步骤防止 Claude 跳过关键验证。

### 实现反馈循环

**常见模式**：运行验证器 → 修复错误 → 重复

此模式大大提高输出质量。

**示例 1：风格指南合规**（无代码技能）：

```markdown  theme={null}
## 内容审查流程

1. 遵循 STYLE_GUIDE.md 中的指南起草内容
2. 根据清单审查：
   - 检查术语一致性
   - 验证示例遵循标准格式
   - 确认所有必需章节都存在
3. 如果发现问题：
   - 记录每个问题及具体章节引用
   - 修订内容
   - 再次检查清单
4. 只有满足所有要求才继续
5. 完成并保存文档
```

**示例 2：文档编辑流程**（有代码技能）：

```markdown  theme={null}
## 文档编辑流程

1. 对 `word/document.xml` 进行编辑
2. **立即验证**：`python ooxml/scripts/validate.py unpacked_dir/`
3. 如果验证失败：
   - 仔细检查错误消息
   - 修复 XML 中的问题
   - 再次运行验证
4. **只有验证通过才继续**
5. 重新打包：`python ooxml/scripts/pack.py unpacked_dir/ output.docx`
6. 测试输出文档
```

## 内容指南

### 避免时效性信息

不要包含会过时的信息：

**坏例子：时效性**（会变错）：

```markdown  theme={null}
如果你在 2025 年 8 月之前做这件事，使用旧 API。
2025 年 8 月之后，使用新 API。
```

**好例子**（使用"旧模式"章节）：

```markdown  theme={null}
## 当前方法

使用 v2 API 端点：`api.example.com/v2/messages`

## 旧模式

<details>
<summary>旧版 v1 API（2025-08 废弃）</summary>

v1 API 使用：`api.example.com/v1/messages`

此端点不再支持。
</details>
```

### 使用一致的术语

选择一个术语并在整个技能中使用它：

**好的——一致**：

* 始终使用"API 端点"
* 始终使用"字段"
* 始终使用"提取"

**坏的——不一致**：

* 混用"API 端点"、"URL"、"API 路由"、"路径"
* 混用"字段"、"框"、"元素"、"控件"
* 混用"提取"、"拉取"、"获取"、"检索"

## 常见模式

### 模板模式

为输出格式提供模板。根据你的需求匹配严格程度。

**对于严格要求**（如 API 响应或数据格式）：

````markdown  theme={null}
## 报告结构

始终使用此精确模板结构：

```markdown
# [分析标题]

## 执行摘要
[关键发现的一段概述]

## 关键发现
- 发现 1 及支持数据
- 发现 2 及支持数据

## 建议
1. 具体可操作的建议
2. 具体可操作的建议
```
````

**对于灵活指导**（当适应有用时）：

````markdown  theme={null}
## 报告结构

以下是合理的默认格式，但根据分析使用你的最佳判断：

```markdown
# [分析标题]

## 执行摘要
[概述]

## 关键发现
[根据发现调整章节]

## 建议
[针对具体上下文定制]
```

根据具体分析类型调整章节。
````

### 示例模式

对于输出质量依赖于看到示例的技能，提供输入/输出对——就像在普通提示中一样：

````markdown  theme={null}
## 提交消息格式

按照这些示例生成提交消息：

**示例 1：**
输入：添加了使用 JWT 令牌的用户认证
输出：
```
feat(auth): implement JWT-based authentication

Add login endpoint and token validation middleware
```

**示例 2：**
输入：修复了日期在报告中显示不正确的 bug
输出：
```
fix(reports): correct date formatting in timezone conversion

Use UTC timestamps consistently across report generation
```
````

示例比单独的描述更清楚地帮助 Claude 理解期望的风格和细节程度。

### 条件工作流模式

引导 Claude 通过决策点：

```markdown  theme={null}
## 文档修改工作流

1. 确定修改类型：

   **创建新内容？** → 遵循下面的"创建工作流"
   **编辑现有内容？** → 遵循下面的"编辑工作流"

2. 创建工作流：
   - 使用 docx-js 库
   - 从头构建文档
   - 导出为 .docx 格式

3. 编辑工作流：
   - 解压现有文档
   - 直接修改 XML
   - 每次更改后验证
   - 完成后重新打包
```

<Tip>
  如果工作流变得很大或有很多步骤，考虑将其推入单独的文件，并告诉 Claude 根据手头任务读取适当的文件。
</Tip>

## 评估和迭代

### 先构建评估

**在编写大量文档之前先创建评估。** 这确保你的技能解决真实问题，而非记录想象的问题。

**评估驱动开发：**

1. **识别缺口**：在没有技能的情况下对代表性任务运行 Claude。记录具体失败或缺失的上下文
2. **创建评估**：构建三个测试这些缺口的场景
3. **建立基准**：衡量 Claude 在没有技能时的性能
4. **编写最小指令**：创建刚好足以解决缺口并通过评估的内容
5. **迭代**：执行评估，与基准比较，并细化

这种方法确保你在解决实际问题，而非预测可能永远不会出现的需求。

**评估结构**：

```json  theme={null}
{
  "skills": ["pdf-processing"],
  "query": "从这个 PDF 文件中提取所有文本并保存到 output.txt",
  "files": ["test-files/document.pdf"],
  "expected_behavior": [
    "使用适当的 PDF 处理库或命令行工具成功读取 PDF 文件",
    "从文档的所有页面提取文本内容，不遗漏任何页面",
    "以清晰、可读的格式将提取的文本保存到名为 output.txt 的文件"
  ]
}
```

<Note>
  此示例演示了带有简单测试标准的数据驱动评估。我们目前不提供运行这些评估的内置方式。用户可以创建自己的评估系统。评估是衡量技能有效性的真理来源。
</Note>

### 与 Claude 迭代开发技能

最有效的技能开发过程涉及 Claude 本身。与一个 Claude 实例（"Claude A"）合作创建将被其他实例（"Claude B"）使用的技能。Claude A 帮助你设计和细化指令，而 Claude B 在真实任务中测试它们。

**创建新技能：**

1. **不带技能完成任务**：与 Claude A 使用普通提示处理问题。在工作过程中，你会自然地提供上下文、解释偏好并分享程序性知识。
2. **识别可复用模式**：完成任务后，识别你提供的哪些上下文对类似的未来任务有用。
3. **请 Claude A 创建技能**："创建一个捕获我们刚刚使用的 BigQuery 分析模式的技能。"
4. **检查简洁性**：确保 Claude A 没有添加不必要的解释。
5. **改进信息架构**：请 Claude A 更有效地组织内容。
6. **在类似任务上测试**：将技能用于 Claude B（加载了技能的新实例）处理相关用例。
7. **基于观察迭代**：如果 Claude B 遇到困难，带着具体情况返回 Claude A。

**迭代现有技能：**

1. **在真实工作流中使用技能**：给 Claude B 实际任务，而非测试场景
2. **观察 Claude B 的行为**：注意它在哪里挣扎、成功或做出意外选择
3. **返回 Claude A 改进**：分享当前 SKILL.md 并描述你观察到的情况
4. **审查 Claude A 的建议**：Claude A 可能建议重新组织使规则更突出，或使用更强的语言
5. **应用并测试变更**：用 Claude A 的细化更新技能，然后用 Claude B 再次测试
6. **根据使用情况重复**：随着遇到新场景继续这个观察-细化-测试循环

**收集团队反馈：**

1. 与队友分享技能并观察他们的使用
2. 询问：技能是否按预期激活？指令是否清晰？缺少什么？
3. 整合反馈以解决自己使用模式中的盲点

### 观察 Claude 如何导航技能

在迭代技能时，关注 Claude 在实践中如何实际使用它们。注意：

* **意外的探索路径**：Claude 是否以你没有预料到的顺序读取文件？
* **未连接的关联**：Claude 是否未能跟随对重要文件的引用？
* **对某些章节的过度依赖**：如果 Claude 反复读取同一文件，考虑该内容是否应该在主 SKILL.md 中
* **被忽略的内容**：如果 Claude 从未访问某个打包文件，它可能不必要或在主指令中信号不够明显

基于这些观察而非假设进行迭代。技能元数据中的 `name` 和 `description` 特别关键——Claude 在决定是否根据当前任务触发技能时使用这些字段。

## 要避免的反模式

### 避免 Windows 风格路径

始终在文件路径中使用正斜杠，即使在 Windows 上：

* ✓ **好的**：`scripts/helper.py`、`reference/guide.md`
* ✗ **避免**：`scripts\helper.py`、`reference\guide.md`

Unix 风格路径在所有平台上都有效，而 Windows 风格路径在 Unix 系统上会导致错误。

### 避免提供太多选项

除非必要，不要呈现多种方法：

````markdown  theme={null}
**坏例子：太多选择**（令人困惑）：
"你可以使用 pypdf，或者 pdfplumber，或者 PyMuPDF，或者 pdf2image，或者..."

**好例子：提供默认值**（带逃脱出口）：
"使用 pdfplumber 进行文本提取：
```python
import pdfplumber
```

对于需要 OCR 的扫描 PDF，改用 pdf2image 和 pytesseract。"
````

## 高级：带可执行代码的技能

以下章节专注于包含可执行脚本的技能。如果你的技能只使用 markdown 指令，跳到[有效技能的检查清单](#checklist-for-effective-skills)。

### 解决问题，而非转嫁问题

为技能编写脚本时，处理错误条件而非将问题转嫁给 Claude。

**好例子：明确处理错误**：

```python  theme={null}
def process_file(path):
    """处理文件，如果不存在则创建。"""
    try:
        with open(path) as f:
            return f.read()
    except FileNotFoundError:
        # 创建带默认内容的文件而非失败
        print(f"文件 {path} 未找到，创建默认文件")
        with open(path, 'w') as f:
            f.write('')
        return ''
    except PermissionError:
        # 提供替代方案而非失败
        print(f"无法访问 {path}，使用默认值")
        return ''
```

配置参数也应该被记录以避免"魔法常数"（Ousterhout 定律）。

**好例子：自文档化**：

```python  theme={null}
# HTTP 请求通常在 30 秒内完成
# 较长的超时考虑到慢速连接
REQUEST_TIMEOUT = 30

# 三次重试在可靠性和速度之间取得平衡
# 大多数间歇性故障在第二次重试时解决
MAX_RETRIES = 3
```

**坏例子：魔法数字**：

```python  theme={null}
TIMEOUT = 47  # 为什么是 47？
RETRIES = 5   # 为什么是 5？
```

### 提供工具脚本

即使 Claude 可以编写脚本，预制脚本也有优势：

**工具脚本的好处**：

* 比生成的代码更可靠
* 节省 token（无需在上下文中包含代码）
* 节省时间（不需要代码生成）
* 确保跨使用的一致性

**重要区别**：在指令中明确 Claude 是否应该：

* **执行脚本**（最常见）："运行 `analyze_form.py` 提取字段"
* **作为参考读取**（对于复杂逻辑）："见 `analyze_form.py` 了解字段提取算法"

对于大多数工具脚本，执行是首选，因为它更可靠和高效。

### 使用视觉分析

当输入可以渲染为图像时，让 Claude 分析它们：

````markdown  theme={null}
## 表单布局分析

1. 将 PDF 转换为图像：
   ```bash
   python scripts/pdf_to_images.py form.pdf
   ```

2. 分析每个页面图像以识别表单字段
3. Claude 可以直观地看到字段位置和类型
````

### 创建可验证的中间输出

当 Claude 执行复杂的开放式任务时，它可能犯错误。"计划-验证-执行"模式通过让 Claude 首先以结构化格式创建计划，然后在执行之前用脚本验证该计划，来尽早发现错误。

**为什么此模式有效：**

* **尽早发现错误**：验证在应用更改之前发现问题
* **机器可验证**：脚本提供客观验证
* **可撤销规划**：Claude 可以在不触及原始文件的情况下迭代计划
* **清晰调试**：错误消息指向具体问题

**使用时机**：批量操作、破坏性更改、复杂验证规则、高风险操作。

### 打包依赖项

技能在具有平台特定限制的代码执行环境中运行：

* **claude.ai**：可以从 npm 和 PyPI 安装包并从 GitHub 仓库拉取
* **Anthropic API**：没有网络访问和运行时包安装

在 SKILL.md 中列出所需包，并在[代码执行工具文档](/en/docs/agents-and-tools/tool-use/code-execution-tool)中验证它们是否可用。

### 运行时环境

技能在具有文件系统访问、bash 命令和代码执行能力的代码执行环境中运行。

**这如何影响你的编写：**

**Claude 如何访问技能：**

1. **预加载元数据**：启动时，所有技能的 YAML 前置元数据中的名称和描述被加载到系统提示中
2. **按需读取文件**：Claude 使用 bash Read 工具在需要时从文件系统访问 SKILL.md 和其他文件
3. **高效执行脚本**：工具脚本可以通过 bash 执行，而无需将其完整内容加载到上下文中
4. **大文件无上下文损耗**：引用文件、数据或文档在实际读取之前不消耗上下文 token

* **文件路径很重要**：Claude 像文件系统一样导航你的技能目录
* **描述性地命名文件**：使用表明内容的名称：`form_validation_rules.md`，而非 `doc2.md`
* **为发现而组织**：按领域或功能结构化目录
* **打包综合资源**：包含完整 API 文档、大量示例、大型数据集；在访问之前无上下文损耗
* **对确定性操作优先使用脚本**：编写 `validate_form.py` 而非要求 Claude 生成验证代码
* **明确执行意图**：
  * "运行 `analyze_form.py` 提取字段"（执行）
  * "见 `analyze_form.py` 了解提取算法"（作为参考读取）
* **测试文件访问模式**：通过真实请求验证 Claude 可以导航你的目录结构

### MCP 工具引用

如果你的技能使用 MCP（模型上下文协议）工具，始终使用完全限定的工具名称以避免"工具未找到"错误。

**格式**：`ServerName:tool_name`

**示例**：

```markdown  theme={null}
使用 BigQuery:bigquery_schema 工具检索表模式。
使用 GitHub:create_issue 工具创建问题。
```

没有服务器前缀时，Claude 可能无法定位工具，特别是当多个 MCP 服务器可用时。

### 避免假设已安装工具

不要假设包已可用：

````markdown  theme={null}
**坏例子：假设已安装**：
"使用 pdf 库处理文件。"

**好例子：明确说明依赖项**：
"安装所需包：`pip install pypdf`

然后使用它：
```python
from pypdf import PdfReader
reader = PdfReader("file.pdf")
```"
````

## 技术注意事项

### YAML 前置元数据要求

SKILL.md 前置元数据只包含 `name`（最多 64 个字符）和 `description`（最多 1024 个字符）字段。完整结构详情请参见[技能概述](/en/docs/agents-and-tools/agent-skills/overview#skill-structure)。

### Token 预算

保持 SKILL.md 正文在 500 行以下以获得最佳性能。如果内容超过此限制，使用前面描述的渐进式披露模式将其拆分为单独文件。

## 有效技能的检查清单

在分享技能之前，验证：

### 核心质量

* [ ] 描述是具体的，包含关键术语
* [ ] 描述同时包含技能的功能和使用时机
* [ ] SKILL.md 正文在 500 行以下
* [ ] 额外详情在单独文件中（如需要）
* [ ] 无时效性信息（或在"旧模式"章节中）
* [ ] 整个文件使用一致的术语
* [ ] 示例是具体的，而非抽象的
* [ ] 文件引用只有一层深度
* [ ] 适当使用渐进式披露
* [ ] 工作流有清晰的步骤

### 代码和脚本

* [ ] 脚本解决问题而非转嫁给 Claude
* [ ] 错误处理是明确且有帮助的
* [ ] 没有"魔法常数"（所有值都有理由）
* [ ] 所需包在指令中列出并已验证可用
* [ ] 脚本有清晰的文档
* [ ] 没有 Windows 风格路径（全部使用正斜杠）
* [ ] 关键操作有验证/确认步骤
* [ ] 质量关键任务包含反馈循环

### 测试

* [ ] 至少创建了三个评估
* [ ] 使用 Haiku、Sonnet 和 Opus 测试过
* [ ] 使用真实使用场景测试过
* [ ] 整合了团队反馈（如适用）

## 下一步

<CardGroup cols={2}>
  <Card title="开始使用 Agent Skills" icon="rocket" href="/en/docs/agents-and-tools/agent-skills/quickstart">
    创建你的第一个技能
  </Card>

  <Card title="在 Claude Code 中使用技能" icon="terminal" href="/en/docs/claude-code/skills">
    在 Claude Code 中创建和管理技能
  </Card>

  <Card title="通过 API 使用技能" icon="code" href="/en/api/skills-guide">
    以编程方式上传和使用技能
  </Card>
</CardGroup>
