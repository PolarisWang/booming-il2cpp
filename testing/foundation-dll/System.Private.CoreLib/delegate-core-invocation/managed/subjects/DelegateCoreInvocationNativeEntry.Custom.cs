// Handwritten custom entry implementations for Delegate Core Invocation.
//
// Methods 5/6 (CreateDelegate), 8 (op_Equality), 9 (op_Inequality) involve
// delegate creation and comparison operators that the auto-generator cannot emit.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

using System;
using Chaos.TestFramework;

public static partial class DelegateCoreInvocationNativeEntry
{
    // [5] System.Delegate::CreateDelegate(Type, Object, String)
    [Fact]
    public static void CustomEntryMethod5()
    {
        var d = (Action)Delegate.CreateDelegate(typeof(Action), new object(), nameof(object.GetHashCode));
    }

    // [6] System.Delegate::CreateDelegate(Type, MethodInfo)
    [Fact]
    public static void CustomEntryMethod6()
    {
        var mi = typeof(object).GetMethod(nameof(object.GetHashCode));
        var d = (Func<int>)Delegate.CreateDelegate(typeof(Func<int>), mi);
    }

    // [8] System.Delegate::op_Equality
    [Fact]
    public static void CustomEntryMethod8()
    {
        Action a = () => { };
        bool _ = a == null;
    }

    // [9] System.Delegate::op_Inequality
    [Fact]
    public static void CustomEntryMethod9()
    {
        Action a = () => { };
        bool _ = a != null;
    }
}
