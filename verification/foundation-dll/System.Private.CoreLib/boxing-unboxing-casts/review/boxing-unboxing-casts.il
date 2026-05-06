// IL code: BoxingUnboxingCastsNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit BoxingUnboxingCastsNativeEntry
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
		// Code size: 151 (0x97)
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
		IL_0006: switch (IL_0039, IL_0041, IL_0049, IL_0051, IL_0059, IL_0061, IL_0069, IL_0071, IL_0079, IL_0081, IL_0089)

		IL_0037: br.s IL_0091

		IL_0039: call int32 BoxingUnboxingCastsNativeEntry::Method0()
		IL_003e: stloc.2
		IL_003f: br.s IL_0095

		IL_0041: call int32 BoxingUnboxingCastsNativeEntry::Method1()
		IL_0046: stloc.2
		IL_0047: br.s IL_0095

		IL_0049: call int32 BoxingUnboxingCastsNativeEntry::Method2()
		IL_004e: stloc.2
		IL_004f: br.s IL_0095

		IL_0051: call int32 BoxingUnboxingCastsNativeEntry::Method3()
		IL_0056: stloc.2
		IL_0057: br.s IL_0095

		IL_0059: call int32 BoxingUnboxingCastsNativeEntry::Method4()
		IL_005e: stloc.2
		IL_005f: br.s IL_0095

		IL_0061: call int32 BoxingUnboxingCastsNativeEntry::Method5()
		IL_0066: stloc.2
		IL_0067: br.s IL_0095

		IL_0069: call int32 BoxingUnboxingCastsNativeEntry::Method6()
		IL_006e: stloc.2
		IL_006f: br.s IL_0095

		IL_0071: call int32 BoxingUnboxingCastsNativeEntry::Method7()
		IL_0076: stloc.2
		IL_0077: br.s IL_0095

		IL_0079: call int32 BoxingUnboxingCastsNativeEntry::Method8()
		IL_007e: stloc.2
		IL_007f: br.s IL_0095

		IL_0081: call int32 BoxingUnboxingCastsNativeEntry::Method9()
		IL_0086: stloc.2
		IL_0087: br.s IL_0095

		IL_0089: call int32 BoxingUnboxingCastsNativeEntry::Method10()
		IL_008e: stloc.2
		IL_008f: br.s IL_0095

		IL_0091: ldc.i4.m1
		IL_0092: stloc.2
		IL_0093: br.s IL_0095

		IL_0095: ldloc.2
		IL_0096: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20f4
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: ldtoken [System.Runtime]System.Byte
		IL_000d: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_0012: call object [System.Runtime]System.Convert::ChangeType(object, class [System.Runtime]System.Type)
		IL_0017: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001c: stloc.0
		IL_001d: ldc.i4.s 42
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.1
		IL_0029: br.s IL_002b

		IL_002b: ldloc.1
		IL_002c: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2130
		// Header size: 12
		// Code size: 46 (0x2e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: ldtoken [System.Runtime]System.Byte
		IL_000d: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_0012: ldnull
		IL_0013: call object [System.Runtime]System.Convert::ChangeType(object, class [System.Runtime]System.Type, class [System.Runtime]System.IFormatProvider)
		IL_0018: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001d: stloc.0
		IL_001e: ldc.i4.s 42
		IL_0020: ldloc.0
		IL_0021: ldnull
		IL_0022: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0027: nop
		IL_0028: ldloc.0
		IL_0029: stloc.1
		IL_002a: br.s IL_002c

		IL_002c: ldloc.1
		IL_002d: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x216c
		// Header size: 12
		// Code size: 48 (0x30)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldtoken [System.Runtime]System.Byte
		IL_0010: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_0015: callvirt instance bool [System.Runtime]System.Type::IsAssignableFrom(class [System.Runtime]System.Type)
		IL_001a: brtrue.s IL_001f

		IL_001c: ldc.i4.0
		IL_001d: br.s IL_0020

		IL_001f: ldc.i4.1

		IL_0020: stloc.0
		IL_0021: ldc.i4.1
		IL_0022: ldloc.0
		IL_0023: ldnull
		IL_0024: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0029: nop
		IL_002a: ldloc.0
		IL_002b: stloc.1
		IL_002c: br.s IL_002e

		IL_002e: ldloc.1
		IL_002f: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x21a8
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldc.i4.s 42
		IL_000d: box [System.Runtime]System.Int32
		IL_0012: callvirt instance bool [System.Runtime]System.Type::IsInstanceOfType(object)
		IL_0017: brtrue.s IL_001c

		IL_0019: ldc.i4.0
		IL_001a: br.s IL_001d

		IL_001c: ldc.i4.1

		IL_001d: stloc.0
		IL_001e: ldc.i4.0
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.1
		IL_0029: br.s IL_002b

		IL_002b: ldloc.1
		IL_002c: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21e4
		// Header size: 12
		// Code size: 48 (0x30)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldtoken [System.Runtime]System.Byte
		IL_0010: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_0015: callvirt instance bool [System.Runtime]System.Type::IsSubclassOf(class [System.Runtime]System.Type)
		IL_001a: brtrue.s IL_001f

		IL_001c: ldc.i4.0
		IL_001d: br.s IL_0020

		IL_001f: ldc.i4.1

		IL_0020: stloc.0
		IL_0021: ldc.i4.0
		IL_0022: ldloc.0
		IL_0023: ldnull
		IL_0024: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0029: nop
		IL_002a: ldloc.0
		IL_002b: stloc.1
		IL_002c: br.s IL_002e

		IL_002e: ldloc.1
		IL_002f: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2220
		// Header size: 12
		// Code size: 48 (0x30)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldtoken [System.Runtime]System.Byte
		IL_0010: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_0015: call instance bool [System.Runtime]System.Type::IsAssignableTo(class [System.Runtime]System.Type)
		IL_001a: brtrue.s IL_001f

		IL_001c: ldc.i4.0
		IL_001d: br.s IL_0020

		IL_001f: ldc.i4.1

		IL_0020: stloc.0
		IL_0021: ldc.i4.1
		IL_0022: ldloc.0
		IL_0023: ldnull
		IL_0024: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0029: nop
		IL_002a: ldloc.0
		IL_002b: stloc.1
		IL_002c: br.s IL_002e

		IL_002e: ldloc.1
		IL_002f: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x225c
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
	} // end of method BoxingUnboxingCastsNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2270
		// Header size: 12
		// Code size: 34 (0x22)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: call object [System.Runtime]System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue(object)
		IL_000d: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
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
	} // end of method BoxingUnboxingCastsNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x22a0
		// Header size: 12
		// Code size: 40 (0x28)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldc.i4.s 42
		IL_000d: call class [System.Runtime]System.Array [System.Runtime]System.Array::CreateInstance(class [System.Runtime]System.Type, int32)
		IL_0012: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0017: stloc.0
		IL_0018: ldc.i4.s 42
		IL_001a: ldloc.0
		IL_001b: ldnull
		IL_001c: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0021: nop
		IL_0022: ldloc.0
		IL_0023: stloc.1
		IL_0024: br.s IL_0026

		IL_0026: ldloc.1
		IL_0027: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x22d4
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldc.i4.s 42
		IL_000d: ldc.i4.s 42
		IL_000f: call class [System.Runtime]System.Array [System.Runtime]System.Array::CreateInstance(class [System.Runtime]System.Type, int32, int32)
		IL_0014: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0019: stloc.0
		IL_001a: ldc.i4 7880838
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.1
		IL_0029: br.s IL_002b

		IL_002b: ldloc.1
		IL_002c: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2310
		// Header size: 12
		// Code size: 46 (0x2e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: ldc.i4.s 42
		IL_000a: box [System.Runtime]System.Int32
		IL_000f: callvirt instance bool [System.Runtime]System.Object::Equals(object)
		IL_0014: brtrue.s IL_0019

		IL_0016: ldc.i4.0
		IL_0017: br.s IL_001a

		IL_0019: ldc.i4.1

		IL_001a: stloc.0
		IL_001b: ldc.i4 35342034
		IL_0020: ldloc.0
		IL_0021: ldnull
		IL_0022: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0027: nop
		IL_0028: ldloc.0
		IL_0029: stloc.1
		IL_002a: br.s IL_002c

		IL_002c: ldloc.1
		IL_002d: ret
	} // end of method BoxingUnboxingCastsNativeEntry::Method10

} // end of class BoxingUnboxingCastsNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
