// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/guid/random/hashcode
// Assembly: System.Private.CoreLib
// Variant: subjects

using Chaos.TestFramework;
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
        try { new Guid("00000000-0000-0000-0000-000000000000"); }
        catch { _exitCode = 1; }
    }

    // [1] System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])
    public static void Subject_1()
    {
        try { new Guid(new byte[] { 1, 2, 3 }); }
        catch { _exitCode = 1; }
    }

    // [2] System.Private.CoreLib/System.Guid::NewGuid:System.Guid()
    public static void Subject_2()
    {
        try { _ = ((Guid.NewGuid()).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [3] System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)
    public static void Subject_3()
    {
        try { _ = ((Guid.Parse("00000000-0000-0000-0000-000000000000")).GetHashCode()); }
        catch { _exitCode = 1; }
    }

    // [4] System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)
    public static void Subject_4()
    {
        try { _ = ((Guid.TryParse("00000000-0000-0000-0000-000000000000", out _)) ? 1 : 0); }
        catch { _exitCode = 1; }
    }

    // [5] System.Private.CoreLib/System.Guid::ToString:System.String()
    public static void Subject_5()
    {
        try { _ = ((Guid.NewGuid().ToString()).Length); }
        catch { _exitCode = 1; }
    }

    // [6] System.Private.CoreLib/System.Random::.ctor:System.Void()
    public static void Subject_6()
    {
        try { new Random(); }
        catch { _exitCode = 1; }
    }

    // [7] System.Private.CoreLib/System.Random::Next:System.Int32()
    public static void Subject_7()
    {
        try { _ = new Random().Next(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)
    public static void Subject_8()
    {
        try { _ = new Random().Next(42); }
        catch { _exitCode = 1; }
    }

    // [9] System.Private.CoreLib/System.Random::NextDouble:System.Double()
    public static void Subject_9()
    {
        try { _ = (int)(new Random().NextDouble()); }
        catch { _exitCode = 1; }
    }

    // [10] System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])
    public static void Subject_10()
    {
        try { new Random().NextBytes(new byte[] { 1, 2, 3 }); }
        catch { _exitCode = 1; }
    }

    // [11] System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)
    public static void Subject_11()
    {
        try { default(HashCode).Add(new object()); }
        catch { _exitCode = 1; }
    }

    // [12] System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()
    public static void Subject_12()
    {
        try { _ = default(HashCode).ToHashCode(); }
        catch { _exitCode = 1; }
    }

    // [13] System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)
    public static void Subject_13()
    {
        try { _ = HashCode.Combine(42, 42); }
        catch { _exitCode = 1; }
    }

}