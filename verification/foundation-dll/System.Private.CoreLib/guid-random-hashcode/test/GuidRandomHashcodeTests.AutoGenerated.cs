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

// Auto-generated skeleton for Guid Random HashCode (family/System.Private.CoreLib/guid/random/hashcode).
public partial class GuidRandomHashcodeTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — .ctor with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Guid_ctor_System_Void_System_String()
    {
    // TODO: needs-manual — .ctor with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Guid::.ctor:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — .ctor with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Guid_ctor_System_Void_System_Byte()
    {
    // TODO: needs-manual — .ctor with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Guid::NewGuid:System.Guid()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Guid_NewGuid_System_Guid()
    {
    var result = Guid.NewGuid();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Guid::Parse:System.Guid(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Parse with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Guid_Parse_System_Guid_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Guid::TryParse:System.Boolean(System.String,System.Guid&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ref/pointer parameter requires unsafe context")]
    public void Method_System_Private_CoreLib_System_Guid_TryParse_System_Boolean_System_String_System_Guid()
    {
    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context
    }

    [MethodSubjectId("System.Private.CoreLib/System.Guid::ToString:System.String()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Guid_ToString_System_String()
    {
    var result = Guid.NewGuid().ToString();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Random::.ctor:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Random_ctor_System_Void()
    {
    new Random();
    }

    [MethodSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Random_Next_System_Int32()
    {
    var result = new Random().Next();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Random::Next:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Random_Next_System_Int32_System_Int32()
    {
    var result = new Random().Next(42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Random::NextDouble:System.Double()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Random_NextDouble_System_Double()
    {
    var result = new Random().NextDouble();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Random::NextBytes:System.Void(System.Byte[])")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Random_NextBytes_System_Void_System_Byte()
    {
    new Random().NextBytes(new byte[] { 1, 2, 3 });
    }

    [MethodSubjectId("System.Private.CoreLib/System.HashCode::Add:System.Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_HashCode_Add_System_Void_System_Object()
    {
    default(HashCode).Add(42);
    }

    [MethodSubjectId("System.Private.CoreLib/System.HashCode::ToHashCode:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_HashCode_ToHashCode_System_Int32()
    {
    var result = default(HashCode).ToHashCode();
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.HashCode::Combine:System.Int32(System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_GuidRandomHashcode)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_HashCode_Combine_System_Int32_System_Int32_System_Int32()
    {
    var result = HashCode.Combine(42, 42);
    Xunit.Assert.NotNull((object)result);
    }}
