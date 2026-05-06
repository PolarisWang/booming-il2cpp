// IL code: IoStreamsBasicsNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit IoStreamsBasicsNativeEntry
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

		IL_0058: call int32 IoStreamsBasicsNativeEntry::Method0()
		IL_005d: stloc.2
		IL_005e: br IL_00f2

		IL_0063: call int32 IoStreamsBasicsNativeEntry::Method1()
		IL_0068: stloc.2
		IL_0069: br IL_00f2

		IL_006e: call int32 IoStreamsBasicsNativeEntry::Method2()
		IL_0073: stloc.2
		IL_0074: br.s IL_00f2

		IL_0076: call int32 IoStreamsBasicsNativeEntry::Method3()
		IL_007b: stloc.2
		IL_007c: br.s IL_00f2

		IL_007e: call int32 IoStreamsBasicsNativeEntry::Method4()
		IL_0083: stloc.2
		IL_0084: br.s IL_00f2

		IL_0086: call int32 IoStreamsBasicsNativeEntry::Method5()
		IL_008b: stloc.2
		IL_008c: br.s IL_00f2

		IL_008e: call int32 IoStreamsBasicsNativeEntry::Method6()
		IL_0093: stloc.2
		IL_0094: br.s IL_00f2

		IL_0096: call int32 IoStreamsBasicsNativeEntry::Method7()
		IL_009b: stloc.2
		IL_009c: br.s IL_00f2

		IL_009e: call int32 IoStreamsBasicsNativeEntry::Method8()
		IL_00a3: stloc.2
		IL_00a4: br.s IL_00f2

		IL_00a6: call int32 IoStreamsBasicsNativeEntry::Method9()
		IL_00ab: stloc.2
		IL_00ac: br.s IL_00f2

		IL_00ae: call int32 IoStreamsBasicsNativeEntry::Method10()
		IL_00b3: stloc.2
		IL_00b4: br.s IL_00f2

		IL_00b6: call int32 IoStreamsBasicsNativeEntry::Method11()
		IL_00bb: stloc.2
		IL_00bc: br.s IL_00f2

		IL_00be: call int32 IoStreamsBasicsNativeEntry::Method12()
		IL_00c3: stloc.2
		IL_00c4: br.s IL_00f2

		IL_00c6: call int32 IoStreamsBasicsNativeEntry::Method13()
		IL_00cb: stloc.2
		IL_00cc: br.s IL_00f2

		IL_00ce: call int32 IoStreamsBasicsNativeEntry::Method14()
		IL_00d3: stloc.2
		IL_00d4: br.s IL_00f2

		IL_00d6: call int32 IoStreamsBasicsNativeEntry::Method15()
		IL_00db: stloc.2
		IL_00dc: br.s IL_00f2

		IL_00de: call int32 IoStreamsBasicsNativeEntry::Method16()
		IL_00e3: stloc.2
		IL_00e4: br.s IL_00f2

		IL_00e6: call int32 IoStreamsBasicsNativeEntry::Method17()
		IL_00eb: stloc.2
		IL_00ec: br.s IL_00f2

		IL_00ee: ldc.i4.m1
		IL_00ef: stloc.2
		IL_00f0: br.s IL_00f2

		IL_00f2: ldloc.2
		IL_00f3: ret
	} // end of method IoStreamsBasicsNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2150
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
	} // end of method IoStreamsBasicsNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
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
	} // end of method IoStreamsBasicsNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2178
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.MemoryStream::.ctor()
		IL_0006: callvirt instance void [System.Runtime]System.IO.Stream::Flush()
		IL_000b: nop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method IoStreamsBasicsNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2198
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.MemoryStream::.ctor()
		IL_0006: ldc.i4.s 42
		IL_0008: conv.i8
		IL_0009: ldc.i4.0
		IL_000a: callvirt instance int64 [System.Runtime]System.IO.Stream::Seek(int64, valuetype [System.Runtime]System.IO.SeekOrigin)
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
	} // end of method IoStreamsBasicsNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21c4
		// Header size: 12
		// Code size: 21 (0x15)
		.maxstack 2
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.MemoryStream::.ctor()
		IL_0006: ldc.i4.s 42
		IL_0008: conv.i8
		IL_0009: callvirt instance void [System.Runtime]System.IO.Stream::SetLength(int64)
		IL_000e: nop
		IL_000f: ldc.i4.0
		IL_0010: stloc.0
		IL_0011: br.s IL_0013

		IL_0013: ldloc.0
		IL_0014: ret
	} // end of method IoStreamsBasicsNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21e8
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.MemoryStream::.ctor()
		IL_0006: callvirt instance int64 [System.Runtime]System.IO.Stream::get_Length()
		IL_000b: conv.i4
		IL_000c: stloc.0
		IL_000d: ldc.i4.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method IoStreamsBasicsNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2210
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.MemoryStream::.ctor()
		IL_0006: callvirt instance int64 [System.Runtime]System.IO.Stream::get_Position()
		IL_000b: conv.i4
		IL_000c: stloc.0
		IL_000d: ldc.i4.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method IoStreamsBasicsNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2238
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
	} // end of method IoStreamsBasicsNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x224c
		// Header size: 12
		// Code size: 38 (0x26)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.IO.StringReader::.ctor(string)
		IL_000b: callvirt instance string [System.Runtime]System.IO.TextReader::ReadLine()
		IL_0010: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0015: stloc.0
		IL_0016: ldc.i4.s 42
		IL_0018: ldloc.0
		IL_0019: ldnull
		IL_001a: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001f: nop
		IL_0020: ldloc.0
		IL_0021: stloc.1
		IL_0022: br.s IL_0024

		IL_0024: ldloc.1
		IL_0025: ret
	} // end of method IoStreamsBasicsNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2280
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.IO.StringReader::.ctor(string)
		IL_000b: callvirt instance string [System.Runtime]System.IO.TextReader::ReadToEnd()
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
	} // end of method IoStreamsBasicsNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x22b4
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 2
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.StringWriter::.ctor()
		IL_0006: ldstr "hello"
		IL_000b: callvirt instance void [System.Runtime]System.IO.TextWriter::Write(string)
		IL_0010: nop
		IL_0011: ldc.i4.0
		IL_0012: stloc.0
		IL_0013: br.s IL_0015

		IL_0015: ldloc.0
		IL_0016: ret
	} // end of method IoStreamsBasicsNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x22d8
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 2
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.StringWriter::.ctor()
		IL_0006: ldstr "hello"
		IL_000b: callvirt instance void [System.Runtime]System.IO.TextWriter::WriteLine(string)
		IL_0010: nop
		IL_0011: ldc.i4.0
		IL_0012: stloc.0
		IL_0013: br.s IL_0015

		IL_0015: ldloc.0
		IL_0016: ret
	} // end of method IoStreamsBasicsNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x22fc
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.IO.StringWriter::.ctor()
		IL_0006: callvirt instance void [System.Runtime]System.IO.TextWriter::WriteLine()
		IL_000b: nop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method IoStreamsBasicsNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x231c
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
	} // end of method IoStreamsBasicsNativeEntry::Method13

	.method private hidebysig static 
		int32 Method14 () cil managed 
	{
		// Method begins at RVA 0x2330
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
	} // end of method IoStreamsBasicsNativeEntry::Method14

	.method private hidebysig static 
		int32 Method15 () cil managed 
	{
		// Method begins at RVA 0x2344
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
	} // end of method IoStreamsBasicsNativeEntry::Method15

	.method private hidebysig static 
		int32 Method16 () cil managed 
	{
		// Method begins at RVA 0x2358
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
	} // end of method IoStreamsBasicsNativeEntry::Method16

	.method private hidebysig static 
		int32 Method17 () cil managed 
	{
		// Method begins at RVA 0x236c
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
	} // end of method IoStreamsBasicsNativeEntry::Method17

} // end of class IoStreamsBasicsNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
