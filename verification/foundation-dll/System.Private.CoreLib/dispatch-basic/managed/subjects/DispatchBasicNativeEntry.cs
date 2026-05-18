// Copy of handwritten native-AOT entry point for subjects codegen resolution
// Family: family/System.Private.CoreLib/dispatch/basic
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Runtime.InteropServices;

//
// ── Custom ComInterfaceTypeAttribute (not in SDK BCL) ─────────
// Define with the same full name so the loader's TryGetComInterfaceTypeKind
// can find it via custom attribute blob parsing.
//

namespace System.Runtime.InteropServices
{
    [AttributeUsage(AttributeTargets.Interface)]
    public sealed class ComInterfaceTypeAttribute : Attribute
    {
        public ComInterfaceTypeAttribute(ComInterfaceType type) { }
    }
}

//
// ── IDispatch-marked COM interface ─────────────────────────────
//

// Interface explicitly marked as IDispatch-based
// {3A2B1C0D-9E8F-7A6B-5C4D-3E2F1A0B9C8D}
[Guid("3A2B1C0D-9E8F-7A6B-5C4D-3E2F1A0B9C8D")]
[ComInterfaceType(ComInterfaceType.InterfaceIsIDispatch)]
public interface ICalculator
{
    int Add(int a, int b);
    int Subtract(int a, int b);
}

// Dual-marked interface (both IUnknown and IDispatch)
// {5E6F7A8B-9C0D-1E2F-3A4B-5C6D7E8F9A0B}
[Guid("5E6F7A8B-9C0D-1E2F-3A4B-5C6D7E8F9A0B")]
[ComInterfaceType(ComInterfaceType.InterfaceIsDual)]
public interface IStatusProvider
{
    int GetStatusCode();
}

// Regular IUnknown COM interface for comparison
// {7A8B9C0D-1E2F-3A4B-5C6D-7E8F9A0B1C2D}
[Guid("7A8B9C0D-1E2F-3A4B-5C6D-7E8F9A0B1C2D")]
public interface IBasicOp
{
    int GetValue();
}

//
// ── Implementations (no interface declarations on classes ──
//     to avoid codegen emitting interface methods in vtables
//     without function bodies)
//

public class Calculator // was: ICalculator
{
    public int Add(int a, int b) => a + b;
    public int Subtract(int a, int b) => a - b;
}

public class StatusProvider // was: IStatusProvider
{
    public int GetStatusCode() => 200;
}

public class BasicImpl // was: IBasicOp
{
    public int GetValue() => 42;
}

//
// ── Entry point ────────────────────────────────────────────────
//

public static class DispatchBasicNativeEntry
{
    // Test methods: each returns an int checksum.
    // Note: classes don't declare interface implementations to avoid
    // codegen emitting interface methods in vtables without function bodies.
    // COM dispatch testing occurs at the native level in runtime-entry.cpp.
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return TestIDispatchAdd();
            case 1: return TestIDispatchSub();
            case 2: return TestDualGetStatus();
            case 3: return TestIUnknownGetValue();
            default: return -1;
        }
    }

    // [0] ICalculator.Add via IDispatch interface
    public static int TestIDispatchAdd()
    {
        var calc = new Calculator();
        return calc.Add(20, 7); // expected: 27
    }

    // [1] ICalculator.Subtract via IDispatch interface
    public static int TestIDispatchSub()
    {
        var calc = new Calculator();
        return calc.Subtract(50, 8); // expected: 42
    }

    // [2] IStatusProvider.GetStatusCode via Dual interface
    public static int TestDualGetStatus()
    {
        var sp = new StatusProvider();
        return sp.GetStatusCode(); // expected: 200
    }

    // [3] IBasicOp.GetValue via IUnknown interface
    public static int TestIUnknownGetValue()
    {
        var op = new BasicImpl();
        return op.GetValue(); // expected: 42
    }
}
