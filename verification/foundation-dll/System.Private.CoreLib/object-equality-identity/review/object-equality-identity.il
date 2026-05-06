// IL code: ObjectEqualityIdentityNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ObjectEqualityIdentityNativeEntry
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
		// Code size: 103 (0x67)
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
		IL_0006: switch (IL_0029, IL_0031, IL_0039, IL_0041, IL_0049, IL_0051, IL_0059)

		IL_0027: br.s IL_0061

		IL_0029: call int32 ObjectEqualityIdentityNativeEntry::Method0()
		IL_002e: stloc.2
		IL_002f: br.s IL_0065

		IL_0031: call int32 ObjectEqualityIdentityNativeEntry::Method1()
		IL_0036: stloc.2
		IL_0037: br.s IL_0065

		IL_0039: call int32 ObjectEqualityIdentityNativeEntry::Method2()
		IL_003e: stloc.2
		IL_003f: br.s IL_0065

		IL_0041: call int32 ObjectEqualityIdentityNativeEntry::Method3()
		IL_0046: stloc.2
		IL_0047: br.s IL_0065

		IL_0049: call int32 ObjectEqualityIdentityNativeEntry::Method4()
		IL_004e: stloc.2
		IL_004f: br.s IL_0065

		IL_0051: call int32 ObjectEqualityIdentityNativeEntry::Method5()
		IL_0056: stloc.2
		IL_0057: br.s IL_0065

		IL_0059: call int32 ObjectEqualityIdentityNativeEntry::Method6()
		IL_005e: stloc.2
		IL_005f: br.s IL_0065

		IL_0061: ldc.i4.m1
		IL_0062: stloc.2
		IL_0063: br.s IL_0065

		IL_0065: ldloc.2
		IL_0066: ret
	} // end of method ObjectEqualityIdentityNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20c4
		// Header size: 12
		// Code size: 40 (0x28)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Object::.ctor()
		IL_0006: ldc.i4.s 42
		IL_0008: box [System.Runtime]System.Int32
		IL_000d: callvirt instance bool [System.Runtime]System.Object::Equals(object)
		IL_0012: brtrue.s IL_0017

		IL_0014: ldc.i4.0
		IL_0015: br.s IL_0018

		IL_0017: ldc.i4.1

		IL_0018: stloc.0
		IL_0019: ldc.i4.0
		IL_001a: ldloc.0
		IL_001b: ldnull
		IL_001c: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0021: nop
		IL_0022: ldloc.0
		IL_0023: stloc.1
		IL_0024: br.s IL_0026

		IL_0026: ldloc.1
		IL_0027: ret
	} // end of method ObjectEqualityIdentityNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x20f8
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: ldc.i4.s 42
		IL_000a: box [System.Runtime]System.Int32
		IL_000f: call bool [System.Runtime]System.Object::Equals(object, object)
		IL_0014: brtrue.s IL_0019

		IL_0016: ldc.i4.0
		IL_0017: br.s IL_001a

		IL_0019: ldc.i4.1

		IL_001a: stloc.0
		IL_001b: ldc.i4.1
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ObjectEqualityIdentityNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2130
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
		IL_000a: box [System.Runtime]System.Int32
		IL_000f: beq.s IL_0014

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
	} // end of method ObjectEqualityIdentityNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2164
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Object::.ctor()
		IL_0006: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000b: stloc.0
		IL_000c: ldc.i4 7880838
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method ObjectEqualityIdentityNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2190
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Object::.ctor()
		IL_0006: callvirt instance string [System.Runtime]System.Object::ToString()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0010: stloc.0
		IL_0011: ldc.i4.s 13
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method ObjectEqualityIdentityNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21c0
		// Header size: 12
		// Code size: 36 (0x24)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Object::.ctor()
		IL_0006: call instance class [System.Runtime]System.Type [System.Runtime]System.Object::GetType()
		IL_000b: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0010: stloc.0
		IL_0011: ldc.i4 35342034
		IL_0016: ldloc.0
		IL_0017: ldnull
		IL_0018: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001d: nop
		IL_001e: ldloc.0
		IL_001f: stloc.1
		IL_0020: br.s IL_0022

		IL_0022: ldloc.1
		IL_0023: ret
	} // end of method ObjectEqualityIdentityNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21f0
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
	} // end of method ObjectEqualityIdentityNativeEntry::Method6

} // end of class ObjectEqualityIdentityNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
