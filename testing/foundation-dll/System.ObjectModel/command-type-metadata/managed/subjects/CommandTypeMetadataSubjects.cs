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
        try { if (((default(TypeConverterAttribute)!.Equals(42)) ? 1 : 0) != ((default(TypeConverterAttribute)!.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.String()
    public static void Subject_1()
    {
        try { if (((default(TypeConverterAttribute)!.ConverterTypeName).Length) != ((default(TypeConverterAttribute)!.ConverterTypeName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Int32()
    public static void Subject_2()
    {
        try { if (default(TypeConverterAttribute)!.GetHashCode() != default(TypeConverterAttribute)!.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void()
    public static void Subject_3()
    {
        try { new TypeConverterAttribute(); }
        catch { _exitCode = 1; }
    }

    // [4] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.Type)
    public static void Subject_4()
    {
        try { new TypeConverterAttribute(typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [5] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.String)
    public static void Subject_5()
    {
        try { new TypeConverterAttribute("hello"); }
        catch { _exitCode = 1; }
    }

    // [6] System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Type()
    public static void Subject_6()
    {
        try { if (((default(ICustomTypeProvider)!.GetCustomType()).GetHashCode()) != ((default(ICustomTypeProvider)!.GetCustomType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler)
    public static void Subject_7()
    {
        // TODO: System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler) could not be auto-generated
    }

    // [8] System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Boolean(System.Object)
    public static void Subject_8()
    {
        try { if (((default(ICommand)!.CanExecute(42)) ? 1 : 0) != ((default(ICommand)!.CanExecute(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(System.Object)
    public static void Subject_9()
    {
        try { default(ICommand)!.Execute(42); }
        catch { _exitCode = 1; }
    }

    // [10] System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler)
    public static void Subject_10()
    {
        // TODO: System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler) could not be auto-generated
    }

    // [11] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Type()
    public static void Subject_11()
    {
        try { if (((default(ValueSerializerAttribute)!.ValueSerializerType).GetHashCode()) != ((default(ValueSerializerAttribute)!.ValueSerializerType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.String()
    public static void Subject_12()
    {
        try { if (((default(ValueSerializerAttribute)!.ValueSerializerTypeName).Length) != ((default(ValueSerializerAttribute)!.ValueSerializerTypeName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.Type)
    public static void Subject_13()
    {
        try { new ValueSerializerAttribute(typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [14] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.String)
    public static void Subject_14()
    {
        try { new ValueSerializerAttribute("hello"); }
        catch { _exitCode = 1; }
    }

}