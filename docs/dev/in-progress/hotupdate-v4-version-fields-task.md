# PatchDataHeader v4 版本兼容字段 — 设计任务书

## 背景

方案1 阶段1+2 已交付（宿主 API + 事务性 apply），但版本兼容字段(`min_host_revision`/`patch_revision`) 尚未实现。这是**二进制格式 v3→v4 升级**，需要两端(C++ loader + C# producer) 逐字节对齐，且保证向后兼容(v3 老 .patchdata 永远可读)。

## 设计约束

### 必须满足的 4 个条件

| # | 条件 | 违反后果 |
|---|------|---------|
| 1 | 现有 v3 .patchdata 在新 loader 上必须仍然可读 | 历史所有 family 的 patch blob 损坏 = 所有 hotupdate 测试崩 |
| 2 | v4 .patchdata 在旧 v3 loader 上必须能被识别并优雅拒绝(而非段错误) | 旧版本宿主加载新 patch 后崩溃 |
| 3 | C++ `PatchDataHeader` 与 C# `FileHeader` 内存布局逐字节一致 | 序列化/反序列化错位 → 读 garbage |
| 4 | `ChaosApplyPatch` 的 `min_host_revision` 检查必须与 `ChaosSetHostRevision` 一致 | 宿主设置版本号但 loader 不检查 = 等于没加 |

### 向后兼容策略

**Trailing optional fields（推荐，不改变 v3 字段位置）**：

```
v3 struct:
  [dependency_offset]  [dependency_count]  // 结束, header_size = 116

v4 struct:
  [dependency_offset]  [dependency_count]  [min_host_revision]  [patch_revision]
  // header_size = 124
```

- **v3 reader 读 v4 blob**: `header_size` = 124 ≥ sizeof(v3 struct) = 116 → 通过检查。
  v3 reader 不访问 `min_host_revision`/`patch_revision`（它们通过 v3 struct 边界外），
  所以不会读错。写入的 `dependency_count` 字段位置不变。
- **v4 reader 读 v3 blob**: `header_size` = 116 < sizeof(v4 struct) = 124 → 代码应
  使用 `min_host_revision = 0`(默认兼容任何 host), 不读取越界内存。

---

## 实现清单

### 1. C++ `contracts/native/v0/patch_data.h` — 2 行

```c
// v4: same as v3 but adds min_host_revision and patch_revision
#define PATCH_DATA_VERSION   4u

// 在 PatchDataHeader 末尾 dependency_count 之后追加:
uint32_t min_host_revision;   // v4+
uint32_t patch_revision;      // v4+
```

**注意**：`sizeof(PatchDataHeader)` 在 v3 = 116, v4 = 124。`min_header` 校验在 `patch_loader.cpp` 用的是 `sizeof(PatchDataHeader)`，所以 v4 自动受益。

### 2. C# `PatchDataExtractor.cs` — FileHeader + serialization

```csharp
// 在 FileHeader struct 末尾追加:
public uint min_host_revision, patch_revision;

// 在 SerializeHeader 中:
version = 4,
min_host_revision = config.MinHostRevision,  // 从配置或命令行传入
patch_revision = config.PatchRevision,
```

**关键**：`FileHeader` 必须加 `[StructLayout(LayoutKind.Sequential)]` 保证字段序与 C++ 一致。当前无此属性，但 `Marshal.SizeOf<FileHeader>()` 在不连续布局下可能给出错误大小——需要加这个属性，且验证后进行 round-trip 测试。

### 3. C++ `patch_loader.cpp` — 兼容性读取

在两个 `ApplyPatchFromMemory` / `ApplyPatchFromMemoryEx` 的入口处，在 Phase 1 验证(事务性 dry-run)内加版本检查：

```cpp
// 在 Phase 1 验证开始时(header 已通过 magic/version/size 校验):
if (header->version >= 4) {
    // 安全读取 trailing 字段: 仅当 header_size 足够大时
    if (header->header_size >= offsetof(PatchDataHeader, patch_revision) + sizeof(uint32_t)) {
        uint32_t host_rev = g_host_revision.load(std::memory_order_relaxed);
        if (host_rev > 0 && header->min_host_revision > host_rev) {
            // patch 要求的最小版本 > 宿主版本 → 拒绝
            return nullptr;  // caller 映射为 CHAOS_PATCH_ERR_VERSION_MISMATCH
        }
    }
    // 若 header_size 不够大(v3 blob 被误标 version=4 的极端情况): 安全降级, 不检查
}
```

### 4. C++ `hotpatch_api.cpp` — 错误码映射

`ChaosApplyPatch` 已定义 `CHAOS_PATCH_ERR_VERSION_MISMATCH` (-2)。当前实现返回 `PARTIAL_ROLLBACK`。阶段3 需要让 loader 的版本检查结果映射到 -2：

```cpp
// ApplyPatchFromMemoryEx 返回 nullptr 时区分原因:
// 现在统一返回 nullptr。需要加一个区分机制，比如:
//   1. SetLastError 风格的全局变量(不推荐)
//   2. 内部返回码枚举(推荐，但改动较大)
// 最小方案: 日志中打印 WARNING, 仍然返回 -4, 但日志可查
```

### 5. 测试: round-trip 序列化验证

**新增文件**: `tests/unit/runtime-native/hot-update/hotpatch_api_version_roundtrip_test.cpp`

```cpp
// 1. 构造已知 v4 header 的 byte buffer (手写 bytes)
// 2. 验证 header->magic, version, min_host_revision, patch_revision 字段正确
// 3. 构造 v3 格式 byte buffer (size = 116)
// 4. 验证 loader 接受 v3 blob (向后兼容)
// 5. 构造 v4 格式 byte buffer, 设 min_host_revision > host_revision
// 6. 验证 ChaosApplyPatch 返回 CHAOS_PATCH_ERR_VERSION_MISMATCH
```

### 6. C# 端 PatchDataExtractor 命令行参数

当前 `PatchDataExtractor.Extract()` 无 `min_host_revision` / `patch_revision` 参数。需要加：

```csharp
public void Extract(string dllPath, string outputPath, ...,
    uint minHostRevision = 0, uint patchRevision = 0)
```

并在 `SerializeHeader` 中写入。

---

## 实现顺序

### Phase 1: 格式对齐 + 向后兼容 (核心)

| 步骤 | 文件 | 行数 | 风险 |
|------|------|------|------|
| 1.1 C++ PatchDataHeader 加 v4 字段 | `contracts/native/v0/patch_data.h` | ~5 | 低(已有草稿) |
| 1.2 C# FileHeader 加字段 + StructLayout | `PatchDataExtractor.cs` | ~5 | 低 |
| 1.3 C# SerializeHeader 写 version=4 + 新字段 | `PatchDataExtractor.cs` | ~10 | 低 |
| 1.4 C# Extract() 参数加 minHostRevision/PatchRevision | `PatchDataExtractor.cs` | ~10 | 低 |
| 1.5 两端 sizeof 验证: C++ static_assert + C# 单元测试 | 新增测试 | ~20 | 中(必须一致) |

### Phase 2: Loader 版本校验 (核心)

| 步骤 | 文件 | 行数 | 风险 |
|------|------|------|------|
| 2.1 ApplyPatchFromMemoryEx 加 version >= 4 兼容读 | `patch_loader.cpp` | ~15 | 中(越界安全) |
| 2.2 v3 blob 兼容性确认(header_size 判别) | `patch_loader.cpp` | ~5 | 高(必须 test) |
| 2.3 ChaosApplyPatch 返回 -2 映射 | `hotpatch_api.cpp` | ~5 | 低 |

### Phase 3: Round-trip 测试 (验证)

| 步骤 | 文件 | 行数 | 风险 |
|------|------|------|------|
| 3.1 手写 v4 byte buffer → 解析验证字段 | `hotpatch_api_version_roundtrip_test.cpp` | ~40 | 低 |
| 3.2 v3 byte buffer 兼容性验证 | 同上 | ~20 | 低 |
| 3.3 min_host_revision > host_revision → -2 验证 | 同上 | ~20 | 低 |
| 3.4 回归: 阶段0 全部 47 测试 + 新测试 | 全跑 | — | 门禁 |

---

## 风险评估

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| C# FileHeader 无 `[StructLayout]` 导致字段序不同 | **高** | 加 `[StructLayout(LayoutKind.Sequential)]` + round-trip 测试验证 |
| C++ sizeof(PatchDataHeader) 因 padding 不是 4n | **中** | 全是 uint32_t, 无 align 问题；static_assert(sizeof == 124) 守护 |
| v3 reader 读 v4 blob 时访问越界 memory | **中** | `min_header` 检查用 `header_size` 而非 sizeof；若写 v4 时 header_size 正确 + v3 不访问 trailing 字段 → 安全 |
| 现有 v3 磁盘 blob 太多 | 低 | 不改旧文件，只改新生产代码 |

---

## 总改造量估计

| 语言 | 文件 | 新增/修改 |
|------|------|----------|
| C++ (.h) | 1 | ~5 行 |
| C++ (.cpp) | 2 | ~25 行 |
| C# (.cs) | 1 | ~25 行 |
| 测试 (C++) | 1 | ~80 行 |
| **总计** | **5** | **~135 行** |

---

## 是否存在更简单的替代方案？

| 方案 | 改造量 | 风险 | 是否推荐 |
|------|--------|------|---------|
| **v4 trailing fields** (本设计) | ~135 行 | 中 | ✅ 推荐 |
| 不升级二进制格式，用外部元数据文件 `.patchdata.meta` 带上版本号 | ~50 行 | 低 | ⚠️ 可行但引入附属文件，违背"自描述 .patchdata"目标 |
| 不增加版本字段，由宿主在 `ChaosSetHostRevision` 时检查 patch 文件名约定 | 0 行 | 高 | ❌ 无强制，容易忘 |

---

## 未解决的问题

1. **C# FileHeader 当前无 `[StructLayout(LayoutKind.Sequential)]`** — 低风险但需验证。
   `Marshal.SizeOf<FileHeader>()` 在 auto-layout 下可能返回非预期值。应先加 `[StructLayout]`
   并验证 v3 round-trip 不变，再叠加 v4 字段。

2. **Phase 2 中 loader 拒绝版本后如何给 ChaosApplyPatch 返回 -2** — 当前 ApplyPatchFromMemoryEx
   返回 nullptr 时 ChaotApplyPatch 返回 -4。需要区分"格式错误"和"版本不匹配"。最小方案：
   在 Phase 1 验证中打印 DIAG 日志，ChaosApplyPatch 在 header 解析成功但 ctx 为 null 时
   检查 `header->version >= 4 && header->min_host_revision > g_host_revision` 返回 -2。

3. **`ChaosSetHostRevision` 已实现但无宿主初始化流程** — 需宿主在启动时调一次。
   当前是设计选择，不是实现 gap。

---

## 批准

| 步骤 | 状态 |
|------|------|
| 整体设计 | 待你批 |
| Phase 1 格式对齐 | 关联 |
| Phase 2 Loader 校验 | 关联 |
| Phase 3 Round-trip 测试 | 关联 |