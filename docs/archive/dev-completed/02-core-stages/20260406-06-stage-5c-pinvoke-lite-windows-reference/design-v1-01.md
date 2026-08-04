# Stage 5C PInvokeLite Windows Reference Design

Date: 2026-04-06 16:05:00 +08:00
Status: ready-for-plan

## 设计目标

在现有 Stage 4 `Windows x64` reference spine、Stage 5A generic foundation 与 Stage 5B reflection foundation 之上，先打通第一条最小 P/Invoke import 路径：

- `DllImport`
- 稳定的整数参数
- 稳定的整数返回值
- 稳定字符串输出

目标不是第一轮就覆盖完整 smoke `PInvokeLite`，而是先让 Stage 5C 有一个可验证、可调试、可继续扩展的 interop foundation。

## 设计边界

### 纳入

- 一个新的最小 `DllImport` import-only proof 输入
- `Loader -> SemanticWorld -> Linker -> MetadataWriter -> CodeGen` 路径上的最小 import descriptor 支持
- generated C++ 中承接最小 Windows import call 的 glue
- 与 Stage 4 / Stage 5A / Stage 5B 并存的回归验证

### 不纳入

- 完整 smoke `PInvokeLite` 的全部 surface
- `UnmanagedCallersOnly`
- native export
- `NativeLibrary.TryLoad` / `TryGetExport`
- `Marshal.StringToCoTaskMemUTF8` / `PtrToStringUTF8`
- 动态库搜索策略扩展
- `macOS` parity
- 任何把样例判断写进 `src/**` 的做法

## 关键设计结论

1. Stage 5C 第一轮 proof 输入必须是 import-only sample，而不是完整 smoke `PInvokeLite`。

推荐形状：

```csharp
using System.Runtime.InteropServices;

internal static class NativeMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern int MulDiv(int number, int numerator, int denominator);
}

internal static class Program
{
    private static int Main()
    {
        Console.WriteLine($"pinvoke-ok|{NativeMethods.MulDiv(6, 7, 3)}");
        return 0;
    }
}
```

这条路径同时覆盖：

- managed `DllImport` declaration
- Windows native import descriptor
- blittable integer parameter passing
- blittable integer return value
- 稳定 proof 输出

但又不会额外引入 export、动态库装载与字符串 marshalling。

2. 第一轮先补 import descriptor 与 generated import glue，再决定是否继续扩到 export/marshal。

原因：

- 当前最早的真实 blocker大概率在 `DllImport` descriptor materialization、符号名/模块名保留与 generated call glue，而不是更高阶 marshalling。
- 如果第一轮就把 `Marshal`、`NativeLibrary` 或 `UnmanagedCallersOnly` 带进来，会把 loader、runtime helper、导出命名与 host lookup 一起混入。
- 只有 import path 先稳定，后续才有资格讨论 native export、动态加载与 UTF-8 buffer marshalling。

3. 第一轮优先使用 Windows 已有系统库与稳定整数结果，不额外引入动态加载。

不能接受的状态是：

- managed closure 能保留 `DllImport` 方法
- 但 descriptor / generated glue 里找不到稳定的模块名、入口点或签名
- 或 proof run 依赖额外的 `LoadLibrary` / `GetProcAddress` 才能工作

如果第一轮只验证 import path，就应尽量把动态加载与导出查找留到后续单独验证。

4. `src/**` 继续保持 sample-agnostic。

允许变化的是：

- `DllImport` descriptor / registration 的最小扩展
- generated import helper / native declaration
- Windows proof host 的最小接线

不允许的是：

- 判断 `PInvokeLite`
- 判断测试工程名
- 针对某个 proof sample 写死模块名/符号名分支

5. Stage 4 `HelloWorldObject`、Stage 5A `GenericEcho` 与 Stage 5B `ReflectionLite` gates 继续作为 Stage 5C 回归基线。

任何 Stage 5C 改动都不能以破坏现有真实 proof 为代价。

## 设计结果

Stage 5C 作为单个 `plan` 子任务继续推进。下一步进入 `plan-v1-01.md`，按 TDD 先钉最小 `DllImport` import RED，再逐层补 descriptor、generated import glue 与 proof host。
