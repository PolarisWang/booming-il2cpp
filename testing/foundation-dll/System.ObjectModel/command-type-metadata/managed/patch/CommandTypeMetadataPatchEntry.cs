// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/command/type/metadata
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class CommandTypeMetadataPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::Equals:System.Boolean(System.Object)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::get_ConverterTypeName:System.String()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::GetHashCode:System.Int32()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.Type)
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.ObjectModel/System.ComponentModel.TypeConverterAttribute::.ctor:System.Void(System.String)
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.ObjectModel/System.Reflection.ICustomTypeProvider::GetCustomType:System.Type()
    public static int Subject_6()
    {
        return unchecked((int)(0xB0000000u + 6));
    }

    // [7] System.ObjectModel/System.Windows.Input.ICommand::add_CanExecuteChanged:System.Void(System.EventHandler)
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.ObjectModel/System.Windows.Input.ICommand::CanExecute:System.Boolean(System.Object)
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.ObjectModel/System.Windows.Input.ICommand::Execute:System.Void(System.Object)
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.ObjectModel/System.Windows.Input.ICommand::remove_CanExecuteChanged:System.Void(System.EventHandler)
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerType:System.Type()
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::get_ValueSerializerTypeName:System.String()
    public static int Subject_12()
    {
        return unchecked((int)(0xB0000000u + 12));
    }

    // [13] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.Type)
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.ObjectModel/System.Windows.Markup.ValueSerializerAttribute::.ctor:System.Void(System.String)
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    public static void Run(int entryIndex)
    {
        try
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
                case 14: Subject_14(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}