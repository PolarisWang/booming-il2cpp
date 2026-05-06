// IL code: ThreadingMonitorInterlockedNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ThreadingMonitorInterlockedNativeEntry
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
		// Code size: 187 (0xbb)
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
		IL_0006: switch (IL_0045, IL_004d, IL_0055, IL_005d, IL_0065, IL_006d, IL_0075, IL_007d, IL_0085, IL_008d, IL_0095, IL_009d, IL_00a5, IL_00ad)

		IL_0043: br.s IL_00b5

		IL_0045: call int32 ThreadingMonitorInterlockedNativeEntry::Method0()
		IL_004a: stloc.2
		IL_004b: br.s IL_00b9

		IL_004d: call int32 ThreadingMonitorInterlockedNativeEntry::Method1()
		IL_0052: stloc.2
		IL_0053: br.s IL_00b9

		IL_0055: call int32 ThreadingMonitorInterlockedNativeEntry::Method2()
		IL_005a: stloc.2
		IL_005b: br.s IL_00b9

		IL_005d: call int32 ThreadingMonitorInterlockedNativeEntry::Method3()
		IL_0062: stloc.2
		IL_0063: br.s IL_00b9

		IL_0065: call int32 ThreadingMonitorInterlockedNativeEntry::Method4()
		IL_006a: stloc.2
		IL_006b: br.s IL_00b9

		IL_006d: call int32 ThreadingMonitorInterlockedNativeEntry::Method5()
		IL_0072: stloc.2
		IL_0073: br.s IL_00b9

		IL_0075: call int32 ThreadingMonitorInterlockedNativeEntry::Method6()
		IL_007a: stloc.2
		IL_007b: br.s IL_00b9

		IL_007d: call int32 ThreadingMonitorInterlockedNativeEntry::Method7()
		IL_0082: stloc.2
		IL_0083: br.s IL_00b9

		IL_0085: call int32 ThreadingMonitorInterlockedNativeEntry::Method8()
		IL_008a: stloc.2
		IL_008b: br.s IL_00b9

		IL_008d: call int32 ThreadingMonitorInterlockedNativeEntry::Method9()
		IL_0092: stloc.2
		IL_0093: br.s IL_00b9

		IL_0095: call int32 ThreadingMonitorInterlockedNativeEntry::Method10()
		IL_009a: stloc.2
		IL_009b: br.s IL_00b9

		IL_009d: call int32 ThreadingMonitorInterlockedNativeEntry::Method11()
		IL_00a2: stloc.2
		IL_00a3: br.s IL_00b9

		IL_00a5: call int32 ThreadingMonitorInterlockedNativeEntry::Method12()
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00b9

		IL_00ad: call int32 ThreadingMonitorInterlockedNativeEntry::Method13()
		IL_00b2: stloc.2
		IL_00b3: br.s IL_00b9

		IL_00b5: ldc.i4.m1
		IL_00b6: stloc.2
		IL_00b7: br.s IL_00b9

		IL_00b9: ldloc.2
		IL_00ba: ret
	} // end of method ThreadingMonitorInterlockedNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2118
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x212c
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2140
		// Header size: 12
		// Code size: 35 (0x23)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: call bool [System.Threading]System.Threading.Monitor::TryEnter(object)
		IL_000d: brtrue.s IL_0012

		IL_000f: ldc.i4.0
		IL_0010: br.s IL_0013

		IL_0012: ldc.i4.1

		IL_0013: stloc.0
		IL_0014: ldc.i4.0
		IL_0015: ldloc.0
		IL_0016: ldnull
		IL_0017: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001c: nop
		IL_001d: ldloc.0
		IL_001e: stloc.1
		IL_001f: br.s IL_0021

		IL_0021: ldloc.1
		IL_0022: ret
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2170
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: ldc.i4.s 42
		IL_000a: call bool [System.Threading]System.Threading.Monitor::TryEnter(object, int32)
		IL_000f: brtrue.s IL_0014

		IL_0011: ldc.i4.0
		IL_0012: br.s IL_0015

		IL_0014: ldc.i4.1

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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21a4
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21b8
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21cc
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x21e0
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x21f4
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2208
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x221c
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x2230
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
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x2244
		// Header size: 12
		// Code size: 13 (0xd)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: call void [System.Threading]System.Threading.Interlocked::MemoryBarrier()
		IL_0006: nop
		IL_0007: ldc.i4.0
		IL_0008: stloc.0
		IL_0009: br.s IL_000b

		IL_000b: ldloc.0
		IL_000c: ret
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x2260
		// Header size: 12
		// Code size: 27 (0x1b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: stloc.0
		IL_0004: ldloca.s 0
		IL_0006: call int32 [System.Threading]System.Threading.Volatile::Read(int32&)
		IL_000b: stloc.1
		IL_000c: ldc.i4.1
		IL_000d: ldloc.1
		IL_000e: ldnull
		IL_000f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0014: nop
		IL_0015: ldloc.1
		IL_0016: stloc.2
		IL_0017: br.s IL_0019

		IL_0019: ldloc.2
		IL_001a: ret
	} // end of method ThreadingMonitorInterlockedNativeEntry::Method13

} // end of class ThreadingMonitorInterlockedNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
