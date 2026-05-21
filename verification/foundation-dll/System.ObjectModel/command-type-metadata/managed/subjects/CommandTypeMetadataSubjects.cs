// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/command/type/metadata
// Assembly: System.ObjectModel
// Variant: subjects

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Windows.Input;
using System.Windows.Markup;

public static partial class CommandTypeMetadataSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Boolean(System.Object)
    public static void Subject_0()
    {
        try { default(TypeConverterAttribute)!.Equals(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.String()
    public static void Subject_1()
    {
        try { _ = default(TypeConverterAttribute)!.ConverterTypeName; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [2] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Int32()
    public static void Subject_2()
    {
        try { default(TypeConverterAttribute)!.GetHashCode(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void()
    public static void Subject_3()
    {
        new TypeConverterAttribute();
    }

    // [4] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.Type)
    public static void Subject_4()
    {
        new TypeConverterAttribute(typeof(byte));
    }

    // [5] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.String)
    public static void Subject_5()
    {
        new TypeConverterAttribute("hello");
    }

    // [6] System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Type()
    public static void Subject_6()
    {
        try { default(ICustomTypeProvider)!.GetCustomType(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler)
    public static void Subject_7()
    {
        // TODO: System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler) could not be auto-generated
    }

    // [8] System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Boolean(System.Object)
    public static void Subject_8()
    {
        try { default(ICommand)!.CanExecute(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(System.Object)
    public static void Subject_9()
    {
        try { default(ICommand)!.Execute(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler)
    public static void Subject_10()
    {
        // TODO: System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler) could not be auto-generated
    }

    // [11] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Type()
    public static void Subject_11()
    {
        try { _ = default(ValueSerializerAttribute)!.ValueSerializerType; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.String()
    public static void Subject_12()
    {
        try { _ = default(ValueSerializerAttribute)!.ValueSerializerTypeName; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.Type)
    public static void Subject_13()
    {
        new ValueSerializerAttribute(typeof(byte));
    }

    // [14] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.String)
    public static void Subject_14()
    {
        new ValueSerializerAttribute("hello");
    }

}