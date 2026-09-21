# 端到端验证：跨代引用在 Gen1 搬移后的正确性

> 日期：2026-09-20
> 装置：`tests/unit/runtime-native/runtime-core/gc/gc_gen1_crossgen_ref_test.cpp`
> target：`test_gc_gen1_crossgen_ref`

## 结论

**cross-gen UAF 确凿存在，且已被 `b459a9a50` 的修复解决。**

双向确认：

| 版本 | 结果 |
|:-----|:-----|
| 修复前（回退 4 个 GC 文件） | ❌ `Gen2HolderSeesRelocatedGen1Target` 失败 |
| 修复后（HEAD） | ✅ 2/2 通过 |

## 场景

```
1. 在 Gen2 分配 holder，打身份图案 0x60DE0000
2. 在 Gen1 分配 target，打身份图案 0x7A26E000
3. holder 的第一个 payload 槽引用 target   ← 跨代边（Gen2 → Gen1）
4. 用 nursery 引用保持 target 存活
5. 触发 GcGen1Collection() → target 走 early_exit 被 promote 到 Gen2 新地址
6. 🔴 复用 Gen1 内存（分配 filler 并填 0xBADBAD00）
7. 通过 holder 的跨代边读回 target，校验身份图案
```

## 🔴 关键：第 6 步「复用内存」是测试有效性的前提

**这是本验证最重要的发现。** 前几轮尝试都得出「修复前后无差异」的假阴性，
原因就在这里：

`GcGen1Collection()` 的快路径只做 `gen1_bump.store(gen1->begin)` ——
**重置 bump 指针，不清零腾出的内存**。因此一条**未重写**的跨代边在内存被
再次分配之前，仍能读到原对象的字节，**看起来完全正确**。

实测诊断（修复前）：

```
[DIAG] BEFORE: holder 的槽 = ...900048   (旧地址)
[DIAG] AFTER : holder 的槽 = ...900048   (未重写，未变)
[DIAG] reused Gen1 area with 4 fillers (pattern=0xBADBAD00)
→ 通过 holder 读到 0xBADBAD00  ← UAF 实证
```

修复后同位置：

```
[DIAG] AFTER : holder 的槽 = ...20ECB0   (已被 Phase 1 重写为新地址)
→ 复用内存后通过 holder 读回正确对象 0x7A26E000
```

**教训**：验证 UAF 类缺陷时，「释放/搬移」与「观测」之间必须有**内存复用**，
否则读到的是尚未被覆盖的陈旧数据，测试会给出假阴性。
（`memcpy` 式搬移尤其如此 —— 源内存内容天然保留。）

## 装置设计上踩过的两个坑（均已修正）

1. **不能用 `via_holder != target` 判断是否搬移**
   `target` 是栈局部变量，保守栈扫描（`RelocateGen1References` Phase 3）会把它
   当根并在搬移后一并更新，两者在搬移后仍可能相等。
   判据必须是**通过跨代边读到的对象身份**。

2. **只用 `CheckPattern` 而不用裸地址比较**
   `same=1`（`via_holder == target`）**不代表**没搬移 —— 见上一条。

## 诚实标注

- 本验证证明的是「快路径漏调 `RelocateGen1References`」这一具体缺陷；
  **不外推**到其他 GC 路径
- fact 层仍为**阴性**（零差异）—— 因为 fact 负载不触发 Gen1 收集
  （见 `fact-layer-impact.md`）。本装置是**主动构造**的复现场景
- `early_exit` 只重置 bump 指针、不清零内存这一行为本身**未作为缺陷处理** ——
  它在语义上没问题（内存未回收），只是让 UAF 更隐蔽

## 复现命令

```bash
cmake -S . -B artifacts/presets/windows-x64-reference
cmake --build artifacts/presets/windows-x64-reference --config Debug \
  --target test_gc_gen1_crossgen_ref
./artifacts/presets/windows-x64-reference/tests/unit/runtime-native/runtime-core/gc/Debug/test_gc_gen1_crossgen_ref.exe
```

回退验证（确认修复前必红）：
```bash
for f in src/native/runtime-core/gc/gc_gen1.cpp src/native/runtime-core/gc/gc_events.h \
         src/native/runtime-core/core/engine_lifecycle.cpp \
         src/native/runtime-core/runtime_stubs/stream_stubs.cpp; do
  git checkout b459a9a50~1 -- $f
done
# 重建并跑 → Gen2HolderSeesRelocatedGen1Target 应失败
```
