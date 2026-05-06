// IL code: ReflectionGenericsNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionGenericsNativeEntry
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
		// Code size: 163 (0xa3)
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
		IL_0006: switch (IL_003d, IL_0045, IL_004d, IL_0055, IL_005d, IL_0065, IL_006d, IL_0075, IL_007d, IL_0085, IL_008d, IL_0095)

		IL_003b: br.s IL_009d

		IL_003d: call int32 ReflectionGenericsNativeEntry::Method0()
		IL_0042: stloc.2
		IL_0043: br.s IL_00a1

		IL_0045: call int32 ReflectionGenericsNativeEntry::Method1()
		IL_004a: stloc.2
		IL_004b: br.s IL_00a1

		IL_004d: call int32 ReflectionGenericsNativeEntry::Method2()
		IL_0052: stloc.2
		IL_0053: br.s IL_00a1

		IL_0055: call int32 ReflectionGenericsNativeEntry::Method3()
		IL_005a: stloc.2
		IL_005b: br.s IL_00a1

		IL_005d: call int32 ReflectionGenericsNativeEntry::Method4()
		IL_0062: stloc.2
		IL_0063: br.s IL_00a1

		IL_0065: call int32 ReflectionGenericsNativeEntry::Method5()
		IL_006a: stloc.2
		IL_006b: br.s IL_00a1

		IL_006d: call int32 ReflectionGenericsNativeEntry::Method6()
		IL_0072: stloc.2
		IL_0073: br.s IL_00a1

		IL_0075: call int32 ReflectionGenericsNativeEntry::Method7()
		IL_007a: stloc.2
		IL_007b: br.s IL_00a1

		IL_007d: call int32 ReflectionGenericsNativeEntry::Method8()
		IL_0082: stloc.2
		IL_0083: br.s IL_00a1

		IL_0085: call int32 ReflectionGenericsNativeEntry::Method9()
		IL_008a: stloc.2
		IL_008b: br.s IL_00a1

		IL_008d: call int32 ReflectionGenericsNativeEntry::Method10()
		IL_0092: stloc.2
		IL_0093: br.s IL_00a1

		IL_0095: call int32 ReflectionGenericsNativeEntry::Method11()
		IL_009a: stloc.2
		IL_009b: br.s IL_00a1

		IL_009d: ldc.i4.m1
		IL_009e: stloc.2
		IL_009f: br.s IL_00a1

		IL_00a1: ldloc.2
		IL_00a2: ret
	} // end of method ReflectionGenericsNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2100
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
		IL_000b: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Type::GetGenericTypeDefinition()
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0015: stloc.0
		IL_0016: ldloc.0
		IL_0017: ldloc.0
		IL_0018: ldnull
		IL_0019: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001e: nop
		IL_001f: ldloc.0
		IL_0020: stloc.1
		IL_0021: br.s IL_0023

		IL_0023: ldloc.1
		IL_0024: ret
	} // end of method ReflectionGenericsNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2134
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call !!0[] [System.Runtime]System.Array::Empty<class [System.Runtime]System.Type>()
		IL_0010: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Type::MakeGenericType(class [System.Runtime]System.Type[])
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldloc.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ReflectionGenericsNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x216c
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
		IL_000b: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Type::GetGenericArguments()
		IL_0010: ldlen
		IL_0011: conv.i4
		IL_0012: stloc.0
		IL_0013: ldc.i4.0
		IL_0014: ldloc.0
		IL_0015: ldnull
		IL_0016: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001b: nop
		IL_001c: ldloc.0
		IL_001d: stloc.1
		IL_001e: br.s IL_0020

		IL_0020: ldloc.1
		IL_0021: ret
	} // end of method ReflectionGenericsNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x219c
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
		IL_000b: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Type::GetGenericParameterConstraints()
		IL_0010: ldlen
		IL_0011: conv.i4
		IL_0012: stloc.0
		IL_0013: ldloc.0
		IL_0014: ldloc.0
		IL_0015: ldnull
		IL_0016: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001b: nop
		IL_001c: ldloc.0
		IL_001d: stloc.1
		IL_001e: br.s IL_0020

		IL_0020: ldloc.1
		IL_0021: ret
	} // end of method ReflectionGenericsNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21cc
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance int32 [System.Runtime]System.Type::get_GenericParameterPosition()
		IL_0010: stloc.0
		IL_0011: ldloc.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method ReflectionGenericsNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21f8
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
		IL_000b: callvirt instance bool [System.Runtime]System.Type::get_ContainsGenericParameters()
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
	} // end of method ReflectionGenericsNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x222c
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
		IL_000b: callvirt instance bool [System.Runtime]System.Type::get_IsGenericType()
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
	} // end of method ReflectionGenericsNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2260
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
		IL_000b: callvirt instance bool [System.Runtime]System.Type::get_IsGenericTypeDefinition()
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
	} // end of method ReflectionGenericsNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2294
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
		IL_000b: callvirt instance bool [System.Runtime]System.Type::get_IsConstructedGenericType()
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
	} // end of method ReflectionGenericsNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x22c8
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
		IL_000b: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Type::MakeArrayType()
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
	} // end of method ReflectionGenericsNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2300
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
		IL_000b: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Type::MakeByRefType()
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0015: stloc.0
		IL_0016: ldc.i4 56793269
		IL_001b: ldloc.0
		IL_001c: ldnull
		IL_001d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0022: nop
		IL_0023: ldloc.0
		IL_0024: stloc.1
		IL_0025: br.s IL_0027

		IL_0027: ldloc.1
		IL_0028: ret
	} // end of method ReflectionGenericsNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x2338
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
		IL_000b: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Type::MakePointerType()
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0015: stloc.0
		IL_0016: ldc.i4 115000
		IL_001b: ldloc.0
		IL_001c: ldnull
		IL_001d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0022: nop
		IL_0023: ldloc.0
		IL_0024: stloc.1
		IL_0025: br.s IL_0027

		IL_0027: ldloc.1
		IL_0028: ret
	} // end of method ReflectionGenericsNativeEntry::Method11

} // end of class ReflectionGenericsNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
