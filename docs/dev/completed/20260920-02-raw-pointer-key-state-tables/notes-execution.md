# 执行记录 — 裸托管指针键状态表（W3）

## Step 1: 可复现装置

**文件**：`tests/unit/runtime-native/runtime-core/gc/gc_raw_key_relocation_test.cpp`
（登记于同目录 CMakeLists.txt，target `test_gc_raw_key_relocation`）

三个用例：
- `Sanity_Gen1CollectionMovesObjects` —— 证明 Gen1 收集确实搬移对象（A/B 的前提）
- `A_PinSetKeySurvivesRelocation` —— pin_set 键跟随搬移
- `B_StreamStateKeyFollowsObjectAddress` —— g_stream_state 键跟随搬移

**构建路径判定的两个坑**（已解决）：
1. `tests/contracts/native/runtime-core/` 是**陈旧副本**；权威树是
   `tests/unit/runtime-native/`（根 CMakeLists.txt:138/221 只引入后者）。
2. 独立 `cmake -S tests/unit/runtime-native` 缺 runtime lib target（既有问题，
   对照 `test_gc_gen1` 同样 LNK1181）；必须用**根工程** `cmake -S . -B artifacts/presets/windows-x64-reference`。
3. `stream_stubs.h` 的声明**无** extern "C" 包裹，而定义在 `extern "C" {}` 内
   —— 测试里需 `extern "C" { #include ... }`，否则 LNK2019。

## 装置设计上的两个自我修正

**修正 1 — 不能用 pinned handle 观测搬移**
首版用 `GcCreatePinnedHandle` + `GcGetHandleTarget` 读新地址。实测 `old == new`：
pin 的语义正是「对象不被搬移」，与观测搬移互斥。改用 `IsInGen1()` 判定。

**修正 2 — 局部变量会被后续调用覆盖**
`void* obj` 在 post-collect 打印时值已变（栈槽被复用），使 old/new 比较失去意义。
改用 `static void*` 稳定存储。

## Step 2/3: 修复

### 2a. GC 快路径漏调 RelocateGen1References（🔴 执行中发现的新缺陷）

`gc_gen1.cpp` 三条快路径在 `memcpy` 搬移后直接 `return`，从不调用
`RelocateGen1References`（正常路径 Phase 4e 会调）：

| 路径 | 位置 | 触发条件 |
|:-----|:-----|:---------|
| Tier 1 | span <= 4096 | 极小 Gen1 |
| Tier 2 | 对象数 <= kGen1MinPromoteThreshold | 稀疏大 span |
| drain | bitmap 分配失败 | 内存压力 |

后果：对象搬到 Gen2 新地址，但 old-gen slot / 静态根 / 线程栈 / GCHandle
全部仍指向已腾空的 Gen1 地址（cross-gen UAF）。
**触发门槛极低** —— 测试里 64 字节对象即命中。

修法：三条路径各自收集 moves 并在提交阶段调用 `RelocateGen1References`。

### 2b. pin_set 键失配

`GcSetHandleTarget`（engine_lifecycle.cpp:321-329）**有**正确的 pin_set 同步逻辑，
但**全仓库零调用者**；GC 实际走的 `GcRelocateHandles` 只更新 handle 表。

修法：在 `GcRelocateHandles` 内就地同步 pin_set（仅 pinned/async_pinned），
并更新 `points_to_nursery`。锁序固定 shard → pin_set；**不**嵌套调用
`GcSetHandleTarget`（会重入同一把 shard 锁自死锁）。

### 2c. g_stream_state 键失配

新增通用搬迁通知 `GcRegisterMoveCallback`（gc_events.h）：任何以裸对象指针为键的
子系统注册回调自维护键，**避免 GC 反向依赖各子系统**（保持分层）。
回调在 shard 锁释放后调用。`stream_stubs.cpp` 注册该回调重写键。

## Step 4: 验证

| 测试 | 修复前 | 修复后 |
|:-----|:------:|:------:|
| Sanity_Gen1CollectionMovesObjects | 红* | ✅ |
| A_PinSetKeySurvivesRelocation | 红 | ✅ |
| B_StreamStateKeyFollowsObjectAddress | 红 | ✅ |

\* Sanity 首版因装置设计错误（pin 观测）而红，见上文修正 1。

**回归**：
- `test_gc_gen1` 13/14 —— 1 失败为**预存在**
- `test_gc_events` 7/7 ✅
- `test_gc_managed_api` 10/10 ✅

**预存在失败的判定方法**：`git stash` 我的改动 → 重建 → 干净树复现同样失败
→ 排除「本次引入」（判据见记忆 `preexisting-failure-not-same-as-unrelated`）。

## 提交

`b459a9a50` fix(gc): 快路径漏调 RelocateGen1References + 裸指针键表搬迁维护

## 诚实标注

- `RelocateGen1References` 的引用清单（old-gen slot / 静态根 / 线程栈 /
  GCHandle）来自**代码作者注释**，非我实测
- 本次修复**未验证** end-to-end chunk 影响（本缺陷零 fact 命中，
  装置是主动构造的复现，不等于现网已发生）
- `AddPinnedRoot()` 亦零调用者（`pinned_roots_` 恒空）—— **GC 域独立问题，
  不在本次范围**，仅记录
