// IL code: StringCharTextCoreNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit StringCharTextCoreNativeEntry
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
		// Code size: 274 (0x112)
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
		IL_0006: switch (IL_0060, IL_006b, IL_0076, IL_0081, IL_008c, IL_0094, IL_009c, IL_00a4, IL_00ac, IL_00b4, IL_00bc, IL_00c4, IL_00cc, IL_00d4, IL_00dc, IL_00e4, IL_00ec, IL_00f4, IL_00fc, IL_0104)

		IL_005b: br IL_010c

		IL_0060: call int32 StringCharTextCoreNativeEntry::Method0()
		IL_0065: stloc.2
		IL_0066: br IL_0110

		IL_006b: call int32 StringCharTextCoreNativeEntry::Method1()
		IL_0070: stloc.2
		IL_0071: br IL_0110

		IL_0076: call int32 StringCharTextCoreNativeEntry::Method2()
		IL_007b: stloc.2
		IL_007c: br IL_0110

		IL_0081: call int32 StringCharTextCoreNativeEntry::Method3()
		IL_0086: stloc.2
		IL_0087: br IL_0110

		IL_008c: call int32 StringCharTextCoreNativeEntry::Method4()
		IL_0091: stloc.2
		IL_0092: br.s IL_0110

		IL_0094: call int32 StringCharTextCoreNativeEntry::Method5()
		IL_0099: stloc.2
		IL_009a: br.s IL_0110

		IL_009c: call int32 StringCharTextCoreNativeEntry::Method6()
		IL_00a1: stloc.2
		IL_00a2: br.s IL_0110

		IL_00a4: call int32 StringCharTextCoreNativeEntry::Method7()
		IL_00a9: stloc.2
		IL_00aa: br.s IL_0110

		IL_00ac: call int32 StringCharTextCoreNativeEntry::Method8()
		IL_00b1: stloc.2
		IL_00b2: br.s IL_0110

		IL_00b4: call int32 StringCharTextCoreNativeEntry::Method9()
		IL_00b9: stloc.2
		IL_00ba: br.s IL_0110

		IL_00bc: call int32 StringCharTextCoreNativeEntry::Method10()
		IL_00c1: stloc.2
		IL_00c2: br.s IL_0110

		IL_00c4: call int32 StringCharTextCoreNativeEntry::Method11()
		IL_00c9: stloc.2
		IL_00ca: br.s IL_0110

		IL_00cc: call int32 StringCharTextCoreNativeEntry::Method12()
		IL_00d1: stloc.2
		IL_00d2: br.s IL_0110

		IL_00d4: call int32 StringCharTextCoreNativeEntry::Method13()
		IL_00d9: stloc.2
		IL_00da: br.s IL_0110

		IL_00dc: call int32 StringCharTextCoreNativeEntry::Method14()
		IL_00e1: stloc.2
		IL_00e2: br.s IL_0110

		IL_00e4: call int32 StringCharTextCoreNativeEntry::Method15()
		IL_00e9: stloc.2
		IL_00ea: br.s IL_0110

		IL_00ec: call int32 StringCharTextCoreNativeEntry::Method16()
		IL_00f1: stloc.2
		IL_00f2: br.s IL_0110

		IL_00f4: call int32 StringCharTextCoreNativeEntry::Method17()
		IL_00f9: stloc.2
		IL_00fa: br.s IL_0110

		IL_00fc: call int32 StringCharTextCoreNativeEntry::Method18()
		IL_0101: stloc.2
		IL_0102: br.s IL_0110

		IL_0104: call int32 StringCharTextCoreNativeEntry::Method19()
		IL_0109: stloc.2
		IL_010a: br.s IL_0110

		IL_010c: ldc.i4.m1
		IL_010d: stloc.2
		IL_010e: br.s IL_0110

		IL_0110: ldloc.2
		IL_0111: ret
	} // end of method StringCharTextCoreNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2170
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: call instance int32 [System.Runtime]System.String::IndexOf(string)
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
	} // end of method StringCharTextCoreNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x219c
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldc.i4.s 65
		IL_0008: call instance int32 [System.Runtime]System.String::IndexOf(char)
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
	} // end of method StringCharTextCoreNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x21c8
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldc.i4.1
		IL_0007: call instance string [System.Runtime]System.String::Substring(int32)
		IL_000c: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0011: stloc.0
		IL_0012: ldc.i4.4
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method StringCharTextCoreNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x21f8
		// Header size: 12
		// Code size: 34 (0x22)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldc.i4.1
		IL_0007: ldc.i4.2
		IL_0008: call instance string [System.Runtime]System.String::Substring(int32, int32)
		IL_000d: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0012: stloc.0
		IL_0013: ldc.i4.2
		IL_0014: ldloc.0
		IL_0015: ldnull
		IL_0016: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001b: nop
		IL_001c: ldloc.0
		IL_001d: stloc.1
		IL_001e: br.s IL_0020

		IL_0020: ldloc.1
		IL_0021: ret
	} // end of method StringCharTextCoreNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2228
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: call int32 [System.Runtime]System.String::Compare(string, string)
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
	} // end of method StringCharTextCoreNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2254
		// Header size: 12
		// Code size: 35 (0x23)
		.maxstack 5
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldc.i4.0
		IL_0007: ldstr "world"
		IL_000c: ldc.i4.0
		IL_000d: ldc.i4.3
		IL_000e: call int32 [System.Runtime]System.String::Compare(string, int32, string, int32, int32)
		IL_0013: stloc.0
		IL_0014: ldloc.0
		IL_0015: ldloc.0
		IL_0016: ldnull
		IL_0017: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001c: nop
		IL_001d: ldloc.0
		IL_001e: stloc.1
		IL_001f: br.s IL_0021

		IL_0021: ldloc.1
		IL_0022: ret
	} // end of method StringCharTextCoreNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2284
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: call string [System.Runtime]System.String::Concat(string, string)
		IL_0010: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0015: stloc.0
		IL_0016: ldc.i4.s 10
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method StringCharTextCoreNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x22b8
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: ldstr "hello"
		IL_0010: call string [System.Runtime]System.String::Concat(string, string, string)
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.s 15
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method StringCharTextCoreNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x22f0
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: call !!0[] [System.Runtime]System.Array::Empty<object>()
		IL_000b: call string [System.Runtime]System.String::Format(string, object[])
		IL_0010: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0015: stloc.0
		IL_0016: ldc.i4.5
		IL_0017: ldloc.0
		IL_0018: ldnull
		IL_0019: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001e: nop
		IL_001f: ldloc.0
		IL_0020: stloc.1
		IL_0021: br.s IL_0023

		IL_0023: ldloc.1
		IL_0024: ret
	} // end of method StringCharTextCoreNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2324
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: call instance bool [System.Runtime]System.String::StartsWith(string)
		IL_0010: brtrue.s IL_0015

		IL_0012: ldc.i4.0
		IL_0013: br.s IL_0016

		IL_0015: ldc.i4.1

		IL_0016: stloc.0
		IL_0017: ldc.i4.1
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method StringCharTextCoreNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2358
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: call instance bool [System.Runtime]System.String::Contains(string)
		IL_0010: brtrue.s IL_0015

		IL_0012: ldc.i4.0
		IL_0013: br.s IL_0016

		IL_0015: ldc.i4.1

		IL_0016: stloc.0
		IL_0017: ldc.i4.1
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method StringCharTextCoreNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x238c
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: ldstr "hello"
		IL_0010: call instance string [System.Runtime]System.String::Replace(string, string)
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.5
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method StringCharTextCoreNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x23c4
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldc.i4.s 65
		IL_0008: ldc.i4.0
		IL_0009: call instance string[] [System.Runtime]System.String::Split(char, valuetype [System.Runtime]System.StringSplitOptions)
		IL_000e: ldlen
		IL_000f: conv.i4
		IL_0010: stloc.0
		IL_0011: ldc.i4.1
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method StringCharTextCoreNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x23f0
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: call instance string [System.Runtime]System.String::ToUpper()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0010: stloc.0
		IL_0011: ldc.i4.5
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method StringCharTextCoreNativeEntry::Method13

	.method private hidebysig static 
		int32 Method14 () cil managed 
	{
		// Method begins at RVA 0x241c
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: call instance string [System.Runtime]System.String::ToLower()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0010: stloc.0
		IL_0011: ldc.i4.5
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method StringCharTextCoreNativeEntry::Method14

	.method private hidebysig static 
		int32 Method15 () cil managed 
	{
		// Method begins at RVA 0x2448
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: call instance string [System.Runtime]System.String::Trim()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0010: stloc.0
		IL_0011: ldc.i4.5
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method StringCharTextCoreNativeEntry::Method15

	.method private hidebysig static 
		int32 Method16 () cil managed 
	{
		// Method begins at RVA 0x2474
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: call !!0[] [System.Runtime]System.Array::Empty<string>()
		IL_000b: call string [System.Runtime]System.String::Join(string, string[])
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
	} // end of method StringCharTextCoreNativeEntry::Method16

	.method private hidebysig static 
		int32 Method17 () cil managed 
	{
		// Method begins at RVA 0x24a8
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 65
		IL_0003: call bool [System.Runtime]System.Char::IsDigit(char)
		IL_0008: brtrue.s IL_000d

		IL_000a: ldc.i4.0
		IL_000b: br.s IL_000e

		IL_000d: ldc.i4.1

		IL_000e: stloc.0
		IL_000f: ldc.i4.0
		IL_0010: ldloc.0
		IL_0011: ldnull
		IL_0012: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0017: nop
		IL_0018: ldloc.0
		IL_0019: stloc.1
		IL_001a: br.s IL_001c

		IL_001c: ldloc.1
		IL_001d: ret
	} // end of method StringCharTextCoreNativeEntry::Method17

	.method private hidebysig static 
		int32 Method18 () cil managed 
	{
		// Method begins at RVA 0x24d4
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 65
		IL_0003: call bool [System.Runtime]System.Char::IsLetter(char)
		IL_0008: brtrue.s IL_000d

		IL_000a: ldc.i4.0
		IL_000b: br.s IL_000e

		IL_000d: ldc.i4.1

		IL_000e: stloc.0
		IL_000f: ldc.i4.1
		IL_0010: ldloc.0
		IL_0011: ldnull
		IL_0012: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0017: nop
		IL_0018: ldloc.0
		IL_0019: stloc.1
		IL_001a: br.s IL_001c

		IL_001c: ldloc.1
		IL_001d: ret
	} // end of method StringCharTextCoreNativeEntry::Method18

	.method private hidebysig static 
		int32 Method19 () cil managed 
	{
		// Method begins at RVA 0x2500
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 65
		IL_0003: call bool [System.Runtime]System.Char::IsWhiteSpace(char)
		IL_0008: brtrue.s IL_000d

		IL_000a: ldc.i4.0
		IL_000b: br.s IL_000e

		IL_000d: ldc.i4.1

		IL_000e: stloc.0
		IL_000f: ldc.i4.0
		IL_0010: ldloc.0
		IL_0011: ldnull
		IL_0012: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0017: nop
		IL_0018: ldloc.0
		IL_0019: stloc.1
		IL_001a: br.s IL_001c

		IL_001c: ldloc.1
		IL_001d: ret
	} // end of method StringCharTextCoreNativeEntry::Method19

} // end of class StringCharTextCoreNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
