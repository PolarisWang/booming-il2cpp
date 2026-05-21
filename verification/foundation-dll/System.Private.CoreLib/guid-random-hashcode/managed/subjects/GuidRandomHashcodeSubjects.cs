// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/guid/random/hashcode
// Assembly: System.Private.CoreLib
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;

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
        try { new Guid(new byte[] { 1, 2, 3 }); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [2] System.Private.CoreLib/System.Guid::NewGuid:System.Guid()
    public static void Subject_2()
    {
        if (((Guid.NewGuid()).GetHashCode()) != 64886455) _exitCode = 1;
    }

    // [3] System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)
    public static void Subject_3()
    {
        if (((Guid.Parse("00000000-0000-0000-0000-000000000000")).GetHashCode()) != 0) _exitCode = 1;
    }

    // [4] System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)
    public static void Subject_4()
    {
        if (((Guid.TryParse("00000000-0000-0000-0000-000000000000", out _)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [5] System.Private.CoreLib/System.Guid::ToString:System.String()
    public static void Subject_5()
    {
        if (((Guid.NewGuid().ToString()).Length) != 36) _exitCode = 1;
    }

    // [6] System.Private.CoreLib/System.Random::.ctor:System.Void()
    public static void Subject_6()
    {
        new Random();
    }

    // [7] System.Private.CoreLib/System.Random::Next:System.Int32()
    public static void Subject_7()
    {
        if (new Random().Next() != 1619816320) _exitCode = 1;
    }

    // [8] System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)
    public static void Subject_8()
    {
        if (new Random().Next(42) != 40) _exitCode = 1;
    }

    // [9] System.Private.CoreLib/System.Random::NextDouble:System.Double()
    public static void Subject_9()
    {
        if ((int)(new Random().NextDouble()) != 0) _exitCode = 1;
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
        if (default(HashCode).ToHashCode() != 143188953) _exitCode = 1;
    }

    // [13] System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)
    public static void Subject_13()
    {
        if (HashCode.Combine(42, 42) != 420125028) _exitCode = 1;
    }

}