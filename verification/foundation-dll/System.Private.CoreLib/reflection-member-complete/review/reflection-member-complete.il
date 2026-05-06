// IL code: ReflectionMemberCompleteNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionMemberCompleteNativeEntry
	extends [System.Runtime]System.Object
{
	// Methods
	.method public hidebysig static 
		int32 Run (
			int32 entryIndex
		) cil managed 
	{
		// Method begins at RVA 0x2050
		// Header size: 12
		// Code size: 304 (0x130)
		.maxstack 1
		.locals init (
			[0] int32,
			[1] int32,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldarg.0
		IL_0002: stloc.1
		IL_0003: ldloc.1
		IL_0004: stloc.0
		IL_0005: ldloc.0
		IL_0006: switch (IL_0068, IL_0073, IL_007e, IL_0089, IL_0094, IL_009f, IL_00aa, IL_00b2, IL_00ba, IL_00c2, IL_00ca, IL_00d2, IL_00da, IL_00e2, IL_00ea, IL_00f2, IL_00fa, IL_0102, IL_010a, IL_0112, IL_011a, IL_0122)

		IL_0063: br IL_012a

		IL_0068: call int32 ReflectionMemberCompleteNativeEntry::Method0()
		IL_006d: stloc.2
		IL_006e: br IL_012e

		IL_0073: call int32 ReflectionMemberCompleteNativeEntry::Method1()
		IL_0078: stloc.2
		IL_0079: br IL_012e

		IL_007e: call int32 ReflectionMemberCompleteNativeEntry::Method2()
		IL_0083: stloc.2
		IL_0084: br IL_012e

		IL_0089: call int32 ReflectionMemberCompleteNativeEntry::Method3()
		IL_008e: stloc.2
		IL_008f: br IL_012e

		IL_0094: call int32 ReflectionMemberCompleteNativeEntry::Method4()
		IL_0099: stloc.2
		IL_009a: br IL_012e

		IL_009f: call int32 ReflectionMemberCompleteNativeEntry::Method5()
		IL_00a4: stloc.2
		IL_00a5: br IL_012e

		IL_00aa: call int32 ReflectionMemberCompleteNativeEntry::Method6()
		IL_00af: stloc.2
		IL_00b0: br.s IL_012e

		IL_00b2: call int32 ReflectionMemberCompleteNativeEntry::Method7()
		IL_00b7: stloc.2
		IL_00b8: br.s IL_012e

		IL_00ba: call int32 ReflectionMemberCompleteNativeEntry::Method8()
		IL_00bf: stloc.2
		IL_00c0: br.s IL_012e

		IL_00c2: call int32 ReflectionMemberCompleteNativeEntry::Method9()
		IL_00c7: stloc.2
		IL_00c8: br.s IL_012e

		IL_00ca: call int32 ReflectionMemberCompleteNativeEntry::Method10()
		IL_00cf: stloc.2
		IL_00d0: br.s IL_012e

		IL_00d2: call int32 ReflectionMemberCompleteNativeEntry::Method11()
		IL_00d7: stloc.2
		IL_00d8: br.s IL_012e

		IL_00da: call int32 ReflectionMemberCompleteNativeEntry::Method12()
		IL_00df: stloc.2
		IL_00e0: br.s IL_012e

		IL_00e2: call int32 ReflectionMemberCompleteNativeEntry::Method13()
		IL_00e7: stloc.2
		IL_00e8: br.s IL_012e

		IL_00ea: call int32 ReflectionMemberCompleteNativeEntry::Method14()
		IL_00ef: stloc.2
		IL_00f0: br.s IL_012e

		IL_00f2: call int32 ReflectionMemberCompleteNativeEntry::Method15()
		IL_00f7: stloc.2
		IL_00f8: br.s IL_012e

		IL_00fa: call int32 ReflectionMemberCompleteNativeEntry::Method16()
		IL_00ff: stloc.2
		IL_0100: br.s IL_012e

		IL_0102: call int32 ReflectionMemberCompleteNativeEntry::Method17()
		IL_0107: stloc.2
		IL_0108: br.s IL_012e

		IL_010a: call int32 ReflectionMemberCompleteNativeEntry::Method18()
		IL_010f: stloc.2
		IL_0110: br.s IL_012e

		IL_0112: call int32 ReflectionMemberCompleteNativeEntry::Method19()
		IL_0117: stloc.2
		IL_0118: br.s IL_012e

		IL_011a: call int32 ReflectionMemberCompleteNativeEntry::Method20()
		IL_011f: stloc.2
		IL_0120: br.s IL_012e

		IL_0122: call int32 ReflectionMemberCompleteNativeEntry::Method21()
		IL_0127: stloc.2
		IL_0128: br.s IL_012e

		IL_012a: ldc.i4.m1
		IL_012b: stloc.2
		IL_012c: br.s IL_012e

		IL_012e: ldloc.2
		IL_012f: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x218c
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance string [System.Runtime]System.Reflection.MemberInfo::get_Name()
		IL_0007: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_000c: stloc.0
		IL_000d: ldloc.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x21b4
		// Header size: 12
		// Code size: 7 (0x7)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.0
		IL_0002: stloc.0
		IL_0003: br.s IL_0005

		IL_0005: ldloc.0
		IL_0006: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x21c8
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Reflection.MemberInfo::get_DeclaringType()
		IL_0007: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000c: stloc.0
		IL_000d: ldc.i4.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x21f0
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Reflection.MemberInfo::get_ReflectedType()
		IL_0007: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000c: stloc.0
		IL_000d: ldloc.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2218
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance int32 [System.Runtime]System.Reflection.MemberInfo::get_MetadataToken()
		IL_0007: stloc.0
		IL_0008: ldloc.0
		IL_0009: ldloc.0
		IL_000a: ldnull
		IL_000b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0010: nop
		IL_0011: ldloc.0
		IL_0012: stloc.1
		IL_0013: br.s IL_0015

		IL_0015: ldloc.1
		IL_0016: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x223c
		// Header size: 12
		// Code size: 7 (0x7)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.0
		IL_0002: stloc.0
		IL_0003: br.s IL_0005

		IL_0005: ldloc.0
		IL_0006: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2250
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Reflection.MethodInfo [System.Runtime]System.Reflection.MethodInfo::GetBaseDefinition()
		IL_0007: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000c: stloc.0
		IL_000d: ldloc.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2278
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.RuntimeMethodHandle,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance valuetype [System.Runtime]System.RuntimeMethodHandle [System.Runtime]System.Reflection.MethodBase::get_MethodHandle()
		IL_0007: stloc.1
		IL_0008: ldloca.s 1
		IL_000a: call instance int32 [System.Runtime]System.RuntimeMethodHandle::GetHashCode()
		IL_000f: stloc.0
		IL_0010: ldc.i4.0
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.2
		IL_001b: br.s IL_001d

		IL_001d: ldloc.2
		IL_001e: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x22a4
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance valuetype [System.Runtime]System.Reflection.CallingConventions [System.Runtime]System.Reflection.MethodBase::get_CallingConvention()
		IL_0007: stloc.0
		IL_0008: ldloc.0
		IL_0009: ldloc.0
		IL_000a: ldnull
		IL_000b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0010: nop
		IL_0011: ldloc.0
		IL_0012: stloc.1
		IL_0013: br.s IL_0015

		IL_0015: ldloc.1
		IL_0016: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x22c8
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.MethodBase::get_IsPublic()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x22f4
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.MethodBase::get_IsStatic()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x2320
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.MethodBase::get_IsVirtual()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x234c
		// Header size: 12
		// Code size: 7 (0x7)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.0
		IL_0002: stloc.0
		IL_0003: br.s IL_0005

		IL_0005: ldloc.0
		IL_0006: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x2360
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.FieldInfo::get_IsStatic()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method13

	.method private hidebysig static 
		int32 Method14 () cil managed 
	{
		// Method begins at RVA 0x238c
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.FieldInfo::get_IsInitOnly()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method14

	.method private hidebysig static 
		int32 Method15 () cil managed 
	{
		// Method begins at RVA 0x23b8
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.FieldInfo::get_IsLiteral()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldc.i4.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method15

	.method private hidebysig static 
		int32 Method16 () cil managed 
	{
		// Method begins at RVA 0x23e4
		// Header size: 12
		// Code size: 7 (0x7)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.0
		IL_0002: stloc.0
		IL_0003: br.s IL_0005

		IL_0005: ldloc.0
		IL_0006: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method16

	.method private hidebysig static 
		int32 Method17 () cil managed 
	{
		// Method begins at RVA 0x23f8
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.PropertyInfo::get_CanRead()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method17

	.method private hidebysig static 
		int32 Method18 () cil managed 
	{
		// Method begins at RVA 0x2424
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.PropertyInfo::get_CanWrite()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method18

	.method private hidebysig static 
		int32 Method19 () cil managed 
	{
		// Method begins at RVA 0x2450
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Reflection.ParameterInfo[] [System.Runtime]System.Reflection.PropertyInfo::GetIndexParameters()
		IL_0007: ldlen
		IL_0008: conv.i4
		IL_0009: stloc.0
		IL_000a: ldloc.0
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method19

	.method private hidebysig static 
		int32 Method20 () cil managed 
	{
		// Method begins at RVA 0x2478
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.RuntimeFieldHandle,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance valuetype [System.Runtime]System.RuntimeFieldHandle [System.Runtime]System.Reflection.FieldInfo::get_FieldHandle()
		IL_0007: stloc.1
		IL_0008: ldloca.s 1
		IL_000a: call instance int32 [System.Runtime]System.RuntimeFieldHandle::GetHashCode()
		IL_000f: stloc.0
		IL_0010: ldc.i4.0
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.2
		IL_001b: br.s IL_001d

		IL_001d: ldloc.2
		IL_001e: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method20

	.method private hidebysig static 
		int32 Method21 () cil managed 
	{
		// Method begins at RVA 0x24a4
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance valuetype [System.Runtime]System.Reflection.CallingConventions [System.Runtime]System.Reflection.MethodBase::get_CallingConvention()
		IL_0007: stloc.0
		IL_0008: ldloc.0
		IL_0009: ldloc.0
		IL_000a: ldnull
		IL_000b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0010: nop
		IL_0011: ldloc.0
		IL_0012: stloc.1
		IL_0013: br.s IL_0015

		IL_0015: ldloc.1
		IL_0016: ret
	} // end of method ReflectionMemberCompleteNativeEntry::Method21

} // end of class ReflectionMemberCompleteNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
