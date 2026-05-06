// IL code: GenericNullableValueNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit GenericNullableValueNativeEntry
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

		IL_0035: call int32 GenericNullableValueNativeEntry::Method0()
		IL_003a: stloc.2
		IL_003b: br.s IL_0089

		IL_003d: call int32 GenericNullableValueNativeEntry::Method1()
		IL_0042: stloc.2
		IL_0043: br.s IL_0089

		IL_0045: call int32 GenericNullableValueNativeEntry::Method2()
		IL_004a: stloc.2
		IL_004b: br.s IL_0089

		IL_004d: call int32 GenericNullableValueNativeEntry::Method3()
		IL_0052: stloc.2
		IL_0053: br.s IL_0089

		IL_0055: call int32 GenericNullableValueNativeEntry::Method4()
		IL_005a: stloc.2
		IL_005b: br.s IL_0089

		IL_005d: call int32 GenericNullableValueNativeEntry::Method5()
		IL_0062: stloc.2
		IL_0063: br.s IL_0089

		IL_0065: call int32 GenericNullableValueNativeEntry::Method6()
		IL_006a: stloc.2
		IL_006b: br.s IL_0089

		IL_006d: call int32 GenericNullableValueNativeEntry::Method7()
		IL_0072: stloc.2
		IL_0073: br.s IL_0089

		IL_0075: call int32 GenericNullableValueNativeEntry::Method8()
		IL_007a: stloc.2
		IL_007b: br.s IL_0089

		IL_007d: call int32 GenericNullableValueNativeEntry::Method9()
		IL_0082: stloc.2
		IL_0083: br.s IL_0089

		IL_0085: ldc.i4.m1
		IL_0086: stloc.2
		IL_0087: br.s IL_0089

		IL_0089: ldloc.2
		IL_008a: ret
	} // end of method GenericNullableValueNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20e8
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Nullable`1<int32>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Nullable`1<int32>
		IL_000a: call instance bool valuetype [System.Runtime]System.Nullable`1<int32>::get_HasValue()
		IL_000f: brtrue.s IL_0014

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
		IL_0020: stloc.2
		IL_0021: br.s IL_0023

		IL_0023: ldloc.2
		IL_0024: ret
	} // end of method GenericNullableValueNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x211c
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
	} // end of method GenericNullableValueNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2130
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Nullable`1<int32>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Nullable`1<int32>
		IL_000a: call instance !0 valuetype [System.Runtime]System.Nullable`1<int32>::GetValueOrDefault()
		IL_000f: stloc.0
		IL_0010: ldc.i4.0
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.2
		IL_001b: br.s IL_001d

		IL_001d: ldloc.2
		IL_001e: ret
	} // end of method GenericNullableValueNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x215c
		// Header size: 12
		// Code size: 33 (0x21)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Nullable`1<int32>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Nullable`1<int32>
		IL_000a: ldc.i4.s 42
		IL_000c: call instance !0 valuetype [System.Runtime]System.Nullable`1<int32>::GetValueOrDefault(!0)
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
	} // end of method GenericNullableValueNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x218c
		// Header size: 12
		// Code size: 51 (0x33)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Nullable`1<int32>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Nullable`1<int32>
		IL_000a: ldc.i4.s 42
		IL_000c: box [System.Runtime]System.Int32
		IL_0011: constrained. valuetype [System.Runtime]System.Nullable`1<int32>
		IL_0017: callvirt instance bool [System.Runtime]System.Object::Equals(object)
		IL_001c: brtrue.s IL_0021

		IL_001e: ldc.i4.0
		IL_001f: br.s IL_0022

		IL_0021: ldc.i4.1

		IL_0022: stloc.0
		IL_0023: ldc.i4.s 42
		IL_0025: ldloc.0
		IL_0026: ldnull
		IL_0027: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_002c: nop
		IL_002d: ldloc.0
		IL_002e: stloc.2
		IL_002f: br.s IL_0031

		IL_0031: ldloc.2
		IL_0032: ret
	} // end of method GenericNullableValueNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21cc
		// Header size: 12
		// Code size: 37 (0x25)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Nullable`1<int32>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Nullable`1<int32>
		IL_000a: constrained. valuetype [System.Runtime]System.Nullable`1<int32>
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0015: stloc.0
		IL_0016: ldc.i4.0
		IL_0017: ldloc.0
		IL_0018: ldnull
		IL_0019: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_001e: nop
		IL_001f: ldloc.0
		IL_0020: stloc.2
		IL_0021: br.s IL_0023

		IL_0023: ldloc.2
		IL_0024: ret
	} // end of method GenericNullableValueNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x2200
		// Header size: 12
		// Code size: 42 (0x2a)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] valuetype [System.Runtime]System.Nullable`1<int32>,
			[2] int32
		)

		IL_0000: nop
		IL_0001: ldloca.s 1
		IL_0003: dup
		IL_0004: initobj valuetype [System.Runtime]System.Nullable`1<int32>
		IL_000a: constrained. valuetype [System.Runtime]System.Nullable`1<int32>
		IL_0010: callvirt instance string [System.Runtime]System.Object::ToString()
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.0
		IL_001c: ldloc.0
		IL_001d: ldnull
		IL_001e: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0023: nop
		IL_0024: ldloc.0
		IL_0025: stloc.2
		IL_0026: br.s IL_0028

		IL_0028: ldloc.2
		IL_0029: ret
	} // end of method GenericNullableValueNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2238
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
		IL_000f: callvirt instance bool [System.Runtime]System.Object::Equals(object)
		IL_0014: brtrue.s IL_0019

		IL_0016: ldc.i4.0
		IL_0017: br.s IL_001a

		IL_0019: ldc.i4.1

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
	} // end of method GenericNullableValueNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2270
		// Header size: 12
		// Code size: 29 (0x1d)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldc.i4.s 42
		IL_0003: box [System.Runtime]System.Int32
		IL_0008: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000d: stloc.0
		IL_000e: ldc.i4.1
		IL_000f: ldloc.0
		IL_0010: ldnull
		IL_0011: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0016: nop
		IL_0017: ldloc.0
		IL_0018: stloc.1
		IL_0019: br.s IL_001b

		IL_001b: ldloc.1
		IL_001c: ret
	} // end of method GenericNullableValueNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x229c
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
		IL_0008: callvirt instance string [System.Runtime]System.Object::ToString()
		IL_000d: callvirt instance int32 [System.Runtime]System.String::get_Length()
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
	} // end of method GenericNullableValueNativeEntry::Method9

} // end of class GenericNullableValueNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
