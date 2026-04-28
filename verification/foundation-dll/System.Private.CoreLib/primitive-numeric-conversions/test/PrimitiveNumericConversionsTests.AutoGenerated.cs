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

// Auto-generated skeleton for Primitive Numeric Conversions (family/System.Private.CoreLib/primitive/numeric-conversions).
public partial class PrimitiveNumericConversionsTests
{
    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToBoolean:System.Boolean(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToBoolean with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToBoolean_System_Boolean_System_String()
    {
    // TODO: needs-manual — ToBoolean with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToByte:System.Byte(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToByte with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToByte_System_Byte_System_String()
    {
    // TODO: needs-manual — ToByte with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToInt16:System.Int16(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToInt16 with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToInt16_System_Int16_System_String()
    {
    // TODO: needs-manual — ToInt16 with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToInt32 with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_String()
    {
    // TODO: needs-manual — ToInt32 with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToInt64:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToInt64 with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToInt64_System_Int64_System_String()
    {
    // TODO: needs-manual — ToInt64 with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToSingle:System.Single(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToSingle with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToSingle_System_Single_System_String()
    {
    // TODO: needs-manual — ToSingle with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToDouble:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToDouble with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToDouble_System_Double_System_String()
    {
    // TODO: needs-manual — ToDouble with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToDecimal with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_String()
    {
    // TODO: needs-manual — ToDecimal with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Convert_ToString_System_String_System_Int32()
    {
    var result = Convert.ToString(42);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToString:System.String(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact]
    public void Method_System_Private_CoreLib_System_Convert_ToString_System_String_System_Double()
    {
    var result = Convert.ToString(42.0);
    Xunit.Assert.NotNull((object)result);
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToDecimal:System.Decimal(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToDecimal with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToDecimal_System_Decimal_System_Double()
    {
    // TODO: needs-manual — ToDecimal with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Convert::ToInt32:System.Int32(System.Double)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — ToInt32 with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Convert_ToInt32_System_Int32_System_Double()
    {
    // TODO: needs-manual — ToInt32 with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Int32::Parse:System.Int32(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Parse with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Int32_Parse_System_Int32_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Int64::Parse:System.Int64(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Parse with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Int64_Parse_System_Int64_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }

    [MethodSubjectId("System.Private.CoreLib/System.Double::Parse:System.Double(System.String)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversions)]
    [VerificationRoute(VerificationRoute.Native)]
    [Fact(Skip = "needs-manual — Parse with 1 params requires manual implementation")]
    public void Method_System_Private_CoreLib_System_Double_Parse_System_Double_System_String()
    {
    // TODO: needs-manual — Parse with 1 params requires manual implementation
    }}
