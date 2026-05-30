// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class FormatterServicesConverterSubjects
{
    // [0] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Object(System.Object,System.Type)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = new FormatterConverter().Convert(null!, typeof(byte)); } catch { }    }

    // [1] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.FormatterConverter::Convert:System.Object(System.Object,System.TypeCode)
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = new FormatterConverter().Convert(null!, default); } catch { }    }

    // [18] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldNames:System.String{}()
    [Fact]
    public static void CustomEntrySubject_18()
    {        try { _ = default(IFieldInfo)!.FieldNames; } catch { }    }

    // [19] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::get_FieldTypes:System.Type{}()
    [Fact]
    public static void CustomEntrySubject_19()
    {        try { _ = default(IFieldInfo)!.FieldTypes; } catch { }    }

    // [20] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldNames:System.Void(System.String{})
    [Fact]
    public static void CustomEntrySubject_20()
    {        try { default(IFieldInfo)!.FieldNames = default; } catch { }    }

    // [21] System.Runtime.Serialization.Formatters/System.Runtime.Serialization.Formatters.IFieldInfo::set_FieldTypes:System.Void(System.Type{})
    [Fact]
    public static void CustomEntrySubject_21()
    {        try { default(IFieldInfo)!.FieldTypes = default; } catch { }    }

}
