objectId: run-trace-viewer-guide
objectType: guide

# run trace — 跨语言 Trace Viewer

## 目的

`run trace` 是跨语言 JSONL trace 的 CLI 查看器，用于开发调试时快速查看 codegen / runtime 流程中的事件记录。

Trace 数据的三种来源：
- **Python**：`build/toolchains/run/testing/trace.py`，约 30 个 `@trace_call` 调用点
- **C#**：`Chaos.IL2CPP.Diagnostics.ChaosTrace`，约 8 个 `Point()` 调用点
- **C++**：`src/native/common/chaos/trace.h`，ring buffer + atexit 刷出

数据文件位置：
- 自动化会话：`artifacts/logs/trace/<session>/trace.jsonl`
- 手动深度追踪：`artifacts/traces/<family>/<file>.jsonl`

## 用法

```bash
run trace                           # 最新 session 的 tree view（≤7 条）
run trace --file <path>             # 加载任意 JSONL 文件
run trace --file <path> --stats     # 统计摘要（大文件自动触发）
run trace --file <path> --errors    # 仅显示异常
run trace --file <path> --group-by reason   # 按 reason 分组
run trace --file <path> --follow "String::.ctor"   # 按 subject_id 搜索
run trace --file <path> --search o=emitter        # 按字段值过滤
run trace --file <path> --follow "String" --context 3   # 匹配行 ±3 上下文
run trace --file <path> --timeline # 相对时间线（显示 +N.Ns 偏移）
run trace --file <path> --compact  # 紧凑单行模式
run trace --diff <fileA> --diff2 <fileB>    # 两个 trace 对比
run trace --json                     # 原始 JSON 输出
```

## 显示模式

| Flag | 默认触发条件 | 显示内容 |
|------|-------------|---------|
| （none） | 记录 ≤ 50 条 | Tree view（enter/exit 缩进） |
| `--stats` | 记录 > 50 条自动 | 操作统计 + 跳过原因 + 异常汇总 |
| `--group-by` | 指定字段 | 分组 + 每组最多 20 条 |
| `--errors` | 指定 | 异常 type + message + source |
| `--follow` | 指定 | 紧凑模式展示匹配的 subject |
| `--search` | 指定 | 按 key=value 过滤 |
| `--compact` | 指定 | 单行，最多 4 个 KV |
| `--timeline` | 指定 | 时间戳替换为 `+N.Ns` |
| `--context N` | 配合 follow/search/errors | 匹配行前后各 N 条 |

## 操作颜色

| 操作类型 | 颜色 |
|---------|------|
| `skip_method` / `skip_isdefined` | 灰色（ANSI 90） |
| `*:exception` | 红色 |
| `generate.*` / `emit.*` / `complete` | 绿色 |
| `create.*` / `reachable.*` | 青色 |
| 其他 | 白色粗体 |

## 性能

- 大文件（>10 万行）自动触发 `_load_trace_stats` 单趟扫描聚合，不存全部记录到内存
- `--errors` / `--follow` / `--search` 自动关闭 max_records 限制确保不漏匹配

## 实现

- 文件：`build/toolchains/run/commands/trace.py`
- 注册：`build/toolchains/run/manifests/run/commands.trace.json`
- 入口：`run.py` 中 `handler == "trace.dispatch"` 路由到 `trace_commands.handle()`
