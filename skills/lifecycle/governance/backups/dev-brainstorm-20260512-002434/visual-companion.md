# 可视化伴侣指南

基于浏览器的可视化头脑风暴伴侣，用于展示原型、图表和选项。

## 何时使用

按问题决定，而非按会话决定。判断标准：**用户通过看到内容是否比阅读更容易理解？**

**使用浏览器**，当内容本身是可视的：

- **UI 原型** — 线框图、布局、导航结构、组件设计
- **架构图** — 系统组件、数据流、关系图
- **并排视觉比较** — 比较两种布局、两种配色方案、两种设计方向
- **设计细节** — 当问题涉及外观和感觉、间距、视觉层次时
- **空间关系** — 状态机、流程图、以图表形式渲染的实体关系

**使用终端**，当内容是文字或表格时：

- **需求和范围问题** — "X 是什么意思？"、"哪些功能在范围内？"
- **概念性 A/B/C 选择** — 用文字描述的方案之间的选择
- **权衡列表** — 优缺点、比较表格
- **技术决策** — API 设计、数据建模、架构方案选择
- **澄清问题** — 任何答案是文字而非视觉偏好的内容

关于 UI 话题的问题不自动等同于视觉问题。"你想要什么样的向导？"是概念性的——使用终端。"哪种向导布局感觉更对？"是视觉的——使用浏览器。

## 工作原理

服务器监视一个目录中的 HTML 文件，并将最新的文件提供给浏览器。你编写 HTML 内容，用户在浏览器中看到它并可以点击选择选项。选择结果记录到 `.events` 文件中，你在下一轮中读取。

**内容片段 vs 完整文档：** 如果你的 HTML 文件以 `<!DOCTYPE` 或 `<html` 开头，服务器将按原样提供（仅注入辅助脚本）。否则，服务器会自动将内容包装在框架模板中——添加标题、CSS 主题、选择指示器和所有交互基础设施。**默认编写内容片段。** 只有在需要完全控制页面时才编写完整文档。

## 启动会话

```bash
# macOS / Linux
scripts/start-server.sh --project-dir /path/to/project

# Windows
scripts\start-server.bat --project-dir C:\path\to\project

# 返回: {"type":"server-started","port":52341,"url":"http://localhost:52341",
#        "screen_dir":"/path/to/project/.booming/brainstorm/12345-1706000000"}
```

保存响应中的 `screen_dir`。告诉用户打开 URL。

**查找连接信息：** 服务器将其启动 JSON 写入 `$SCREEN_DIR/.server-info`。如果你在后台启动了服务器且未捕获标准输出，可读取该文件获取 URL 和端口。使用 `--project-dir` 时，在 `<project>/.booming/brainstorm/` 中查找会话目录。

**注意：** 将项目根目录作为 `--project-dir` 传入，这样原型文件会持久化到 `.booming/brainstorm/`，并在服务器重启后保留。不使用该参数时，文件会存到 `/tmp` 并被清理。提醒用户将 `.booming/` 添加到 `.gitignore`（如果尚未添加）。

**按平台启动服务器：**

**Claude Code（macOS/Linux）：**
```bash
# 默认模式有效——脚本自己在后台运行服务器
scripts/start-server.sh --project-dir /path/to/project
```

**Claude Code（Windows）：**
```bat
scripts\start-server.bat --project-dir C:\path\to\project
```

**Codex：**
```bash
# Codex 会回收后台进程。脚本自动检测 CODEX_CI 并
# 切换到前台模式。正常运行——不需要额外标志。
scripts/start-server.sh --project-dir /path/to/project
```

**Gemini CLI：**
```bash
# 使用 --foreground 并在 shell 工具调用中设置 is_background: true
# 以便进程在轮次间保持运行
scripts/start-server.sh --project-dir /path/to/project --foreground
```

**其他环境：** 服务器必须在对话轮次间持续在后台运行。如果你的环境会回收分离的进程，请使用 `--foreground` 并通过平台的后台执行机制启动命令。

如果 URL 无法从浏览器访问（常见于远程/容器化环境），绑定非回环地址：

```bash
# macOS / Linux
scripts/start-server.sh \
  --project-dir /path/to/project \
  --host 0.0.0.0 \
  --url-host localhost

# Windows
scripts\start-server.bat --project-dir C:\path\to\project --host 0.0.0.0 --url-host localhost
```

使用 `--url-host` 控制返回的 URL JSON 中显示的主机名。

## 工作循环

1. **检查服务器是否存活**，然后**将 HTML 写入** `screen_dir` 中的新文件：
   - 每次写入前，检查 `$SCREEN_DIR/.server-info` 是否存在。如果不存在（或 `.server-stopped` 存在），服务器已关闭——在继续前用 `start-server.sh`（macOS/Linux）或 `start-server.bat`（Windows）重启它。服务器在无活动 30 分钟后自动退出。
   - 使用语义化文件名：`platform.html`、`visual-style.html`、`layout.html`
   - **不要重用文件名** — 每个屏幕使用新文件
   - 使用 Write 工具 — **不要使用 cat/heredoc**（会在终端输出噪音）
   - 服务器自动提供最新文件

2. **告诉用户预期内容并结束本轮：**
   - 提醒他们 URL（每步都提醒，不仅仅是第一步）
   - 给出屏幕上内容的简短文字摘要（例如，"显示首页的 3 种布局选项"）
   - 请他们在终端中回应："请查看并告诉我你的想法。如果你想选择一个选项，可以点击。"

3. **下一轮** — 用户在终端中回应后：
   - 如果 `$SCREEN_DIR/.events` 存在则读取——其中包含用户的浏览器交互（点击、选择）JSON 行
   - 与用户的终端文字合并以获取完整信息
   - 终端消息是主要反馈；`.events` 提供结构化交互数据

4. **迭代或推进** — 如果反馈改变了当前屏幕，写入新文件（例如 `layout-v2.html`）。只有在当前步骤经过验证后才推进到下一个问题。

5. **返回终端时卸载** — 当下一步不需要浏览器时（例如澄清问题、权衡讨论），推送等待屏幕以清除过期内容：

   ```html
   <!-- filename: waiting.html (或 waiting-2.html 等) -->
   <div style="display:flex;align-items:center;justify-content:center;min-height:60vh">
     <p class="subtitle">在终端中继续...</p>
   </div>
   ```

   这防止用户在对话已经继续时还盯着已解决的选择。当下一个视觉问题出现时，像往常一样推送新内容文件。

6. 重复直到完成。

## 编写内容片段

只需编写放入页面内部的内容。服务器会自动将其包装在框架模板中（标题、主题 CSS、选择指示器和所有交互基础设施）。

**最简示例：**

```html
<h2>哪种布局更好？</h2>
<p class="subtitle">考虑可读性和视觉层次</p>

<div class="options">
  <div class="option" data-choice="a" onclick="toggleSelect(this)">
    <div class="letter">A</div>
    <div class="content">
      <h3>单列</h3>
      <p>简洁、专注的阅读体验</p>
    </div>
  </div>
  <div class="option" data-choice="b" onclick="toggleSelect(this)">
    <div class="letter">B</div>
    <div class="content">
      <h3>双列</h3>
      <p>侧边导航与主内容区</p>
    </div>
  </div>
</div>
```

就这些。不需要 `<html>`、CSS 或 `<script>` 标签。服务器提供所有这些。

## 可用 CSS 类

框架模板为你的内容提供以下 CSS 类：

### 选项（A/B/C 选择）

```html
<div class="options">
  <div class="option" data-choice="a" onclick="toggleSelect(this)">
    <div class="letter">A</div>
    <div class="content">
      <h3>标题</h3>
      <p>描述</p>
    </div>
  </div>
</div>
```

**多选：** 在容器上添加 `data-multiselect` 以允许用户选择多个选项。每次点击切换该项。指示栏显示计数。

```html
<div class="options" data-multiselect>
  <!-- 相同的选项标记 — 用户可以选择/取消选择多个 -->
</div>
```

### 卡片（视觉设计）

```html
<div class="cards">
  <div class="card" data-choice="design1" onclick="toggleSelect(this)">
    <div class="card-image"><!-- 原型内容 --></div>
    <div class="card-body">
      <h3>名称</h3>
      <p>描述</p>
    </div>
  </div>
</div>
```

### 原型容器

```html
<div class="mockup">
  <div class="mockup-header">预览：仪表盘布局</div>
  <div class="mockup-body"><!-- 你的原型 HTML --></div>
</div>
```

### 分屏视图（并排）

```html
<div class="split">
  <div class="mockup"><!-- 左侧 --></div>
  <div class="mockup"><!-- 右侧 --></div>
</div>
```

### 优缺点

```html
<div class="pros-cons">
  <div class="pros"><h4>优点</h4><ul><li>好处</li></ul></div>
  <div class="cons"><h4>缺点</h4><ul><li>不足</li></ul></div>
</div>
```

### 模拟元素（线框构建块）

```html
<div class="mock-nav">Logo | 首页 | 关于 | 联系</div>
<div style="display: flex;">
  <div class="mock-sidebar">导航</div>
  <div class="mock-content">主内容区</div>
</div>
<button class="mock-button">操作按钮</button>
<input class="mock-input" placeholder="输入框">
<div class="placeholder">占位区域</div>
```

### 排版和区块

- `h2` — 页面标题
- `h3` — 区块标题
- `.subtitle` — 标题下方的次要文字
- `.section` — 带底部边距的内容块
- `.label` — 小号大写标签文字

## 浏览器事件格式

当用户在浏览器中点击选项时，其交互被记录到 `$SCREEN_DIR/.events`（每行一个 JSON 对象）。推送新屏幕时文件会自动清空。

```jsonl
{"type":"click","choice":"a","text":"选项 A - 简单布局","timestamp":1706000101}
{"type":"click","choice":"c","text":"选项 C - 复杂网格","timestamp":1706000108}
{"type":"click","choice":"b","text":"选项 B - 混合方案","timestamp":1706000115}
```

完整事件流显示用户的探索路径——他们可能点击多个选项后才决定。最后一个 `choice` 事件通常是最终选择，但点击模式可以揭示犹豫或值得追问的偏好。

如果 `.events` 不存在，说明用户没有与浏览器交互——仅使用他们的终端文字。

## 设计技巧

- **根据问题调整保真度** — 布局问题用线框图，细节问题用精细设计
- **在每页解释问题** — "哪种布局更专业？"而非仅"选一个"
- **推进前先迭代** — 如果反馈改变了当前屏幕，写入新版本
- **每屏最多 2-4 个选项**
- **必要时使用真实内容** — 对于摄影作品集，使用真实图片（Unsplash）。占位内容会掩盖设计问题。
- **保持原型简洁** — 专注于布局和结构，而非像素级完美设计

## 文件命名

- 使用语义化名称：`platform.html`、`visual-style.html`、`layout.html`
- 不要重用文件名——每个屏幕必须是新文件
- 迭代时：追加版本后缀，如 `layout-v2.html`、`layout-v3.html`
- 服务器按修改时间提供最新文件

## 清理

```bash
# macOS / Linux
scripts/stop-server.sh $SCREEN_DIR

# Windows
scripts\stop-server.bat %SCREEN_DIR%
```

如果会话使用了 `--project-dir`，原型文件会持久化到 `.booming\brainstorm\` 以供后续参考。只有 `/tmp`（Linux/macOS）或 `%TEMP%`（Windows）会话在停止时被删除。

## 参考

- 框架模板（CSS 参考）：`scripts/frame-template.html`
- 辅助脚本（客户端）：`scripts/helper.js`
