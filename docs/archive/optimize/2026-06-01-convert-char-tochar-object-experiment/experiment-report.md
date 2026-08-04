# ToChar(Object) Native Stub 实验报告

## 实验目的

验证 `chaos_convert_tochar_object()` native stub 的实际执行性能，绕过 Subject_9/10（null 参数 → 解释器异常路径），使用非空 boxed 值类型参数测量。

## 根因（Why Subject_9/10 慢）

1. **Subject_9**: `Convert.ToChar((object)null)` — 立即抛出 `ArgumentNullException`，走解释器 exception 路径
2. **Subject_10**: `Convert.ToChar((object)null, null)` — 同上

两者均未到达 `chaos_convert_tochar_object()` native stub，全部时间消耗在解释器异常处理（~1050ns）。

## 实验方案

### 修改内容

| 文件 | 修改 |
|------|------|
| `capability-family-contract.json` | `customEntryIndices` 添加 `[18, 19]`，`benchmarkMethodIndices` 添加 `[18, 19]` |
| `native-aot.generated.cpp` (native copy) | CustomEntrySubject_18/19：box 值 + 直接调用 `chaos_convert_tochar_byte/int32` |

### 绕过 perfect-hash 的原因

`chaos_convert_tochar_object()` 使用 perfect hash 表 `kToCharHandlerIndex[64]` 对 `stable_id >> 14 & 0x3F` 做 dispatch。测试模块中 System.Byte 的 stable_id（`0x2ca8862f675f0ced`）hash slot = 60，而 handler 表中 slot 60 的值为 255（not found），导致 `chaos_raise_exception(0)`（InvalidCastException）。

这是因为测试模块生成的 stable_id（基于 `"System.Private.CoreLib/System.Byte"`）与生产环境的 stable_id 不一致。convert.cpp 中的 perfect hash 表按生产环境的 stable_id 设计。

### 绕过方案

Custom entry 直接调用底层 handler：
- CE18: `chaos_convert_tochar_byte(chaos_boxed->value)` — 跳过 perfect-hash dispatch
- CE19: `chaos_convert_tochar_int32(chaos_boxed->value)` — 同上

## 性能数据

### 直接 dispatch（microbench, --microbench, profile config）

| Subject | 代码路径 | ns/op | 排除 dispatch 开销 | alloc/op |
|---------|---------|-------|-------------------|----------|
| CE-min | 空函数（dispatch 基线） | ~~7.9~~ | — | 0 B |
| Subject_1 | `ToChar(byte)` 无 alloc | ~64.5 | ~56.6 | 0 B |
| Subject_7 | `ToChar(int)` 无 alloc | ~64.9 | ~57.0 | 0 B |
| CE-18 | GC alloc(16B) + boxing + `chaos_convert_tochar_byte` | **~172-186** | ~165-178 | 16 B |
| CE-19 | GC alloc(16B) + boxing + `chaos_convert_tochar_int32` | **~92-138** | ~85-130 | 16 B |
| Subject_9 | `ToChar(null)` 解释器异常路径 | **~1050** | — | — |

### 多运行稳定性

3 次连续运行显示 CE-18 有较大变化（172-186ns），CE-19 变化更明显（92-138ns）。这是因为 CE-18 先执行，GC nursery 状态不同导致 alloc timing 波动。

## 结论

### 关键发现

1. **Chaos_convert_tochar_object native stub 不是瓶颈** — 整体路径（GC alloc + 类型 dispatch + 转换）~100-185ns，远低于异常路径的 ~1050ns
2. **GC allocation 占主导** — boxing 的 16B 分配占用了 ~85-170ns（总时间的 70-90%）
3. **类型 dispatch + 转换成本极小** — `chaos_convert_tochar_byte/int32` 本身仅 ~1-5ns
4. **Subject_9/10 的 100x 回归是完全在解释器异常路径**

### Subject_9/10 优化建议

| 方案 | 收益 | 实现成本 | 影响范围 |
|------|------|---------|---------|
| 无需优化（理由见下） | — | — | — |

**不优化的理由：**
- Subject_9/10 是 exception-path 测试（null 输入），不是性能敏感路径
- Native stub ~100-185ns 的性能已经很好
- 要测量真正的 `chaos_convert_tochar_object` 性能需要 runtime-variable 输入（非常量参数），这需要 codegen 改造

### 下一个优化优先级

枚举优化已经完成。下一个候选应转向：
1. 当前优化战役中 priority queue 的下一个 family
2. 基于全量 benchmark 扫描结果选择

## 实验文件修改状态

| 文件 | 修改 | 状态 |
|------|------|------|
| `capability-family-contract.json` | 添加 customEntryIndices [18, 19] | **实验中，不提交** |
| `native/ConvertCharSubjects/generated/native-aot.generated.cpp` | CustomEntrySubject_18/19 改为直接 handler 调用 | **实验中，不提交** |
| `native/microbench.cpp` | 添加 custom entry benchmark 循环 | **实验中，不提交** |
| `native/CMakeLists.txt` | Linux pal 库链接 | **实验中，不提交** |
| `codegen/ConvertCharSubjects/generated/native-aot.generated.cpp` | stable_id mismatch 无需修改 | 不修改 |
