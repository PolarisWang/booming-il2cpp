using System;
using System.Reflection;
using Chaos.TestFramework;

public static partial class ReflectionMemberCompleteSubjects
{
    // [0] System.Private.CoreLib/System.Reflection.MemberInfo::get_Name:System.String()
    public static void CustomEntrySubject_0()
    {
        MemberInfo mi = typeof(int).GetMethods()[0];
        Assert.IsNotNull(mi);
        Assert.IsFalse(string.IsNullOrEmpty(mi.Name));
    }

    // [1] System.Private.CoreLib/System.Reflection.MemberInfo::get_MemberType:System.Reflection.MemberTypes()
    public static void CustomEntrySubject_1()
    {
        MemberInfo mi = typeof(int).GetMethods()[0];
        Assert.IsNotNull(mi);
        Assert.AreEqual((int)MemberTypes.Method, (int)mi.MemberType);
    }

    // [2] System.Private.CoreLib/System.Reflection.MemberInfo::get_DeclaringType:System.Type()
    public static void CustomEntrySubject_2()
    {
        MemberInfo mi = typeof(int).GetMethods()[0];
        Assert.IsNotNull(mi);
        Assert.IsNotNull(mi.DeclaringType);
    }

    // [3] System.Private.CoreLib/System.Reflection.MemberInfo::get_ReflectedType:System.Type()
    public static void CustomEntrySubject_3()
    {
        MemberInfo mi = typeof(int).GetMethods()[0];
        Assert.IsNotNull(mi);
        Assert.IsNotNull(mi.ReflectedType);
    }

    // [4] System.Private.CoreLib/System.Reflection.MemberInfo::get_MetadataToken:System.Int32()
    public static void CustomEntrySubject_4()
    {
        MemberInfo mi = typeof(int).GetMethods()[0];
        Assert.IsNotNull(mi);
        Assert.IsTrue(mi.MetadataToken > 0);
    }

    // [5] System.Private.CoreLib/System.Reflection.MethodInfo::get_ReturnType:System.Type()
    public static void CustomEntrySubject_5()
    {
        Type[] paramTypes = new Type[1];
        paramTypes[0] = typeof(int);
        MethodInfo mi = typeof(int).GetMethod("CompareTo", paramTypes);
        Assert.IsNotNull(mi);
        Assert.IsNotNull(mi.ReturnType);
    }

    // [6] System.Private.CoreLib/System.Reflection.MethodInfo::GetBaseDefinition:System.Reflection.MethodInfo()
    public static void CustomEntrySubject_6()
    {
        Type[] paramTypes = new Type[1];
        paramTypes[0] = typeof(int);
        MethodInfo mi = typeof(int).GetMethod("CompareTo", paramTypes);
        Assert.IsNotNull(mi);
        Assert.IsNotNull(mi.GetBaseDefinition());
    }

    // [13] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsStatic:System.Boolean()
    public static void CustomEntrySubject_13()
    {
        FieldInfo fi = typeof(int).GetField("MaxValue");
        Assert.IsNotNull(fi);
        Assert.IsTrue(fi.IsStatic);
    }

    // [14] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsInitOnly:System.Boolean()
    public static void CustomEntrySubject_14()
    {
        FieldInfo fi = typeof(string).GetField("Empty");
        Assert.IsNotNull(fi);
        Assert.IsTrue(fi.IsInitOnly);
    }

    // [15] System.Private.CoreLib/System.Reflection.FieldInfo::get_IsLiteral:System.Boolean()
    public static void CustomEntrySubject_15()
    {
        FieldInfo fi = typeof(int).GetField("MaxValue");
        Assert.IsNotNull(fi);
        Assert.IsTrue(fi.IsLiteral);
    }

    // [17] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanRead:System.Boolean()
    public static void CustomEntrySubject_17()
    {
        PropertyInfo pi = typeof(string).GetProperty("Length");
        Assert.IsNotNull(pi);
        Assert.IsTrue(pi.CanRead);
    }

    // [18] System.Private.CoreLib/System.Reflection.PropertyInfo::get_CanWrite:System.Boolean()
    public static void CustomEntrySubject_18()
    {
        PropertyInfo pi = typeof(string).GetProperty("Length");
        Assert.IsNotNull(pi);
        Assert.IsFalse(pi.CanWrite);
    }

    // [19] System.Private.CoreLib/System.Reflection.PropertyInfo::GetIndexParameters:System.Reflection.ParameterInfo[]()
    public static void CustomEntrySubject_19()
    {
        Type[] indexParamTypes = new Type[1];
        indexParamTypes[0] = typeof(int);
        PropertyInfo pi = typeof(string).GetProperty("Chars", typeof(char), indexParamTypes);
        Assert.IsNotNull(pi);
        ParameterInfo[] parameters = pi.GetIndexParameters();
        Assert.IsNotNull(parameters);
        Assert.IsTrue(parameters.Length >= 1);
    }

    // [20] System.Private.CoreLib/System.Reflection.FieldInfo::get_FieldHandle:System.RuntimeFieldHandle()
    public static void CustomEntrySubject_20()
    {
        FieldInfo fi = typeof(int).GetField("MaxValue");
        Assert.IsNotNull(fi);
        RuntimeFieldHandle h = fi.FieldHandle;
        Assert.IsTrue(h.Value != System.IntPtr.Zero);
    }

    // [21] System.Private.CoreLib/System.Reflection.ConstructorInfo::get_CallingConvention:System.Reflection.CallingConventions()
    public static void CustomEntrySubject_21()
    {
        Type[] ctorParamTypes = new Type[2];
        ctorParamTypes[0] = typeof(char);
        ctorParamTypes[1] = typeof(int);
        ConstructorInfo ci = typeof(string).GetConstructor(ctorParamTypes);
        Assert.IsNotNull(ci);
        Assert.IsTrue((int)ci.CallingConvention > 0);
    }
}
