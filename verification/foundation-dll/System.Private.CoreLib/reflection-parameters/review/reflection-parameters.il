// IL code: ReflectionParametersNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionParametersNativeEntry
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

		IL_0035: call int32 ReflectionParametersNativeEntry::Method0()
		IL_003a: stloc.2
		IL_003b: br.s IL_0089

		IL_003d: call int32 ReflectionParametersNativeEntry::Method1()
		IL_0042: stloc.2
		IL_0043: br.s IL_0089

		IL_0045: call int32 ReflectionParametersNativeEntry::Method2()
		IL_004a: stloc.2
		IL_004b: br.s IL_0089

		IL_004d: call int32 ReflectionParametersNativeEntry::Method3()
		IL_0052: stloc.2
		IL_0053: br.s IL_0089

		IL_0055: call int32 ReflectionParametersNativeEntry::Method4()
		IL_005a: stloc.2
		IL_005b: br.s IL_0089

		IL_005d: call int32 ReflectionParametersNativeEntry::Method5()
		IL_0062: stloc.2
		IL_0063: br.s IL_0089

		IL_0065: call int32 ReflectionParametersNativeEntry::Method6()
		IL_006a: stloc.2
		IL_006b: br.s IL_0089

		IL_006d: call int32 ReflectionParametersNativeEntry::Method7()
		IL_0072: stloc.2
		IL_0073: br.s IL_0089

		IL_0075: call int32 ReflectionParametersNativeEntry::Method8()
		IL_007a: stloc.2
		IL_007b: br.s IL_0089

		IL_007d: call int32 ReflectionParametersNativeEntry::Method9()
		IL_0082: stloc.2
		IL_0083: br.s IL_0089

		IL_0085: ldc.i4.m1
		IL_0086: stloc.2
		IL_0087: br.s IL_0089

		IL_0089: ldloc.2
		IL_008a: ret
	} // end of method ReflectionParametersNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20e8
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance string [System.Runtime]System.Reflection.ParameterInfo::get_Name()
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
	} // end of method ReflectionParametersNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2110
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Reflection.ParameterInfo::get_ParameterType()
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
	} // end of method ReflectionParametersNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2138
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance object [System.Runtime]System.Reflection.ParameterInfo::get_DefaultValue()
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
	} // end of method ReflectionParametersNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2160
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance object [System.Runtime]System.Reflection.ParameterInfo::get_RawDefaultValue()
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
	} // end of method ReflectionParametersNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x2188
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance bool [System.Runtime]System.Reflection.ParameterInfo::get_HasDefaultValue()
		IL_0007: brtrue.s IL_000c

		IL_0009: ldc.i4.0
		IL_000a: br.s IL_000d

		IL_000c: ldc.i4.1

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
	} // end of method ReflectionParametersNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21b4
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance valuetype [System.Runtime]System.Reflection.ParameterAttributes [System.Runtime]System.Reflection.ParameterInfo::get_Attributes()
		IL_0007: stloc.0
		IL_0008: ldloc.0
		IL_0009: ldloc.0
		IL_000a: ldnull
		IL_000b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0010: nop
		IL_0011: ldloc.0
		IL_0012: stloc.1
		IL_0013: br.s IL_0015

		IL_0015: ldloc.1
		IL_0016: ret
	} // end of method ReflectionParametersNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21d8
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance int32 [System.Runtime]System.Reflection.ParameterInfo::get_Position()
		IL_0007: stloc.0
		IL_0008: ldloc.0
		IL_0009: ldloc.0
		IL_000a: ldnull
		IL_000b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0010: nop
		IL_0011: ldloc.0
		IL_0012: stloc.1
		IL_0013: br.s IL_0015

		IL_0015: ldloc.1
		IL_0016: ret
	} // end of method ReflectionParametersNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x21fc
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Reflection.ParameterInfo::GetRequiredCustomModifiers()
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
	} // end of method ReflectionParametersNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2224
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Reflection.ParameterInfo::GetOptionalCustomModifiers()
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
	} // end of method ReflectionParametersNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x224c
		// Header size: 12
		// Code size: 25 (0x19)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Reflection.ParameterInfo[] [System.Runtime]System.Reflection.MethodBase::GetParameters()
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
	} // end of method ReflectionParametersNativeEntry::Method9

} // end of class ReflectionParametersNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
