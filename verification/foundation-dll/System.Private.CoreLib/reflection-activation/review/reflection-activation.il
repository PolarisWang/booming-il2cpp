// IL code: ReflectionActivationNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionActivationNativeEntry
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
		// Code size: 115 (0x73)
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
		IL_0006: switch (IL_002d, IL_0035, IL_003d, IL_0045, IL_004d, IL_0055, IL_005d, IL_0065)

		IL_002b: br.s IL_006d

		IL_002d: call int32 ReflectionActivationNativeEntry::Method0()
		IL_0032: stloc.2
		IL_0033: br.s IL_0071

		IL_0035: call int32 ReflectionActivationNativeEntry::Method1()
		IL_003a: stloc.2
		IL_003b: br.s IL_0071

		IL_003d: call int32 ReflectionActivationNativeEntry::Method2()
		IL_0042: stloc.2
		IL_0043: br.s IL_0071

		IL_0045: call int32 ReflectionActivationNativeEntry::Method3()
		IL_004a: stloc.2
		IL_004b: br.s IL_0071

		IL_004d: call int32 ReflectionActivationNativeEntry::Method4()
		IL_0052: stloc.2
		IL_0053: br.s IL_0071

		IL_0055: call int32 ReflectionActivationNativeEntry::Method5()
		IL_005a: stloc.2
		IL_005b: br.s IL_0071

		IL_005d: call int32 ReflectionActivationNativeEntry::Method6()
		IL_0062: stloc.2
		IL_0063: br.s IL_0071

		IL_0065: call int32 ReflectionActivationNativeEntry::Method7()
		IL_006a: stloc.2
		IL_006b: br.s IL_0071

		IL_006d: ldc.i4.m1
		IL_006e: stloc.2
		IL_006f: br.s IL_0071

		IL_0071: ldloc.2
		IL_0072: ret
	} // end of method ReflectionActivationNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20d0
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call object [System.Runtime]System.Activator::CreateInstance(class [System.Runtime]System.Type)
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
	} // end of method ReflectionActivationNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2104
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call !!0[] [System.Runtime]System.Array::Empty<object>()
		IL_0010: call object [System.Runtime]System.Activator::CreateInstance(class [System.Runtime]System.Type, object[])
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldc.i4.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ReflectionActivationNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x213c
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
	} // end of method ReflectionActivationNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2150
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 5
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldc.i4.0
		IL_000c: ldnull
		IL_000d: call !!0[] [System.Runtime]System.Array::Empty<object>()
		IL_0012: ldnull
		IL_0013: call object [System.Runtime]System.Activator::CreateInstance(class [System.Runtime]System.Type, valuetype [System.Runtime]System.Reflection.BindingFlags, class [System.Runtime]System.Reflection.Binder, object[], class [System.Runtime]System.Globalization.CultureInfo)
		IL_0018: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001d: stloc.0
		IL_001e: ldc.i4.0
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.1
		IL_0029: br.s IL_002b

		IL_002b: ldloc.1
		IL_002c: ret
	} // end of method ReflectionActivationNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x218c
		// Header size: 12
		// Code size: 39 (0x27)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldc.i4.s 42
		IL_000d: call class [System.Runtime]System.Array [System.Runtime]System.Array::CreateInstance(class [System.Runtime]System.Type, int32)
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
	} // end of method ReflectionActivationNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21c0
		// Header size: 12
		// Code size: 45 (0x2d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldc.i4.s 42
		IL_000d: ldc.i4.s 42
		IL_000f: call class [System.Runtime]System.Array [System.Runtime]System.Array::CreateInstance(class [System.Runtime]System.Type, int32, int32)
		IL_0014: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0019: stloc.0
		IL_001a: ldc.i4 7880838
		IL_001f: ldloc.0
		IL_0020: ldnull
		IL_0021: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0026: nop
		IL_0027: ldloc.0
		IL_0028: stloc.1
		IL_0029: br.s IL_002b

		IL_002b: ldloc.1
		IL_002c: ret
	} // end of method ReflectionActivationNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21fc
		// Header size: 12
		// Code size: 46 (0x2e)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: call !!0[] [System.Runtime]System.Array::Empty<int32>()
		IL_0010: call class [System.Runtime]System.Array [System.Runtime]System.Array::CreateInstance(class [System.Runtime]System.Type, int32[])
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldc.i4 35342034
		IL_0020: ldloc.0
		IL_0021: ldnull
		IL_0022: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0027: nop
		IL_0028: ldloc.0
		IL_0029: stloc.1
		IL_002a: br.s IL_002c

		IL_002c: ldloc.1
		IL_002d: ret
	} // end of method ReflectionActivationNativeEntry::Method6

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
	} // end of method ReflectionActivationNativeEntry::Method7

} // end of class ReflectionActivationNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
