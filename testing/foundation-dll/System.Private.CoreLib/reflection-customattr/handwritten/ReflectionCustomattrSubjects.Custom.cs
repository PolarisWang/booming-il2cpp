// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class ReflectionCustomattrSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::IsDefined:System.Boolean(System.Type,System.Boolean)
    [Fact]
    public static void CustomEntrySubject_0()
    {
        _ = default(MemberInfo)!.IsDefined(typeof(byte), true);
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::GetCustomAttribute:System.Object(System.Type,System.Boolean)
    [Fact]
    public static void CustomEntrySubject_1()
    {
        _ = default(MemberInfo)!.GetCustomAttribute(typeof(byte), true);
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::GetCustomAttributes:System.Object[](System.Boolean)
    [Fact]
    public static void CustomEntrySubject_2()
    {
        _ = default(MemberInfo)!.GetCustomAttributes(true);
    }

}
