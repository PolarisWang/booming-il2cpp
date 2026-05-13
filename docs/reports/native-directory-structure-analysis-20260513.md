# Native C++ 目录结构与库边界综合分析报告

**评估日期：** 2026-05-13
**评估范围：** `src/native/` — 178 个源文件，7 个 CMake 库目标
**评估视角：** 目录结构合理性、功能分类清晰度、分库组织模式

---

## 摘要

`src/native/` 的物理目录结构在宏观上遵循了功能模块划分（runtime-core / interpreter / bootstrap / support / hot-update / common / engine-bridge），但在中观粒度上存在显著的**目录结构与逻辑边界不匹配**问题：

- **核心问题：** `chaos_runtime_core` 单库承载了 75% 的源文件（134/178）和约 90% 的代码行，而其中包含至少 22 个逻辑独立的子命名空间
- **总体评估：** 目录结构在宏观层面合理（顶层 7 个目录的职能分工清晰），但在中观层面（runtime-core 内部）缺乏分层；部分代码的物理位置与逻辑归属不一致

---

## 1. 模块大小分布分析

### 1.1 文件数量分布

| 模块 | 目录 | 文件数 | 占比 | 是否独立 .lib |
|------|------|--------|------|--------------|
| **chaos_runtime_core** | `runtime-core/` + `runtime-core/runtime_stubs/` | 134 | 75.3% | 是（单一 .lib） |
| chaos_common | `common/` + `common/chaos/` | 26 | 14.6% | 是 |
| chaos_interpreter | `interpreter/` + `interpreter/generated/` | 4 | 2.2% | 是 |
| chaos_hot_update | `hot-update/` | 4 | 2.2% | 是 |
| chaos_bootstrap | `bootstrap/` | 2 | 1.1% | 是 |
| chaos_engine_bridge | `engine-bridge/` | 2 | 1.1% | 是 |
| chaos_support | `support/` | 2 | 1.1% | 是 |
| *benchmark-host* | `benchmark-host/` | 3 | 1.7% | **否 — 无 CMakeLists** |
| *proof-host* | `proof-host/` | 1 | 0.6% | **否 — 无 CMakeLists** |
| **TOTAL** | | **178** | **100%** | 7 个正式 .lib + 2 个游离 |

### 1.2 代码行数分布

| 模块 | 估算行数 | 占比 | 备注 |
|------|----------|------|------|
| chaos_runtime_core | ~29,243 | ~88% | 含 runtime_stubs ~2,841 行 |
| chaos_common | ~2,200 | ~7% | 含 json、profile、log 等 |
| chaos_interpreter | ~1,200 | ~4% | interpreter_vm 核心 + generated |
| 其余均 <500 | ~500 | ~1% | 薄封装层 |

### 1.3 评估

**问题：runtime-core 单库巨大化。** 一个静态库承载了全部代码的 88%，这在工业化生产中是明显的架构告警信号：

- 编译时间：任何 runtime-core 文件的修改都触发全部 134 个文件的重新链接
- 职责边界模糊：GC、解释器、内存域、vtable、marshal、类型系统等不同职能的代码在同一个 .lib 中，没有编译隔离
- 团队协作：不同职能的开发者修改同一库目标，增加了冲突概率

**结论：warn — 需要拆分。**

---

## 2. runtime-core 内部结构深度分析

### 2.1 目录布局

```
runtime-core/
├── runtime_core.cpp/h          # 4,358 行 — 巨型文件
├── reflection_api.cpp/h        # 1,616 行
├── runtime_instantiation.cpp/h # 1,435 行
├── interpreter_entry.cpp/h     # 1,344 行
├── fast_dispatch.cpp/h         # 解释器快速路径
├── token_resolver.cpp/h        # IL token 解析
├── gc_*.cpp/h (21 files)       # GC 子系统
├── vtable_registry.cpp/h       # vtable 管理
├── method_table.cpp/h           # 方法表
├── memory_domain.cpp/h          # 内存域
│
├── runtime_stubs/ (36 files)   # extern "C" ABI 薄封装层
│   ├── array_stubs.cpp/h
│   ├── string_stubs.cpp/h
│   ├── math_stubs.cpp/h
│   ├── collection_stubs.cpp/h
│   ├── ... (19 组功能模块)
│   └── stub_common.h
│
├── extension_points/           # 扩展点（少量文件）
└── generated/                  # 生成代码
```

### 2.2 子命名空间 vs CMake 边界

runtime-core 内部存在 **22 个独立命名空间**，但全部编译到同一个 `chaos_runtime_core` 静态库中：

| 子命名空间 | 逻辑职能 | 物理位置 | 独立编译？ |
|-----------|---------|---------|-----------|
| `chaos::il2cpp::runtime_core` | 核心运行时 | runtime-core/ | ❌ 在 chaos_runtime_core 中 |
| `chaos::il2cpp::interpreter` | 解释器循环 | runtime-core/ + interpreter/ | ❌ 散布两处 |
| `chaos::il2cpp::gc_*` (implied) | GC 子系统 | runtime-core/gc_*.cpp/h | ❌ |
| `chaos::il2cpp::vtable_registry` | vtable 管理 | runtime-core/ | ❌ |
| `chaos::il2cpp::method_table` | 方法表 | runtime-core/ | ❌ |
| `chaos::il2cpp::memory_domain` | 内存域分配 | runtime-core/ | ❌ |
| `chaos::il2cpp::runtime_instantiation` | 运行时实例化 | runtime-core/ | ❌ |
| `chaos::il2cpp::runtime_vtable` | 运行时 vtable | runtime-core/ | ❌ |
| `chaos::il2cpp::layout` | 类型布局 | runtime-core/ | ❌ |
| `chaos::il2cpp::marshal_abi` | Marshal ABI | runtime-core/ | ❌ |
| `chaos::il2cpp::string_table` | 字符串表 | runtime-core/ | ❌ |
| `chaos::il2cpp::vector_fixed` | 固定向量 | runtime-core/ | ❌ |
| `chaos::il2cpp::generic_context` | 泛型上下文 | runtime-core/ | ❌ |
| `chaos::il2cpp::common` | profile_globals | runtime-core/ | ❌ + 跨模块归属异常 |
| `chaos::il2cpp::com_abi` | COM ABI | runtime-core/ | ❌ |
| `chaos::il2cpp::marshal_copy_helpers` | Marshal 拷贝 | runtime-core/ | ❌ |
| `chaos::il2cpp::math_kernel_helpers` | 数学内核 | runtime-core/ | ❌ |
| `chaos::il2cpp::numerics_carriers` | 数值载体 | runtime-core/ | ❌ |
| `chaos::il2cpp::runtime_capability` | 运行时能力 | runtime-core/ | ❌ |
| `chaos::il2cpp::runtime_core::aot_metadata` | AOT 元数据 | runtime-core/ | ❌ |
| `chaos::il2cpp::runtime_core::threading` | 线程管理 | runtime-core/ | ❌ |
| `chaos::il2cpp::struct_marshal` | 结构体 Marshal | runtime-core/ | ❌ |

**结论：子命名空间有独立的逻辑边界但没有独立的编译边界。** 这是项目目前最严重的架构治理问题。

### 2.3 巨型文件分析

| 文件 | 行数 | 主要职责 | 风险 |
|------|------|---------|------|
| `runtime_core.cpp` | 4,358 | RuntimeState、GcAllocate、ABI 导出、异常 API、jmp_buf、线程管理 | **6 个职责混编** |
| `reflection_api.cpp` | 1,616 | 反射 API 实现 | 单一职责但膨胀 |
| `runtime_instantiation.cpp` | 1,435 | 运行时实例化逻辑 | 可拆 |
| `interpreter_entry.cpp` | 1,344 | 解释器入口 + 分派 | 与 interpreter/ 职责重叠 |

`runtime_core.cpp` 是问题的典型代表：6 个以上独立职责写在一个文件中，包括运行时状态管理、GC 分配入口、引擎绑定生命周期、ABI 函数表导出、异常元数据 API、以及 jmp_buf 恢复机制。按工业化标准，这个文件应拆分为 5-6 个独立的编译单元。

---

## 3. 跨模块代码散布问题

### 3.1 解释器代码跨模块散布

解释器（Interpreter）的逻辑分布在两个物理模块中：

```
# 应该在 interpreter/ 模块中的代码
src/native/interpreter/
├── interpreter_vm.cpp/h          # InterpreterVM 核心引擎

# 不应在 runtime-core 中的解释器代码
src/native/runtime-core/
├── interpreter_entry.cpp/h       # 解释器入口点 (1,344 行)
├── fast_dispatch.cpp/h           # 快速解释器循环
├── token_resolver.cpp/h          # IL token 解析
├── runtime_stubs/interpreter_entry_direct.cpp  # extern "C" 入口
```

**问题：** `interpreter_entry.cpp`、`fast_dispatch.cpp`、`token_resolver.cpp` 逻辑上属于解释器子系统，但物理上存放在 `runtime-core/` 中。这使得 `chaos_interpreter` 库实际只是一个薄层，核心实现却在 `chaos_runtime_core` 中。

**根因：** 这些文件被放在 runtime-core 是因为 fast_dispatch 是性能热点路径，需要直接访问 runtime-core 的内部数据结构。但这恰恰说明**依赖方向有问题** — 解释器应该依赖 runtime-core，而不是把解释器代码放入 runtime-core。

### 3.2 profile_globals.cpp 归属异常

```cpp
// src/native/runtime-core/profile_globals.cpp
namespace chaos::il2cpp::common {  // ← 命名空间是 common，但文件在 runtime-core/
    ProfileSlot g_profile_table[kProfileTableSize];
    HashEntry g_profile_hash_table[kProfileHashSize];
    // ...
}
```

**问题：** 使用 `chaos::il2cpp::common` 命名空间但放在 `runtime-core/` 目录。profile.h 在 `common/chaos/profile.h` 中定义，其全局变量定义却在 runtime-core 中。这是历史的 "放哪都能编译" 遗留问题。

**影响：** `architecture.yaml` 的模块边界定义无法准确反映物理位置。按规范，profile.h 的全局变量应在 `common/` 模块中定义。

### 3.3 Bridge 文件散布

```
# 散布在多个位置的 bridge 文件
runtime-core/arithmetic_chaos_bridge.h      # 算数桥
runtime-core/load_store_chaos_bridge.h       # 加载存储桥
engine-bridge/engine_bridge.cpp/h            # 引擎桥
```

这些 bridge 文件的功能都是「桥接」，但没有统一目录。建议集中到 `bridge/` 子目录或在 runtime-core 内统一 `_bridge` 后缀管理。

---

## 4. runtime_stubs 组织模式评价

### 4.1 正面评价

`runtime_stubs/` 是项目中组织最清晰的子目录：

```
runtime-stubs 组织原则:
  功能模块                    extern "C" 源文件
  ┌─────────────┐           ┌──────────────────┐
  │ Array 操作   │ ────────→ │ array_stubs.cpp  │
  │ String 操作  │ ────────→ │ string_stubs.cpp │
  │ Math 函数    │ ────────→ │ math_stubs.cpp   │
  │ Convert 转换  │ ────────→ │ convert_stubs.cpp│
  │ ...          │           │ ...              │
  └─────────────┘           └──────────────────┘
```

- 19 组功能文件按主题命名，一目了然
- 每组文件短小精悍（平均 ~150 行）
- extern "C" 封装层与实现隔离清晰

### 4.2 可改进之处

- 36 个文件在单目录中略显扁平，可考虑按子领域分组（如 `math/`、`text/`、`collections/`）
- `stub_common.h` 和 `stubs.h` 的职责边界需文档化
- `interpreter_entry_direct.cpp` 放在这里属于特例（它不是一个 stub，而是一个入口点），应移到 `runtime-core/` 根目录或 `interpreter/` 中

---

## 5. benchmark-host / proof-host 缺少 CMakeLists

这两个目录目前游离在构建系统之外：

| 目录 | 文件 | 状态 | 风险 |
|------|------|------|------|
| `benchmark-host/` | 3 个 .cpp（fact_runtime_main, native_aot_main, native_verify_main） | 无 CMakeLists.txt，仅通过 foundation-dll pipeline 脚本编译 | 不可被标准 `cmake --build` 覆盖；IDE 集成缺失 |
| `proof-host/` | 1 个 .cpp（native_aot_main） | 同上 | 同上 |

虽然这些文件是专门为 foundation-dll 验证管线服务的，但缺少正式 CMake target 意味着：
- 无法在 IDE 中打开项目并编译这些目标
- CI 中只能通过 pipeline 脚本调用，不受 CMake 增量编译管理
- `build/architecture.yaml` 无法为它们声明依赖和约束

---

## 6. GC 子系统自成体系（正面评价）

GC 子系统的组织是 runtime-core 内部最好的：

```
gc_bit_utils.h          gc_bump_arena.h       gc_bump_cache.h
gc_card_table.cpp/h     gc_helpers.h          gc_layout.cpp/h
gc_old_gen.cpp/h        gc_region.cpp/h       gc_root_scanner.cpp/h
gc_scheduler.cpp/h      gc_stats.cpp/h        gc_transition.h
gc_young_collector.cpp/h
```

**正面：** 全部以 `gc_` 前缀命名，一目了然；21 个文件覆盖了从 layout、region、card table、收集器到 scheduler 的完整子系统。

**可优化：** GC 虽然在文件命名上自成体系，但仍编译在 `chaos_runtime_core` 库中。如果 GC 子系统未来需要独立演进（如替换为不同收集器策略），应考虑拆分为独立的 CMake object library。

---

## 7. 综合评分

| 维度 | 评分 | 说明 |
|------|------|------|
| **宏观目录划分** | 4.5/5 | 顶层 7 目录职能清晰，runtime_stubs 组织优秀 |
| **库边界与命名空间匹配** | 2.0/5 | 22 个命名空间共用 1 个 .lib，无编译隔离 |
| **文件内职责单一** | 2.5/5 | runtime_core.cpp 混编 6+ 职责 |
| **跨模块代码一致性** | 3.0/5 | 解释器代码散布两处，profile_globals 跨模块 |
| **游离文件治理** | 3.0/5 | benchmark-host/proof-host 无 CMakeLists |
| **GC 子系统独立性** | 4.0/5 | 文件命名统一，但仍在同一 .lib 中 |
| **综合** | **3.2/5** | 宏观合理，中观需拆分 |

---

## 8. 改进路线图

### Phase 1: 低风险 — 文件级重构（可独立进行）

| 任务 | 复杂度 | 预期效果 |
|------|--------|---------|
| 1.1 拆分 `runtime_core.cpp` (4,358 → ~800 行/文件) | 中 | 消除巨型文件 |
| 1.2 拆分 `reflection_api.cpp` (1,616 行) | 中 | 同上 |
| 1.3 拆分 `runtime_instantiation.cpp` (1,435 行) | 中 | 同上 |
| 1.4 移动 `interpreter_entry_direct.cpp` → runtime-core/ 根目录 | 低 | 消除 stubs 目录中的异常 |
| 1.5 为 benchmark-host/ 添加 CMakeLists.txt | 低 | 纳入标准构建 |
| 1.6 为 proof-host/ 添加 CMakeLists.txt | 低 | 纳入标准构建 |

### Phase 2: 中风险 — 模块边界调整

| 任务 | 复杂度 | 预期效果 |
|------|--------|---------|
| 2.1 将 `profile_globals.cpp` 移到 `common/` 目录 | 低 | 消除命名空间/目录不匹配 |
| 2.2 GC 子系统 → 独立 object library | 中 | 实现编译隔离 |
| 2.3 Bridge 文件统一到 `runtime-core/_bridge/` | 低 | 消除散布 |
| 2.4 runtime_stubs 按领域分子目录（可选） | 低 | 减少目录扁平度 |

### Phase 3: 高风险 — 库拆分

| 任务 | 复杂度 | 预期效果 |
|------|--------|---------|
| 3.1 解释器代码统一到 `chaos_interpreter` 库 | **高** | 消除跨模块散布 |
| 3.2 `chaos_gc` 独立库 | **高** | GC 独立演进 |
| 3.3 `chaos_reflection` 独立库 | 中 | 反射 API 独立 |
| 3.4 按 architecture.yaml 分库治理 | 中 | 架构规范落地 |

### Phase 3 风险评估

**解释器移至 chaos_interpreter 的风险：**
- `fast_dispatch.cpp` 是性能热点，直接访问 runtime-core 的内联数据
- 需要确定接口边界（通过虚函数？回调？直接 include 头文件？）
- 短期建议：暂不移除，但在 architecture.yaml 中标记 interdependency

**GC 独立库的风险：**
- GC 大量使用 runtime-core 的类型系统（TypeInfo、ObjectHeader 等）
- 依赖方向是 GC → runtime-core 还是 runtime-core → GC？
- 目前是双向依赖，拆库需要先解耦

---

## 9. 建议优先级

| 优先级 | 任务 | 理由 |
|--------|------|------|
| **P0** | Phase 1：拆分巨型文件 | 无风险，立即改善可维护性 |
| **P0** | Phase 1：benchmark-host/proof-host 加 CMakeLists | 5 分钟工作量 |
| **P1** | Phase 2：profile_globals.cpp 移入 common/ | 消除逻辑/物理不匹配 |
| **P1** | Phase 2：Bridge 文件统一 | 低风险规范化 |
| **P2** | Phase 3：GC 独立 object library | 中等风险，需要接口设计 |
| **P3** | Phase 3：解释器代码统一 | 高风险，需要架构决策 |

---

## 10. 结论

`src/native/` 的宏观目录结构**在顶层设计上是合理的** — 7 个模块目录的职能分工清晰，`runtime_stubs` 的组织模式值得推广。主要问题集中在**中观粒度**：

1. **最重要的改进方向：** 拆分 `chaos_runtime_core` 单库。这不是简单的"把文件移到不同目录"，而是需要为每个逻辑子系统（GC、解释器、反射、vtable 等）定义清晰的接口边界。
2. **最易执行且见效快：** 拆分 4 个巨型文件（8,753 行总量）。无架构风险，纯机械拆分即可。
3. **最需架构决策：** 解释器代码统一和 GC 独立库。这两个方向依赖方向复杂，需要先完成接口解耦再移动代码。

**总体建议：** 当前阶段优先完成 Phase 1（文件级重构），在过程中逐步识别接口边界，为 Phase 3 的库拆分积累经验。不建议在没有清晰接口边界的情况下直接进行库拆分，否则会导致循环依赖和编译爆炸。