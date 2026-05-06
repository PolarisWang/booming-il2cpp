// IL code: MathNumericsNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit MathNumericsNativeEntry
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

		IL_0058: call int32 MathNumericsNativeEntry::Method0()
		IL_005d: stloc.2
		IL_005e: br IL_00f2

		IL_0063: call int32 MathNumericsNativeEntry::Method1()
		IL_0068: stloc.2
		IL_0069: br IL_00f2

		IL_006e: call int32 MathNumericsNativeEntry::Method2()
		IL_0073: stloc.2
		IL_0074: br.s IL_00f2

		IL_0076: call int32 MathNumericsNativeEntry::Method3()
		IL_007b: stloc.2
		IL_007c: br.s IL_00f2

		IL_007e: call int32 MathNumericsNativeEntry::Method4()
		IL_0083: stloc.2
		IL_0084: br.s IL_00f2

		IL_0086: call int32 MathNumericsNativeEntry::Method5()
		IL_008b: stloc.2
		IL_008c: br.s IL_00f2

		IL_008e: call int32 MathNumericsNativeEntry::Method6()
		IL_0093: stloc.2
		IL_0094: br.s IL_00f2

		IL_0096: call int32 MathNumericsNativeEntry::Method7()
		IL_009b: stloc.2
		IL_009c: br.s IL_00f2

		IL_009e: call int32 MathNumericsNativeEntry::Method8()
		IL_00a3: stloc.2
		IL_00a4: br.s IL_00f2

		IL_00a6: call int32 MathNumericsNativeEntry::Method9()
		IL_00ab: stloc.2
		IL_00ac: br.s IL_00f2

		IL_00ae: call int32 MathNumericsNativeEntry::Method10()
		IL_00b3: stloc.2
		IL_00b4: br.s IL_00f2

		IL_00b6: call int32 MathNumericsNativeEntry::Method11()
		IL_00bb: stloc.2
		IL_00bc: br.s IL_00f2

		IL_00be: call int32 MathNumericsNativeEntry::Method12()
		IL_00c3: stloc.2
		IL_00c4: br.s IL_00f2

		IL_00c6: call int32 MathNumericsNativeEntry::Method13()
		IL_00cb: stloc.2
		IL_00cc: br.s IL_00f2

		IL_00ce: call int32 MathNumericsNativeEntry::Method14()
		IL_00d3: stloc.2
		IL_00d4: br.s IL_00f2

		IL_00d6: call int32 MathNumericsNativeEntry::Method15()
		IL_00db: stloc.2
		IL_00dc: br.s IL_00f2

		IL_00de: call int32 MathNumericsNativeEntry::Method16()
		IL_00e3: stloc.2
		IL_00e4: br.s IL_00f2

		IL_00e6: call int32 MathNumericsNativeEntry::Method17()
		IL_00eb: stloc.2
		IL_00ec: br.s IL_00f2

		IL_00ee: ldc.i4.m1
		IL_00ef: stloc.2
		IL_00f0: br.s IL_00f2

		IL_00f2: ldloc.2
		IL_00f3: ret
	} // end of method MathNumericsNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2150
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call int32 [System.Runtime]System.Math::Abs(int32)
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
	} // end of method MathNumericsNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2178
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Abs(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 42
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method MathNumericsNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x21a8
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Ceiling(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 42
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method MathNumericsNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x21d8
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Floor(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 42
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method MathNumericsNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2208
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Round(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 42
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method MathNumericsNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2238
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Round(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 42
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method MathNumericsNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2268
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: ldc.i4.s 42
		IL_0005: call int32 [System.Runtime]System.Math::Max(int32, int32)
		IL_000a: stloc.0
		IL_000b: ldc.i4.s 42
		IL_000d: ldloc.0
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.0
		IL_0016: stloc.1
		IL_0017: br.s IL_0019

		IL_0019: ldloc.1
		IL_001a: ret
	} // end of method MathNumericsNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2290
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: ldc.r8 42
		IL_0013: call float64 [System.Runtime]System.Math::Max(float64, float64)
		IL_0018: conv.i4
		IL_0019: stloc.0
		IL_001a: ldc.i4.s 42
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method MathNumericsNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x22c8
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: ldc.i4.s 42
		IL_0005: call int32 [System.Runtime]System.Math::Min(int32, int32)
		IL_000a: stloc.0
		IL_000b: ldc.i4.s 42
		IL_000d: ldloc.0
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.0
		IL_0016: stloc.1
		IL_0017: br.s IL_0019

		IL_0019: ldloc.1
		IL_001a: ret
	} // end of method MathNumericsNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x22f0
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Sqrt(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.6
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method MathNumericsNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x231c
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: ldc.r8 42
		IL_0013: call float64 [System.Runtime]System.Math::Pow(float64, float64)
		IL_0018: conv.i4
		IL_0019: stloc.0
		IL_001a: ldc.i4 2147483647
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.1
		IL_0029: br.s IL_002b

		IL_002b: ldloc.1
		IL_002c: ret
	} // end of method MathNumericsNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x2358
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Sin(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method MathNumericsNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x2384
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call float64 [System.Runtime]System.Math::Cos(float64)
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method MathNumericsNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x23b0
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: ldc.i4.s 42
		IL_0005: call int64 [System.Runtime]System.Math::BigMul(int32, int32)
		IL_000a: conv.i4
		IL_000b: stloc.0
		IL_000c: ldc.i4 1764
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method MathNumericsNativeEntry::Method13

	.method private hidebysig static 
		int32 Method14 () cil managed 
	{
		// Method begins at RVA 0x23dc
		// Header size: 12
		// Code size: 26 (0x1a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: call uint8[] [System.Runtime]System.BitConverter::GetBytes(int32)
		IL_0008: ldlen
		IL_0009: conv.i4
		IL_000a: stloc.0
		IL_000b: ldc.i4.4
		IL_000c: ldloc.0
		IL_000d: ldnull
		IL_000e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0013: nop
		IL_0014: ldloc.0
		IL_0015: stloc.1
		IL_0016: br.s IL_0018

		IL_0018: ldloc.1
		IL_0019: ret
	} // end of method MathNumericsNativeEntry::Method14

	.method private hidebysig static 
		int32 Method15 () cil managed 
	{
		// Method begins at RVA 0x2404
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.r8 42
		IL_000a: call uint8[] [System.Runtime]System.BitConverter::GetBytes(float64)
		IL_000f: ldlen
		IL_0010: conv.i4
		IL_0011: stloc.0
		IL_0012: ldc.i4.8
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method MathNumericsNativeEntry::Method15

	.method private hidebysig static 
		int32 Method16 () cil managed 
	{
		// Method begins at RVA 0x2434
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
	} // end of method MathNumericsNativeEntry::Method16

	.method private hidebysig static 
		int32 Method17 () cil managed 
	{
		// Method begins at RVA 0x2448
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
	} // end of method MathNumericsNativeEntry::Method17

} // end of class MathNumericsNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
