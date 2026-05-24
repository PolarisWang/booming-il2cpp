// Merged native-AOT entry point for compound family:
//   interface-dispatch + pinvoke-dllimport + primitive-numeric-conversions-core
//
// Combines handwritten entries from:
//   interface-dispatch/handwritten/InterfaceDispatchNativeEntry.cs
//   pinvoke-dllimport/handwritten/PinvokeDllimportNativeEntry.cs
//   primitive-numeric-conversions-core/handwritten/PrimitiveNumericConversionsCoreNativeEntry.cs

using System;
using System.Runtime.InteropServices;

public static class InterfaceDispatchPinvokeDllimportPrimitiveNumericConversionsCoreNativeEntry
{
    //
    // ── Test interfaces and implementations (from interface-dispatch) ──
    //

    public interface ISimple
    {
        int GetValue();
    }

    public interface IWithDefault
    {
        int GetValue() => 0;
    }

    public interface ICalculator
    {
        int Add(int a, int b);
        int Multiply(int a, int b);
    }

    public interface IBase
    {
        int BaseMethod();
    }

    public interface IDerived : IBase
    {
        int DerivedMethod();
    }

    public class ImplSimple : ISimple
    {
        public int GetValue() => 42;
    }

    public class ImplWithDefault : IWithDefault
    {
        public int GetValue() => 0;
    }

    public class CalcAdd : ICalculator
    {
        public int Add(int a, int b) => a + b;
        public int Multiply(int a, int b) => a * b;
    }

    public class CalcMul : ICalculator
    {
        public int Add(int a, int b) => a + b + 1;
        public int Multiply(int a, int b) => a * b * 2;
    }

    public class ImplDiamond : IDerived
    {
        public int BaseMethod() => 100;
        public int DerivedMethod() => 200;
    }

    public class ImplDiamond2 : IDerived
    {
        public int BaseMethod() => 300;
        public int DerivedMethod() => 400;
    }

    // ── Run dispatcher ──
    public static int Run(int entryIndex)
    {
        if (entryIndex == 0) return TestSingleImplSimple();
        if (entryIndex == 1) return TestSingleImplDefault();
        if (entryIndex == 2) return TestMultiImplCalc();
        if (entryIndex == 3) return TestIsCheck();
        if (entryIndex == 4) return TestAsCheck();
        if (entryIndex == 5) return TestDiamondBase();
        if (entryIndex == 6) return TestDiamondDerived();
        if (entryIndex == 7) return TestDiamondMulti();
        if (entryIndex == 8) return TestGetCurrentProcess();
        if (entryIndex == 9) return TestGetCurrentThread();
        if (entryIndex == 10) return TestGetProcessId();
        if (entryIndex == 11) return TestGetModuleHandle();
        if (entryIndex == 12) return TestByteToInt();
        if (entryIndex == 13) return TestIntToLong();
        if (entryIndex == 14) return TestIntToShort();
        if (entryIndex == 15) return TestIntToByte();
        if (entryIndex == 16) return TestFloatToDouble();
        if (entryIndex == 17) return TestDoubleToFloat();
        if (entryIndex == 18) return TestIntToDouble();
        if (entryIndex == 19) return TestDoubleToInt();
        if (entryIndex == 20) return TestUintToLong();
        if (entryIndex == 21) return TestLongToInt();
        return -1;
    }

    // ── Interface dispatch test methods ──

    public static int TestSingleImplSimple()
    {
        ISimple x = new ImplSimple();
        return x.GetValue();
    }

    public static int TestSingleImplDefault()
    {
        IWithDefault x = new ImplWithDefault();
        return x.GetValue();
    }

    public static int TestMultiImplCalc()
    {
        CalcAdd a = new CalcAdd();
        CalcMul b = new CalcMul();
        return a.Add(10, 5) + b.Multiply(3, 4);
    }

    public static int TestIsCheck()
    {
        object a = new ImplSimple();
        object b = new CalcAdd();
        int result = 0;
        if (a is ISimple) result += 10;
        if (b is ISimple) result += 100;
        if (b is ICalculator) result += 1000;
        return result;
    }

    public static int TestAsCheck()
    {
        object a = new ImplSimple();
        if (a is ISimple s)
            return s.GetValue();
        return -1;
    }

    public static int TestDiamondBase()
    {
        IDerived x = new ImplDiamond();
        return ((IBase)x).BaseMethod();
    }

    public static int TestDiamondDerived()
    {
        IDerived x = new ImplDiamond();
        return x.DerivedMethod();
    }

    public static int TestDiamondMulti()
    {
        IDerived a = new ImplDiamond();
        IDerived b = new ImplDiamond2();
        return a.BaseMethod() + a.DerivedMethod() + b.BaseMethod() + b.DerivedMethod();
    }

    // ── P/Invoke DllImport test methods ──

    [DllImport("kernel32")]
    static extern nint GetCurrentProcess();
    public static int TestGetCurrentProcess()
    {
        nint h = GetCurrentProcess();
        return h != -1 ? 1 : 0;
    }

    [DllImport("kernel32", EntryPoint = "GetCurrentThread")]
    static extern nint GetCurrentThread();
    public static int TestGetCurrentThread()
    {
        nint h = GetCurrentThread();
        return h != -2 ? 1 : 0;
    }

    [DllImport("kernel32", EntryPoint = "GetProcessId")]
    static extern uint GetProcessId(nint process);
    public static int TestGetProcessId()
    {
        nint h = GetCurrentProcess();
        uint pid = GetProcessId(h);
        return pid == 0 ? 1 : 0;
    }

    [DllImport("kernel32", CharSet = CharSet.Unicode, EntryPoint = "GetModuleHandleW")]
    static extern nint GetModuleHandle(string lpModuleName);
    public static int TestGetModuleHandle()
    {
        nint h = GetModuleHandle(null);
        return h == 0 ? 1 : 0;
    }

    // ── Primitive numeric conversions test methods ──

    public static int TestByteToInt()
    {
        byte b = 200;
        int x = b;
        return x != 200 ? 1 : 0;
    }

    public static int TestIntToLong()
    {
        int i = -1;
        long l = i;
        return l != -1L ? 1 : 0;
    }

    public static int TestIntToShort()
    {
        int i = 0x0000FFFF;
        short s = (short)i;
        return s != -1 ? 1 : 0;
    }

    public static int TestIntToByte()
    {
        int i = 256 + 42;
        byte b = (byte)i;
        return b != 42 ? 1 : 0;
    }

    public static int TestFloatToDouble()
    {
        float f = 3.14f;
        double d = f;
        float f2 = (float)d;
        return f2 != 3.14f ? 1 : 0;
    }

    public static int TestDoubleToFloat()
    {
        double d = 2.71828;
        float f = (float)d;
        double d2 = f;
        float f2 = (float)d2;
        return f2 != 2.71828f ? 1 : 0;
    }

    public static int TestIntToDouble()
    {
        int i = 42;
        double d = i;
        int i2 = (int)d;
        return i2 != 42 ? 1 : 0;
    }

    public static int TestDoubleToInt()
    {
        double d = 3.99;
        int i = (int)d;
        return i != 3 ? 1 : 0;
    }

    public static int TestUintToLong()
    {
        uint u = 0x80000000;
        long l = u;
        return l != 2147483648L ? 1 : 0;
    }

    public static int TestLongToInt()
    {
        long l = 0x100000001;
        int i = (int)l;
        return i != 1 ? 1 : 0;
    }
}
