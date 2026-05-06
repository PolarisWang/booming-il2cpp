// IL code: BufferMemoryNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit BufferMemoryNativeEntry
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
		// Code size: 139 (0x8b)
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
		IL_0006: switch (IL_0035, IL_003d, IL_0045, IL_004d, IL_0055, IL_005d, IL_0065, IL_006d, IL_0075, IL_007d)

		IL_0033: br.s IL_0085

		IL_0035: call int32 BufferMemoryNativeEntry::Method0()
		IL_003a: stloc.2
		IL_003b: br.s IL_0089

		IL_003d: call int32 BufferMemoryNativeEntry::Method1()
		IL_0042: stloc.2
		IL_0043: br.s IL_0089

		IL_0045: call int32 BufferMemoryNativeEntry::Method2()
		IL_004a: stloc.2
		IL_004b: br.s IL_0089

		IL_004d: call int32 BufferMemoryNativeEntry::Method3()
		IL_0052: stloc.2
		IL_0053: br.s IL_0089

		IL_0055: call int32 BufferMemoryNativeEntry::Method4()
		IL_005a: stloc.2
		IL_005b: br.s IL_0089

		IL_005d: call int32 BufferMemoryNativeEntry::Method5()
		IL_0062: stloc.2
		IL_0063: br.s IL_0089

		IL_0065: call int32 BufferMemoryNativeEntry::Method6()
		IL_006a: stloc.2
		IL_006b: br.s IL_0089

		IL_006d: call int32 BufferMemoryNativeEntry::Method7()
		IL_0072: stloc.2
		IL_0073: br.s IL_0089

		IL_0075: call int32 BufferMemoryNativeEntry::Method8()
		IL_007a: stloc.2
		IL_007b: br.s IL_0089

		IL_007d: call int32 BufferMemoryNativeEntry::Method9()
		IL_0082: stloc.2
		IL_0083: br.s IL_0089

		IL_0085: ldc.i4.m1
		IL_0086: stloc.2
		IL_0087: br.s IL_0089

		IL_0089: ldloc.2
		IL_008a: ret
	} // end of method BufferMemoryNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20e8
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
	} // end of method BufferMemoryNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x20fc
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
	} // end of method BufferMemoryNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2110
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call !!0[] [System.Runtime]System.Array::Empty<uint8>()
		IL_0006: call int32 [System.Runtime]System.Buffer::ByteLength(class [System.Runtime]System.Array)
		IL_000b: stloc.0
		IL_000c: ldc.i4.0
		IL_000d: ldloc.0
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.0
		IL_0016: stloc.1
		IL_0017: br.s IL_0019

		IL_0019: ldloc.1
		IL_001a: ret
	} // end of method BufferMemoryNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2138
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
	} // end of method BufferMemoryNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x214c
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
	} // end of method BufferMemoryNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2160
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
	} // end of method BufferMemoryNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2174
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
	} // end of method BufferMemoryNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2188
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
	} // end of method BufferMemoryNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x219c
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
	} // end of method BufferMemoryNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x21b0
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
	} // end of method BufferMemoryNativeEntry::Method9

} // end of class BufferMemoryNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
