# pre-push 深度评审判据（本 hook 专用严重度）

> 严重度用 **L1–L4**，避免与仓库既有的"优先级 P* / CodeGen 覆盖分层 P* / 路线图优先级 P*"命名冲突。
> `git push` 被阻断仅当存在 ≥1 个 **L1 或 L2** 问题。
> 此文件是唯一权威判据；`review_runner.py` 将它与 diff 一同交给 Claude 评审。

## L1 — CRITICAL（阻断 push，必改）
- 崩溃 / 段错误 / 死锁 / 数据损坏 / 安全漏洞
- 泄露凭据或密钥：AKIA…, AIza…, sk-…, ghp_…, 私钥块
- 测试框架 / 编译 / CI 主链整体破坏（dotnet 崩溃、4 层越界无 BOUNDARY_OVERRIDE、no-skip 被违反）
- 误删源码或合法依赖；把 `.dll/.o/.a/bin/obj` 构建产物提交进 index
- 内存：越界写、悬垂指针、alloc/free 跨域不匹配（GC↔heap↔raw 混用）

## L2 — SEVERE（阻断 push，须修后重审）
- 确定的逻辑错误：空指针解引用、竞态、资源泄漏、错误分支
- 意外改变 ABI / 公共 API 行为（签名、布局、序列化格式、枚举值）
- GC 域违规：绕过统一内存约束（CHAOS_IL2CPP_ 宏）、全局 operator new/delete 重载
- 大文件（>5MB）或明显误提交的生成物进入 diff
- 明显的数据竞态 / 线程安全回归 / EH 深度不安全

## L3 — WARNING（不阻断，报告）
- 风格 / 命名不一致、死代码、明显可维护性问题
- 调试残留（printf / console.log / Debug.WriteLine / 硬编码临时值未清理）

## L4 — SUGGESTION（不阻断，仅记录）
- 优化机会 / 重构 / 更清晰写法 / 可加注释处

## 判定规则
- `verdict=block` ⇔ 存在 ≥1 个 L1/L2。
- 3 个及以上 L3 可视为风险信号，报告但默认不阻断（由触发者决定）。
- **允许例外**：被构建管线引用的合法 `.dll`（src/dll、crypto-refs、third_party/mono.cecil）是合法依赖，不是误提交——需结合 project_rules 判断。

## 输出（必须为单行 JSON）
```json
{"verdict":"allow|block","severity":"L1|L2|L3|L4|none","issue_count":N,
 "issues":[{"file":"相对路径","line":N,"severity":"L1","msg":"一句话描述"}], "summary":"1-2 句总评"}
```
- `verdict` 必须显式给出；不确定时倾向 `block`（fail-closed）。
