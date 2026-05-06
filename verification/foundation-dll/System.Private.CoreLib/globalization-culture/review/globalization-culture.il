// IL code: GlobalizationCultureNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit GlobalizationCultureNativeEntry
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

		IL_0041: call int32 GlobalizationCultureNativeEntry::Method0()
		IL_0046: stloc.2
		IL_0047: br.s IL_00ad

		IL_0049: call int32 GlobalizationCultureNativeEntry::Method1()
		IL_004e: stloc.2
		IL_004f: br.s IL_00ad

		IL_0051: call int32 GlobalizationCultureNativeEntry::Method2()
		IL_0056: stloc.2
		IL_0057: br.s IL_00ad

		IL_0059: call int32 GlobalizationCultureNativeEntry::Method3()
		IL_005e: stloc.2
		IL_005f: br.s IL_00ad

		IL_0061: call int32 GlobalizationCultureNativeEntry::Method4()
		IL_0066: stloc.2
		IL_0067: br.s IL_00ad

		IL_0069: call int32 GlobalizationCultureNativeEntry::Method5()
		IL_006e: stloc.2
		IL_006f: br.s IL_00ad

		IL_0071: call int32 GlobalizationCultureNativeEntry::Method6()
		IL_0076: stloc.2
		IL_0077: br.s IL_00ad

		IL_0079: call int32 GlobalizationCultureNativeEntry::Method7()
		IL_007e: stloc.2
		IL_007f: br.s IL_00ad

		IL_0081: call int32 GlobalizationCultureNativeEntry::Method8()
		IL_0086: stloc.2
		IL_0087: br.s IL_00ad

		IL_0089: call int32 GlobalizationCultureNativeEntry::Method9()
		IL_008e: stloc.2
		IL_008f: br.s IL_00ad

		IL_0091: call int32 GlobalizationCultureNativeEntry::Method10()
		IL_0096: stloc.2
		IL_0097: br.s IL_00ad

		IL_0099: call int32 GlobalizationCultureNativeEntry::Method11()
		IL_009e: stloc.2
		IL_009f: br.s IL_00ad

		IL_00a1: call int32 GlobalizationCultureNativeEntry::Method12()
		IL_00a6: stloc.2
		IL_00a7: br.s IL_00ad

		IL_00a9: ldc.i4.m1
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00ad

		IL_00ad: ldloc.2
		IL_00ae: ret
	} // end of method GlobalizationCultureNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x210c
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_CurrentCulture()
		IL_0006: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000b: stloc.0
		IL_000c: ldc.i4 -1144399932
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method GlobalizationCultureNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2138
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000b: stloc.0
		IL_000c: ldc.i4 -469957930
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method GlobalizationCultureNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2164
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
	} // end of method GlobalizationCultureNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2178
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance string [System.Runtime]System.Globalization.CultureInfo::get_Name()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
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
	} // end of method GlobalizationCultureNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21a4
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance string [System.Runtime]System.Globalization.CultureInfo::get_DisplayName()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
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
	} // end of method GlobalizationCultureNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21d0
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.DateTimeFormatInfo [System.Runtime]System.Globalization.CultureInfo::get_DateTimeFormat()
		IL_000b: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 38
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method GlobalizationCultureNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2200
		// Header size: 12
		// Code size: 36 (0x24)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.NumberFormatInfo [System.Runtime]System.Globalization.CultureInfo::get_NumberFormat()
		IL_000b: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0010: stloc.0
		IL_0011: ldc.i4 7880838
		IL_0016: ldloc.0
		IL_0017: ldnull
		IL_0018: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001d: nop
		IL_001e: ldloc.0
		IL_001f: stloc.1
		IL_0020: br.s IL_0022

		IL_0022: ldloc.1
		IL_0023: ret
	} // end of method GlobalizationCultureNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2230
		// Header size: 12
		// Code size: 46 (0x2e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.CompareInfo [System.Runtime]System.Globalization.CultureInfo::get_CompareInfo()
		IL_000b: ldstr "hello"
		IL_0010: ldstr "hello"
		IL_0015: callvirt instance int32 [System.Runtime]System.Globalization.CompareInfo::Compare(string, string)
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
	} // end of method GlobalizationCultureNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x226c
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 4
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.CompareInfo [System.Runtime]System.Globalization.CultureInfo::get_CompareInfo()
		IL_000b: ldstr "hello"
		IL_0010: ldstr "hello"
		IL_0015: ldc.i4.0
		IL_0016: callvirt instance int32 [System.Runtime]System.Globalization.CompareInfo::Compare(string, string, valuetype [System.Runtime]System.Globalization.CompareOptions)
		IL_001b: stloc.0
		IL_001c: ldc.i4.0
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method GlobalizationCultureNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x22a4
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.CompareInfo [System.Runtime]System.Globalization.CultureInfo::get_CompareInfo()
		IL_000b: ldstr "hello"
		IL_0010: ldstr "hello"
		IL_0015: callvirt instance int32 [System.Runtime]System.Globalization.CompareInfo::IndexOf(string, string)
		IL_001a: stloc.0
		IL_001b: ldc.i4.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method GlobalizationCultureNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x22dc
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.TextInfo [System.Runtime]System.Globalization.CultureInfo::get_TextInfo()
		IL_000b: ldstr "hello"
		IL_0010: callvirt instance string [System.Runtime]System.Globalization.TextInfo::ToUpper(string)
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method GlobalizationCultureNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x2314
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.TextInfo [System.Runtime]System.Globalization.CultureInfo::get_TextInfo()
		IL_000b: ldstr "hello"
		IL_0010: callvirt instance string [System.Runtime]System.Globalization.TextInfo::ToLower(string)
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
	} // end of method GlobalizationCultureNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x234c
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Globalization.CultureInfo::get_InvariantCulture()
		IL_0006: callvirt instance class [System.Runtime]System.Globalization.TextInfo [System.Runtime]System.Globalization.CultureInfo::get_TextInfo()
		IL_000b: callvirt instance string [System.Runtime]System.Globalization.TextInfo::get_CultureName()
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
	} // end of method GlobalizationCultureNativeEntry::Method12

} // end of class GlobalizationCultureNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
