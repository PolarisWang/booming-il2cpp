// IL code: GuidRandomHashcodeNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit GuidRandomHashcodeNativeEntry
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

		IL_0045: call int32 GuidRandomHashcodeNativeEntry::Method0()
		IL_004a: stloc.2
		IL_004b: br.s IL_00b9

		IL_004d: call int32 GuidRandomHashcodeNativeEntry::Method1()
		IL_0052: stloc.2
		IL_0053: br.s IL_00b9

		IL_0055: call int32 GuidRandomHashcodeNativeEntry::Method2()
		IL_005a: stloc.2
		IL_005b: br.s IL_00b9

		IL_005d: call int32 GuidRandomHashcodeNativeEntry::Method3()
		IL_0062: stloc.2
		IL_0063: br.s IL_00b9

		IL_0065: call int32 GuidRandomHashcodeNativeEntry::Method4()
		IL_006a: stloc.2
		IL_006b: br.s IL_00b9

		IL_006d: call int32 GuidRandomHashcodeNativeEntry::Method5()
		IL_0072: stloc.2
		IL_0073: br.s IL_00b9

		IL_0075: call int32 GuidRandomHashcodeNativeEntry::Method6()
		IL_007a: stloc.2
		IL_007b: br.s IL_00b9

		IL_007d: call int32 GuidRandomHashcodeNativeEntry::Method7()
		IL_0082: stloc.2
		IL_0083: br.s IL_00b9

		IL_0085: call int32 GuidRandomHashcodeNativeEntry::Method8()
		IL_008a: stloc.2
		IL_008b: br.s IL_00b9

		IL_008d: call int32 GuidRandomHashcodeNativeEntry::Method9()
		IL_0092: stloc.2
		IL_0093: br.s IL_00b9

		IL_0095: call int32 GuidRandomHashcodeNativeEntry::Method10()
		IL_009a: stloc.2
		IL_009b: br.s IL_00b9

		IL_009d: call int32 GuidRandomHashcodeNativeEntry::Method11()
		IL_00a2: stloc.2
		IL_00a3: br.s IL_00b9

		IL_00a5: call int32 GuidRandomHashcodeNativeEntry::Method12()
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00b9

		IL_00ad: call int32 GuidRandomHashcodeNativeEntry::Method13()
		IL_00b2: stloc.2
		IL_00b3: br.s IL_00b9

		IL_00b5: ldc.i4.m1
		IL_00b6: stloc.2
		IL_00b7: br.s IL_00b9

		IL_00b9: ldloc.2
		IL_00ba: ret
	} // end of method GuidRandomHashcodeNativeEntry::Run

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
	} // end of method GuidRandomHashcodeNativeEntry::Method0

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
	} // end of method GuidRandomHashcodeNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2140
		// Header size: 12
		// Code size: 36 (0x24)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Guid,
			[2] int32
		)

		IL_0000: nop
		IL_0001: call valuetype [System.Runtime]System.Guid [System.Runtime]System.Guid::NewGuid()
		IL_0006: stloc.1
		IL_0007: ldloca.s 1
		IL_0009: constrained. [System.Runtime]System.Guid
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
	} // end of method GuidRandomHashcodeNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2170
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
	} // end of method GuidRandomHashcodeNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2184
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
	} // end of method GuidRandomHashcodeNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2198
		// Header size: 12
		// Code size: 41 (0x29)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Guid,
			[2] int32
		)

		IL_0000: nop
		IL_0001: call valuetype [System.Runtime]System.Guid [System.Runtime]System.Guid::NewGuid()
		IL_0006: stloc.1
		IL_0007: ldloca.s 1
		IL_0009: constrained. [System.Runtime]System.Guid
		IL_000f: callvirt instance string [System.Runtime]System.Object::ToString()
		IL_0014: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0019: stloc.0
		IL_001a: ldc.i4.0
		IL_001b: ldloc.0
		IL_001c: ldnull
		IL_001d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0022: nop
		IL_0023: ldloc.0
		IL_0024: stloc.2
		IL_0025: br.s IL_0027

		IL_0027: ldloc.2
		IL_0028: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21d0
		// Header size: 12
		// Code size: 13 (0xd)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Random::.ctor()
		IL_0006: pop
		IL_0007: ldc.i4.0
		IL_0008: stloc.0
		IL_0009: br.s IL_000b

		IL_000b: ldloc.0
		IL_000c: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x21ec
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Random::.ctor()
		IL_0006: callvirt instance int32 [System.Runtime]System.Random::Next()
		IL_000b: stloc.0
		IL_000c: ldc.i4 -941579362
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2218
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Random::.ctor()
		IL_0006: ldc.i4.s 42
		IL_0008: callvirt instance int32 [System.Runtime]System.Random::Next(int32)
		IL_000d: stloc.0
		IL_000e: ldc.i4.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2244
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Random::.ctor()
		IL_0006: callvirt instance float64 [System.Runtime]System.Random::NextDouble()
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
	} // end of method GuidRandomHashcodeNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x226c
		// Header size: 12
		// Code size: 35 (0x23)
		.maxstack 4
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Random::.ctor()
		IL_0006: ldc.i4.3
		IL_0007: newarr [System.Runtime]System.Byte
		IL_000c: dup
		IL_000d: ldtoken field valuetype '<PrivateImplementationDetails>'/'__StaticArrayInitTypeSize=3' '<PrivateImplementationDetails>'::'039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81'
		IL_0012: call void [System.Runtime]System.Runtime.CompilerServices.RuntimeHelpers::InitializeArray(class [System.Runtime]System.Array, valuetype [System.Runtime]System.RuntimeFieldHandle)
		IL_0017: callvirt instance void [System.Runtime]System.Random::NextBytes(uint8[])
		IL_001c: nop
		IL_001d: ldc.i4.0
		IL_001e: stloc.0
		IL_001f: br.s IL_0021

		IL_0021: ldloc.0
		IL_0022: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x229c
		// Header size: 12
		// Code size: 24 (0x18)
		.maxstack 2
		.locals init (
			[0] valuetype [System.Runtime]System.HashCode,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 0
		IL_0003: dup
		IL_0004: initobj [System.Runtime]System.HashCode
		IL_000a: ldc.i4.s 42
		IL_000c: call instance void [System.Runtime]System.HashCode::Add<int32>(!!0)
		IL_0011: nop
		IL_0012: ldc.i4.0
		IL_0013: stloc.1
		IL_0014: br.s IL_0016

		IL_0016: ldloc.1
		IL_0017: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x22c0
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.HashCode,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj [System.Runtime]System.HashCode
		IL_000a: call instance int32 [System.Runtime]System.HashCode::ToHashCode()
		IL_000f: stloc.0
		IL_0010: ldc.i4.s 36
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.2
		IL_001c: br.s IL_001e

		IL_001e: ldloc.2
		IL_001f: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x22ec
		// Header size: 12
		// Code size: 26 (0x1a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: ldc.i4.s 42
		IL_0005: call int32 [System.Runtime]System.HashCode::Combine<int32, int32>(!!0, !!1)
		IL_000a: stloc.0
		IL_000b: ldc.i4.0
		IL_000c: ldloc.0
		IL_000d: ldnull
		IL_000e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0013: nop
		IL_0014: ldloc.0
		IL_0015: stloc.1
		IL_0016: br.s IL_0018

		IL_0018: ldloc.1
		IL_0019: ret
	} // end of method GuidRandomHashcodeNativeEntry::Method13

} // end of class GuidRandomHashcodeNativeEntry

.class private auto ansi sealed '<PrivateImplementationDetails>'
	extends [System.Runtime]System.Object
{
	.custom instance void [System.Runtime]System.Runtime.CompilerServices.CompilerGeneratedAttribute::.ctor() = (
		01 00 00 00
	)
	// Nested Types
	.class nested assembly explicit ansi sealed '__StaticArrayInitTypeSize=3'
		extends [System.Runtime]System.ValueType
	{
		.pack 1
		.size 3

	} // end of class __StaticArrayInitTypeSize=3


	// Fields
	.field assembly static initonly valuetype '<PrivateImplementationDetails>'/'__StaticArrayInitTypeSize=3' '039058C6F2C0CB492C533B0A4D14EF77CC0F78ABCCCED5287D84A1A2011CFB81' at I_00002E00
	.data cil I_00002E00 = bytearray (
		01 02 03
	)

} // end of class <PrivateImplementationDetails>

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
