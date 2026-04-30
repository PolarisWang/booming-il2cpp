# SIMD语义降级与多后端IR方案C3

## 背景

当前 `System.Numerics.Vector<T>`、`Vector64/128/256/512<T>` 与 ISA 族（`Sse*`、`Avx*`、`AdvSimd`、`PackedSimd`）的 runtime skeleton 补齐工作，已经证明单纯按方法名和 ISA 前缀逐个补 `case` 可以推进覆盖，但长期会遇到三个问题：

1. planner 过度依赖 descriptor 命中顺序  
2. 同一语义在多个 family 中重复展开  
3. 新增能力时容易引入 ABI、helper、验证路径的碎片化

方案 C3 的目标不是继续堆叠 `vector-kernel` 特化分支，而是抽象出一个更通用的 SIMD 语义系统，使同一套抽象可以覆盖更多 carrier、更多 lane type、更多 ISA，并保持 IL2CPP 与 hotupdate 的最终形态稳定。

## 目标

方案 C3 需要同时满足：

- 性能最优：优先 native shared helper，禁止 managed fallback 成为常态路径
- IL2CPP 最优解：稳定 ABI、稳定 codegen contract、稳定 generated 产物形态
- hotupdate 适配友好：宿主 contract 固定，patch 只扩 semantic，不改执行协议
- 类型扩展性：支持更多 fixed-vector carrier、更多 lane type、更多 numeric / permute / convert 语义

## 核心思想

方案 C3 采用三层模型：

1. `Shape IR`
2. `Semantic IR`
3. `Backend Lowering`

它和“按方法名直接映射 helper”的最大区别在于：

- planner 先回答“这是什么 shape”
- 再回答“这是哪种 semantic”
- 最后回答“该语义用哪个 backend 落地”

### 1. Shape IR

`Shape IR` 只描述方法的结构，不描述具体语义。

建议最小 shape 集：

- `UnaryVector`
- `UnaryVectorImmediate`
- `UnaryVectorConversion`
- `BinaryVector`
- `BinaryVectorImmediate`
- `BinaryVectorIndex`
- `TernaryVector`
- `TernaryVectorMaskImmediate`
- `TernaryVectorIndexBlend`

每个 shape 至少携带：

- `carrierKind`
- `vectorWidthBytes`
- `valueLaneKind`
- `valueLaneBitWidth`
- `indexLaneKind`
- `indexLaneBitWidth`
- `returnLaneKind`
- `returnLaneBitWidth`
- `arity`
- `immediateKind`

### 2. Semantic IR

`Semantic IR` 描述真正的运算语义，不再直接绑定某个方法名。

建议按语义簇建模：

- `Arithmetic`
  - `Add`
  - `Sub`
  - `Mul`
  - `Div`
- `Compare`
  - `Eq`
  - `Ne`
  - `Gt`
  - `Ge`
  - `Lt`
  - `Le`
- `Bitwise`
  - `And`
  - `Or`
  - `Xor`
  - `AndNot`
  - `Select`
  - `TernaryLogic`
- `ShiftRotate`
  - `ShiftLeft`
  - `ShiftRightLogical`
  - `ShiftRightArithmetic`
  - `RotateLeft`
  - `RotateRight`
- `Convert`
  - `Plain`
  - `Saturating`
  - `Truncating`
- `Permute`
  - `SingleSourceIndex`
  - `DualSourceIndex`
  - `LaneShuffleImmediate`
- `NumericSpecial`
  - `GetExponent`
  - `GetMantissa`
  - `Reciprocal14`
  - `ReciprocalSqrt14`
  - `RoundScale`
  - `Scale`
  - `Fixup`

### 3. Backend Lowering

同一 semantic 可以有多个 backend lowering：

- `GenericScalarized`
- `GenericFixedCarrier`
- `PlatformIntrinsic`

对当前仓库的建议是：

- 默认使用 `GenericFixedCarrier`
- 只在已经存在、且验证稳定时才引入 `PlatformIntrinsic`
- 不提供 managed fallback 作为常规 backend

## 为什么这是更通用的名字

本方案不是某个单独 `vector-kernel` family 的局部实现技巧，而是一个：

- 面向 SIMD / fixed-vector 语义
- 面向多 carrier / 多 ISA
- 面向多 backend lowering

的统一抽象。

因此命名应避免局限在 `vector kernel`，更合适的长期名称是：

- `SIMD语义降级`
- `固定向量语义系统`
- `多后端SIMD Lowering`

本页采用 `SIMD语义降级与多后端IR` 作为正式叫法。

## 与当前仓库的关系

当前仓库已经具备 C3 的一部分基础：

- fixed-carrier helper 正在沉淀到 `runtime_core.h`
- planner 已经开始形成统一 shape 提取函数
- `System.Numerics.Vector<T>` 与 `Vector64/128/256/512<T>` 的 carrier 解析已被逐步统一

但还没有完成以下关键步骤：

1. planner 内部的统一 `Shape IR`
2. 统一的 `Semantic IR` 映射表
3. 统一的 backend lowering 选择策略
4. 消除“shape 命中后过早 return false”这一结构缺陷

## 方案C3的三个候选实现路线

### 方案C3-A：IR 只在 planner 内部存在

做法：

- `Shape IR`、`Semantic IR` 只作为 `RuntimeSkeletonVectorKernelCore.cs` 内部中间结构
- 最终仍直接生成现有 plan / helper call

优点：

- 风险最低
- 不改 ABI
- 最适合当前仓库渐进演进

缺点：

- runtime helper 层仍需手工保持一致

适用：

- 当前阶段的主推荐实现路线

### 方案C3-B：IR 驱动 shared helper family

做法：

- planner 输出统一 IR
- helper 层按 semantic family 组织
- 不同方法只是同一 semantic 的实例

优点：

- 复用率高
- 更利于扩类型与扩 ISA

缺点：

- helper 设计需要前置整理

适用：

- 当 `Convert` / `Permute` / `NumericSpecial` family 已经稳定后

### 方案C3-C：IR 驱动多 backend

做法：

- planner 输出统一 IR
- lowering 再分发到 `GenericScalarized` / `GenericFixedCarrier` / `PlatformIntrinsic`

优点：

- 可扩展性最强
- 平台上限最高

缺点：

- backend 选择、验证矩阵、回归复杂度最高

适用：

- 只有在仓库明确进入“平台编译器工程化”阶段时才建议采用

## 性能、IL2CPP、hotupdate 三维权衡

### 性能

最优原则：

- 运行时不解释 IR
- 不走 managed fallback
- helper 使用静态模板 / 固定 carrier 展开

因此：

- `Shape IR` 与 `Semantic IR` 应仅存在于生成期
- 最终 runtime path 仍然必须是静态 native helper 调用

### IL2CPP 最优解

最优原则：

- 单 ABI
- 单 contract family
- planner 输出稳定
- generated cpp 的结构可预测

因此：

- `IR` 不应直接暴露给 runtime ABI
- `IR` 不应成为 patch 与宿主之间的协议
- ABI 层仍建议保持单一 `vector-kernel` / future unified SIMD family

### hotupdate 适配

最优原则：

- patch 只扩 semantic，不改宿主协议
- 宿主 helper namespace 与 carrier schema 稳定
- 不依赖运行时 JIT 或 managed fallback

因此：

- `C3` 只能做生成期抽象
- 不能做运行时 bytecode 解释器
- 不能把 fallback 设计成 managed invoke 主路径

## 允许与禁止的 fallback

### 允许

- `GenericFixedCarrier`
- 少量 `GenericScalarized`，仅当 fixed-carrier 无法表达时使用

### 禁止

- managed invoke fallback 作为常规方案
- 为单个 method 引入专用 ABI
- 在 host / patch 间引入新的 runtime IR 协议

## 类型扩展策略

方案 C3 不应该抽象成“任意泛型 T”，而应该抽象到：

- lane category
  - signed integer
  - unsigned integer
  - floating
  - native integer
- lane bit width
  - 8 / 16 / 32 / 64
- vector width
  - 64 / 128 / 256 / 512

这样能自然覆盖：

- `byte/sbyte`
- `short/ushort`
- `int/uint`
- `long/ulong`
- `float/double`
- 后续 `nint/nuint`

不建议在 C3 中直接引入：

- 任意用户 struct lane
- 任意 blittable `T`
- 任意自定义 mask lane

## 当前阶段的推荐落地顺序

如果以当前仓库状态落 C3，建议顺序是：

1. 先完成 `Shape IR`
2. 再做 `Semantic IR` 表驱动
3. 先统一 `Convert` family
4. 再统一 `Permute` family
5. 再统一 `NumericSpecial` family
6. 最后考虑是否需要多 backend

## 推荐结论

对本项目，C3 的最优实现不是一步做成“全功能 IR 平台”，而是：

- 用 C3 的抽象边界指导当前 SIMD / fixed-vector 重构
- 先落 `C3-A`
- 再逐步演进到 `C3-B`
- 暂不直接进入 `C3-C`

一句话总结：

> C3 应该作为生成期的 SIMD 通用抽象，而不是运行时解释系统；它的价值在于统一 shape、semantic 与 backend lowering，而不是替代现有 native helper 的最终执行路径。
