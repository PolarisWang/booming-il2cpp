// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/object-manager-id-generator
// Assembly: System.Runtime.Serialization.Formatters
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;

public static partial class ObjectManagerIdGeneratorNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::GetId:System.Int64(System.Object,System.Boolean&)
    public static void Method0()
    {
    var refLocal_1 = true;
        try { if ((int)(ObjectIDGenerator.GetId(42, ref refLocal_1)) != (int)(ObjectIDGenerator.GetId(42, ref refLocal_1))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::HasId:System.Int64(System.Object,System.Boolean&)
    public static void Method1()
    {
    var refLocal_1 = true;
        try { if ((int)(ObjectIDGenerator.HasId(42, ref refLocal_1)) != (int)(ObjectIDGenerator.HasId(42, ref refLocal_1))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::.ctor:System.Void()
    public static void Method2()
    {
        try { new ObjectIDGenerator(); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::DoFixups:System.Void()
    public static void Method3()
    {
        try { ObjectManager.DoFixups(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::GetObject:System.Object(System.Int64)
    public static void Method4()
    {
        try { if (((ObjectManager.GetObject(42L)).GetHashCode()) != ((ObjectManager.GetObject(42L)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseDeserializationEvent:System.Void()
    public static void Method5()
    {
        try { ObjectManager.RaiseDeserializationEvent(); }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseOnDeserializingEvent:System.Void(System.Object)
    public static void Method6()
    {
        try { ObjectManager.RaiseOnDeserializingEvent(42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32,System.Int64)
    public static void Method7()
    {
        try { ObjectManager.RecordArrayElementFixup(42L, 42, 42L); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32{},System.Int64)
    public static void Method8()
    {
        try { ObjectManager.RecordArrayElementFixup(42L, null!, 42L); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordDelayedFixup:System.Void(System.Int64,System.String,System.Int64)
    public static void Method9()
    {
        try { ObjectManager.RecordDelayedFixup(42L, "hello", 42L); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordFixup:System.Void(System.Int64,System.Reflection.MemberInfo,System.Int64)
    public static void Method10()
    {
        try { ObjectManager.RecordFixup(42L, null!, 42L); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64)
    public static void Method11()
    {
        try { ObjectManager.RegisterObject(42, 42L); }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64,System.Runtime.Serialization.SerializationInfo)
    public static void Method12()
    {
        try { ObjectManager.RegisterObject(42, 42L, null!); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64,System.Runtime.Serialization.SerializationInfo,System.Int64,System.Reflection.MemberInfo)
    public static void Method13()
    {
        try { ObjectManager.RegisterObject(42, 42L, null!, 42L, null!); }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64,System.Runtime.Serialization.SerializationInfo,System.Int64,System.Reflection.MemberInfo,System.Int32{})
    public static void Method14()
    {
        try { ObjectManager.RegisterObject(42, 42L, null!, 42L, null!, null!); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::.ctor:System.Void(System.Runtime.Serialization.ISurrogateSelector,System.Runtime.Serialization.StreamingContext)
    public static void Method15()
    {
        try { new ObjectManager(null!, null!); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}