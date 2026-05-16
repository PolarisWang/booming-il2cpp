// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/boxing/unboxing/casts
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Runtime.CompilerServices;

public static partial class BoxingUnboxingCastsSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
    public static void Subject_0()
    {
        if (((Convert.ChangeType(42, typeof(byte))).GetHashCode()) != ((Convert.ChangeType(42, typeof(byte))).GetHashCode())) _exitCode = 1;
    }

    // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
    public static void Subject_1()
    {
        if (((Convert.ChangeType(42, typeof(byte), null)).GetHashCode()) != ((Convert.ChangeType(42, typeof(byte), null)).GetHashCode())) _exitCode = 1;
    }

    // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
    public static void Subject_2()
    {
        if (((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsAssignableFrom(typeof(byte))) ? 1 : 0)) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
    public static void Subject_3()
    {
        if (((typeof(byte).IsInstanceOfType(42)) ? 1 : 0) != ((typeof(byte).IsInstanceOfType(42)) ? 1 : 0)) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
    public static void Subject_4()
    {
        if (((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsSubclassOf(typeof(byte))) ? 1 : 0)) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
    public static void Subject_5()
    {
        if (((typeof(byte).IsAssignableTo(typeof(byte))) ? 1 : 0) != ((typeof(byte).IsAssignableTo(typeof(byte))) ? 1 : 0)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
    public static void Subject_6()
    {
        // TODO: System.Private.CoreLib/System.Object::MemberwiseClone:System.Object() could not be auto-generated
    }

    // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
    public static void Subject_7()
    {
        if (((RuntimeHelpers.GetObjectValue(42)).GetHashCode()) != ((RuntimeHelpers.GetObjectValue(42)).GetHashCode())) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
    public static void Subject_8()
    {
        if (((Array.CreateInstance(typeof(byte), 42)).GetHashCode()) != ((Array.CreateInstance(typeof(byte), 42)).GetHashCode())) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
    public static void Subject_9()
    {
        if (((Array.CreateInstance(typeof(byte), 42, 42)).GetHashCode()) != ((Array.CreateInstance(typeof(byte), 42, 42)).GetHashCode())) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
    public static void Subject_10()
    {
        if (((((ValueType)42).Equals(42)) ? 1 : 0) != ((((ValueType)42).Equals(42)) ? 1 : 0)) _exitCode = 1;
    }

    public static void Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: Subject_0(); break;
            case 1: Subject_1(); break;
            case 2: Subject_2(); break;
            case 3: Subject_3(); break;
            case 4: Subject_4(); break;
            case 5: Subject_5(); break;
            case 6: Subject_6(); break;
            case 7: Subject_7(); break;
            case 8: Subject_8(); break;
            case 9: Subject_9(); break;
            case 10: Subject_10(); break;
        }
    }

}