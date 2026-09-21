# W3 fact 层影响验证（阴性结果）

> 日期：2026-09-20
> 目的：验证 GC 快路径重定位修复 + 裸指针键表维护在 fact 层的实际影响

## 结论

**零差异** —— 修复前后 XML chunk 的 746 个 subject 逐项完全一致：

| 指标 | 修复前 | 修复后 |
|:-----|:------:|:------:|
| passed | 590 | 590 |
| failed | 96 | 96 |
| realTotal | 232 | 232 |
| realVerified | 136 | 136 |
| stubGap | 185 | 185 |
| unassertable | 173 | 173 |
| assertionFailed | 11 | 11 |

**逐 subject 差异数 = 0**（非"改善与回退相抵"，是完全无差异）。
`only-before = 0`，`only-after = 0`。

## 机制解释（为什么必然为零）

fact 运行**不触发 Gen1 收集**：

1. `entry.exe` 的 stderr 中**零条** `[CRAG]` / `gen1_collection` 日志
   （对照：GC 配置、外部 fallback 等日志均有输出，说明日志通道正常，
   是**真的没发生**而非"没打印"）
2. Gen1 触发门槛为占用 >80% **或**碎片 >50%
   （`gc_gen1.cpp:103/109`）
3. fact 负载是「每个 subject 独立构造对象 → 断言 → 丢弃」，
   短生命周期小对象，Gen1 占用与碎片都达不到门槛

**因此**：
- `RelocateGen1References` 从未被调用 → 快路径重定位修复无从生效
- 对象从未被搬移 → `pin_set` / `g_stream_state` 的键从未失效

## 这个阴性结果的价值

1. **确认修复无副作用** —— 零差异本身就是回归证据：GC 核心路径改动
   没有破坏任何既有行为
2. **界定了缺陷的真实暴露面** —— 本缺陷在 fact 验证体系下**不可见**，
   只有长生命周期 / 高分配负载（真实游戏运行时）才会触发
3. **解释了为何此前未暴露** —— 不是"运气好"，是 fact 负载模型天然规避

## 诚实标注

- 本缺陷**仍然只有装置级证据**（`gc_raw_key_relocation_test.cpp`），
  **无端到端证据**。fact 层验证给出的是「无影响」而非「已修复」
- 未验证其他 chunk（threading 等）是否触发 Gen1 收集 —— 它们分配压力可能
  更大，理论上更可能触发。**本结论不外推**
- 若需端到端证据，需要构造高分配压力场景（如 GC 压力测试 + 长生命周期对象），
  当前 fact 框架不提供

## 附：验证命令

```bash
# 同步 SDK（必须先做，否则 pipeline 跑旧 lib）
python tests/e2e/translation/artifacts/build_presets.py --preset windows-x64-reference

# 跑 chunk
CHAOS_FOUNDATION_DLL=$(pwd)/testing/foundation-dll PYTHONPATH=$(pwd)/tests/e2e \
  python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.Xml --chunk xml --stages build,fact --native-config check

# 检查 Gen1 是否触发（关键判据）
./artifacts/foundation-dll/System.Private.Xml/chunks/xml/native/entry.exe --fact-json 2>&1 >/dev/null | grep -c CRAG
# 期望 0 —— 即 Gen1 收集未发生
```
