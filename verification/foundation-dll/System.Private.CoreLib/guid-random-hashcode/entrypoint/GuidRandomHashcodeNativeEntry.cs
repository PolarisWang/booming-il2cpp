// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/guid/random/hashcode
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

public static class GuidRandomHashcodeNativeEntry
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
            case 11: return Method11();
            case 12: return Method12();
            case 13: return Method13();
            default: return -1;
        }
    }

    // [0] System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)
    static int Method0()
    {
        // TODO: System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String) could not be auto-generated
        return 0;
    }

    // [1] System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])
    static int Method1()
    {
        // TODO: System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[]) could not be auto-generated
        return 0;
    }

    // [2] System.Private.CoreLib/System.Guid::NewGuid:System.Guid()
    static int Method2()
    {
        return (int)(Guid.NewGuid());
    }

    // [3] System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)
    static int Method3()
    {
        // TODO: System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String) could not be auto-generated
        return 0;
    }

    // [4] System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)
    static int Method4()
    {
    var refLocal_1 = Guid.Empty;
        return (int)(Guid.TryParse("hello", ref refLocal_1));
    }

    // [5] System.Private.CoreLib/System.Guid::ToString:System.String()
    static int Method5()
    {
        return (int)(Guid.NewGuid().ToString());
    }

    // [6] System.Private.CoreLib/System.Random::.ctor:System.Void()
    static int Method6()
    {
        new Random();
        return (int)0;
    }

    // [7] System.Private.CoreLib/System.Random::Next:System.Int32()
    static int Method7()
    {
        return new Random().Next();
    }

    // [8] System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)
    static int Method8()
    {
        return new Random().Next(42);
    }

    // [9] System.Private.CoreLib/System.Random::NextDouble:System.Double()
    static int Method9()
    {
        return (int)(new Random().NextDouble());
    }

    // [10] System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])
    static int Method10()
    {
        new Random().NextBytes(new byte[] { 1, 2, 3 });
        return (int)0;
    }

    // [11] System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)
    static int Method11()
    {
        default(HashCode).Add(42);
        return (int)0;
    }

    // [12] System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()
    static int Method12()
    {
        return default(HashCode).ToHashCode();
    }

    // [13] System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)
    static int Method13()
    {
        return HashCode.Combine(42, 42);
    }

}