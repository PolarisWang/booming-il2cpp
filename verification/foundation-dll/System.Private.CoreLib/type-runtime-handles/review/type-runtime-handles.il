// IL code: TypeRuntimeHandlesNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit TypeRuntimeHandlesNativeEntry
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
		// Code size: 175 (0xaf)
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
		IL_0006: switch (IL_0041, IL_0049, IL_0051, IL_0059, IL_0061, IL_0069, IL_0071, IL_0079, IL_0081, IL_0089, IL_0091, IL_0099, IL_00a1)

		IL_003f: br.s IL_00a9

		IL_0041: call int32 TypeRuntimeHandlesNativeEntry::Method0()
		IL_0046: stloc.2
		IL_0047: br.s IL_00ad

		IL_0049: call int32 TypeRuntimeHandlesNativeEntry::Method1()
		IL_004e: stloc.2
		IL_004f: br.s IL_00ad

		IL_0051: call int32 TypeRuntimeHandlesNativeEntry::Method2()
		IL_0056: stloc.2
		IL_0057: br.s IL_00ad

		IL_0059: call int32 TypeRuntimeHandlesNativeEntry::Method3()
		IL_005e: stloc.2
		IL_005f: br.s IL_00ad

		IL_0061: call int32 TypeRuntimeHandlesNativeEntry::Method4()
		IL_0066: stloc.2
		IL_0067: br.s IL_00ad

		IL_0069: call int32 TypeRuntimeHandlesNativeEntry::Method5()
		IL_006e: stloc.2
		IL_006f: br.s IL_00ad

		IL_0071: call int32 TypeRuntimeHandlesNativeEntry::Method6()
		IL_0076: stloc.2
		IL_0077: br.s IL_00ad

		IL_0079: call int32 TypeRuntimeHandlesNativeEntry::Method7()
		IL_007e: stloc.2
		IL_007f: br.s IL_00ad

		IL_0081: call int32 TypeRuntimeHandlesNativeEntry::Method8()
		IL_0086: stloc.2
		IL_0087: br.s IL_00ad

		IL_0089: call int32 TypeRuntimeHandlesNativeEntry::Method9()
		IL_008e: stloc.2
		IL_008f: br.s IL_00ad

		IL_0091: call int32 TypeRuntimeHandlesNativeEntry::Method10()
		IL_0096: stloc.2
		IL_0097: br.s IL_00ad

		IL_0099: call int32 TypeRuntimeHandlesNativeEntry::Method11()
		IL_009e: stloc.2
		IL_009f: br.s IL_00ad

		IL_00a1: call int32 TypeRuntimeHandlesNativeEntry::Method12()
		IL_00a6: stloc.2
		IL_00a7: br.s IL_00ad

		IL_00a9: ldc.i4.m1
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00ad

		IL_00ad: ldloc.2
		IL_00ae: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x210c
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
	} // end of method TypeRuntimeHandlesNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2120
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
	} // end of method TypeRuntimeHandlesNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2134
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
	} // end of method TypeRuntimeHandlesNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2148
		// Header size: 12
		// Code size: 36 (0x24)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.RuntimeTypeHandle,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: initobj [System.Runtime]System.RuntimeTypeHandle
		IL_0009: ldloc.1
		IL_000a: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000f: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0014: stloc.0
		IL_0015: ldc.i4.0
		IL_0016: ldloc.0
		IL_0017: ldnull
		IL_0018: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001d: nop
		IL_001e: ldloc.0
		IL_001f: stloc.2
		IL_0020: br.s IL_0022

		IL_0022: ldloc.2
		IL_0023: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2178
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call instance bool [System.Runtime]System.Type::get_IsValueType()
		IL_0010: brtrue.s IL_0015

		IL_0012: ldc.i4.0
		IL_0013: br.s IL_0016

		IL_0015: ldc.i4.1

		IL_0016: stloc.0
		IL_0017: ldc.i4.0
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21ac
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance bool [System.Runtime]System.Type::get_IsEnum()
		IL_0010: brtrue.s IL_0015

		IL_0012: ldc.i4.0
		IL_0013: br.s IL_0016

		IL_0015: ldc.i4.1

		IL_0016: stloc.0
		IL_0017: ldc.i4.0
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21e0
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call instance bool [System.Runtime]System.Type::get_IsArray()
		IL_0010: brtrue.s IL_0015

		IL_0012: ldc.i4.0
		IL_0013: br.s IL_0016

		IL_0015: ldc.i4.1

		IL_0016: stloc.0
		IL_0017: ldloc.0
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2214
		// Header size: 12
		// Code size: 40 (0x28)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.RuntimeTypeHandle,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance valuetype [System.Runtime]System.RuntimeTypeHandle [System.Runtime]System.Type::get_TypeHandle()
		IL_0010: stloc.1
		IL_0011: ldloca.s 1
		IL_0013: call instance int32 [System.Runtime]System.RuntimeTypeHandle::GetHashCode()
		IL_0018: stloc.0
		IL_0019: ldc.i4.1
		IL_001a: ldloc.0
		IL_001b: ldnull
		IL_001c: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0021: nop
		IL_0022: ldloc.0
		IL_0023: stloc.2
		IL_0024: br.s IL_0026

		IL_0026: ldloc.2
		IL_0027: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2248
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance string [System.Runtime]System.Type::get_FullName()
		IL_0010: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0015: stloc.0
		IL_0016: ldc.i4.0
		IL_0017: ldloc.0
		IL_0018: ldnull
		IL_0019: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001e: nop
		IL_001f: ldloc.0
		IL_0020: stloc.1
		IL_0021: br.s IL_0023

		IL_0023: ldloc.1
		IL_0024: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x227c
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance string [System.Runtime]System.Reflection.MemberInfo::get_Name()
		IL_0010: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0015: stloc.0
		IL_0016: ldc.i4.0
		IL_0017: ldloc.0
		IL_0018: ldnull
		IL_0019: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001e: nop
		IL_001f: ldloc.0
		IL_0020: stloc.1
		IL_0021: br.s IL_0023

		IL_0023: ldloc.1
		IL_0024: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x22b0
		// Header size: 12
		// Code size: 41 (0x29)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Type::get_BaseType()
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0015: stloc.0
		IL_0016: ldc.i4 35342034
		IL_001b: ldloc.0
		IL_001c: ldnull
		IL_001d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0022: nop
		IL_0023: ldloc.0
		IL_0024: stloc.1
		IL_0025: br.s IL_0027

		IL_0027: ldloc.1
		IL_0028: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x22e8
		// Header size: 12
		// Code size: 35 (0x23)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call instance class [System.Runtime]System.Reflection.MethodInfo[] [System.Runtime]System.Type::GetMethods()
		IL_0010: ldlen
		IL_0011: conv.i4
		IL_0012: stloc.0
		IL_0013: ldc.i4.s 11
		IL_0015: ldloc.0
		IL_0016: ldnull
		IL_0017: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001c: nop
		IL_001d: ldloc.0
		IL_001e: stloc.1
		IL_001f: br.s IL_0021

		IL_0021: ldloc.1
		IL_0022: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x2318
		// Header size: 12
		// Code size: 34 (0x22)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call instance class [System.Runtime]System.Reflection.FieldInfo[] [System.Runtime]System.Type::GetFields()
		IL_0010: ldlen
		IL_0011: conv.i4
		IL_0012: stloc.0
		IL_0013: ldc.i4.4
		IL_0014: ldloc.0
		IL_0015: ldnull
		IL_0016: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001b: nop
		IL_001c: ldloc.0
		IL_001d: stloc.1
		IL_001e: br.s IL_0020

		IL_0020: ldloc.1
		IL_0021: ret
	} // end of method TypeRuntimeHandlesNativeEntry::Method12

} // end of class TypeRuntimeHandlesNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
