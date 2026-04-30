// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/boxing/unboxing/casts
// Assembly: System.Private.CoreLib
// Variant: semantic-patch

using System;
using System.Runtime.CompilerServices;

public static class BoxingUnboxingCastsSemanticPatchEntry
{
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return Method0();
            case 1: return Method1();
            case 2: return Method2();
            case 3: return Method3();
            case 4: return Method4();
            case 5: return Method5();
            case 6: return Method6();
            case 7: return Method7();
            case 8: return Method8();
            case 9: return Method9();
            case 10: return Method10();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
    static int Method0()
    {
        return (Convert.ChangeType(99, typeof(int))?.GetHashCode() ?? 0);
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    static int Method1()
    {
        return (Convert.ChangeType(99, typeof(int), null!)?.GetHashCode() ?? 0);
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    static int Method2()
    {
        return (typeof(int).IsAssignableFrom(typeof(int)) ? 1 : 0);
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    static int Method3()
    {
        return (typeof(int).IsInstanceOfType(99) ? 1 : 0);
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    static int Method4()
    {
        return (typeof(int).IsSubclassOf(typeof(int)) ? 1 : 0);
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    static int Method5()
    {
        return (typeof(int).IsAssignableTo(typeof(int)) ? 1 : 0);
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    static int Method6()
    {
        // TODO: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object() could not be auto-generated for semantic-patch
        return 0;
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    static int Method7()
    {
        return (RuntimeHelpers.GetObjectValue(99)?.GetHashCode() ?? 0);
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    static int Method8()
    {
        return Array.CreateInstance(typeof(int), 99).GetHashCode();
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    static int Method9()
    {
        return Array.CreateInstance(typeof(int), 99, 99).GetHashCode();
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    static int Method10()
    {
        return (((ValueType)99).Equals(99) ? 1 : 0);
    }

}