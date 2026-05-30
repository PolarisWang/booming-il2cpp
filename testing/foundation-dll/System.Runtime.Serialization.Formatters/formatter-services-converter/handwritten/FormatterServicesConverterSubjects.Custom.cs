// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class FormatterServicesConverterSubjects
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Object(System.Object,System.Type)
    public static void CustomEntrySubject_0()
    {        try { _ = new FormatterConverter().Convert(null!, typeof(byte)); } catch { _exitCode = 1; }    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Object(System.Object,System.TypeCode)
    public static void CustomEntrySubject_1()
    {        try { _ = new FormatterConverter().Convert(null!, default); } catch { _exitCode = 1; }    }

    // [18] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.String{}()
    public static void CustomEntrySubject_18()
    {        try { _ = default(IFieldInfo)!.FieldNames; } catch { _exitCode = 1; }    }

    // [19] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Type{}()
    public static void CustomEntrySubject_19()
    {        try { _ = default(IFieldInfo)!.FieldTypes; } catch { _exitCode = 1; }    }

    // [20] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(System.String{})
    public static void CustomEntrySubject_20()
    {        try { default(IFieldInfo)!.FieldNames = default; } catch { _exitCode = 1; }    }

    // [21] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(System.Type{})
    public static void CustomEntrySubject_21()
    {        try { default(IFieldInfo)!.FieldTypes = default; } catch { _exitCode = 1; }    }

}
