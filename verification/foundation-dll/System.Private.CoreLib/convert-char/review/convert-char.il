// IL code: ConvertCharNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ConvertCharNativeEntry
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
		// Code size: 244 (0xf4)
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
		IL_0006: switch (IL_0058, IL_0063, IL_006e, IL_0076, IL_007e, IL_0086, IL_008e, IL_0096, IL_009e, IL_00a6, IL_00ae, IL_00b6, IL_00be, IL_00c6, IL_00ce, IL_00d6, IL_00de, IL_00e6)

		IL_0053: br IL_00ee

		IL_0058: call int32 ConvertCharNativeEntry::Method0()
		IL_005d: stloc.2
		IL_005e: br IL_00f2

		IL_0063: call int32 ConvertCharNativeEntry::Method1()
		IL_0068: stloc.2
		IL_0069: br IL_00f2

		IL_006e: call int32 ConvertCharNativeEntry::Method2()
		IL_0073: stloc.2
		IL_0074: br.s IL_00f2

		IL_0076: call int32 ConvertCharNativeEntry::Method3()
		IL_007b: stloc.2
		IL_007c: br.s IL_00f2

		IL_007e: call int32 ConvertCharNativeEntry::Method4()
		IL_0083: stloc.2
		IL_0084: br.s IL_00f2

		IL_0086: call int32 ConvertCharNativeEntry::Method5()
		IL_008b: stloc.2
		IL_008c: br.s IL_00f2

		IL_008e: call int32 ConvertCharNativeEntry::Method6()
		IL_0093: stloc.2
		IL_0094: br.s IL_00f2

		IL_0096: call int32 ConvertCharNativeEntry::Method7()
		IL_009b: stloc.2
		IL_009c: br.s IL_00f2

		IL_009e: call int32 ConvertCharNativeEntry::Method8()
		IL_00a3: stloc.2
		IL_00a4: br.s IL_00f2

		IL_00a6: call int32 ConvertCharNativeEntry::Method9()
		IL_00ab: stloc.2
		IL_00ac: br.s IL_00f2

		IL_00ae: call int32 ConvertCharNativeEntry::Method10()
		IL_00b3: stloc.2
		IL_00b4: br.s IL_00f2

		IL_00b6: call int32 ConvertCharNativeEntry::Method11()
		IL_00bb: stloc.2
		IL_00bc: br.s IL_00f2

		IL_00be: call int32 ConvertCharNativeEntry::Method12()
		IL_00c3: stloc.2
		IL_00c4: br.s IL_00f2

		IL_00c6: call int32 ConvertCharNativeEntry::Method13()
		IL_00cb: stloc.2
		IL_00cc: br.s IL_00f2

		IL_00ce: call int32 ConvertCharNativeEntry::Method14()
		IL_00d3: stloc.2
		IL_00d4: br.s IL_00f2

		IL_00d6: call int32 ConvertCharNativeEntry::Method15()
		IL_00db: stloc.2
		IL_00dc: br.s IL_00f2

		IL_00de: call int32 ConvertCharNativeEntry::Method16()
		IL_00e3: stloc.2
		IL_00e4: br.s IL_00f2

		IL_00e6: call int32 ConvertCharNativeEntry::Method17()
		IL_00eb: stloc.2
		IL_00ec: br.s IL_00f2

		IL_00ee: ldc.i4.m1
		IL_00ef: stloc.2
		IL_00f0: br.s IL_00f2

		IL_00f2: ldloc.2
		IL_00f3: ret
	} // end of method ConvertCharNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2150
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.1
		IL_0002: call char [System.Runtime]System.Convert::ToChar(bool)
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
	} // end of method ConvertCharNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2174
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(uint8)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x219c
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 65
		IL_0003: call char [System.Runtime]System.Convert::ToChar(char)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 65
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x21c4
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call valuetype [System.Runtime]System.DateTime [System.Runtime]System.DateTime::get_UtcNow()
		IL_0006: call char [System.Runtime]System.Convert::ToChar(valuetype [System.Runtime]System.DateTime)
		IL_000b: stloc.0
		IL_000c: ldloc.0
		IL_000d: ldloc.0
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.0
		IL_0016: stloc.1
		IL_0017: br.s IL_0019

		IL_0019: ldloc.1
		IL_001a: ret
	} // end of method ConvertCharNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21ec
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: newobj instance void [System.Runtime]System.Decimal::.ctor(int32)
		IL_0008: call char [System.Runtime]System.Convert::ToChar(valuetype [System.Runtime]System.Decimal)
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
	} // end of method ConvertCharNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2218
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call char [System.Runtime]System.Convert::ToChar(float64)
		IL_000f: stloc.0
		IL_0010: ldloc.0
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method ConvertCharNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2244
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(int16)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x226c
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(int32)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2294
		// Header size: 12
		// Code size: 26 (0x1a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: conv.i8
		IL_0004: call char [System.Runtime]System.Convert::ToChar(int64)
		IL_0009: stloc.0
		IL_000a: ldc.i4.s 42
		IL_000c: ldloc.0
		IL_000d: ldnull
		IL_000e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0013: nop
		IL_0014: ldloc.0
		IL_0015: stloc.1
		IL_0016: br.s IL_0018

		IL_0018: ldloc.1
		IL_0019: ret
	} // end of method ConvertCharNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x22bc
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(int32)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x22e4
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: ldnull
		IL_0009: call char [System.Runtime]System.Convert::ToChar(object, class [System.Runtime]System.IFormatProvider)
		IL_000e: stloc.0
		IL_000f: ldc.i4.s 42
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method ConvertCharNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x2310
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(int8)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x2338
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r4 42
		IL_0006: call char [System.Runtime]System.Convert::ToChar(float32)
		IL_000b: stloc.0
		IL_000c: ldloc.0
		IL_000d: ldloc.0
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.0
		IL_0016: stloc.1
		IL_0017: br.s IL_0019

		IL_0019: ldloc.1
		IL_001a: ret
	} // end of method ConvertCharNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x2360
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: call char [System.Runtime]System.Convert::ToChar(string)
		IL_000b: stloc.0
		IL_000c: ldloc.0
		IL_000d: ldloc.0
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.0
		IL_0016: stloc.1
		IL_0017: br.s IL_0019

		IL_0019: ldloc.1
		IL_001a: ret
	} // end of method ConvertCharNativeEntry::Method13

	.method private hidebysig static 
		int32 Method14 () cil managed 
	{
		// Method begins at RVA 0x2388
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldnull
		IL_0007: call char [System.Runtime]System.Convert::ToChar(string, class [System.Runtime]System.IFormatProvider)
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
	} // end of method ConvertCharNativeEntry::Method14

	.method private hidebysig static 
		int32 Method15 () cil managed 
	{
		// Method begins at RVA 0x23b0
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(uint16)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method15

	.method private hidebysig static 
		int32 Method16 () cil managed 
	{
		// Method begins at RVA 0x23d8
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call char [System.Runtime]System.Convert::ToChar(uint32)
		IL_0008: stloc.0
		IL_0009: ldc.i4.s 42
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ConvertCharNativeEntry::Method16

	.method private hidebysig static 
		int32 Method17 () cil managed 
	{
		// Method begins at RVA 0x2400
		// Header size: 12
		// Code size: 26 (0x1a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: conv.i8
		IL_0004: call char [System.Runtime]System.Convert::ToChar(uint64)
		IL_0009: stloc.0
		IL_000a: ldc.i4.s 42
		IL_000c: ldloc.0
		IL_000d: ldnull
		IL_000e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0013: nop
		IL_0014: ldloc.0
		IL_0015: stloc.1
		IL_0016: br.s IL_0018

		IL_0018: ldloc.1
		IL_0019: ret
	} // end of method ConvertCharNativeEntry::Method17

} // end of class ConvertCharNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
