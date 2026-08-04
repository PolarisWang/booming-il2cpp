# clib-iw-a1: Core Type Tests — type_info + native_types 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。若当前任务带有 `parent_task_id`（commonlib-industrialization）和 `source_relation: roadmap-child*`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 为 `type_info.h` (268行) 和 `native_types.h` (411行) 编写单元测试，覆盖 static_assert 布局验证、类型大小、宏正确性和模板函数行为。

**架构：** 纯 GoogleTest 单元测试，链接 chaos_common 库。测试文件遵循已有模式 (`common_json_reader_test.cpp`)，命名 `common_*_test.cpp`。

**技术栈：** GoogleTest, chaos_common (仅头文件), fmtlib

**架构审核模式：** normal

**结构告警重点：** 与已有测试模式保持一致，不新增测试框架抽象，不重复测试逻辑。

**权责图审核主题：** n/a

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: n/a
- capabilityItem: n/a
- ownerSubjectId: n/a
- proofRequired: false
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: []
- requiredGates: []

**设计文档：** n/a (父 roadmap 提供充分上下文)

**问题清零来源：** parent-roadmap (commonlib-industrialization roadmap-v1-01)

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a (测试代码不产生长期文档知识)

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 回写父任务"固定链路。

---

## 任务列表

### 文件列表

| 文件 | 职责 | 操作 |
|------|------|------|
| `testing/src/native/common/common_native_types_test.cpp` | 新增 — native_types.h 测试 | 新建 |
| `testing/src/native/common/common_type_info_test.cpp` | 新增 — type_info.h 测试 | 新建 |
| `testing/src/native/common/CMakeLists.txt` | 添加两个测试目标 | 修改 |

### 任务 A1a: 创建 common_native_types_test.cpp

#### 测试用例

1. **TypeSizes** — 验证每个 CHAOS_IL2CPP_INT8/16/32/64/UINT8/16/32/64 的 `sizeof ==` 预期值
2. **NumericLimitsMinMax** — 验证 CHAOS_IL2CPP_NUMERIC_LIMITS_MIN(INT32) == -2147483648, MAX(INT32) == 2147483647 等
3. **ExplicitMinMax** — 验证 CHAOS_IL2CPP_INT32_MIN/MAX, INT64_MIN/MAX, UINT64_MAX 等宏展开正确
4. **TypeEquivalence** — 验证 CHAOS_IL2CPP_IS_SAME(CHAOS_IL2CPP_INT32, int32_t) 等
5. **PauseHintCompiles** — CHAOS_IL2CPP_PAUSE_HINT() 编译期验证（不执行）
6. **AtomicIntrinsics** — 验证 ATOMIC_INC/DEC/XCHG/CAS/ADD 在 MSVC 下可编译
7. **ResolveNativeIntSlot** — chaos_resolve_native_int_slot 指针转换
8. **LoadIndirect** — chaos_load_indirect 带/不带 tag、零地址保护
9. **StoreIndirect** — chaos_store_indirect 带/不带 tag、零地址保护
10. **RawPointerTag** — CHAOS_IL2CPP_RAW_POINTER_TAG 值验证
11. **FailMacro** — CHAOS_IL2CPP_FAILED/SUCCEEDED 宏
12. **MemFuncs** — CHAOS_IL2CPP_MEMCPY/MEMCMP/MEMSET 编译期验证
13. **IntptrMatch** — CHAOS_IL2CPP_INTPTR 和 intptr_t 大小一致

#### 验证

```bash
# 编译
cmake --build build/testing --target test_common_native_types --config RelWithDebInfo
# 运行
build/testing/common/RelWithDebInfo/test_common_native_types.exe
```

### 任务 A1b: 创建 common_type_info_test.cpp

#### 测试用例

1. **TypeInfoHotSize** — static_assert `sizeof(TypeInfoHot) == 32`
2. **TypeInfoWarmSize** — static_assert `sizeof(TypeInfoWarm) == 32`
3. **TypeInfoV0Size** — static_assert `sizeof(TypeInfoV0) == 64`
4. **TypeInfoV2Size** — static_assert `sizeof(TypeInfoV2) == 112`
5. **InterfaceMapEntrySize** — static_assert `sizeof(InterfaceMapEntry) == 16`
6. **MethodTableSize** — static_assert `sizeof(MethodTable) == 64`
7. **MethodTableOffsetMatch** — 验证 offsetof(MethodTable, parent_mt) == offsetof(TypeInfoHot, parent) 等全部 7 个对齐断言
8. **GetWarmPtr** — 构造 TypeInfoV0 在栈上，设置 warm_delta=32，GetWarmPtr 返回正确地址
9. **AsMethodTable** — TypeInfoHot* 转 MethodTable* 转换
10. **StableIdFnv1a** — chaos_compute_type_stable_id 对空字符串、普通字符串的哈希值可预测
11. **TypeShapeConstants** — chaos_type_shape_reference/value/interface 值验证
12. **FlagsConstants** — kTypeInfoHeaderKindMask, kTypeInfoHasFinalizer 等
13. **TypeInfoV2InlineVtable** — inline_slots 偏移和大小
14. **V0WarmOffset** — TypeInfoV0 warm 紧随 hot

#### 验证

```bash
# 编译
cmake --build build/testing --target test_common_type_info --config RelWithDebInfo
# 运行
build/testing/common/RelWithDebInfo/test_common_type_info.exe
```

### 任务 A1c: 修改 CMakeLists.txt

在 `testing/src/native/common/CMakeLists.txt` 末尾添加：

```cmake
add_chaos_test(test_common_native_types
    common_native_types_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)

add_chaos_test(test_common_type_info
    common_type_info_test.cpp
    LIBS ${CHAOS_COMMON_LIBS}
    LABELS "unit;common"
)
```

---

## 验证与收尾

### 验证命令

```bash
# 全部 common 测试（含新增）编译
cmake --build build/testing --target test_common_native_types --config RelWithDebInfo
cmake --build build/testing --target test_common_type_info --config RelWithDebInfo

# 运行全部 common 测试确保不破坏已有
build/testing/common/RelWithDebInfo/test_common_native_types.exe
build/testing/common/RelWithDebInfo/test_common_type_info.exe
```

### 收尾链路

1. 完成所有测试并验证通过
2. 结构告警 — 检查是否出现重复测试模式、文件职责清晰
3. 测试通过 — 所有 new + existing 测试 100% pass
4. 归档 completed — 物理移目录到 `docs/dev/completed/clib-iw-a1/`
5. 回写父任务 — 更新 commonlib-industrialization 的 child mapping、`latest_stop_point`、下一步
6. 自动建议下一 child `clib-iw-a2`
