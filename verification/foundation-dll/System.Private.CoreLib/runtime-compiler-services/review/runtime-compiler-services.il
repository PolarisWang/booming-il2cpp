// IL code: RuntimeCompilerServicesNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit RuntimeCompilerServicesNativeEntry
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

		IL_0035: call int32 RuntimeCompilerServicesNativeEntry::Method0()
		IL_003a: stloc.2
		IL_003b: br.s IL_0089

		IL_003d: call int32 RuntimeCompilerServicesNativeEntry::Method1()
		IL_0042: stloc.2
		IL_0043: br.s IL_0089

		IL_0045: call int32 RuntimeCompilerServicesNativeEntry::Method2()
		IL_004a: stloc.2
		IL_004b: br.s IL_0089

		IL_004d: call int32 RuntimeCompilerServicesNativeEntry::Method3()
		IL_0052: stloc.2
		IL_0053: br.s IL_0089

		IL_0055: call int32 RuntimeCompilerServicesNativeEntry::Method4()
		IL_005a: stloc.2
		IL_005b: br.s IL_0089

		IL_005d: call int32 RuntimeCompilerServicesNativeEntry::Method5()
		IL_0062: stloc.2
		IL_0063: br.s IL_0089

		IL_0065: call int32 RuntimeCompilerServicesNativeEntry::Method6()
		IL_006a: stloc.2
		IL_006b: br.s IL_0089

		IL_006d: call int32 RuntimeCompilerServicesNativeEntry::Method7()
		IL_0072: stloc.2
		IL_0073: br.s IL_0089

		IL_0075: call int32 RuntimeCompilerServicesNativeEntry::Method8()
		IL_007a: stloc.2
		IL_007b: br.s IL_0089

		IL_007d: call int32 RuntimeCompilerServicesNativeEntry::Method9()
		IL_0082: stloc.2
		IL_0083: br.s IL_0089

		IL_0085: ldc.i4.m1
		IL_0086: stloc.2
		IL_0087: br.s IL_0089

		IL_0089: ldloc.2
		IL_008a: ret
	} // end of method RuntimeCompilerServicesNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20e8
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: call int32 [System.Runtime]System.Runtime.CompilerServices.RuntimeHelpers::GetHashCode(object)
		IL_000d: stloc.0
		IL_000e: ldc.i4 7880838
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method RuntimeCompilerServicesNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2118
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
		IL_000f: call bool [System.Runtime]System.Runtime.CompilerServices.RuntimeHelpers::Equals(object, object)
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
	} // end of method RuntimeCompilerServicesNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2150
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
		IL_0008: call object [System.Runtime]System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue(object)
		IL_000d: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0012: stloc.0
		IL_0013: ldc.i4.s 42
		IL_0015: ldloc.0
		IL_0016: ldnull
		IL_0017: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001c: nop
		IL_001d: ldloc.0
		IL_001e: stloc.1
		IL_001f: br.s IL_0021

		IL_0021: ldloc.1
		IL_0022: ret
	} // end of method RuntimeCompilerServicesNativeEntry::Method2

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
	} // end of method RuntimeCompilerServicesNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2194
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
	} // end of method RuntimeCompilerServicesNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21a8
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
	} // end of method RuntimeCompilerServicesNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21bc
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
	} // end of method RuntimeCompilerServicesNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x21d0
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
		IL_000b: call class [System.Runtime]System.FormattableString [System.Runtime]System.Runtime.CompilerServices.FormattableStringFactory::Create(string, object[])
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
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
	} // end of method RuntimeCompilerServicesNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2204
		// Header size: 12
		// Code size: 20 (0x14)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: newobj instance void [System.Runtime]System.Runtime.CompilerServices.RuntimeWrappedException::.ctor(object)
		IL_000d: pop
		IL_000e: ldc.i4.0
		IL_000f: stloc.0
		IL_0010: br.s IL_0012

		IL_0012: ldloc.0
		IL_0013: ret
	} // end of method RuntimeCompilerServicesNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2224
		// Header size: 12
		// Code size: 39 (0x27)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: newobj instance void [System.Runtime]System.Runtime.CompilerServices.RuntimeWrappedException::.ctor(object)
		IL_000d: call instance object [System.Runtime]System.Runtime.CompilerServices.RuntimeWrappedException::get_WrappedException()
		IL_0012: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0017: stloc.0
		IL_0018: ldc.i4.0
		IL_0019: ldloc.0
		IL_001a: ldnull
		IL_001b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0020: nop
		IL_0021: ldloc.0
		IL_0022: stloc.1
		IL_0023: br.s IL_0025

		IL_0025: ldloc.1
		IL_0026: ret
	} // end of method RuntimeCompilerServicesNativeEntry::Method9

} // end of class RuntimeCompilerServicesNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
