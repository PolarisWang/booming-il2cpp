// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class CommandTypeMetadataSubjects
{
    // [6] System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Type()
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { _ = default(ICustomTypeProvider)!.GetCustomType(); } catch { }    }

    // [7] System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { default(ICommand)!.add_CanExecuteChanged(default); } catch { }    }

    // [8] System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Boolean(System.Object)
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { _ = default(ICommand)!.CanExecute(null!); } catch { }    }

    // [9] System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(System.Object)
    [Fact]
    public static void CustomEntrySubject_9()
    {        try { default(ICommand)!.Execute(null!); } catch { }    }

    // [10] System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler)
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { default(ICommand)!.remove_CanExecuteChanged(default); } catch { }    }

}
