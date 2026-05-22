// Auto-generated native-AOT entry point
// Family: family/System.Runtime.Serialization.Formatters/object/manager/id/generator
// Assembly: System.Runtime.Serialization.Formatters
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Runtime.Serialization;

public static partial class ObjectManagerIdGeneratorSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::GetId:System.Int64(System.Object,System.Boolean&)
    public static void Subject_0()
    {
        try { if ((int)(default(ObjectIDGenerator)!.GetId(42, out _)) != (int)(default(ObjectIDGenerator)!.GetId(42, out _))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::HasId:System.Int64(System.Object,System.Boolean&)
    public static void Subject_1()
    {
        try { if ((int)(default(ObjectIDGenerator)!.HasId(42, out _)) != (int)(default(ObjectIDGenerator)!.HasId(42, out _))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectIDGenerator::.ctor:System.Void()
    public static void Subject_2()
    {
        try { new ObjectIDGenerator(); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::DoFixups:System.Void()
    public static void Subject_3()
    {
        try { default(ObjectManager)!.DoFixups(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::GetObject:System.Object(System.Int64)
    public static void Subject_4()
    {
        try { if (((default(ObjectManager)!.GetObject(42L)).GetHashCode()) != ((default(ObjectManager)!.GetObject(42L)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseDeserializationEvent:System.Void()
    public static void Subject_5()
    {
        try { default(ObjectManager)!.RaiseDeserializationEvent(); }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RaiseOnDeserializingEvent:System.Void(System.Object)
    public static void Subject_6()
    {
        try { default(ObjectManager)!.RaiseOnDeserializingEvent(42); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32,System.Int64)
    public static void Subject_7()
    {
        // TODO: System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32,System.Int64) could not be auto-generated
    }

    // [8] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32{},System.Int64)
    public static void Subject_8()
    {
        // TODO: System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordArrayElementFixup:System.Void(System.Int64,System.Int32{},System.Int64) could not be auto-generated
    }

    // [9] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordDelayedFixup:System.Void(System.Int64,System.String,System.Int64)
    public static void Subject_9()
    {
        try { default(ObjectManager)!.RecordDelayedFixup(42L, "hello", 42L); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RecordFixup:System.Void(System.Int64,System.Reflection.MemberInfo,System.Int64)
    public static void Subject_10()
    {
        try { default(ObjectManager)!.RecordFixup(42L, null!, 42L); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64)
    public static void Subject_11()
    {
        try { default(ObjectManager)!.RegisterObject(42, 42L); }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64,System.Runtime.Serialization.SerializationInfo)
    public static void Subject_12()
    {
        try { default(ObjectManager)!.RegisterObject(42, 42L, null!); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64,System.Runtime.Serialization.SerializationInfo,System.Int64,System.Reflection.MemberInfo)
    public static void Subject_13()
    {
        try { default(ObjectManager)!.RegisterObject(42, 42L, null!, 42L, null!); }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::RegisterObject:System.Void(System.Object,System.Int64,System.Runtime.Serialization.SerializationInfo,System.Int64,System.Reflection.MemberInfo,System.Int32{})
    public static void Subject_14()
    {
        try { default(ObjectManager)!.RegisterObject(42, 42L, null!, 42L, null!, default); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.ObjectManager::.ctor:System.Void(System.Runtime.Serialization.ISurrogateSelector,System.Runtime.Serialization.StreamingContext)
    public static void Subject_15()
    {
        try { new ObjectManager(null!, default); }
        catch { _exitCode = 1; }
    }

}