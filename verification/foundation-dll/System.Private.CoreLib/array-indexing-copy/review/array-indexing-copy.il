// IL code: ArrayIndexingCopyNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ArrayIndexingCopyNativeEntry
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

		IL_0041: call int32 ArrayIndexingCopyNativeEntry::Method0()
		IL_0046: stloc.2
		IL_0047: br.s IL_00ad

		IL_0049: call int32 ArrayIndexingCopyNativeEntry::Method1()
		IL_004e: stloc.2
		IL_004f: br.s IL_00ad

		IL_0051: call int32 ArrayIndexingCopyNativeEntry::Method2()
		IL_0056: stloc.2
		IL_0057: br.s IL_00ad

		IL_0059: call int32 ArrayIndexingCopyNativeEntry::Method3()
		IL_005e: stloc.2
		IL_005f: br.s IL_00ad

		IL_0061: call int32 ArrayIndexingCopyNativeEntry::Method4()
		IL_0066: stloc.2
		IL_0067: br.s IL_00ad

		IL_0069: call int32 ArrayIndexingCopyNativeEntry::Method5()
		IL_006e: stloc.2
		IL_006f: br.s IL_00ad

		IL_0071: call int32 ArrayIndexingCopyNativeEntry::Method6()
		IL_0076: stloc.2
		IL_0077: br.s IL_00ad

		IL_0079: call int32 ArrayIndexingCopyNativeEntry::Method7()
		IL_007e: stloc.2
		IL_007f: br.s IL_00ad

		IL_0081: call int32 ArrayIndexingCopyNativeEntry::Method8()
		IL_0086: stloc.2
		IL_0087: br.s IL_00ad

		IL_0089: call int32 ArrayIndexingCopyNativeEntry::Method9()
		IL_008e: stloc.2
		IL_008f: br.s IL_00ad

		IL_0091: call int32 ArrayIndexingCopyNativeEntry::Method10()
		IL_0096: stloc.2
		IL_0097: br.s IL_00ad

		IL_0099: call int32 ArrayIndexingCopyNativeEntry::Method11()
		IL_009e: stloc.2
		IL_009f: br.s IL_00ad

		IL_00a1: call int32 ArrayIndexingCopyNativeEntry::Method12()
		IL_00a6: stloc.2
		IL_00a7: br.s IL_00ad

		IL_00a9: ldc.i4.m1
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00ad

		IL_00ad: ldloc.2
		IL_00ae: ret
	} // end of method ArrayIndexingCopyNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x210c
		// Header size: 12
		// Code size: 26 (0x1a)
		.maxstack 3
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.4
		IL_0008: newarr [System.Runtime]System.Byte
		IL_000d: ldc.i4.4
		IL_000e: call void [System.Runtime]System.Array::Copy(class [System.Runtime]System.Array, class [System.Runtime]System.Array, int32)
		IL_0013: nop
		IL_0014: ldc.i4.0
		IL_0015: stloc.0
		IL_0016: br.s IL_0018

		IL_0018: ldloc.0
		IL_0019: ret
	} // end of method ArrayIndexingCopyNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2134
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 5
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.0
		IL_0008: ldc.i4.4
		IL_0009: newarr [System.Runtime]System.Byte
		IL_000e: ldc.i4.0
		IL_000f: ldc.i4.4
		IL_0010: call void [System.Runtime]System.Array::Copy(class [System.Runtime]System.Array, int32, class [System.Runtime]System.Array, int32, int32)
		IL_0015: nop
		IL_0016: ldc.i4.0
		IL_0017: stloc.0
		IL_0018: br.s IL_001a

		IL_001a: ldloc.0
		IL_001b: ret
	} // end of method ArrayIndexingCopyNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x215c
		// Header size: 12
		// Code size: 21 (0x15)
		.maxstack 3
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.0
		IL_0008: ldc.i4.4
		IL_0009: call void [System.Runtime]System.Array::Clear(class [System.Runtime]System.Array, int32, int32)
		IL_000e: nop
		IL_000f: ldc.i4.0
		IL_0010: stloc.0
		IL_0011: br.s IL_0013

		IL_0013: ldloc.0
		IL_0014: ret
	} // end of method ArrayIndexingCopyNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2180
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
	} // end of method ArrayIndexingCopyNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2194
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: call !!0[] [System.Runtime]System.Array::Empty<uint8>()
		IL_0006: call void [System.Runtime]System.Array::Sort<uint8>(!!0[])
		IL_000b: nop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ArrayIndexingCopyNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21b4
		// Header size: 12
		// Code size: 20 (0x14)
		.maxstack 2
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.1
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldnull
		IL_0008: call void [System.Runtime]System.Array::Sort<uint8>(!!0[], class [System.Runtime]System.Collections.Generic.IComparer`1<!!0>)
		IL_000d: nop
		IL_000e: ldc.i4.0
		IL_000f: stloc.0
		IL_0010: br.s IL_0012

		IL_0012: ldloc.0
		IL_0013: ret
	} // end of method ArrayIndexingCopyNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21d4
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.s 42
		IL_0009: call int32 [System.Runtime]System.Array::BinarySearch<uint8>(!!0[], !!0)
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
	} // end of method ArrayIndexingCopyNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2200
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 4
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.0
		IL_0008: ldc.i4.4
		IL_0009: ldc.i4.s 42
		IL_000b: call int32 [System.Runtime]System.Array::BinarySearch<uint8>(!!0[], int32, int32, !!0)
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
	} // end of method ArrayIndexingCopyNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x222c
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.s 42
		IL_0009: call int32 [System.Runtime]System.Array::IndexOf<uint8>(!!0[], !!0)
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
	} // end of method ArrayIndexingCopyNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2258
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: newarr [System.Runtime]System.Byte
		IL_0007: ldc.i4.s 42
		IL_0009: call int32 [System.Runtime]System.Array::LastIndexOf<uint8>(!!0[], !!0)
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
	} // end of method ArrayIndexingCopyNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2284
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: call !!0[] [System.Runtime]System.Array::Empty<uint8>()
		IL_0006: call void [System.Runtime]System.Array::Reverse<uint8>(!!0[])
		IL_000b: nop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ArrayIndexingCopyNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x22a4
		// Header size: 12
		// Code size: 30 (0x1e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.4
		IL_0002: ldc.i4.4
		IL_0003: newobj instance void int32[0..., 0...]::.ctor(int32, int32)
		IL_0008: ldc.i4.0
		IL_0009: call instance int32 [System.Runtime]System.Array::GetLength(int32)
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
	} // end of method ArrayIndexingCopyNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x22d0
		// Header size: 12
		// Code size: 34 (0x22)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.1
		IL_0002: newarr [System.Runtime]System.Int32
		IL_0007: ldc.i4.0
		IL_0008: call instance object [System.Runtime]System.Array::GetValue(int32)
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
	} // end of method ArrayIndexingCopyNativeEntry::Method12

} // end of class ArrayIndexingCopyNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
