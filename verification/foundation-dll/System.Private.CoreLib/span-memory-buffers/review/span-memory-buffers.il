// IL code: SpanMemoryBuffersNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit SpanMemoryBuffersNativeEntry
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

		IL_0045: call int32 SpanMemoryBuffersNativeEntry::Method0()
		IL_004a: stloc.2
		IL_004b: br.s IL_00b9

		IL_004d: call int32 SpanMemoryBuffersNativeEntry::Method1()
		IL_0052: stloc.2
		IL_0053: br.s IL_00b9

		IL_0055: call int32 SpanMemoryBuffersNativeEntry::Method2()
		IL_005a: stloc.2
		IL_005b: br.s IL_00b9

		IL_005d: call int32 SpanMemoryBuffersNativeEntry::Method3()
		IL_0062: stloc.2
		IL_0063: br.s IL_00b9

		IL_0065: call int32 SpanMemoryBuffersNativeEntry::Method4()
		IL_006a: stloc.2
		IL_006b: br.s IL_00b9

		IL_006d: call int32 SpanMemoryBuffersNativeEntry::Method5()
		IL_0072: stloc.2
		IL_0073: br.s IL_00b9

		IL_0075: call int32 SpanMemoryBuffersNativeEntry::Method6()
		IL_007a: stloc.2
		IL_007b: br.s IL_00b9

		IL_007d: call int32 SpanMemoryBuffersNativeEntry::Method7()
		IL_0082: stloc.2
		IL_0083: br.s IL_00b9

		IL_0085: call int32 SpanMemoryBuffersNativeEntry::Method8()
		IL_008a: stloc.2
		IL_008b: br.s IL_00b9

		IL_008d: call int32 SpanMemoryBuffersNativeEntry::Method9()
		IL_0092: stloc.2
		IL_0093: br.s IL_00b9

		IL_0095: call int32 SpanMemoryBuffersNativeEntry::Method10()
		IL_009a: stloc.2
		IL_009b: br.s IL_00b9

		IL_009d: call int32 SpanMemoryBuffersNativeEntry::Method11()
		IL_00a2: stloc.2
		IL_00a3: br.s IL_00b9

		IL_00a5: call int32 SpanMemoryBuffersNativeEntry::Method12()
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00b9

		IL_00ad: call int32 SpanMemoryBuffersNativeEntry::Method13()
		IL_00b2: stloc.2
		IL_00b3: br.s IL_00b9

		IL_00b5: ldc.i4.m1
		IL_00b6: stloc.2
		IL_00b7: br.s IL_00b9

		IL_00b9: ldloc.2
		IL_00ba: ret
	} // end of method SpanMemoryBuffersNativeEntry::Run

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
	} // end of method SpanMemoryBuffersNativeEntry::Method0

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
	} // end of method SpanMemoryBuffersNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2140
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
	} // end of method SpanMemoryBuffersNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2154
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
	} // end of method SpanMemoryBuffersNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2168
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 2
		.locals init (
			[0] valuetype [System.Runtime]System.Span`1<uint8>,
			[1] valuetype [System.Runtime]System.Span`1<uint8>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 0
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Span`1<uint8>
		IL_000a: ldloca.s 1
		IL_000c: initobj valuetype [System.Runtime]System.Span`1<uint8>
		IL_0012: ldloc.1
		IL_0013: call instance void valuetype [System.Runtime]System.Span`1<uint8>::CopyTo(valuetype [System.Runtime]System.Span`1<!0>)
		IL_0018: nop
		IL_0019: ldc.i4.0
		IL_001a: stloc.2
		IL_001b: br.s IL_001d

		IL_001d: ldloc.2
		IL_001e: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2194
		// Header size: 12
		// Code size: 36 (0x24)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Span`1<uint8>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: call valuetype [System.Runtime]System.Span`1<!0> valuetype [System.Runtime]System.Span`1<uint8>::get_Empty()
		IL_0006: stloc.1
		IL_0007: ldloca.s 1
		IL_0009: constrained. valuetype [System.Runtime]System.Span`1<uint8>
		IL_000f: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0014: stloc.0
		IL_0015: ldc.i4.0
		IL_0016: ldloc.0
		IL_0017: ldnull
		IL_0018: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001d: nop
		IL_001e: ldloc.0
		IL_001f: stloc.2
		IL_0020: br.s IL_0022

		IL_0022: ldloc.2
		IL_0023: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21c4
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
	} // end of method SpanMemoryBuffersNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x21d8
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
	} // end of method SpanMemoryBuffersNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x21ec
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.ReadOnlySpan`1<uint8>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.ReadOnlySpan`1<uint8>
		IL_000a: call instance !0[] valuetype [System.Runtime]System.ReadOnlySpan`1<uint8>::ToArray()
		IL_000f: ldlen
		IL_0010: conv.i4
		IL_0011: stloc.0
		IL_0012: ldc.i4.0
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.2
		IL_001d: br.s IL_001f

		IL_001f: ldloc.2
		IL_0020: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
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
	} // end of method SpanMemoryBuffersNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2230
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Memory`1<uint8>,
			[2] valuetype [System.Runtime]System.Span`1<uint8>,
			[3] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Memory`1<uint8>
		IL_000a: call instance valuetype [System.Runtime]System.Span`1<!0> valuetype [System.Runtime]System.Memory`1<uint8>::get_Span()
		IL_000f: stloc.2
		IL_0010: ldloca.s 2
		IL_0012: constrained. valuetype [System.Runtime]System.Span`1<uint8>
		IL_0018: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001d: stloc.0
		IL_001e: ldc.i4.0
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.3
		IL_0029: br.s IL_002b

		IL_002b: ldloc.3
		IL_002c: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x226c
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Memory`1<uint8>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Memory`1<uint8>
		IL_000a: call instance !0[] valuetype [System.Runtime]System.Memory`1<uint8>::ToArray()
		IL_000f: ldlen
		IL_0010: conv.i4
		IL_0011: stloc.0
		IL_0012: ldc.i4.0
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.2
		IL_001d: br.s IL_001f

		IL_001f: ldloc.2
		IL_0020: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x229c
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.ReadOnlySpan`1<uint8>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: initobj valuetype [System.Runtime]System.ReadOnlySpan`1<uint8>
		IL_0009: ldloc.1
		IL_000a: call !!0& [System.Runtime]System.Runtime.InteropServices.MemoryMarshal::GetReference<uint8>(valuetype [System.Runtime]System.ReadOnlySpan`1<!!0>)
		IL_000f: ldind.u1
		IL_0010: stloc.0
		IL_0011: ldc.i4.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.2
		IL_001c: br.s IL_001e

		IL_001e: ldloc.2
		IL_001f: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x22c8
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Span`1<uint8>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: initobj valuetype [System.Runtime]System.Span`1<uint8>
		IL_0009: ldloc.1
		IL_000a: call !!0& [System.Runtime]System.Runtime.InteropServices.MemoryMarshal::GetReference<uint8>(valuetype [System.Runtime]System.Span`1<!!0>)
		IL_000f: ldind.u1
		IL_0010: stloc.0
		IL_0011: ldloc.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.2
		IL_001c: br.s IL_001e

		IL_001e: ldloc.2
		IL_001f: ret
	} // end of method SpanMemoryBuffersNativeEntry::Method13

} // end of class SpanMemoryBuffersNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
