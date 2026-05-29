// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/reflection/member/complete
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class ReflectionMemberCompletePatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    public static int Subject_1()
    {
        return unchecked((int)(0xB0000000u + 1));
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    public static int Subject_2()
    {
        return unchecked((int)(0xB0000000u + 2));
    }

    // [3] System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()
    public static int Subject_3()
    {
        return unchecked((int)(0xB0000000u + 3));
    }

    // [4] System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()
    public static int Subject_4()
    {
        return unchecked((int)(0xB0000000u + 4));
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    public static int Subject_5()
    {
        return unchecked((int)(0xB0000000u + 5));
    }

    // [6] System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()
    public static void CustomEntryMethod6()
    {
    }

    // [7] System.Private.CoreLib/System.Reflection.MethodBase::get_MethodHandle:System.RuntimeMethodHandle()
    public static int Subject_7()
    {
        return unchecked((int)(0xB0000000u + 7));
    }

    // [8] System.Private.CoreLib/System.Reflection.MethodBase::get_CallingConvention:System.Reflection.CallingConventions()
    public static int Subject_8()
    {
        return unchecked((int)(0xB0000000u + 8));
    }

    // [9] System.Private.CoreLib/System.Reflection.MethodBase::get_IsPublic:System.Boolean()
    public static int Subject_9()
    {
        return unchecked((int)(0xB0000000u + 9));
    }

    // [10] System.Private.CoreLib/System.Reflection.MethodBase::get_IsStatic:System.Boolean()
    public static int Subject_10()
    {
        return unchecked((int)(0xB0000000u + 10));
    }

    // [11] System.Private.CoreLib/System.Reflection.MethodBase::get_IsVirtual:System.Boolean()
    public static int Subject_11()
    {
        return unchecked((int)(0xB0000000u + 11));
    }

    // [12] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldType:System.Type()
    public static void CustomEntryMethod12()
    {
    }

    // [13] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()
    public static int Subject_13()
    {
        return unchecked((int)(0xB0000000u + 13));
    }

    // [14] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()
    public static int Subject_14()
    {
        return unchecked((int)(0xB0000000u + 14));
    }

    // [15] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()
    public static int Subject_15()
    {
        return unchecked((int)(0xB0000000u + 15));
    }

    // [16] System.Private.CoreLib/System.Reflection.PropertyInfo::get_PropertyType:System.Type()
    public static void CustomEntryMethod16()
    {
    }

    // [17] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()
    public static int Subject_17()
    {
        return unchecked((int)(0xB0000000u + 17));
    }

    // [18] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()
    public static int Subject_18()
    {
        return unchecked((int)(0xB0000000u + 18));
    }

    // [19] System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()
    public static void CustomEntryMethod19()
    {
    }

    // [20] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()
    public static int Subject_20()
    {
        return unchecked((int)(0xB0000000u + 20));
    }

    // [21] System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()
    public static int Subject_21()
    {
        return unchecked((int)(0xB0000000u + 21));
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
                case 6: CustomEntryMethod6(); break;
                case 7: Subject_7(); break;
                case 8: Subject_8(); break;
                case 9: Subject_9(); break;
                case 10: Subject_10(); break;
                case 11: Subject_11(); break;
                case 12: CustomEntryMethod12(); break;
                case 13: Subject_13(); break;
                case 14: Subject_14(); break;
                case 15: Subject_15(); break;
                case 16: CustomEntryMethod16(); break;
                case 17: Subject_17(); break;
                case 18: Subject_18(); break;
                case 19: CustomEntryMethod19(); break;
                case 20: Subject_20(); break;
                case 21: Subject_21(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}