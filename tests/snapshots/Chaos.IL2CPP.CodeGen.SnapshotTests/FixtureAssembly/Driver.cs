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
    public static int Classify() => 0;
}

public static class LdftnHelper
{
    // Used by fixture 12-ldftn
    public static int GetFnPtr()
    {
        var fn = Helper.GetValue;
        return 0;
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
    public static int RunCompare() => 0;
}

// --- Fixture 22: branch dup (dup, brtrue) ---
public static class BranchDupHelper
{
    // Used by fixture 22-branch-dup
    public static int RunBranchDup() => 0;
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
        obj.Compute();
        return 0;
    }
}

// --- Fixture 26: bitwise logical (and, or, xor, not) ---
public static class BitwiseHelper
{
    // Used by fixture 26-bitwise-logical
    public static int RunBitwise() => 0;
}

// --- Fixture 27: shift (shl, shr) ---
public static class ShiftHelper
{
    // Used by fixture 27-shift
    public static int RunShift() => 0;
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
public static class ValueTypeHelper
{
    // Used by fixture 29-value-type-initobj
    public static int RunValueType() => 0;
}

// --- Fixture 30: localloc ---
public static class LocalAllocHelper
{
    // Used by fixture 30-localloc
    public static unsafe int RunAlloc() => 0;
}

// --- Fixture 31: arithmetic mul/div ---
public static class MulDivHelper
{
    // Used by fixture 31-arithmetic-mul-div
    public static int RunMulDiv() => 0;
}

// --- Fixture 32: branch compare (beq, bgt, blt, bne.un) ---
public static class BranchCompareHelper
{
    // Used by fixture 32-branch-compare
    public static int RunBranchCompare() => 0;
}

// --- Fixture 33: float ops (ldc.r4, ldc.r8, conv.r4, conv.r8) ---
public static class FloatOpsHelper
{
    // Used by fixture 33-float-ops
    public static int RunFloatOps() => 0;
}

// --- Fixture 34: conv wide (conv.i8, conv.u8, conv.u) ---
public static class ConvWideHelper
{
    // Used by fixture 34-conv-wide
    public static int RunConvWide() => 0;
}

// --- Fixture 35: neg + shr.un ---
public static class NegShiftHelper
{
    // Used by fixture 35-neg-shrun
    public static int RunNegShift() => 0;
}

// --- Fixture 36: ldc.i8 (8-byte integer constant) ---
public static class LdcI8Helper
{
    // Used by fixture 36-ldc-i8
    public static int RunLdcI8() => 0;
}

// --- Fixture 37: conv small int (conv.i1, conv.i2, conv.u1, conv.u2) ---
public static class ConvSmallIntHelper
{
    // Used by fixture 37-conv-small-int
    public static int RunConvSmall() => 0;
}

// --- Fixture 38: ldind.i1 (indirect load int8) ---
public static class LdindI1Helper
{
    // Used by fixture 38-ldind-i1
    public static int RunLdindI1() => 0;
}

// --- Fixture 39: branch complement (ble, bge, bge.un) ---
public static class BranchCompareBHelper
{
    // Used by fixture 39-branch-complement
    public static int RunBranchCompareB() => 0;
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
    public static int RunLdtoken() => 0;
}

// --- Fixture 42: ldelema (load element address) ---
public static class LdelemaHelper
{
    // Used by fixture 42-ldelema
    public static int RunLdelema() => 0;
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
    public static int RunStindNarrow() => 0;
}
