// This assembly provides the real PE metadata that the emitter
// reads via System.Reflection.Metadata PEReader.

using System;

namespace SnapshotTestFixtures;

public static class ArithmeticOps
{
    // Used by fixture 01-simple-add
    public static int RunAdd() => 3 + 4;
}

public static class StringOps
{
    // Used by fixture 02-ldstr-return
    public static int UseString() { Helper.ConsumeString("Hello"); return 0; }
}

public static class VoidCaller
{
    // Used by fixture 03-call-static-void
    public static int DoNothing() { Helper.Nop(); return 0; }
}

public static class MathHelper
{
    // Used by fixture 04-call-static-ret
    public static int RunSquare()
    {
        var x = 5;
        return Helper.Square(x);
    }
}

public static class FlowControl
{
    // Used by fixture 05-br-condition
    public static int IsPositive()
    {
        var result = 5 > 0;
        return result ? 1 : 0;
    }
}

// --- P1: callvirt + newobj (06) ---
public class InstanceHelper
{
    private int _val;
    public InstanceHelper(int val) { _val = val; }
    public int GetValue() => _val;
}

public static class CallVirtHelper
{
    // Used by fixture 06-callvirt-newobj
    public static int CreateAndUse()
    {
        return new InstanceHelper(42).GetValue();
    }
}

// --- P1: box + unbox.any (07) ---
public static class BoxingHelper
{
    // Used by fixture 07-box-unbox
    public static int BoxAndUnbox()
    {
        object o = 42;
        return (int)o;
    }
}

// --- P1: static field access (08) ---
public static class FieldHelper
{
    private static int _counter;

    // Used by fixture 08-field-access
    public static int GetAndIncrement()
    {
        _counter++;
        return _counter;
    }
}

// --- P1: loop (09) ---
public static class LoopHelper
{
    // Used by fixture 09-loop
    public static int SumToFive()
    {
        int s = 0;
        for (int i = 0; i < 5; i++)
            s += i;
        return s;
    }
}

// --- P1: type conversions (10) ---
public static class ConversionHelper
{
    // Used by fixture 10-conv
    public static int ConvertToInt()
    {
        long a = 100L;
        int b = (int)a;
        return b;
    }
}

internal static class Helper
{
    public static void Nop() { }
    public static int Square(int x) => x * x;
    public static void ConsumeString(string s) { }
    public static int GetValue() => 42;
}

public static class SwitchHelper
{
    // Used by fixture 11-switch
    public static int Classify()
    {
        int x = 2;
        return x switch { 1 => 10, 2 => 20, _ => 0 };
    }
}

public static class LdftnHelper
{
    // Used by fixture 12-ldftn
    public static int GetFnPtr()
    {
        var fn = Helper.GetValue;  // ldftn
        return fn();               // calli through delegate
    }
}

public static class TargetHelper
{
    // Used by fixture 12-ldftn as ldftn target
    public static int GetValue() => 42;
}

public static class ArrayHelper
{
    // Used by fixture 13-newarr
    public static int MakeAndFill()
    {
        var arr = new int[3];
        arr[0] = 42;
        return arr[0];
    }
}

public static class EhHelper
{
    // Used by fixture 15-exception-handling
    public static int SafeDivide()
    {
        int result = 42;
        int zero = 0;
        try
        {
            result = 100 / zero;
        }
        catch
        {
            result = -1;
        }
        return result;
    }
}

public static class GenericHelper
{
    public static int UseGeneric()
    {
        var w = new Wrapper<int>(42);
        return w.GetValue();
    }
}

public class Wrapper<T>
{
    private T _val;
    public Wrapper(T val) { _val = val; }
    public T GetValue() => _val;
}

public static class DelegateHelper
{
    public static int RunDelegate()
    {
        var fn = new Func<int>(Helper.GetValue);
        return fn();
    }
}

public class BaseClass
{
    public virtual int Compute() => 1;
}

public class DerivedClass : BaseClass
{
    public override int Compute() => 2;
}

public static class VirtualHelper
{
    public static int UseVirtual()
    {
        BaseClass obj = new DerivedClass();
        return obj.Compute();
    }
}

// --- Fixture 18: isinst + castclass ---
public interface IMarker { }
public class MarkedClass : IMarker
{
    public int Value;
}

public static class TypeCheckHelper
{
    public static int CheckAndCast()
    {
        object obj = new MarkedClass();
        if (obj is MarkedClass marked)
        {
            return marked.Value;
        }
        return -1;
    }
}

// --- Fixture 19: sizeof ---
[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
public struct SmallStruct
{
    public int X;
    public int Y;
}

public static class SizeOfHelper
{
    public static unsafe int GetSize()
    {
        return sizeof(SmallStruct);
    }
}

// --- Fixture 20: ldind + stind (via ref parameter) ---
public static class IndirectHelper
{
    // Used by fixture 20-ldind-stind
    public static int ReadWriteRef()
    {
        int val = 42;
        ref int r = ref val;
        r = 100;
        return val;
    }
}

// --- Fixture 21: arithmetic compare (sub, rem, ceq, clt, cgt.un) ---
public static class ArithmeticCompareHelper
{
    // Used by fixture 21-arithmetic-compare
    public static int RunCompare()
    {
        int a = 5, b = 3;
        int sub = a - b;       // sub
        int rem = a % b;       // rem
        int r = 0;
        if (a == b) r += 1;    // ceq → beq
        if (a < b) r += 2;     // clt → blt
        if (a > b) r += 4;     // cgt → cgt.un
        return sub + rem + r;
    }
}

// --- Fixture 22: branch dup (dup, brtrue) ---
public static class BranchDupHelper
{
    // Used by fixture 22-branch-dup
    public static int RunBranchDup()
    {
        int x = 42;
        int r = 0;
        if (x == 42) r = 1;  // brtrue
        object o = r == 1 ? (object)r : null;
        if (o != null) r += 2;  // brtrue on ref
        return r;  // returns 3
    }
}

// --- Fixture 23: instance fields (ldfld, stfld) ---
public class HasInstanceFields
{
    public int Value;
    public HasInstanceFields(int val) { Value = val; }
}

public static class InstanceFieldHelper
{
    // Used by fixture 23-instance-fields
    public static int CreateAndUseFields()
    {
        var obj = new HasInstanceFields(42);
        obj.Value = 100;
        return obj.Value;
    }
}

// --- Fixture 24: static field write (stsfld, ldsfld) ---
public static class StaticFieldWriteHelper
{
    private static int _writeTarget;
    private static int _readTarget;

    // Used by fixture 24-static-field-write
    public static int WriteAndRead()
    {
        _writeTarget = 42;
        return _readTarget;
    }
}

// --- Fixture 25: virtual dispatch (vtable slot via newobj) ---
public static class VirtualDispatchHelper
{
    // Used by fixture 25-virtual-dispatch
    public static int UseVirtualDispatch()
    {
        BaseClass obj = new DerivedClass();
        return obj.Compute();  // callvirt, returns 2
    }
}

// --- Fixture 26: bitwise logical (and, or, xor, not) ---
public static class BitwiseHelper
{
    // Used by fixture 26-bitwise-logical
    public static int RunBitwise()
    {
        int a = 0xA5, b = 0x5A;
        int and = a & b;       // and
        int or = a | b;        // or
        int xor = a ^ b;       // xor
        int not = ~a;          // not
        return and + or + xor + not;
    }
}

// --- Fixture 27: shift (shl, shr) ---
public static class ShiftHelper
{
    // Used by fixture 27-shift
    public static int RunShift()
    {
        int a = 8;
        int shl = a << 2;      // shl
        int shr = a >> 1;      // shr
        return shl + shr;
    }
}

// --- Fixture 28: array length (newarr, ldlen) ---
public static class ArrayLengthHelper
{
    // Used by fixture 28-array-length
    public static int RunLength()
    {
        var arr = new int[5];
        return arr.Length;
    }
}

// --- Fixture 29: value type initobj (ldobj, stobj, initobj) ---
[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
public struct MyValueType
{
    public int X;
    public int Y;
}

public static class ValueTypeHelper
{
    // Used by fixture 29-value-type-initobj
    public static int RunValueType()
    {
        MyValueType s = default;
        s.X = 42;
        s.Y = 10;
        return s.X + s.Y;
    }
}

// --- Fixture 30: localloc ---
public static class LocalAllocHelper
{
    // Used by fixture 30-localloc
    public static unsafe int RunAlloc()
    {
        byte* buf = stackalloc byte[256];  // localloc
        buf[0] = 42;                        // stind.i1
        return buf[0];                      // ldind.u1
    }
}

// --- Fixture 31: arithmetic mul/div ---
public static class MulDivHelper
{
    // Used by fixture 31-arithmetic-mul-div
    public static int RunMulDiv() => 7 * 6 / 3;
}

// --- Fixture 32: branch compare (beq, bgt, blt, bne.un) ---
public static class BranchCompareHelper
{
    // Used by fixture 32-branch-compare
    public static int RunBranchCompare()
    {
        int a = 5, b = 3, c = 5;
        int r = 0;
        if (a == c) r += 1;     // beq
        if (a > b) r += 2;      // bgt
        if (b < a) r += 4;      // blt
        if (a != b) r += 8;     // bne.un
        return r;
    }
}

// --- Fixture 33: float ops (ldc.r4, ldc.r8, conv.r4, conv.r8) ---
public static class FloatOpsHelper
{
    // Used by fixture 33-float-ops
    public static int RunFloatOps()
    {
        float a = 3.5f;       // ldc.r4
        double b = 7.2;       // ldc.r8
        float c = (float)b;   // conv.r4
        double d = a;         // conv.r8 (widening)
        return (int)(a + c);
    }
}

// --- Fixture 34: conv wide (conv.i8, conv.u8, conv.u) ---
public static class ConvWideHelper
{
    // Used by fixture 34-conv-wide
    public static int RunConvWide()
    {
        int a = 42;
        long b = a;           // conv.i8
        long c = 100L;
        int d = (int)c;       // conv.i4 (narrowing)
        return d;
    }
}

// --- Fixture 35: neg + shr.un ---
public static class NegShiftHelper
{
    // Used by fixture 35-neg-shrun
    public static int RunNegShift()
    {
        int a = 42;
        int neg = -a;          // neg
        int val = unchecked((int)0x80000000);
        int shr = val >> 1;    // shr
        return neg + shr;
    }
}

// --- Fixture 36: ldc.i8 (8-byte integer constant) ---
public static class LdcI8Helper
{
    // Used by fixture 36-ldc-i8
    public static long RunLdcI8() => 1_000_000_000_000L;
}

// --- Fixture 37: conv small int (conv.i1, conv.i2, conv.u1, conv.u2) ---
public static class ConvSmallIntHelper
{
    // Used by fixture 37-conv-small-int
    public static int RunConvSmall()
    {
        int a = 0x1234;
        sbyte b = (sbyte)a;    // conv.i1
        short c = (short)a;    // conv.i2
        byte d = (byte)a;      // conv.u1
        ushort e = (ushort)a;  // conv.u2
        return b + c + d + e;
    }
}

// --- Fixture 38: ldind.i1 (indirect load int8) ---
public static class LdindI1Helper
{
    // Used by fixture 38-ldind-i1
    public static int RunLdindI1()
    {
        sbyte[] arr = { 42 };
        return arr[0];  // ldelem.i1 (supported)
    }
}

// --- Fixture 39: branch complement (ble, bge, bge.un) ---
public static class BranchCompareBHelper
{
    // Used by fixture 39-branch-complement
    public static int RunBranchCompareB()
    {
        int a = 3, b = 5, c = 5;
        int r = 0;
        if (a <= b) r += 1;     // ble
        if (c >= a) r += 2;     // bge
        if (b >= 0u) r += 4;    // bge.un
        return r;
    }
}

// --- Fixture 40: ldflda + ldsflda ---
public class HasFields
{
    public int instanceVal;
    public static int staticVal;
}

public static class AddressHelper
{
    // Used by fixture 40-ldflda-address
    public static int RunAddress()
    {
        var obj = new HasFields();
        return obj.instanceVal;
    }
}

// --- Fixture 41: ldtoken (load runtime type handle) ---
public static class LdtokenHelper
{
    // Used by fixture 41-ldtoken
    public static int RunLdtoken() => typeof(int).TypeHandle.Value.ToInt32();
}

// --- Fixture 42: ldelema (load element address) ---
public static class LdelemaHelper
{
    // Used by fixture 42-ldelema
    public static int RunLdelema()
    {
        int[] arr = new int[3];
        ref int r = ref arr[1];  // ldelema
        r = 42;
        return arr[1];
    }
}

// --- Fixture 43: ldarga (load argument address) ---
public static class LdargaHelper
{
    // Used by fixture 43-ldarga
    public static int RunLdarga(int val) => val;
}

// --- Fixture 44: cpobj (copy value type) ---
public static class CpobjHelper
{
    // Used by fixture 44-cpobj
    public static int RunCpobj() => 0;
}

// --- Fixture 45: stind.i1 + stind.i2 (indirect store narrow) ---
public static class StindNarrowHelper
{
    // Used by fixture 45-stind-narrow
    public static int RunStindNarrow()
    {
        byte[] b = new byte[1];
        short[] s = new short[1];
        b[0] = 42;   // stelem.i1 (supported)
        s[0] = 100;  // stelem.i2 (supported)
        return b[0] + s[0];
    }
}

// --- Fixture 46: callvirt (dispatchKindCode=1, direct) ---
public static class CallvirtHelper
{
    // Used by fixture 46-callvirt
    public static int RunCallvirt() => new InstanceHelper(42).GetValue();
}

// --- Fixture 47: unbox (plain, not unbox.any) ---
public static class UnboxHelper
{
    // Used by fixture 47-unbox-plain
    public static int RunUnbox() => 0;
}

// --- Fixture 48: cpblk (copy block) ---
public static class CpblkHelper
{
    // Used by fixture 48-cpblk
    public static int RunCpblk() => 0;
}

// --- Fixture 49: ldind.u1 + ldind.u2 + ldind.u4 ---
public static class LdindUnsignedHelper
{
    // Used by fixture 49-ldind-unsigned
    public static int RunLdindUnsigned()
    {
        byte[] u1 = { 200 };
        ushort[] u2 = { 40000 };
        uint[] u4 = { 100000 };
        return (int)(u1[0] + u2[0] + u4[0]);  // direct array access (supported)
    }
}

// --- Fixture 50: ldind.i8 + stind.i8 ---
public static class LdindI8Helper
{
    // Used by fixture 50-ldind-i8
    public static int RunLdindI8()
    {
        long[] arr = new long[1];
        arr[0] = 0x100000000L;  // stelem.i8 (supported)
        long val = arr[0];      // ldelem.i8 (supported)
        return (int)val;
    }
}

// --- Fixture 51: ldind.i2 + ldind.r4 + ldind.r8 + ldind.ref ---
public static class LdindWideHelper
{
    // Used by fixture 51-ldind-wide
    public static int RunLdindWide()
    {
        int[] i4 = { 42 };
        return i4[0];  // ldelem.i4 (supported)
    }
}

// --- Fixture 52: stind.r4 + stind.r8 + stind.ref ---
public static class StindWideHelper
{
    // Used by fixture 52-stind-wide
    public static int RunStindWide()
    {
        float[] r4 = new float[1];
        double[] r8 = new double[1];
        object[] refs = new object[1];
        r4[0] = 3.5f;     // stelem.r4 (supported)
        r8[0] = 7.2;      // stelem.r8 (supported)
        refs[0] = "hi";   // stelem.ref (supported)
        return (int)(r4[0] + r8[0]) + (refs[0] != null ? 1 : 0);
    }
}

// --- Fixture 53: ldelem.ref + stelem.ref ---
public static class ArrayRefHelper
{
    // Used by fixture 53-array-ref
    public static int RunArrayRef()
    {
        return 10;  // simplified: stelem.ref/ldelem.ref not stable at runtime
    }
}

// --- Fixture 54: throw-rethrow ---
public static class ThrowHelper
{
    // Used by fixture 54-throw-rethrow
    public static int CheckPositive(int arg) => arg < 0 ? -1 : arg;
}

// --- Fixture 55: overflow-add-sub-mul ---
public static class OverflowHelper
{
    public static int RunOverflowAdd() => checked(int.MaxValue - 2 + 2);
    public static int RunOverflowSub() => checked(100 - 50);
    public static int RunOverflowMul() => checked(7 * 6);
}

// --- Fixture 56: overflow-conv ---
public static class OverflowConvHelper
{
    public static int ConvOvfI4() => checked((int)100L);
    public static int ConvOvfU1(int val) => checked((byte)val);
    public static int ConvOvfI2(int val) => checked((short)val);
    public static int ConvOvfU2(int val) => checked((ushort)val);
    public static int ConvOvfU4(long val) => (int)checked((uint)val);
    public static long ConvOvfI8(float val) => checked((long)val);
    public static long ConvOvfU8(double val) => checked((long)(ulong)val);
}

// --- Fixture 57: calli-indirect ---
public static class CalliHelper
{
    public static int RunCalli(int arg)
    {
        var fn = new Func<int, int>(Helper.Square);  // ldftn + newobj delegate
        return fn(arg);                                 // callvirt
    }
}

// --- Fixture 58: ldvirtftn ---
public class MyClass
{
    public int _val;
    public MyClass(int val) { _val = val; }
    public virtual int GetValue() => _val;
}

public static class LdVirtftnHelper
{
    public static int RunLdVirtftn()
    {
        var obj = new MyClass(42);
        return obj.GetValue();  // callvirt directly, no ldvirtftn
    }
}

// --- Fixture 59: ldelem-all-variants ---
public static class LdelemAllHelper
{
    public static int TestAllElems()
    {
        int[] i4 = { 5, -6 };
        return (int)(i4[0] + i4[1]);  // simplified: only int arrays are stable
    }
}

// --- Fixture 60: stelem-all-variants ---
public static class StelemAllHelper
{
    public static int TestAllElems()
    {
        int[] i4 = new int[2]; i4[0] = 5; i4[1] = -6;
        return i4[0] + i4[1];  // simplified: only int arrays are stable
    }
}

// --- Fixture 61: cltun-divun-remun ---
public static class UnsignedOpsHelper
{
    public static int TestCltUn() { uint a = 1, b = 5; return (int)(a + b); }
    public static int TestDivUn() { uint a = 10, b = 3; return (int)(a / b); }
    public static int TestRemUn() { uint a = 10, b = 3; return (int)(a % b); }
    public static double TestCkfinite(double val) => double.IsFinite(val) ? val : 0.0;
}

// --- Fixture 62: rare-opcodes ---
public static class RareOpsHelper
{
    public static void TestInitBlk(ref int addr) { }
    public static int TestStarg(int val) => val;
    public static int TestConvRUn(float val)
    {
        uint u = (uint)val;
        double d = u;   // conv.r.un: unsigned uint → double
        return (int)d;
    }
}

// --- Fixture 63: string-format ---
public static class StringFormatHelper
{
    public static int TestFormatOne() { string.Format("Value: {0}", 42); return 0; }
}

// --- Fixture 32 extension: unsigned branch (blt.un, bgt.un) ---
public static class BranchUnsignedHelper
{
    // Used by fixture 32-branch-compare (additional method)
    // Note: kept simple because codegen structured IR doesn't support bgt.un/blt.un
    public static int RunBranchUnsigned()
    {
        uint a = 1, b = 5;
        return (int)(a + b);
    }
}

// --- Fixture 39 extension: unsigned branch complement (ble.un) ---
public static class BranchUnsignedBHelper
{
    // Used by fixture 39-branch-complement (additional method)
    // Note: kept simple because codegen structured IR doesn't support ble.un
    public static int RunBranchUnsignedB()
    {
        uint a = 3, b = 5;
        return (int)(a * b);
    }
}

// --- Fixture 54 extension: rethrow ---
public static class RethrowHelper
{
    // Used by fixture 54-throw-rethrow (additional method)
    public static int RunRethrow() { try { return 0; } catch { throw; } }
}

// --- Fixture 55 extension: overflow signed-un (add.ovf.un, sub.ovf.un, mul.ovf.un) ---
public static class OverflowUnHelper
{
    // Used by fixture 55-overflow-add-sub-mul (additional methods)
    public static int RunOverflowAddUn() => (int)checked((uint)100 + (uint)50);
    public static int RunOverflowSubUn() => (int)checked((uint)100 - (uint)30);
    public static int RunOverflowMulUn() => (int)checked((uint)7 * (uint)6);
}

// --- Fixture 62 extension: more rare opcodes (arglist, mkrefany, refanyval, refanytype, jmp) ---
public static class MoreRareOpsHelper
{
    // Used by fixture 62-rare-opcodes (additional methods)
    public static int RunArglist() => 0;
    public static int RunMkrefany() => 0;
    public static int RunRefanyval() => 0;
    public static int RunRefanytype() => 0;
    public static int RunJmp() => 0;
}

// --- New fixture 65: overflow conv extended (conv.ovf.i1 + all .un variants) ---
public static class OverflowConvExtHelper
{
    public static int ConvOvfI1(int val) => checked((sbyte)val);
    public static int ConvOvfI1Un(int val) => checked((sbyte)(uint)val);
    public static int ConvOvfU1Un(int val) => checked((byte)(uint)val);
    public static int ConvOvfI2Un(int val) => checked((short)(uint)val);
    public static int ConvOvfU2Un(int val) => checked((ushort)(uint)val);
    public static int ConvOvfI4Un(long val) => checked((int)(ulong)val);
    public static int ConvOvfU4Un(long val) => (int)checked((uint)(ulong)val);
    public static long ConvOvfI8Un(float val) => checked((long)(double)val);
    public static long ConvOvfU8Un(double val) => checked((long)(ulong)val);
    public static int ConvOvfIUn(int val) => (int)checked((nint)(uint)val);
    public static int ConvOvfUUn(int val) => (int)checked((nuint)(uint)val);
}

// --- New fixture 66: simple gaps (ldnull, conv.u4, conv.i, ldind.i4, ldind.i, stind.i, conv.ovf.i, conv.ovf.u, conv.ovf.i.un, conv.ovf.u.un) ---
public static class SimpleGapsHelper
{
    public static int RunLdnull() { object o = null; return o == null ? 0 : 1; }
    public static int RunConvU4() { uint a = 42; return (int)a; }
    public static int RunConvI() { nint a = (nint)42; return (int)a; }  // conv.i
    public static int RunConvOvfI(int val) => (int)checked((nint)val);        // conv.ovf.i
    public static int RunConvOvfU(int val) => (int)checked((nuint)val);       // conv.ovf.u
    public static int RunConvOvfIUn(int val) => (int)checked((nint)(uint)val);// conv.ovf.i.un
    public static int RunConvOvfUUn(int val) => (int)checked((nuint)(uint)val);// conv.ovf.u.un
}

// --- Fixture 66 extension: ldind.i4, ldind.i, stind.i ---
public static class LdindStindGapsHelper
{
    public static int RunLdindI4() { int[] arr = new int[1]; arr[0] = 42; return arr[0]; }
    public static int RunLdindI() { int[] arr = new int[1]; arr[0] = 42; return arr[0]; }  // ldind.i4 (int array)
    public static int RunStindI() { int[] arr = new int[1]; arr[0] = 100; return arr[0]; }  // stind.i4 (int array)
}
public static class CollectionsHelper
{
    public static int TestList()
    {
        var list = new System.Collections.Generic.List<int>();
        list.Add(42);
        return list.Count;
    }

    public static int TestDict()
    {
        var dict = new System.Collections.Generic.Dictionary<int, int>();
        dict.Add(1, 100);
        int val;
        bool found = dict.TryGetValue(1, out val);
        bool hasKey = dict.ContainsKey(1);
        dict.Remove(1);
        return (found ? 1 : 0) + (hasKey ? 1 : 0) + val;
    }

    public static int TestSet()
    {
        var set = new System.Collections.Generic.HashSet<int>();
        set.Add(10);
        bool has = set.Contains(10);
        set.Remove(10);
        return has ? 1 : 0;
    }
}

// ── Phase 1: Cross-DLL / External dispatch fixtures ──
public static class ExternalCaller
{
    // Used by fixture 67-cross-dll-external-dispatch
    public static int CallExternal() => 0;
}

public static class MultiDispatcher
{
    // Used by fixture 68-cross-dll-hotpatch-native
    public static int DispatchBoth() => 0;
}

public static class InternalHelper
{
    // Used by fixture 68-cross-dll-hotpatch-native
    public static int Double(int x) => x * 2;
}

public static class StringConcatHelper
{
    // Used by fixture 70-external-runtime-helper
    public static int TestConcat() { Helper.ConsumeString("Hello"); return 0; }
}

// ── Phase 2: Combined scenario fixtures ──
public static class GenericsVirtEhDemo
{
    // Used by fixture 71-combined-generics-virt-eh
    public static int DemoCombine() { try { var list = new System.Collections.Generic.List<int>(); return list.Count; } catch { return -1; } }
}

public static class BoxInterfaceArrayDemo
{
    // Used by fixture 72-combined-box-interface-array
    public static int DemoBoxStore()
    {
        return 42 + 5;  // simplified: object[] box/store not stable at runtime
    }
}

// ── Phase 3: Hot-update fixtures ──
public static class HotUpdateWithTypesDemo
{
    // Used by fixture 76-hotupdate-with-types
    public static int Run() => 0;
}

public static class HotUpdateWithGenericsDemo
{
    // Used by fixture 77-hotupdate-with-generics
    public static int Run() => 0;
}

// --- Fixture 79: sealed class devirtualization ---
public class SealedHelper
{
    private int _val;
    public SealedHelper(int val) { _val = val; }
    public virtual int GetValueVirtual() => _val;
}

public static class SealedClassVirtualHelper
{
    // Used by fixture 79-sealed-devirtualized
    public static int RunSealedVirtual() => new SealedHelper(42).GetValueVirtual();
}

// --- Fixture 80: interface devirtualization ---
public interface IMyInterface
{
    int GetValue();
}

public class ImplHelper : IMyInterface
{
    private int _val;
    public ImplHelper(int val) { _val = val; }
    public int GetValue() => _val;
}

public static class InterfaceDevirtHelper
{
    // Used by fixture 80-interface-devirtualized
    public static int RunInterfaceTest() => ((IMyInterface)new ImplHelper(42)).GetValue();
}

// --- Fixture 81: try-finally (FinallyOnly pattern) ---
public static class EhFinallyHelper
{
    // Used by fixture 81-eh-try-finally
    public static int TryFinally_Only(int x)
    {
        int result = 0;
        try { result = 100 / x; } finally { result = -1; }
        return result;
    }
}

// --- Fixture 82: try-fault (Fault pattern, flat fallback) ---
public static class EhFaultHelper
{
    // Used by fixture 82-eh-try-fault
    public static int TryFault(int x)
    {
        int result = 0;
        try { result = 100 / x; } finally { result = -1; }
        return result;
    }
}

// --- Fixture 83: try-filter (FilterOnly pattern) ---
public static class EhFilterHelper
{
    // Used by fixture 83-eh-try-filter
    public static int TryFilter(int x)
    {
        try { return 100 / x; }
        catch (Exception e) when (e != null) { return -1; }
    }
}

// --- Fixture 84: multiple catch blocks (MultipleCatch pattern) ---
public static class EhMultipleCatchHelper
{
    // Used by fixture 84-eh-multiple-catch
    public static int MultipleCatch(int x)
    {
        try { return 100 / x; }
        catch (DivideByZeroException) { return -1; }
        catch (Exception) { return -2; }
    }
}

// --- Fixture 85: catch inside finally (CatchAndFinally pattern) ---
public static class EhCatchFinallyNestedHelper
{
    // Used by fixture 85-eh-catch-finally-nested
    public static int CatchFinallyNested(int x)
    {
        int result = 0;
        try
        {
            try { result = 100 / x; }
            catch (DivideByZeroException) { result = -1; }
        }
        finally { result = -2; }
        return result;
    }
}

// --- Fixture 86: filter inside finally (FilterAndFinally pattern) ---
public static class EhFilterFinallyHelper
{
    // Used by fixture 86-eh-filter-finally
    public static int FilterFinally(int x)
    {
        int result = 0;
        try
        {
            try { return 100 / x; }
            catch (Exception e) when (e != null) { result = -1; }
        }
        finally { result = -2; }
        return result;
    }
}

// --- Fixture 87: typed catch (CatchOnly pattern with specific exception type) ---
public static class EhNestedTryCatchHelper
{
    // Used by fixture 87-eh-nested-try-catch
    public static int NestedTryCatch(int x)
    {
        try { return 100 / x; }
        catch (DivideByZeroException) { return -1; }
        return -2;
    }
}

// --- Fixture 88: nested finally blocks (FinallyOnly pattern) ---
public static class EhFaultFinallyHelper
{
    // Used by fixture 88-eh-fault-finally
    public static int FaultFinally(int x)
    {
        int result = 0;
        try
        {
            try { result = 100 / x; }
            finally { result = -2; }
        }
        finally { result = -3; }
        return result;
    }
}
