# REF-RISK-7 codegen 接线 — 实施进展与卡点

> 日期：2026-09-13 ｜ 状态：**部分完成，卡在 helper 提供者**

## 已完成

### 1. native 侧 RAII 支架（`reflection_api.h`）
在既有 `ChaosReflectionPushExecutingImage` / `PopExecutingImage` 之上新增：

```cpp
struct ChaosExecutingImageScope {
    CHAOS_IL2CPP_INTPTR previous;
    explicit ChaosExecutingImageScope(CHAOS_IL2CPP_INTPTR image_handle) noexcept
        : previous(ChaosReflectionPushExecutingImage(image_handle)) {}
    ~ChaosExecutingImageScope() noexcept { ChaosReflectionPopExecutingImage(previous); }
    // 禁拷贝
};
```

**为什么用 RAII 而非手写 push/pop**：生成的方法体可能提前 return、也可能经异常展开
退出；手写 pop 在这些路径上会被跳过，把 thread-local 留在"已返回的方法"上。
析构恢复保证**每条退出路径**都正确。

### 2. codegen 按方法注入（`Methods.ModuleData.Helpers.cs`）
- `InjectExecutingImageScope(method, source)` 在 `BuildMethodSource` 产出文本后调用
- **仅在方法体确实引用** `ChaosReflectionGetCallingAssembly` /
  `ChaosReflectionGetExecutingAssembly` 时注入 —— 满足 P1（不给全量方法加热路径
  TLS 写），与既有 `chaos_static_` / `chaos_mt_` 的"按符号出现检测"同一思路
- 注入点是方法体首个 `{`（单独成行）之后，要求是**定义**而非声明

## 卡点：`chaos_executing_image_handle()` 的提供者缺失

注入的代码调用 `chaos_executing_image_handle()`，它应返回**本 TU 的镜像句柄**
（即 `EncodeReflectionQueryImageHandle(&kReflImage)`）。当前代码里不存在该函数。

**难处**：`kReflImage` 定义在 `native-aot.generated.cpp`（第 58117 行），
但**不在 header 中**（已核实 grep 无结果）。因此：
- 放在 header 的 helper 看不到 `kReflImage`
- 放在 cpp 的 helper 又晚于方法体（方法体可能在不同 page TU）

## 待决策（两条路）

**路 A：把 `kReflImage` 提到 header 可见**
- 在 `native-aot.generated.header.h` 中 `extern const ReflectionQueryImageDescriptor kReflImage;`
- 并在 header 提供 `static inline CHAOS_IL2CPP_INTPTR chaos_executing_image_handle()`
- 需要确认 header 能拿到 `EncodeReflectionQueryImageHandle`（来自 `reflection_query_model.h`）
- 风险：改 header 生成顺序，影响所有 chunk（需全量验证）

**路 B：改用已有的模块注册表查本模块镜像**
- 生成代码不依赖 `kReflImage`，而是通过 `ChaosReflectionGetExecutingAssembly()`
  的既有回退（返回 CoreLib）—— 但那正是要修的问题，会形成循环
- 或新增一个 native API：按"当前模块 id"返回镜像，由生成代码传入本模块 id 常量
- 风险：新增 ABI 面

## 建议

**路 A**。理由：`kReflImage` 已是本 TU 的权威镜像描述符，把它提到 header 可见是
最小改动；且 header 的 extern 声明是既有惯例（该文件本就含大量 `extern` 与
`static inline` 辅助函数）。**但需全量 chunk 验证**，因为改的是共享 header 的发射。

## 逐轮尝试记录（重要：注入点比预想难）

按「先检测、再注入」实现后，注入点定位经三轮未果：

1. **第一版**：`EmitManagedMethod` 内直接插 push —— 无法保证每条退出路径都有 pop
   （提前 return / 异常展开会漏），产生 TLS 泄漏。**设计错误，改用 RAII 后废弃。**
2. **第二版**：注入到「首个 `{`（单独成行）之后」—— 实测注入进了**内层 lambda**，
   生成的 C++ 语法错误（`[&]() -> void { ChaosExecutingImageScope ...`）。
3. **第三版**：改用「最后一个 `{`」—— 但 `BuildMethodSource` 的产物里，
   在真正的函数定义**之前**还有一大段 `extern "C" ... ;` **声明列表**，
   检测命中的是声明而非定义。

**结论**：该注入点需要区分「声明列表」与「函数定义」，而二者的尾部形态相近；
`BuildMethodSource` 的产物流里也混入了声明。继续试错风险高（且是 P1 敏感路径），
**按三修规则停止**，改为记录待专项设计。

## 当前提交状态
native RAII 与 codegen 注入**已实现且 codegen 编译通过**，但因 helper 缺失，
**尚未跑通端到端**（生成的 C 代码会 C3861）。在补齐 helper 前不应合入 main。
