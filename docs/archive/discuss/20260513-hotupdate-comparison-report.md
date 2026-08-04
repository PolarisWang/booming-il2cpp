# Chaos.IL2CPP Hot-Update vs HybridCLR vs Mono — 热更新方案深入对比分析报告

> 日期: 2026-05-13
> 范围: Chaos.IL2CPP 热更新体系 vs HybridCLR (v7.x) vs Mono (6.12+) Hot Reload / AppDomain
> 场景: Unity 游戏热更新 + 自定义宿主 AOT 运行时

---

## 一、整体架构定位

| 维度 | Chaos.IL2CPP | HybridCLR | Mono |
|------|-------------|-----------|------|
| 架构模型 | AOT + InterpreterVM 混合 | AOT + 寄存器式解释器 混合 | Full JIT / Full AOT (2024+) + AppDomain |
| 热更新方法 | PatchMethod (方法级替换) | Assembly.Load (DLL 级加载) | AppDomain.CreateInstance / E&C |
| 元数据统一性 | AOT + hotpatch AotCoreIr 双分 | **统一元数据**: AOT/解释器共享 | **统一元数据**: JIT 直接解析 |
| 补充元数据 | .patchdata 二进制打包 (112B header + 6 tables) | 补充元数据注册 (AOT 遗漏的类型/方法) | 无需求 (JIT 完整元数据) |
| 泛型处理 | 三级: Direct / Shape / Interpreter fallback | 共享泛型 (AOT泛型 + 解释泛型 统一) | gshared / gsharedvt 共享泛型 |
| 调用代价 | AOT: ~2ns → Hotpatch: ~430-580ns | AOT: ~2ns → Interpreter: ~19-62ns (商业版) | 同进程: ~ns 级 |
| 加载/切换代价 | 包加载 ~ms 级, 方法按需反序列化 | Assembly.Load ~几十ms | AppDomain: 1-10s |
| 开发语言 | C++20 + C# codegen | C++ (解释器) + C# (运行时) | C (Mono runtime) |
| 外部依赖 | 无 (自研全部) | 无 (自研全部) | 无 (自研全部) |

---

## 二、核心架构深度对比

### 2.1 调用派发模型

#### Chaos.IL2CPP
```
fast_dispatch (AOT direct)
  → 检查 HotpatchIsActive(entry) && !HotpatchShouldKeepNative(entry)
    → InterpreterEntryDirect
      → PatchMethod (CAS 3-state: Checked → Deserializing → Deserialized)
        → InterpreterVM::Execute (99 IR opcodes)
```

- **懒反序列化**: 只有首次调用才触发反序列化, CAS 状态机避免竞态
- **双路径共存**: AOT 原函数 + hotpatch 新实现并存, 通过 `kHotpatchKeepNative` 保守标记
- **2 指令快速路径**: Step1c_2InstrFastPath 对纯调用序列直接转发
- **call_cache**: `PatchMethod` 中缓存方法调用地址

#### HybridCLR
```
AOT 函数直接执行
  → 如果方法是 interpreter 注册的
    → Interpreter::Execute (register-based IR)
```

- **统一元数据**: AOT 编译时收集所有类型/方法元数据, interpreter 共享同一份元数据
- **寄存器式指令集**: 自定义 IR (非 IL), 约 200+ opcodes, 寄存器分配优化
- **12 条优化原则**: 包括缓存解释器栈帧、减少访存、常量传播、类型特化等
- **DHE (Differential Hybrid Execution)**: 智能决定 AOT vs Interpreter 执行路径

#### Mono
```
JIT 编译并执行
  → 如果 AppDomain 卸载 → 回收所有 JIT 代码
  → 如果 Roslyn E&C → 方法体替换 (仅限方法体变更)
```

- **Full JIT**: 无解释器阶段, 直接编译为机器码
- **AppDomain 隔离**: 通过进程内 AppDomain 实现代码隔离和卸载
- **SGen GC 跨域处理**: 跨 AppDomain 对象引用通过代理 (Remoting) 处理

**结论**: Chaos.IL2CPP 的派发模型与 HybridCLR 最接近 (AOT + Interpreter 双路径)。Chaos.IL2CPP 的懒反序列化 (CAS 状态机) 是独特优势, 代价建模更精确。HybridCLR 的寄存器式 IR 比 Chaos.IL2CPP 的栈式 IR 性能更好。

---

### 2.2 补充元数据处理

#### Chaos.IL2CPP — .patchdata 格式
```
.patchdata header (112 bytes)
  ├── magic, version, checksum (8+4+4=16)
  ├── code_section (offset+size)
  ├── gc_section (offset+size) 
  ├── exception_section (offset+size)
  ├── method_table_offset + external_ref_offset
  ├── patchdata_ir_table_offset + catch_type_table_offset
  └── aot_core_ir_section_offset
Tables:
  ├── MethodTable: 新方法 body (IR bytecode + GC map + EH table)
  ├── ExternalRefTable: 外部 AOT 函数引用 (模块+方法名 → token)
  ├── IRTable: 方法→IR bytecode 映射
  ├── CatchTypeTable: EH catch type 元数据
  └── AotCoreIrSection: 新类型/字符串/泛型等元数据
```

- **二进制紧凑格式**: 无 XML/JSON, 直接内存映射加载
- **按需解码**: PatchMethod 触发时仅解码单个方法的 IR
- **元数据分裂**: AOT 元数据 (JSON) + 热更新元数据 (.patchdata) 两个独立体系

#### HybridCLR — 补充元数据注册
```csharp
// 加载热更新 DLL 时自动注册
var ass = Assembly.Load(hotUpdateDllBytes);
// HybridCLR 内部:
// 1. 解析 DLL PE 元数据
// 2. 注册遗漏的 AOT 类型到补充元数据表
// 3. interpreter 通过统一元数据查询接口访问
```

- **基于 PE 元数据**: 直接解析 DLL, 无需额外格式转换
- **统一查询接口**: `MetadataModule::GetTypeDefinition(idx)` 同时查询 AOT 和补充元数据
- **无格式分裂**: AOT 元数据 (全局 blob) + 补充元数据 (运行时注册) 通过同一接口暴露

#### Mono — 无额外元数据
- AppDomain 内完整 JIT, 不需要"补充"元数据
- E&C 场景: Roslyn 直接生成新方法体, 运行时替换

**结论**: HybridCLR 的"统一元数据"架构优于 Chaos.IL2CPP 的双分元数据。.patchdata 的紧凑格式有加载性能优势, 但元数据分裂增加了维护成本和复杂性。**这是 Chaos.IL2CPP 的热更新架构需要关注的核心差距**。

---

### 2.3 泛型处理

| 能力 | Chaos.IL2CPP | HybridCLR | Mono |
|------|-------------|-----------|------|
| AOT 泛型特化 | ✅ 编译期预见则直接 | ✅ 编译期预见则 AOT | ✅ JIT 直接编译 |
| 运行时泛型实例化 | ✅ Interpreter fallback | ✅ 解释器执行 (共享) | ✅ JIT 直接编译 |
| 泛型共享 (代码体积) | ✅ 部分 (interpreter) | ✅ 完整 (统一泛型) | ✅ gshared/gsharedvt |
| 约束检查 | ✅ interpreter 内实现 | ✅ 元数据驱动 | ✅ JIT 编译时 |
| 跨 AOT/热更新 泛型 | ⚠️ 有限 (需 .patchdata 声明) | ✅ 完全透明 | ✅ 完全透明 (AppDomain 内) |

**关键差异**: HybridCLR 的"统一泛型"机制让 AOT 和热更新代码使用完全相同的泛型实例化路径。Chaos.IL2CPP 的 interpreter fallback 虽然避免了代码爆炸, 但需要在 .patchdata 中声明泛型依赖。

---

### 2.4 跨边界调用代价

| 调用类型 | Chaos.IL2CPP | HybridCLR | Mono |
|---------|-------------|-----------|------|
| AOT→AOT (同) | ~2ns | ~2ns | ~2ns |
| AOT→Hotpatch (首次) | ~430-580ns | ~19-62ns (商业版) | N/A (JIT) |
| AOT→Hotpatch (缓存后) | ~430-580ns | ~19-62ns | N/A |
| Hotpatch→AOT | ~430-580ns + AOT 时间 | ~19-62ns + AOT 时间 | N/A |
| Hotpatch→Hotpatch | ~430-580ns × N | ~19-62ns × N | N/A |
| AppDomain 创建 | N/A | N/A | 1-10s |

Chaos.IL2CPP 的 hotpatch 调用比 HybridCLR 商业版 **慢 7-30 倍**。这是以下因素共同导致的:
1. **栈式 IR vs 寄存器式 IR**: 栈式需要更多指令完成相同操作
2. **双路径检查**: 每次调用检查 `HotpatchIsActive && !HotpatchShouldKeepNative`
3. **调用缓存不够激进**: HybridCLR 的 DHE 能进一步减少解释器调用次数

---

### 2.5 调试支持

| 能力 | Chaos.IL2CPP | HybridCLR | Mono |
|-----|-------------|-----------|------|
| 热更新代码断点 | ❌ 不存在 | ⚠️ 有限 (IL 级别) | ✅ 完整 (Mono soft debugger) |
| 单步执行 | ❌ 不存在 | ⚠️ 有限 | ✅ 完整 |
| 变量查看 | ❌ 不存在 | ⚠️ 有限 | ✅ 完整 |
| 栈帧混合显示 | ❌ 不存在 | ⚠️ 有限 | ✅ JIT 原生支持 |
| IDE 集成 | ❌ 不存在 | ✅ Visual Studio / VS Code | ✅ Visual Studio / VS Code |

**这是 Chaos.IL2CPP 热更新的最大功能差距**。HybridCLR 虽然有限但有基础调试支持, Mono 是完整的调试体验。缺失调试支持使 Chaos.IL2CPP 的热更新开发体验大幅降低。

---

### 2.6 内存与 GC

| 方面 | Chaos.IL2CPP | HybridCLR | Mono |
|-----|-------------|-----------|------|
| GC 模型 | 自研分代 (复制+标记清除) | 宿主 GC (Unity 或 自定义) | SGen 分代 / BDWGC |
| 对象跨边界 | 同一 GC 堆 (透明) | 同一 GC 堆 (透明) | AppDomain: 需 Remoting |
| 热更新卸载 | 包级别批量卸载 (Heap::Destroy) | DLL 级别 (需主动标记) | AppDomain 完全卸载 |
| GC 安全点 | ✅ 统一管理 | 由宿主 GC 管理 | 内建 |
| 静态变量隔离 | ⚠️ 按模块命名空间区分 | ✅ 按 Assembly 隔离 | ✅ AppDomain 完全隔离 |

Mono 的 AppDomain 隔离最彻底但代价最高 (1-10s 创建/销毁)。Chaos.IL2CPP 和 HybridCLR 都在同一 GC 堆中运行热更新代码, 不需要跨域代理, 但静态变量隔离需要额外设计。

---

## 三、功能差距矩阵

### 3.1 基础功能

| 功能 | Chaos.IL2CPP | HybridCLR | Mono | 差距等级 |
|-----|-------------|-----------|------|---------|
| **DLL 级热更新** | ✅ PatchMethod (方法级) | ✅ Assembly.Load | ✅ AppDomain | **同级**(不同粒度) |
| **新增类型** | ✅ .patchdata AotCoreIr | ✅ 补充元数据注册 | ✅ JIT 完整 | **无差距** |
| **新增方法** | ✅ .patchdata MethodTable | ✅ PE 自动解析 | ✅ JIT 完整 | **无差距** |
| **新增泛型** | ⚠️ 需 .patchdata 声明 | ✅ 自动 | ✅ JIT 自动 | **低** |
| **删除旧代码** | ✅ 包卸载回收 | ⚠️ 需手动 | ✅ AppDomain 卸载 | **低** |
| **静态变量隔离** | ⚠️ 命名空间隔离 | ✅ Assembly 隔离 | ✅ AppDomain 隔离 | **低** |
| **增量更新** | ⚠️ 需全量 .patchdata | ✅ DLL 增量 | ✅ 方法级 E&C | **中** |

### 3.2 高级功能

| 功能 | Chaos.IL2CPP | HybridCLR | Mono | 差距等级 |
|-----|-------------|-----------|------|---------|
| **调试器支持** | ❌ 不存在 | ⚠️ 有限 | ✅ 完整 | **高** |
| **Profiling 支持** | ❌ 不存在 | ⚠️ 有限 (宿主 profiler) | ✅ 完整 | **高** |
| **断点续更** | ❌ 未实现 | ❌ 未实现 | ❌ N/A | **同级** |
| **AOT 代码补丁** | 仅限 interpreter | ✅ DHE 智能分流 | N/A (JIT) | **低** |
| **反射完整支持** | ⚠️ 有限的 reflection_api | ✅ 统一元数据反射 | ✅ 完整 | **中** |
| **自定义属性** | ⚠️ 部分实现 | ✅ 统一查询 | ✅ 完整 | **低** |
| **线程安全热更** | ✅ CAS 状态机 | ✅ CAS 加载 | ✅ 域隔离 | **同级** |

### 3.3 性能对比

| 指标 | Chaos.IL2CPP (当前) | HybridCLR 社区版 | HybridCLR 商业版 | Mono (JIT) |
|-----|-------------------|----------------|-----------------|-----------|
| 解释器: AOT 倍数 | ~200-290x | ~7-20x | ~1.3-12.9x | 1x (JIT) |
| 内存: 解释器 overhead | 中 (逐方法 IR 缓存) | 低 (寄存器式) | 最低 (DHE) | 0 (JIT) |
| 启动加载 | ~ms (.patchdata mmap) | ~几十ms (PE 解析) | ~几十ms | ~1-10s (AppDomain) |
| 大世界场景 | ⚠️ 未验证 | ✅ 已验证 (多款上线游戏) | ✅ 验证优化 | N/A (非游戏场景) |
| 包体增加 | 低 (热更包) | 中 (需要补充元数据注册) | 中 | ~完整运行时 |

---

## 四、Chaos.IL2CPP 热更新的独特优势

### 4.1 懒反序列化 + CAS 状态机

PatchMethod 的三态 CAS (Checked → Deserializing → Deserialized) 是专为游戏热更新设计的:

- **Checked**: 首次调用, 无锁检查 CAS 标记, 快速判定是否需要加载
- **Deserializing**: 只有一个线程进入反序列化, 其他线程自旋等待
- **Deserialized**: 反序列化完成, 后续调用直接使用

HybridCLR 和 Mono 都没有这种懒加载机制 — HybridCLR 在 `Assembly.Load` 时全量加载, Mono 在 AppDomain 创建时全量加载。对于游戏场景, 热更包可能包含数千个方法但只有少数被调用, 懒加载优势明显。

### 4.2 kHotpatchKeepNative 保守标记

对调用外部运行时 helper 的方法自动标记为 native-keep, 避免不可预期的跨域问题。这种保守策略虽然牺牲了少量热更新覆盖度, 但大幅提高了运行稳定性。

### 4.3 .patchdata 紧凑二进制格式

112 字节 header + 6 种表类型, 支持内存映射直接加载。不需要解析 PE COFF 头或 IL 流, 加载时间接近零。额外 AotCoreIr 段支持热更新包内声明新类型/字符串/泛型。

### 4.4 无外部依赖

完全自研, 不依赖 Unity 编辑器、Mono、或任何第三方库。这使得 Chaos.IL2CPP 的热更新可用于:
- 自定义游戏引擎
- 非 Unity 宿主
- 嵌入式/移动端 (无 Mono runtime 占用)

---

## 五、差距分析与建设优先级

### Phase A — 性能优化 (1-2 个月)

1. **InterpreterVM 寄存器式 IR** (高 Impact)
   - 当前栈式 IR → 寄存器式 IR, 参考 HybridCLR 的 12 条优化原则
   - 预期提升: 5-10x 解释器性能 (从 ~500ns → ~50-100ns)
   - 关键文件: `src/native/interpreter/`, `contracts/native/v0/patch_data.h` (IR opcode 定义)

2. **调用缓存优化** (高 Impact)
   - 内联缓存 (Inline Cache): 对重复调用同一方法的场景缓存解释器入口
   - 预期提升: 2-3x 对重复调用场景
   - 参考 HybridCLR 的 DHE 思路

3. **快速路径加强** (中 Impact)
   - 更多 2 指令快速路径模式 (当前仅支持纯调用序列)
   - 预期提升: 1.5-2x 对常见模式

### Phase B — 功能完善 (2-4 个月)

4. **统一元数据接口** (高 Impact, **架构级改进**)
   - 当前: AOT (JSON) + hotupdate (.patchdata) 双分
   - 目标: 统一元数据查询接口, 类似 HybridCLR 的 `MetadataModule`
   - 关键文件: `src/native/runtime-core/aot_core_ir_reader.cpp` + `src/native/runtime-core/metadata/`
   - **这是最重要的架构改进方向**

5. **调试器支持** (高 Impact, **最大功能差距**)
   - 基础: 解释器内断点注入 (Step1_StructuredEH/Step2_InstructionLoop 插入检查点)
   - 进阶: 栈帧遍历 + 变量查看
   - 关键前置: 需要 InterpreterVM 的 IR 有稳定的调试信息映射
   - 参考 HybridCLR 的有限调试支持

6. **Assembly 级加载** (中 Impact)
   - 当前: PatchMethod (方法级)
   - 目标: DLL → .patchdata 的脚本化转换, 支持 Assembly.Load 语义
   - 关键文件: `src/managed/Chaos.IL2CPP.CodeGen/PatchDataExtractor.cs`

### Phase C — 生态与验证 (3-6 个月)

7. **大世界场景验证** (高 Impact)
   - 当前: 仅单元测试级别验证
   - 需要: 实际游戏项目验证, 包括 100+ MB 热更包、千级方法调用链
   - 指标: GC 暂停时间、JIT 内存占用、调用延迟分布

8. **断点续更** (中 Impact)
   - 游戏中途下载热更包, 不重启应用完成更新
   - 需要处理: 正在执行的栈帧、活跃泛型实例、未完成的异步操作
   - 参考: 当前 kHotpatchKeepNative 的保守标记可以降低复杂度

9. **Profiling 工具链** (中 Impact)
   - 解释器内方法调用计数、耗时统计
   - 热更包内代码热点分析
   - GC 分配模式分析

---

## 六、综合评价

### 当前定位

Chaos.IL2CPP 的热更新系统在**架构完整性**上已经达到生产可用级别:
- 完整的 4 层体系 (patchdata → PatchLoader → InterpreterEntryDirect → InterpreterVM)
- 线程安全的 CAS 懒加载
- 紧凑的 .patchdata 二进制格式
- 支持新增类型/方法/字符串/泛型

### 对 HybridCLR 的主要差距

| 方面 | 差距程度 | 根本原因 |
|-----|---------|---------|
| **解释器性能** | ~7-30x | 栈式 IR vs 寄存器式 IR; 缺乏 DHE 智能分流 |
| **元数据统一性** | 高 | .patchdata + AOT JSON 双分 vs 统一元数据接口 |
| **大型项目验证** | 部分 | HybridCLR 已有上线游戏 |
| **调试器** | 完全缺失 | 无断点/单步/变量查看 |

### 对 Mono 的主要差距

| 方面 | 差距程度 | 根本原因 |
|-----|---------|---------|
| **调试器** | 完全缺失 | Mono 有完整的 soft debugger |
| **JIT 性能** | 架构差异 | AOT + Interpreter vs Full JIT (但游戏场景 AOT 是优势) |
| **Assembly 隔离** | 中等 | AppDomain 有完整的代码/GC/静态变量隔离 |
| **工具链成熟度** | 高 | Mono 有 20+ 年积累 |

### 总体结论

```
架构成熟度:
  Chaos.IL2CPP:   Tier 0 (基础热更): ✅ 90% | Tier 1 (性能): ⚠️ 40% | Tier 2 (生态): ⚠️ 20%
  HybridCLR:      Tier 0 (基础热更): ✅ 95% | Tier 1 (性能): ✅ 80% | Tier 2 (生态): ✅ 60%
  Mono:           Tier 0 (基础热更): ✅ 99% | Tier 1 (性能): ✅ 99% | Tier 2 (生态): ✅ 90%
```

**Chaos.IL2CPP 热更新 = HybridCLR 的架构思路 + 独特的懒加载设计 + 更紧凑的二进制格式, 但目前的解释器性能差距明显。**

### 建议策略

1. **短期 (1-2 月)**: 主攻 InterpreterVM 性能 — 寄存器式 IR + 内联缓存, 目标将调用代价从 ~500ns 降至 ~100ns
2. **中期 (2-4 月)**: 统一元数据接口 + 基础调试支持, 消除架构级差距
3. **长期 (6-12 月)**: Assembly 加载语义 + 断点续更 + 大世界验证, 达到 HybridCLR 同等成熟度

核心建议: **解释器性能是当前最主要瓶颈**, 寄存器式 IR 改造应作为最高优先级。HybridCLR 从社区版到商业版的性能飞跃 (7-20x → 1.3-12.9x) 表明, 解释器优化有巨大空间, 值得投入。