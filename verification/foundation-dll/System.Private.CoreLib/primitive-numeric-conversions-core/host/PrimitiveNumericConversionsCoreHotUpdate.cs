using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Hot-update stubs for family/System.Private.CoreLib/primitive/numeric-conversions-core
public static class PrimitiveNumericConversionsCoreHotUpdate
{
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestByteToInt after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestByteToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestByteToInt_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestByteToInt(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestIntToLong after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToLong:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToLong_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToLong(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestIntToShort after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToShort:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToShort_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToShort(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestIntToByte after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToByte:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToByte_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToByte(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestFloatToDouble after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestFloatToDouble:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestFloatToDouble_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestFloatToDouble(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToFloat after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToFloat:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToFloat_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToFloat(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestIntToDouble after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestIntToDouble:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestIntToDouble_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestIntToDouble(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToInt after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestDoubleToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestDoubleToInt_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestDoubleToInt(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestUintToLong after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestUintToLong:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestUintToLong_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestUintToLong(); }
    // Verify PrimitiveNumericConversionsCoreNativeEntry.TestLongToInt after hot-update (patch side)
    [HotUpdateSubjectId("PrimitiveNumericConversionsCoreNativeEntry/PrimitiveNumericConversionsCoreNativeEntry::TestLongToInt:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_PrimitiveNumericConversionsCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_PrimitiveNumericConversionsCoreNativeEntry_PrimitiveNumericConversionsCoreNativeEntry_TestLongToInt_System_Int32() { _ = PrimitiveNumericConversionsCoreNativeEntry.TestLongToInt(); }}
