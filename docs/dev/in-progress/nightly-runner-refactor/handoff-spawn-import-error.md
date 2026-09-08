# Handoff — Nightly Runner spawn ImportError (Windows multiprocessing)

## 交接给谁
CI / devops 专家 agent（或 nightly_runner 维护者）。

## 一句话 TL;DR
`nightly_runner.main` 在 Windows 上因 `multiprocessing spawn` 子进程找不到 `verification.nightly_runner` 模块而挂起/失败。不是 path 参数问题，是 multiprocessing 跨进程模块解析的 base issue。

## 症状
```
ImportError: Error while finding module specification for 'verification.nightly_runner.main'
(ModuleNotFoundError: No module named 'verification.nightly_runner')
```
在 `ProcessPoolExecutor` 的 worker 子进程中发生，父进程正常。

## 根因
Windows 的 `multiprocessing` 默认使用 `spawn` 模式（而非 `fork`），每次启动 worker 子进程时重新 import `__main__`（即 `verification.nightly_runner.main` 模块）。子进程不继承父进程的 `sys.path`，而 `verification/` 包位于 `tests/e2e/` 下，不在默认的 Python 模块搜索路径中。

父进程通过 `-m verification.nightly_runner.main` 启动时，`-m` 把入口模块所在目录加入 `sys.path`，但 spawn 的子进程不会重复这个 `-m` 行为。

## 复现方式
```bash
export PYTHONPATH=/d/agent/chaos-il2cpp/tests/e2e
export CHAOS_FOUNDATION_DLL=/d/agent/chaos-il2cpp/tests/e2e/translation
python -m verification.nightly_runner.main --assembly System.ObjectModel --max-workers 2 --no-logs --no-profile
```
父进程启动正常，worker 子进程（约 10s 后）报 ImportError 挂起。

## 已尝试的方案
| 方案 | 结果 |
|------|------|
| `ProcessPoolExecutor(initializer=_pool_initializer)` 在 worker 中追加 sys.path | ❌ 太晚 — `_fixup_main_from_name` 在 initializer 之前运行 |
| 父进程 `os.environ["PYTHONPATH"]` 注入 | ⚠️ 未验证 — 最可能有效 |
| revert 回 `nightly_build.py`（单进程 + subprocess 调用） | 可行但损失并行 |

## 建议修复方向
### 方案 A（推荐，最小改动）
在 `nightly_runner/config.py` 或 `main.py` 的 `main()` 入口处，**启动 `ProcessPoolExecutor` 前** 设置 `os.environ["PYTHONPATH"]` 为包含 `tests/e2e` 的路径字符串。子进程继承 `PYTHONPATH` 环境变量后，`verification.*` 包可解析。

```python
# 在 main() 中，创建 config 后、调 orchestrator.run() 前：
_e2e_root = Path(__file__).resolve().parents[1]  # tests/e2e/
current = os.environ.get("PYTHONPATH", "")
if str(_e2e_root) not in current:
    os.environ["PYTHONPATH"] = str(_e2e_root) + (os.pathsep + current if current else "")
```

### 方案 B（架构级）
将 nightly_runner 改为不使用 `ProcessPoolExecutor`，而是通过 `subprocess.Popen` 并行启动多个 `python -m verification.chunk_pipeline` 子进程（每个 chunk 一个进程）。这样每个子进程都通过 `-m` 入口启动，`sys.path` 自然正确。

### 方案 C（降级）
如果修复跨进程模块解析的代价太大，可退回到 `nightly_build.py`（单进程主循环，不并行），将 nightly_runner 的日志/报告功能保留，但并行执行留给后续 CI 重构。

## 文件索引
- `tests/e2e/verification/nightly_runner/main.py` — CLI 入口
- `tests/e2e/verification/nightly_runner/orchestrator.py` — ProcessPoolExecutor 使用处（3 处）
- `tests/e2e/verification/nightly_build.py` — 原始单进程入口（可作为回退验证基准）