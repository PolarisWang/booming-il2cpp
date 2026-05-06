// IL code: ReflectionBindingNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionBindingNativeEntry
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
		// Code size: 163 (0xa3)
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
		IL_0006: switch (IL_003d, IL_0045, IL_004d, IL_0055, IL_005d, IL_0065, IL_006d, IL_0075, IL_007d, IL_0085, IL_008d, IL_0095)

		IL_003b: br.s IL_009d

		IL_003d: call int32 ReflectionBindingNativeEntry::Method0()
		IL_0042: stloc.2
		IL_0043: br.s IL_00a1

		IL_0045: call int32 ReflectionBindingNativeEntry::Method1()
		IL_004a: stloc.2
		IL_004b: br.s IL_00a1

		IL_004d: call int32 ReflectionBindingNativeEntry::Method2()
		IL_0052: stloc.2
		IL_0053: br.s IL_00a1

		IL_0055: call int32 ReflectionBindingNativeEntry::Method3()
		IL_005a: stloc.2
		IL_005b: br.s IL_00a1

		IL_005d: call int32 ReflectionBindingNativeEntry::Method4()
		IL_0062: stloc.2
		IL_0063: br.s IL_00a1

		IL_0065: call int32 ReflectionBindingNativeEntry::Method5()
		IL_006a: stloc.2
		IL_006b: br.s IL_00a1

		IL_006d: call int32 ReflectionBindingNativeEntry::Method6()
		IL_0072: stloc.2
		IL_0073: br.s IL_00a1

		IL_0075: call int32 ReflectionBindingNativeEntry::Method7()
		IL_007a: stloc.2
		IL_007b: br.s IL_00a1

		IL_007d: call int32 ReflectionBindingNativeEntry::Method8()
		IL_0082: stloc.2
		IL_0083: br.s IL_00a1

		IL_0085: call int32 ReflectionBindingNativeEntry::Method9()
		IL_008a: stloc.2
		IL_008b: br.s IL_00a1

		IL_008d: call int32 ReflectionBindingNativeEntry::Method10()
		IL_0092: stloc.2
		IL_0093: br.s IL_00a1

		IL_0095: call int32 ReflectionBindingNativeEntry::Method11()
		IL_009a: stloc.2
		IL_009b: br.s IL_00a1

		IL_009d: ldc.i4.m1
		IL_009e: stloc.2
		IL_009f: br.s IL_00a1

		IL_00a1: ldloc.2
		IL_00a2: ret
	} // end of method ReflectionBindingNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2100
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
		IL_000b: ldstr "hello"
		IL_0010: call instance class [System.Runtime]System.Reflection.MethodInfo [System.Runtime]System.Type::GetMethod(string)
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldloc.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ReflectionBindingNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2138
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldstr "hello"
		IL_0010: ldc.i4.0
		IL_0011: call instance class [System.Runtime]System.Reflection.MethodInfo [System.Runtime]System.Type::GetMethod(string, valuetype [System.Runtime]System.Reflection.BindingFlags)
		IL_0016: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001b: stloc.0
		IL_001c: ldloc.0
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionBindingNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2170
		// Header size: 12
		// Code size: 47 (0x2f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldstr "hello"
		IL_0010: call !!0[] [System.Runtime]System.Array::Empty<class [System.Runtime]System.Type>()
		IL_0015: call instance class [System.Runtime]System.Reflection.MethodInfo [System.Runtime]System.Type::GetMethod(string, class [System.Runtime]System.Type[])
		IL_001a: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001f: stloc.0
		IL_0020: ldloc.0
		IL_0021: ldloc.0
		IL_0022: ldnull
		IL_0023: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0028: nop
		IL_0029: ldloc.0
		IL_002a: stloc.1
		IL_002b: br.s IL_002d

		IL_002d: ldloc.1
		IL_002e: ret
	} // end of method ReflectionBindingNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x21ac
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
		IL_000b: ldstr "hello"
		IL_0010: call instance class [System.Runtime]System.Reflection.FieldInfo [System.Runtime]System.Type::GetField(string)
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldloc.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ReflectionBindingNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21e4
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldstr "hello"
		IL_0010: ldc.i4.0
		IL_0011: callvirt instance class [System.Runtime]System.Reflection.FieldInfo [System.Runtime]System.Type::GetField(string, valuetype [System.Runtime]System.Reflection.BindingFlags)
		IL_0016: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001b: stloc.0
		IL_001c: ldloc.0
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionBindingNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x221c
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
		IL_000b: ldstr "hello"
		IL_0010: call instance class [System.Runtime]System.Reflection.PropertyInfo [System.Runtime]System.Type::GetProperty(string)
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldloc.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ReflectionBindingNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2254
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldstr "hello"
		IL_0010: ldc.i4.0
		IL_0011: call instance class [System.Runtime]System.Reflection.PropertyInfo [System.Runtime]System.Type::GetProperty(string, valuetype [System.Runtime]System.Reflection.BindingFlags)
		IL_0016: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001b: stloc.0
		IL_001c: ldloc.0
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionBindingNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x228c
		// Header size: 12
		// Code size: 47 (0x2f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldstr "hello"
		IL_0010: call !!0[] [System.Runtime]System.Array::Empty<class [System.Runtime]System.Type>()
		IL_0015: call instance class [System.Runtime]System.Reflection.PropertyInfo [System.Runtime]System.Type::GetProperty(string, class [System.Runtime]System.Type[])
		IL_001a: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001f: stloc.0
		IL_0020: ldloc.0
		IL_0021: ldloc.0
		IL_0022: ldnull
		IL_0023: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0028: nop
		IL_0029: ldloc.0
		IL_002a: stloc.1
		IL_002b: br.s IL_002d

		IL_002d: ldloc.1
		IL_002e: ret
	} // end of method ReflectionBindingNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x22c8
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
		IL_000b: ldstr "hello"
		IL_0010: call instance class [System.Runtime]System.Reflection.EventInfo [System.Runtime]System.Type::GetEvent(string)
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldloc.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.1
		IL_0026: br.s IL_0028

		IL_0028: ldloc.1
		IL_0029: ret
	} // end of method ReflectionBindingNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2300
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: ldstr "hello"
		IL_0010: ldc.i4.0
		IL_0011: callvirt instance class [System.Runtime]System.Reflection.EventInfo [System.Runtime]System.Type::GetEvent(string, valuetype [System.Runtime]System.Reflection.BindingFlags)
		IL_0016: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001b: stloc.0
		IL_001c: ldloc.0
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionBindingNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2338
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
	} // end of method ReflectionBindingNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x234c
		// Header size: 12
		// Code size: 43 (0x2b)
		.maxstack 6
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: ldc.i4.s 42
		IL_0004: box [System.Runtime]System.Int32
		IL_0009: ldc.i4.0
		IL_000a: ldnull
		IL_000b: call !!0[] [System.Runtime]System.Array::Empty<object>()
		IL_0010: ldnull
		IL_0011: callvirt instance object [System.Runtime]System.Reflection.MethodBase::Invoke(object, valuetype [System.Runtime]System.Reflection.BindingFlags, class [System.Runtime]System.Reflection.Binder, object[], class [System.Runtime]System.Globalization.CultureInfo)
		IL_0016: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001b: stloc.0
		IL_001c: ldc.i4.0
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionBindingNativeEntry::Method11

} // end of class ReflectionBindingNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
