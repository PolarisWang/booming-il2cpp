# Lowering 与 Emission 决策

**来源**: `docs/archive/discuss/il2cpp-mainline/20260407-05-*`, `20260407-06-*`, `20260408-05-*`, `20260408-08-*`, `20260408-09-*`, `20260408-18-*`, `20260408-22-*`, `20260408-23-*`

## 决策

### LoweringFamily Vocabulary

可扩展的 lowering 方案枚举，每条 IL 指令/模式至少对应一个 family：

| Family | 用途 | 示例 |
|--------|------|------|
| DirectCall | 直接函数调用 | `call` 静态方法 |
| VirtualCall | 虚方法分发 | `callvirt` |
| DelegateInvoke | 委托调用 | `delegate_invoke` |
| NewObj | 对象构造 | `newobj` |
| Box | 装箱 | `box` |
| Unbox | 拆箱 | `unbox` / `unbox.any` |
| TypeCheck | 类型检查 | `castclass` / `isinst` |
| ArrayOp | 数组操作 | `newarr` / `ldelem` / `stelem` |
| EH | 异常处理 | try/catch/filter/finally |
| StringId | 字符串 ID | `ldstr` / StringId 操作 |
| StaticInit | 静态初始化 | cctor 降级 |

### LoweringPlan Role Contract

LoweringPlan 连接 CodeGen 与 Emitter 的合约：

```
LoweringPlan
 ├── MethodHeader     -- 方法签名、栈大小、EH 表
 ├── InstructionPlans -- 每条指令的 lowering 方案列表
 ├── HelperRequests   -- 需要的 runtime helper 集合
 ├── StringIdTable    -- 编译期已知字符串 ID 表
 ├── GenericContext   -- 泛型上下文信息
 └── EmissionHints    -- Emitter 选择提示（非指令级 shape）
```

- Emitter 只能消费 LoweringPlan，不得扫 IL 猜 shape
- CodeGen 不得让 Emitter 反推语义

### EmissionOrder

Emitter 的驱动管线：
1. 方法头（签名、栈帧、EH 区域声明）
2. 每条指令按 IL 顺序 emission
3. Helper 声明和方法声明配对
4. 方法尾（闭合块、返回）

### SemanticShapes

CodeGen 识别的语义形状，每个 shape 对应一组 emission 模板：

| Shape | 对应模板 | 说明 |
|-------|---------|------|
| PrimitiveComputation | 直接 C++ 运算符 | add/sub/mul 等 |
| MethodInvocation | 函数调用 | call/callvirt 等 |
| TypeCast | 类型转换 | castclass/isinst 等 |
| ObjectAllocation | 对象分配 | newobj/newarr 等 |
| ExceptionHandling | EH 结构 | try/catch/finally 等 |
| StringOperation | 字符串操作 | ldstr/字符串比较 |

### CapabilityBundles

类型能力的描述集合，供 CodeGen 选择 lowering 方案：
- 值大小、向量宽度
- 是否有 finalizer
- 是否为值类型/引用类型
- 是否为泛型实例化

### MethodSemanticBody

方法体的完整语义表示（CodeGen 的输入，Emission 的依据）：

```
MethodSemanticBody
 ├── Parameters         -- 参数列表
 ├── LocalVariables     -- 局部变量表
 ├── Operations         -- 语义操作序列
 ├── EHRegions          -- 异常处理区域
 └── CapabilityHints    -- 类型能力提示
```

## 关键约束

- LoweringFamily 的选择完全由 CodeGen 决定
- Emitter 只消费 LoweringPlan + SemanticShape
- Emitter 不得通过指令形状探测反推"这是哪一种 proof"
