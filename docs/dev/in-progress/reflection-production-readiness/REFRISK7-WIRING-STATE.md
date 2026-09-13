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

---

## 2026-09-13 续：接线已完成（`8de983b58`）

### 实现

1. **native**：`ChaosExecutingImageScope`（RAII）—— 生成代码只需一行声明，
   析构自动恢复，覆盖提前 return 与异常展开。
2. **codegen 注入点**：`EmitManagedMethod` 内、函数体开括号之后。
   之前三轮失败的原因是**在成品文本上后处理**：文本里有前置的 extern 声明列表，
   函数体本身又以内层 lambda 开头，文本搜索无法定位定义括号。
   在发射过程中插入则无歧义。
3. **触发条件**：按指令的 `callee`/`targetSymbol` 是否指向这两个访问器。
   实测该 IR **只填 `callee`**（形如
   `System.Private.CoreLib/System.Reflection.Assembly::GetExecutingAssembly:...`），
   `targetSymbol` 缺席 —— 故按方法名匹配两种拼写。仅命中的方法获得 bracket
   （P1：不给全量方法加 TLS 写）。
4. **header helper**：`chaos_executing_image_handle()` 返回本 TU `kReflImage` 的
   编码句柄。**必须放在与定义相同的 codegen namespace 内** —— 否则 C++ 修饰名
   不同，链接期报 unresolved external（已实测并修正）。

### 验证

- build ✅（495 subjects → entry.exe）、contract ✅、fact 与基线逐项一致
- 注入 21 处，形态确认为函数体首行

### ⚠️ 未解决：AOT 侧无法验证其语义

尝试了三层验证，**均无法判定 REF-RISK-7 是否真的生效**：

| 验证方式 | 为何不能证明 |
|---|---|
| reflection chunk 的 fact 断言 | 只查 `result != null`。CoreLib 回退同样非 null，**无法判别** |
| 语义契约套件（`corelib-reflection-contract`） | 是**独立 .NET 8 控制台**（`dotnet run`），跑在参考实现上，**不消费 AOT 产物** |
| 反向测试（关掉接线看是否转红） | 实测**仍然 73/73 通过** —— 坐实了契约不覆盖 AOT |

**结论**：REF-RISK-7 的 AOT 语义**当前无验证手段**。要证明它，需要一条能在
`entry.exe` 内完成判别性断言的路径（例如让 ATG 生成「比较 GetExecutingAssembly
的程序集名与预期」的断言），属**独立的基础设施工作**。

**已如实标注**：契约套件中相关断言的注释已改为 "reference-runtime contract"，
并加了 scope note 说明它不覆盖 AOT。
