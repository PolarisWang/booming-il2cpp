// Auto-generated native-AOT entry point
// Family: family/System.ObjectModel/command-type-metadata
// Assembly: System.ObjectModel
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Linq;
using System.Reflection;
using System.Windows.Input;
using System.Windows.Markup;

public static partial class CommandTypeMetadataNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Boolean(System.Object)
    public static void Method0()
    {
        try { if (((TypeConverterAttribute.Equals(42)) ? 1 : 0) != ((TypeConverterAttribute.Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.String()
    public static void Method1()
    {
        try { if (((TypeConverterAttribute.ConverterTypeName).Length) != ((TypeConverterAttribute.ConverterTypeName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Int32()
    public static void Method2()
    {
        try { if (TypeConverterAttribute.GetHashCode() != TypeConverterAttribute.GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void()
    public static void Method3()
    {
        try { new TypeConverterAttribute(); }
        catch { _exitCode = 1; }
    }

    // [4] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.Type)
    public static void Method4()
    {
        try { new TypeConverterAttribute(typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [5] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.String)
    public static void Method5()
    {
        try { new TypeConverterAttribute("hello"); }
        catch { _exitCode = 1; }
    }

    // [6] System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Type()
    public static void Method6()
    {
        try { if (((ICustomTypeProvider.GetCustomType()).GetHashCode()) != ((ICustomTypeProvider.GetCustomType()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler)
    public static void Method7()
    {
        try { ICommand.add_CanExecuteChanged(null!); }
        catch { _exitCode = 1; }
    }

    // [8] System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Boolean(System.Object)
    public static void Method8()
    {
        try { if (((ICommand.CanExecute(42)) ? 1 : 0) != ((ICommand.CanExecute(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [9] System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(System.Object)
    public static void Method9()
    {
        try { ICommand.Execute(42); }
        catch { _exitCode = 1; }
    }

    // [10] System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler)
    public static void Method10()
    {
        try { ICommand.remove_CanExecuteChanged(null!); }
        catch { _exitCode = 1; }
    }

    // [11] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Type()
    public static void Method11()
    {
        try { if (((ValueSerializerAttribute.ValueSerializerType).GetHashCode()) != ((ValueSerializerAttribute.ValueSerializerType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.String()
    public static void Method12()
    {
        try { if (((ValueSerializerAttribute.ValueSerializerTypeName).Length) != ((ValueSerializerAttribute.ValueSerializerTypeName).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.Type)
    public static void Method13()
    {
        try { new ValueSerializerAttribute(typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [14] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.String)
    public static void Method14()
    {
        try { new ValueSerializerAttribute("hello"); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}