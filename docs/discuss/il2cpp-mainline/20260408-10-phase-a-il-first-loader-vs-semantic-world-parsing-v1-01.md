# Phase A IL-First Loader vs SemanticWorld Parsing v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

在 `IL-first` 路线下，经常会出现一个很自然但也很危险的混淆：

- 既然不从 C# 源码 AST 出发，那 `Loader` 和 `SemanticWorld` 不都是在“解析 IL”吗？

表面上看确实都是在处理：

- assembly
- metadata
- method body
- IL 指令

但如果不把这两层的“解析”区分开，后面很容易重新塌成一层：

1. `Loader` 开始顺手做语义判断
2. `SemanticWorld` 退化成更漂亮的 passthrough
3. `Linker / CodeGen / Emitter` 又要回来补语义

所以这一步真正要回答的是：

- **在 IL-first 下，`Loader` 和 `SemanticWorld` 分别到底在“解析”什么。**

## 2. 一句话先说清

我建议先用两句话把它们钉住：

- `Loader` 解析的是：**编译后输入的结构事实**
- `SemanticWorld` 解析的是：**这些结构事实在当前支持切片里的语义含义**

也就是说：

- `Loader` 更接近 `decode`
- `SemanticWorld` 更接近 `semantic normalization`

## 3. `Loader` 到底在解析什么

在 `IL-first` 下，`Loader` 处理的对象是：

- PE/assembly 容器
- ECMA-335 metadata tables
- method body blob
- raw IL instruction stream
- member/type/signature/import 引用

它的任务可以理解成：

- **把“编译器产物里的字节和表”读成结构化记录**

### `Loader` 的核心职责

我建议继续保持下面这个边界：

1. 读取 PE / metadata / IL
2. 解析 type / field / property / method / parameter 的原始记录
3. 解析 signature、owner、generic instantiation、member reference
4. 解析 method body 中的 raw IL instructions
5. 形成一套 raw but structured 的 loaded model

### `Loader` 的输出更像什么

更像：

- “我把文件里的东西读出来了”

而不是：

- “我已经知道这段程序在当前主线里属于哪种 semantic slice”

### 一个最小例子

看这段代码：

```csharp
class Greeter
{
    private readonly string _name;

    public Greeter(string name)
    {
        _name = name;
    }

    public string Build()
    {
        return "Hello, " + _name;
    }
}

static void Main()
{
    var g = new Greeter("Alice");
    Console.WriteLine(g.Build());
}
```

`Loader` 在这里会读出类似这样的事实：

- 有一个 type：`Greeter`
- 有一个 field：`Greeter::_name`
- 有一个 method：`Greeter::.ctor(string)`
- 有一个 method：`Greeter::Build()`
- 有一个 method：`Program::Main()`
- `Main()` 的 method body 里有：
  - `ldstr "Alice"`
  - `newobj Greeter::.ctor(string)`
  - `callvirt Greeter::Build()`
  - `call System.Console::WriteLine(string)`
- `Build()` 的 method body 里有：
  - `ldstr "Hello, "`
  - `ldfld Greeter::_name`
  - `call System.String::Concat(...)`

注意这里 `Loader` 还没有说：

- 这是不是 “captured-state-instance-message” family
- `_name` 是不是当前主线里的 captured state field
- `String.Concat(a,b,c)` 最后要不要拆成 pair concat

这些都不该由它回答。

## 4. `SemanticWorld` 到底在解析什么

`SemanticWorld` 处理的不是文件字节，而是：

- `Loader` 已经读出来的结构化事实

它的任务是：

- **把这些 raw 结构事实，归一化成当前支持切片里的 canonical semantic meaning**

也就是说它回答的是：

- “这在当前主线里到底意味着什么”

### `SemanticWorld` 的核心职责

我建议继续保持前面已经收住的 5 类输出：

1. canonical identity
2. canonical member shape
3. canonical semantic operation stream
4. capability / requirement hints
5. semantic diagnostics

### 它不是再读一遍 IL，而是在解释 IL

这里最重要的一点是：

- `SemanticWorld` 不是重新做字节解码
- 它是在当前支持语义子集里，对 `Loader` 输出做“语义定性”

### 还是看同一个例子

对 `Greeter` 例子，`SemanticWorld` 应该开始讲出这样的语义：

- `Greeter`
  - 是一个 reference type
- `_name`
  - 是 instance field
- `.ctor(string)`
  - 是 constructor role
- `Build()`
  - 是 instance-message method
- `Main()`
  - 是 entry method

method body 级别，它不再只说：

- `newobj`
- `ldfld`
- `call`

而是开始归一化成：

- `new-object`
- `read-instance-field`
- `call-instance`
- `string-concat`
- `call-static(console-write-line)`

同时它还会输出 capability / requirement hints，例如：

- `uses-instance-field-state`
- `requires-string-concat`
- `requires-console-string-output`

这时才真正开始进入“当前主线语义世界”。

## 5. 两者最核心的区别

如果把它们压到最本质，可以这样区分：

### `Loader`

回答：

- 文件里有什么
- IL 里写了什么
- metadata 里指向了谁

### `SemanticWorld`

回答：

- 这些东西在当前支持切片里意味着什么
- 它们属于什么 canonical semantic shape
- 后续 `Linker / CodeGen` 应该看见什么语义对象

所以它们不是：

- 同一层的两种写法

而是：

- 前者做结构解码
- 后者做语义归一化

## 6. 再举一个更容易混淆的例子

看这句：

```csharp
Console.WriteLine(string.Concat(a, b, c));
```

### `Loader` 会看到什么

- 一个对 `System.String::Concat(...)` 的 raw call
- 一个对 `System.Console::WriteLine(string)` 的 raw call

### `SemanticWorld` 应该讲什么

- 当前存在 `string-concat` 语义
- 当前存在 `console-string-output` 语义
- 输出 capability：
  - `requires-string-concat`
  - `requires-console-string-output`

### 哪个层不该做什么

- `Loader`
  - 不该说这是 pair concat 还是 direct concat
- `SemanticWorld`
  - 也不该决定最后选哪个 lowering
- `CodeGen`
  - 才负责把 `semantic.string-concat` 进一步落成某种 lowering family / lowering policy

这个例子特别能说明：

- `SemanticWorld` 的“解析”不是 lowering
- 它仍然停留在 semantic layer

## 7. 在 IL-first 下，两层分别依赖什么技术底座

从当前仓库材料看，这条路线已经相当明确：

- 主前端读取层
  - `System.Reflection.Metadata (incl. PEReader)`
- 对照/测试基线
  - `Mono.Cecil`

所以更准确地说：

- `Loader`
  - 更接近建立在 `PEReader + MetadataReader + method body decode` 之上
- `SemanticWorld`
  - 更接近建立在仓库自己的 canonical semantic vocabulary 之上

也就是说：

- `Loader` 依赖底层读取库
- `SemanticWorld` 依赖我们自己定义的 semantic contracts

## 8. `Loader` 明确不该解析到哪里

在 IL-first 下，我建议继续明确禁止 `Loader` 做这些事：

- capability 分类
- semantic slice 判定
- retained/dependency policy
- lowering family 选择
- sample/proof shape 识别

一句话说：

- **`Loader` 只负责“把编译后事实读出来”，不负责“给这些事实下业务语义结论”。**

## 9. `SemanticWorld` 明确不该解析到哪里

同样也要明确 `SemanticWorld` 不该继续越界到：

- retained world 决策
- external dependency closure
- lowering family 选择
- template/backend 选择

一句话说：

- **`SemanticWorld` 负责“讲清语义”，不负责“决定保留什么/怎么生成”。**

## 10. 我给你的最终理解框架

如果你想快速记住，我建议就记下面这组：

- `Loader`
  - `assembly/metadata/IL decoder`
- `SemanticWorld`
  - `current-slice semantic normalizer`

再展开一点就是：

1. `Loader`
   - 把 PE、metadata、IL 指令、signature、member refs 读成结构化记录
2. `SemanticWorld`
   - 把这些结构化记录归一化成 canonical semantic objects、semantic operations、capability hints
3. `Linker`
   - 决定保留什么、依赖什么
4. `CodeGen`
   - 决定怎么降成 C++

## 11. 我的结论

所以对“IL-first 下 Loader/SemanticWorld 分别怎么解析”的最短回答是：

- `Loader` 解析 **编译后输入的结构**
- `SemanticWorld` 解析 **这些结构在当前支持切片里的语义**

如果用更口语一点的话：

- `Loader` 负责“读出来”
- `SemanticWorld` 负责“看明白”

## 12. 这一步之后最值得继续讨论的点

如果沿这个理解继续，下一步最自然的问题就是：

- **`SemanticWorld` 的 canonical semantic operation stream 到底要冻结到什么粒度**

因为只要 operation 粒度不稳：

- `Loader -> SemanticWorld`
 这条边界就还会继续摇摆。
