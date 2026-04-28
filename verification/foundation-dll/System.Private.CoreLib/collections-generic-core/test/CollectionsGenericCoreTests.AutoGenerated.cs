using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;
using Xunit;

// Auto-generated skeleton for Generic Collections Core (family/System.Private.CoreLib/collections/generic-core).
public partial class CollectionsGenericCoreTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void()
    {
    new List<int>().Clear();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Remove with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T()
    {
    // TODO: needs-manual — Remove with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — RemoveAt with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32()
    {
    // TODO: needs-manual — RemoveAt with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void()
    {
    new List<int>().Sort();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T()
    {
    _ = new List<int>().ToArray();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32()
    {
    var result = new Dictionary<string, int>().Count;
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ContainsKey with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey()
    {
    // TODO: needs-manual — ContainsKey with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Remove with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey()
    {
    // TODO: needs-manual — Remove with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Add with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T()
    {
    // TODO: needs-manual — Add with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Contains with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T()
    {
    // TODO: needs-manual — Contains with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Remove with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T()
    {
    // TODO: needs-manual — Remove with 1 params requires manual implementation
    }}
