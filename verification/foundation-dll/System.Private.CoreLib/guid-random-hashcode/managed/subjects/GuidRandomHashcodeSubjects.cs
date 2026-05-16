// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/guid/random/hashcode
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;

public static partial class GuidRandomHashcodeSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)
    public static void Subject_0()
    {
        new Guid("00000000-0000-0000-0000-000000000000");
    }

    // [1] System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])
    public static void Subject_1()
    {
        new Guid("00000000-0000-0000-0000-000000000000");
    }

    // [2] System.Private.CoreLib/System.Guid::NewGuid:System.Guid()
    public static void Subject_2()
    {
        if (((Guid.NewGuid()).GetHashCode()) != ((Guid.NewGuid()).GetHashCode())) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)
    public static void Subject_3()
    {
        if (((Guid.Parse("00000000-0000-0000-0000-000000000000")).GetHashCode()) != ((Guid.Parse("00000000-0000-0000-0000-000000000000")).GetHashCode())) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)
    public static void Subject_4()
    {
        // TODO: System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&) could not be auto-generated
    }

    // [5] System.Private.CoreLib/System.Guid::ToString:System.String()
    public static void Subject_5()
    {
        if (((Guid.NewGuid().ToString()).Length) != ((Guid.NewGuid().ToString()).Length)) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Random::.ctor:System.Void()
    public static void Subject_6()
    {
        new Random();
    }

    // [7] System.Private.CoreLib/System.Random::Next:System.Int32()
    public static void Subject_7()
    {
        if (new Random().Next() != new Random().Next()) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)
    public static void Subject_8()
    {
        if (new Random().Next(42) != new Random().Next(42)) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Random::NextDouble:System.Double()
    public static void Subject_9()
    {
        if ((int)(new Random().NextDouble()) != (int)(new Random().NextDouble())) _exitCode = 1;
    }

    // [10] System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])
    public static void Subject_10()
    {
        new Random().NextBytes(new byte[] { 1, 2, 3 });
    }

    // [11] System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)
    public static void Subject_11()
    {
        default(HashCode).Add(42);
    }

    // [12] System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()
    public static void Subject_12()
    {
        if (default(HashCode).ToHashCode() != default(HashCode).ToHashCode()) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)
    public static void Subject_13()
    {
        if (HashCode.Combine(42, 42) != HashCode.Combine(42, 42)) _exitCode = 1;
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
            case 11: Subject_11(); break;
            case 12: Subject_12(); break;
            case 13: Subject_13(); break;
        }
    }

}