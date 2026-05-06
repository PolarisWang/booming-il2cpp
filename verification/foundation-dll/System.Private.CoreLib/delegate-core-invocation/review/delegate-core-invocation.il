// IL code: DelegateCoreInvocationNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit DelegateCoreInvocationNativeEntry
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

		IL_0035: call int32 DelegateCoreInvocationNativeEntry::Method0()
		IL_003a: stloc.2
		IL_003b: br.s IL_0089

		IL_003d: call int32 DelegateCoreInvocationNativeEntry::Method1()
		IL_0042: stloc.2
		IL_0043: br.s IL_0089

		IL_0045: call int32 DelegateCoreInvocationNativeEntry::Method2()
		IL_004a: stloc.2
		IL_004b: br.s IL_0089

		IL_004d: call int32 DelegateCoreInvocationNativeEntry::Method3()
		IL_0052: stloc.2
		IL_0053: br.s IL_0089

		IL_0055: call int32 DelegateCoreInvocationNativeEntry::Method4()
		IL_005a: stloc.2
		IL_005b: br.s IL_0089

		IL_005d: call int32 DelegateCoreInvocationNativeEntry::Method5()
		IL_0062: stloc.2
		IL_0063: br.s IL_0089

		IL_0065: call int32 DelegateCoreInvocationNativeEntry::Method6()
		IL_006a: stloc.2
		IL_006b: br.s IL_0089

		IL_006d: call int32 DelegateCoreInvocationNativeEntry::Method7()
		IL_0072: stloc.2
		IL_0073: br.s IL_0089

		IL_0075: call int32 DelegateCoreInvocationNativeEntry::Method8()
		IL_007a: stloc.2
		IL_007b: br.s IL_0089

		IL_007d: call int32 DelegateCoreInvocationNativeEntry::Method9()
		IL_0082: stloc.2
		IL_0083: br.s IL_0089

		IL_0085: ldc.i4.m1
		IL_0086: stloc.2
		IL_0087: br.s IL_0089

		IL_0089: ldloc.2
		IL_008a: ret
	} // end of method DelegateCoreInvocationNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20e8
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: ldnull
		IL_0003: call class [System.Runtime]System.Delegate [System.Runtime]System.Delegate::Combine(class [System.Runtime]System.Delegate, class [System.Runtime]System.Delegate)
		IL_0008: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method DelegateCoreInvocationNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2114
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: ldnull
		IL_0003: call class [System.Runtime]System.Delegate [System.Runtime]System.Delegate::Remove(class [System.Runtime]System.Delegate, class [System.Runtime]System.Delegate)
		IL_0008: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000d: stloc.0
		IL_000e: ldloc.0
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method DelegateCoreInvocationNativeEntry::Method1

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
	} // end of method DelegateCoreInvocationNativeEntry::Method2

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
	} // end of method DelegateCoreInvocationNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2168
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
	} // end of method DelegateCoreInvocationNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x217c
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
	} // end of method DelegateCoreInvocationNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2190
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
	} // end of method DelegateCoreInvocationNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
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
	} // end of method DelegateCoreInvocationNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
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
	} // end of method DelegateCoreInvocationNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
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
	} // end of method DelegateCoreInvocationNativeEntry::Method9

} // end of class DelegateCoreInvocationNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
