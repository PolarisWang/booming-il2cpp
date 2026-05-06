// IL code: ReflectionModuleNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionModuleNativeEntry
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
		// Code size: 91 (0x5b)
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
		IL_0006: switch (IL_0025, IL_002d, IL_0035, IL_003d, IL_0045, IL_004d)

		IL_0023: br.s IL_0055

		IL_0025: call int32 ReflectionModuleNativeEntry::Method0()
		IL_002a: stloc.2
		IL_002b: br.s IL_0059

		IL_002d: call int32 ReflectionModuleNativeEntry::Method1()
		IL_0032: stloc.2
		IL_0033: br.s IL_0059

		IL_0035: call int32 ReflectionModuleNativeEntry::Method2()
		IL_003a: stloc.2
		IL_003b: br.s IL_0059

		IL_003d: call int32 ReflectionModuleNativeEntry::Method3()
		IL_0042: stloc.2
		IL_0043: br.s IL_0059

		IL_0045: call int32 ReflectionModuleNativeEntry::Method4()
		IL_004a: stloc.2
		IL_004b: br.s IL_0059

		IL_004d: call int32 ReflectionModuleNativeEntry::Method5()
		IL_0052: stloc.2
		IL_0053: br.s IL_0059

		IL_0055: ldc.i4.m1
		IL_0056: stloc.2
		IL_0057: br.s IL_0059

		IL_0059: ldloc.2
		IL_005a: ret
	} // end of method ReflectionModuleNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20b8
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance string [System.Runtime]System.Reflection.Module::get_FullyQualifiedName()
		IL_0007: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_000c: stloc.0
		IL_000d: ldloc.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionModuleNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x20e0
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance string [System.Runtime]System.Reflection.Module::get_Name()
		IL_0007: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_000c: stloc.0
		IL_000d: ldloc.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionModuleNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2108
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Reflection.Module::get_Assembly()
		IL_0007: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000c: stloc.0
		IL_000d: ldloc.0
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ReflectionModuleNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2130
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: ldstr "hello"
		IL_0007: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Reflection.Module::GetType(string)
		IL_000c: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0011: stloc.0
		IL_0012: ldloc.0
		IL_0013: ldloc.0
		IL_0014: ldnull
		IL_0015: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001a: nop
		IL_001b: ldloc.0
		IL_001c: stloc.1
		IL_001d: br.s IL_001f

		IL_001f: ldloc.1
		IL_0020: ret
	} // end of method ReflectionModuleNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2160
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Reflection.Module::GetTypes()
		IL_0007: ldlen
		IL_0008: conv.i4
		IL_0009: stloc.0
		IL_000a: ldloc.0
		IL_000b: ldloc.0
		IL_000c: ldnull
		IL_000d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0012: nop
		IL_0013: ldloc.0
		IL_0014: stloc.1
		IL_0015: br.s IL_0017

		IL_0017: ldloc.1
		IL_0018: ret
	} // end of method ReflectionModuleNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
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
	} // end of method ReflectionModuleNativeEntry::Method5

} // end of class ReflectionModuleNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
