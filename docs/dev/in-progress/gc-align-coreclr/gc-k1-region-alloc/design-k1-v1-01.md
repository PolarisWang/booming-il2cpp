# Design — GC-K1：Region Allocator 地基（对齐 CoreCLR region）

> 版本：design-k1-v1-01
> 创建：2026-08-10
> 上游：`docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md`（Phase 5, GC-K1）
> 对标基准：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\region_allocator.cpp` + `interface.cpp:455`（basic region 4/2/1MB 自适应）
> **性质**：Phase 5 区域化重构的第一步，**跨会话开放起步**。本次实现可独立验证的地基，非一次性完成整个区域化。

---

## 1. 现状（CRAG RegionManager::AllocateRegion）

`gc_region.cpp:700` 按 kind 用固定 region_size：
- NURSERY: kDefaultRegionSize (512KB)，GEN1: 64MB，TENURED: 1MB，DOMAIN: 64KB，POH: kPoh。
- `min_size` 覆盖，free-list 复用，否则 VirtualAlloc。

**无自适应 region 大小**，无 CoreCLR 式"按负载选 4/2/1MB class"。

## 2. CoreCLR 要点（对齐）

`interface.cpp:455-467`：basic region 大小自动择 **4/2/1 MB**（未显式设置时 `max_region_size>=4MB→4MB`）。
`region_allocator.cpp:19`：large region = `LARGE_REGION_FACTOR × alignment`，UOH(LOH/POH) 用 large region，SOH 用小 region。

## 3. GC-K1 地基（本次实现）

**新增 `RegionManager::SelectRegionSize(kind, min_size)` 自适应 helper** —— 引入 CoreCLR 式 region size class，同时**保留既有 kind 大小不破坏行为**：

```
SelectRegionSize(kind, min_size):
  - NURSERY / DOMAIN / POH / GEN1: 保持既有固定大小（不破坏现行分配）。
  - TENURED / oversized (min_size 大): 按 CoreCLR 4/2/1MB class 选：
       min_size > 2MB  -> 4MB
       min_size > 1MB  -> 2MB
       否则            -> 1MB (kTenuredRegionSize)
       return max(selected, min_size)
```

**接入 `AllocateRegion`**：TENURED/oversized 路径的 region_size 改用 `SelectRegionSize`（替代固定 kTenuredRegionSize 或裸 min_size）。

**为什么这是"地基"而非完整区域化**：完整 CoreCLR region 需 LOH-via-region、世代写屏障 region→gen 映射、分代重绑。本增量只引入 **region size class 概念 + 自适应选择**，作为后续 K2(世代写屏障)/K3(分代重绑)/LOH-via-region 的可验证基础。

## 4. 验收（exit criteria）

- [ ] `chaos_runtime_core` Debug 编译通过
- [ ] TENURED/oversized AllocateRegion 用 SelectRegionSize（4/2/1MB class）
- [ ] NURSERY/POH/DOMAIN/GEN1 大小不变（无回归）
- [ ] GC 单测批无回归

## 5. 备选

若 SelectRegionSize 影响现有行为，降级为"仅新增 helper 不接入 AllocateRegion"，列为后续 K1b 接线。**推荐接入**（TENURED 用 1MB 本就是 kTenuredRegionSize，SelectRegionSize 对 1MB 返回 1MB，行为等价；仅 oversized 时升类）。
