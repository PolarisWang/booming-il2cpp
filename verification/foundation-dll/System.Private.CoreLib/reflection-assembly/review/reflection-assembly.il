// IL code: ReflectionAssemblyNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ReflectionAssemblyNativeEntry
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
		// Code size: 304 (0x130)
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
		IL_0006: switch (IL_0068, IL_0073, IL_007e, IL_0089, IL_0094, IL_009f, IL_00aa, IL_00b2, IL_00ba, IL_00c2, IL_00ca, IL_00d2, IL_00da, IL_00e2, IL_00ea, IL_00f2, IL_00fa, IL_0102, IL_010a, IL_0112, IL_011a, IL_0122)

		IL_0063: br IL_012a

		IL_0068: call int32 ReflectionAssemblyNativeEntry::Method0()
		IL_006d: stloc.2
		IL_006e: br IL_012e

		IL_0073: call int32 ReflectionAssemblyNativeEntry::Method1()
		IL_0078: stloc.2
		IL_0079: br IL_012e

		IL_007e: call int32 ReflectionAssemblyNativeEntry::Method2()
		IL_0083: stloc.2
		IL_0084: br IL_012e

		IL_0089: call int32 ReflectionAssemblyNativeEntry::Method3()
		IL_008e: stloc.2
		IL_008f: br IL_012e

		IL_0094: call int32 ReflectionAssemblyNativeEntry::Method4()
		IL_0099: stloc.2
		IL_009a: br IL_012e

		IL_009f: call int32 ReflectionAssemblyNativeEntry::Method5()
		IL_00a4: stloc.2
		IL_00a5: br IL_012e

		IL_00aa: call int32 ReflectionAssemblyNativeEntry::Method6()
		IL_00af: stloc.2
		IL_00b0: br.s IL_012e

		IL_00b2: call int32 ReflectionAssemblyNativeEntry::Method7()
		IL_00b7: stloc.2
		IL_00b8: br.s IL_012e

		IL_00ba: call int32 ReflectionAssemblyNativeEntry::Method8()
		IL_00bf: stloc.2
		IL_00c0: br.s IL_012e

		IL_00c2: call int32 ReflectionAssemblyNativeEntry::Method9()
		IL_00c7: stloc.2
		IL_00c8: br.s IL_012e

		IL_00ca: call int32 ReflectionAssemblyNativeEntry::Method10()
		IL_00cf: stloc.2
		IL_00d0: br.s IL_012e

		IL_00d2: call int32 ReflectionAssemblyNativeEntry::Method11()
		IL_00d7: stloc.2
		IL_00d8: br.s IL_012e

		IL_00da: call int32 ReflectionAssemblyNativeEntry::Method12()
		IL_00df: stloc.2
		IL_00e0: br.s IL_012e

		IL_00e2: call int32 ReflectionAssemblyNativeEntry::Method13()
		IL_00e7: stloc.2
		IL_00e8: br.s IL_012e

		IL_00ea: call int32 ReflectionAssemblyNativeEntry::Method14()
		IL_00ef: stloc.2
		IL_00f0: br.s IL_012e

		IL_00f2: call int32 ReflectionAssemblyNativeEntry::Method15()
		IL_00f7: stloc.2
		IL_00f8: br.s IL_012e

		IL_00fa: call int32 ReflectionAssemblyNativeEntry::Method16()
		IL_00ff: stloc.2
		IL_0100: br.s IL_012e

		IL_0102: call int32 ReflectionAssemblyNativeEntry::Method17()
		IL_0107: stloc.2
		IL_0108: br.s IL_012e

		IL_010a: call int32 ReflectionAssemblyNativeEntry::Method18()
		IL_010f: stloc.2
		IL_0110: br.s IL_012e

		IL_0112: call int32 ReflectionAssemblyNativeEntry::Method19()
		IL_0117: stloc.2
		IL_0118: br.s IL_012e

		IL_011a: call int32 ReflectionAssemblyNativeEntry::Method20()
		IL_011f: stloc.2
		IL_0120: br.s IL_012e

		IL_0122: call int32 ReflectionAssemblyNativeEntry::Method21()
		IL_0127: stloc.2
		IL_0128: br.s IL_012e

		IL_012a: ldc.i4.m1
		IL_012b: stloc.2
		IL_012c: br.s IL_012e

		IL_012e: ldloc.2
		IL_012f: ret
	} // end of method ReflectionAssemblyNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x218c
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Reflection.Assembly::GetExecutingAssembly()
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
	} // end of method ReflectionAssemblyNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x21b8
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Reflection.Assembly::GetCallingAssembly()
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
	} // end of method ReflectionAssemblyNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x21e4
		// Header size: 12
		// Code size: 31 (0x1f)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: call class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Reflection.Assembly::GetEntryAssembly()
		IL_0006: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_000b: stloc.0
		IL_000c: ldc.i4 35342034
		IL_0011: ldloc.0
		IL_0012: ldnull
		IL_0013: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0018: nop
		IL_0019: ldloc.0
		IL_001a: stloc.1
		IL_001b: br.s IL_001d

		IL_001d: ldloc.1
		IL_001e: ret
	} // end of method ReflectionAssemblyNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2210
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance class [System.Runtime]System.Reflection.AssemblyName [System.Runtime]System.Reflection.Assembly::GetName()
		IL_0015: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_001a: stloc.0
		IL_001b: ldc.i4 56793269
		IL_0020: ldloc.0
		IL_0021: ldnull
		IL_0022: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0027: nop
		IL_0028: ldloc.0
		IL_0029: stloc.1
		IL_002a: br.s IL_002c

		IL_002c: ldloc.1
		IL_002d: ret
	} // end of method ReflectionAssemblyNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x224c
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance string [System.Runtime]System.Reflection.Assembly::get_FullName()
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.s 90
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionAssemblyNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x2284
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance string [System.Runtime]System.Reflection.Assembly::get_Location()
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.s 86
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionAssemblyNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x22bc
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance string [System.Runtime]System.Reflection.Assembly::get_ImageRuntimeVersion()
		IL_0015: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_001a: stloc.0
		IL_001b: ldc.i4.s 10
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionAssemblyNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x22f4
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Reflection.Assembly::GetTypes()
		IL_0015: ldlen
		IL_0016: conv.i4
		IL_0017: stloc.0
		IL_0018: ldc.i4 2897
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionAssemblyNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x232c
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Reflection.Assembly::GetExportedTypes()
		IL_0015: ldlen
		IL_0016: conv.i4
		IL_0017: stloc.0
		IL_0018: ldc.i4 1378
		IL_001d: ldloc.0
		IL_001e: ldnull
		IL_001f: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0024: nop
		IL_0025: ldloc.0
		IL_0026: stloc.1
		IL_0027: br.s IL_0029

		IL_0029: ldloc.1
		IL_002a: ret
	} // end of method ReflectionAssemblyNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2364
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance class [System.Runtime]System.Type[] [System.Runtime]System.Reflection.Assembly::GetForwardedTypes()
		IL_0015: ldlen
		IL_0016: conv.i4
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
	} // end of method ReflectionAssemblyNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2398
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldstr "hello"
		IL_0015: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Reflection.Assembly::GetType(string)
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
	} // end of method ReflectionAssemblyNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x23d4
		// Header size: 12
		// Code size: 48 (0x30)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldstr "hello"
		IL_0015: ldc.i4.1
		IL_0016: callvirt instance class [System.Runtime]System.Type [System.Runtime]System.Reflection.Assembly::GetType(string, bool)
		IL_001b: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0020: stloc.0
		IL_0021: ldloc.0
		IL_0022: ldloc.0
		IL_0023: ldnull
		IL_0024: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0029: nop
		IL_002a: ldloc.0
		IL_002b: stloc.1
		IL_002c: br.s IL_002e

		IL_002e: ldloc.1
		IL_002f: ret
	} // end of method ReflectionAssemblyNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x2410
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldstr "hello"
		IL_0015: callvirt instance class [System.Runtime]System.Reflection.Module [System.Runtime]System.Reflection.Assembly::GetModule(string)
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
	} // end of method ReflectionAssemblyNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x244c
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance class [System.Runtime]System.Reflection.Module[] [System.Runtime]System.Reflection.Assembly::GetModules()
		IL_0015: ldlen
		IL_0016: conv.i4
		IL_0017: stloc.0
		IL_0018: ldc.i4.1
		IL_0019: ldloc.0
		IL_001a: ldnull
		IL_001b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0020: nop
		IL_0021: ldloc.0
		IL_0022: stloc.1
		IL_0023: br.s IL_0025

		IL_0025: ldloc.1
		IL_0026: ret
	} // end of method ReflectionAssemblyNativeEntry::Method13

	.method private hidebysig static 
		int32 Method14 () cil managed 
	{
		// Method begins at RVA 0x2480
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldstr "hello"
		IL_0015: callvirt instance class [System.Runtime]System.IO.Stream [System.Runtime]System.Reflection.Assembly::GetManifestResourceStream(string)
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
	} // end of method ReflectionAssemblyNativeEntry::Method14

	.method private hidebysig static 
		int32 Method15 () cil managed 
	{
		// Method begins at RVA 0x24bc
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance string[] [System.Runtime]System.Reflection.Assembly::GetManifestResourceNames()
		IL_0015: ldlen
		IL_0016: conv.i4
		IL_0017: stloc.0
		IL_0018: ldc.i4.4
		IL_0019: ldloc.0
		IL_001a: ldnull
		IL_001b: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0020: nop
		IL_0021: ldloc.0
		IL_0022: stloc.1
		IL_0023: br.s IL_0025

		IL_0025: ldloc.1
		IL_0026: ret
	} // end of method ReflectionAssemblyNativeEntry::Method15

	.method private hidebysig static 
		int32 Method16 () cil managed 
	{
		// Method begins at RVA 0x24f0
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
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance class [System.Runtime]System.Reflection.MethodInfo [System.Runtime]System.Reflection.Assembly::get_EntryPoint()
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
	} // end of method ReflectionAssemblyNativeEntry::Method16

	.method private hidebysig static 
		int32 Method17 () cil managed 
	{
		// Method begins at RVA 0x2528
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance string [System.Runtime]System.Reflection.AssemblyName::get_Name()
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
	} // end of method ReflectionAssemblyNativeEntry::Method17

	.method private hidebysig static 
		int32 Method18 () cil managed 
	{
		// Method begins at RVA 0x2550
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Version [System.Runtime]System.Reflection.AssemblyName::get_Version()
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
	} // end of method ReflectionAssemblyNativeEntry::Method18

	.method private hidebysig static 
		int32 Method19 () cil managed 
	{
		// Method begins at RVA 0x2578
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldnull
		IL_0002: callvirt instance class [System.Runtime]System.Globalization.CultureInfo [System.Runtime]System.Reflection.AssemblyName::get_CultureInfo()
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
	} // end of method ReflectionAssemblyNativeEntry::Method19

	.method private hidebysig static 
		int32 Method20 () cil managed 
	{
		// Method begins at RVA 0x25a0
		// Header size: 12
		// Code size: 41 (0x29)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0015: stloc.0
		IL_0016: ldc.i4 1495009
		IL_001b: ldloc.0
		IL_001c: ldnull
		IL_001d: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0022: nop
		IL_0023: ldloc.0
		IL_0024: stloc.1
		IL_0025: br.s IL_0027

		IL_0027: ldloc.1
		IL_0028: ret
	} // end of method ReflectionAssemblyNativeEntry::Method20

	.method private hidebysig static 
		int32 Method21 () cil managed 
	{
		// Method begins at RVA 0x25d8
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
	} // end of method ReflectionAssemblyNativeEntry::Method21

} // end of class ReflectionAssemblyNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
