// IL code: ExceptionThrowDiagnosticsNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit ExceptionThrowDiagnosticsNativeEntry
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

		IL_0045: call int32 ExceptionThrowDiagnosticsNativeEntry::Method0()
		IL_004a: stloc.2
		IL_004b: br.s IL_00b9

		IL_004d: call int32 ExceptionThrowDiagnosticsNativeEntry::Method1()
		IL_0052: stloc.2
		IL_0053: br.s IL_00b9

		IL_0055: call int32 ExceptionThrowDiagnosticsNativeEntry::Method2()
		IL_005a: stloc.2
		IL_005b: br.s IL_00b9

		IL_005d: call int32 ExceptionThrowDiagnosticsNativeEntry::Method3()
		IL_0062: stloc.2
		IL_0063: br.s IL_00b9

		IL_0065: call int32 ExceptionThrowDiagnosticsNativeEntry::Method4()
		IL_006a: stloc.2
		IL_006b: br.s IL_00b9

		IL_006d: call int32 ExceptionThrowDiagnosticsNativeEntry::Method5()
		IL_0072: stloc.2
		IL_0073: br.s IL_00b9

		IL_0075: call int32 ExceptionThrowDiagnosticsNativeEntry::Method6()
		IL_007a: stloc.2
		IL_007b: br.s IL_00b9

		IL_007d: call int32 ExceptionThrowDiagnosticsNativeEntry::Method7()
		IL_0082: stloc.2
		IL_0083: br.s IL_00b9

		IL_0085: call int32 ExceptionThrowDiagnosticsNativeEntry::Method8()
		IL_008a: stloc.2
		IL_008b: br.s IL_00b9

		IL_008d: call int32 ExceptionThrowDiagnosticsNativeEntry::Method9()
		IL_0092: stloc.2
		IL_0093: br.s IL_00b9

		IL_0095: call int32 ExceptionThrowDiagnosticsNativeEntry::Method10()
		IL_009a: stloc.2
		IL_009b: br.s IL_00b9

		IL_009d: call int32 ExceptionThrowDiagnosticsNativeEntry::Method11()
		IL_00a2: stloc.2
		IL_00a3: br.s IL_00b9

		IL_00a5: call int32 ExceptionThrowDiagnosticsNativeEntry::Method12()
		IL_00aa: stloc.2
		IL_00ab: br.s IL_00b9

		IL_00ad: call int32 ExceptionThrowDiagnosticsNativeEntry::Method13()
		IL_00b2: stloc.2
		IL_00b3: br.s IL_00b9

		IL_00b5: ldc.i4.m1
		IL_00b6: stloc.2
		IL_00b7: br.s IL_00b9

		IL_00b9: ldloc.2
		IL_00ba: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x2118
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.Exception::.ctor(string)
		IL_000b: pop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2138
		// Header size: 12
		// Code size: 19 (0x13)
		.maxstack 2
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldnull
		IL_0007: newobj instance void [System.Runtime]System.Exception::.ctor(string, class [System.Runtime]System.Exception)
		IL_000c: pop
		IL_000d: ldc.i4.0
		IL_000e: stloc.0
		IL_000f: br.s IL_0011

		IL_0011: ldloc.0
		IL_0012: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2158
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Exception::.ctor()
		IL_0006: callvirt instance string [System.Runtime]System.Exception::get_Message()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0010: stloc.0
		IL_0011: ldc.i4.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2184
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Exception::.ctor()
		IL_0006: call instance class [System.Runtime]System.Exception [System.Runtime]System.Exception::get_InnerException()
		IL_000b: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0010: stloc.0
		IL_0011: ldc.i4.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x21b0
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
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21c4
		// Header size: 12
		// Code size: 28 (0x1c)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Exception::.ctor()
		IL_0006: call instance int32 [System.Runtime]System.Exception::get_HResult()
		IL_000b: stloc.0
		IL_000c: ldc.i4.s 48
		IL_000e: ldloc.0
		IL_000f: ldnull
		IL_0010: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0015: nop
		IL_0016: ldloc.0
		IL_0017: stloc.1
		IL_0018: br.s IL_001a

		IL_001a: ldloc.1
		IL_001b: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21ec
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Exception::.ctor()
		IL_0006: callvirt instance string [System.Runtime]System.Object::ToString()
		IL_000b: callvirt instance int32 [System.Runtime]System.String::get_Length()
		IL_0010: stloc.0
		IL_0011: ldloc.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x2218
		// Header size: 12
		// Code size: 32 (0x20)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: newobj instance void [System.Runtime]System.Exception::.ctor()
		IL_0006: callvirt instance class [System.Runtime]System.Exception [System.Runtime]System.Exception::GetBaseException()
		IL_000b: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0010: stloc.0
		IL_0011: ldc.i4.0
		IL_0012: ldloc.0
		IL_0013: ldnull
		IL_0014: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0019: nop
		IL_001a: ldloc.0
		IL_001b: stloc.1
		IL_001c: br.s IL_001e

		IL_001e: ldloc.1
		IL_001f: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x2244
		// Header size: 12
		// Code size: 23 (0x17)
		.maxstack 2
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: ldstr "hello"
		IL_000b: newobj instance void [System.Runtime]System.ArgumentException::.ctor(string, string)
		IL_0010: pop
		IL_0011: ldc.i4.0
		IL_0012: stloc.0
		IL_0013: br.s IL_0015

		IL_0015: ldloc.0
		IL_0016: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method8

	.method private hidebysig static 
		int32 Method9 () cil managed 
	{
		// Method begins at RVA 0x2268
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.ArgumentNullException::.ctor(string)
		IL_000b: pop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method9

	.method private hidebysig static 
		int32 Method10 () cil managed 
	{
		// Method begins at RVA 0x2288
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.ArgumentOutOfRangeException::.ctor(string)
		IL_000b: pop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method10

	.method private hidebysig static 
		int32 Method11 () cil managed 
	{
		// Method begins at RVA 0x22a8
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.InvalidOperationException::.ctor(string)
		IL_000b: pop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method11

	.method private hidebysig static 
		int32 Method12 () cil managed 
	{
		// Method begins at RVA 0x22c8
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.NotSupportedException::.ctor(string)
		IL_000b: pop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method12

	.method private hidebysig static 
		int32 Method13 () cil managed 
	{
		// Method begins at RVA 0x22e8
		// Header size: 12
		// Code size: 18 (0x12)
		.maxstack 1
		.locals init (
			[0] int32
		)

		IL_0000: nop
		IL_0001: ldstr "hello"
		IL_0006: newobj instance void [System.Runtime]System.NotImplementedException::.ctor(string)
		IL_000b: pop
		IL_000c: ldc.i4.0
		IL_000d: stloc.0
		IL_000e: br.s IL_0010

		IL_0010: ldloc.0
		IL_0011: ret
	} // end of method ExceptionThrowDiagnosticsNativeEntry::Method13

} // end of class ExceptionThrowDiagnosticsNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
