// IL code: AttributesCustomMetadataNativeEntry
.class private auto ansi '<Module>'
{
} // end of class <Module>

.class public auto ansi abstract sealed beforefieldinit AttributesCustomMetadataNativeEntry
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
		// Code size: 127 (0x7f)
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
		IL_0006: switch (IL_0031, IL_0039, IL_0041, IL_0049, IL_0051, IL_0059, IL_0061, IL_0069, IL_0071)

		IL_002f: br.s IL_0079

		IL_0031: call int32 AttributesCustomMetadataNativeEntry::Method0()
		IL_0036: stloc.2
		IL_0037: br.s IL_007d

		IL_0039: call int32 AttributesCustomMetadataNativeEntry::Method1()
		IL_003e: stloc.2
		IL_003f: br.s IL_007d

		IL_0041: call int32 AttributesCustomMetadataNativeEntry::Method2()
		IL_0046: stloc.2
		IL_0047: br.s IL_007d

		IL_0049: call int32 AttributesCustomMetadataNativeEntry::Method3()
		IL_004e: stloc.2
		IL_004f: br.s IL_007d

		IL_0051: call int32 AttributesCustomMetadataNativeEntry::Method4()
		IL_0056: stloc.2
		IL_0057: br.s IL_007d

		IL_0059: call int32 AttributesCustomMetadataNativeEntry::Method5()
		IL_005e: stloc.2
		IL_005f: br.s IL_007d

		IL_0061: call int32 AttributesCustomMetadataNativeEntry::Method6()
		IL_0066: stloc.2
		IL_0067: br.s IL_007d

		IL_0069: call int32 AttributesCustomMetadataNativeEntry::Method7()
		IL_006e: stloc.2
		IL_006f: br.s IL_007d

		IL_0071: call int32 AttributesCustomMetadataNativeEntry::Method8()
		IL_0076: stloc.2
		IL_0077: br.s IL_007d

		IL_0079: ldc.i4.m1
		IL_007a: stloc.2
		IL_007b: br.s IL_007d

		IL_007d: ldloc.2
		IL_007e: ret
	} // end of method AttributesCustomMetadataNativeEntry::Run

	.method private hidebysig static 
		int32 Method0 () cil managed 
	{
		// Method begins at RVA 0x20dc
		// Header size: 12
		// Code size: 56 (0x38)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldtoken [System.Runtime]System.Reflection.AssemblyDescriptionAttribute
		IL_0015: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_001a: call class [System.Runtime]System.Attribute [System.Runtime]System.Reflection.CustomAttributeExtensions::GetCustomAttribute(class [System.Runtime]System.Reflection.Assembly, class [System.Runtime]System.Type)
		IL_001f: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0024: stloc.0
		IL_0025: ldc.i4 -1167624669
		IL_002a: ldloc.0
		IL_002b: ldnull
		IL_002c: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0031: nop
		IL_0032: ldloc.0
		IL_0033: stloc.1
		IL_0034: br.s IL_0036

		IL_0036: ldloc.1
		IL_0037: ret
	} // end of method AttributesCustomMetadataNativeEntry::Method0

	.method private hidebysig static 
		int32 Method1 () cil managed 
	{
		// Method begins at RVA 0x2120
		// Header size: 12
		// Code size: 56 (0x38)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldtoken [System.Runtime]System.Reflection.AssemblyDescriptionAttribute
		IL_0015: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_001a: call class [System.Runtime]System.Attribute [System.Runtime]System.Reflection.CustomAttributeExtensions::GetCustomAttribute(class [System.Runtime]System.Reflection.Assembly, class [System.Runtime]System.Type)
		IL_001f: callvirt instance int32 [System.Runtime]System.Object::GetHashCode()
		IL_0024: stloc.0
		IL_0025: ldc.i4 -1167624669
		IL_002a: ldloc.0
		IL_002b: ldnull
		IL_002c: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_0031: nop
		IL_0032: ldloc.0
		IL_0033: stloc.1
		IL_0034: br.s IL_0036

		IL_0036: ldloc.1
		IL_0037: ret
	} // end of method AttributesCustomMetadataNativeEntry::Method1

	.method private hidebysig static 
		int32 Method2 () cil managed 
	{
		// Method begins at RVA 0x2164
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
	} // end of method AttributesCustomMetadataNativeEntry::Method2

	.method private hidebysig static 
		int32 Method3 () cil managed 
	{
		// Method begins at RVA 0x2178
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
	} // end of method AttributesCustomMetadataNativeEntry::Method3

	.method private hidebysig static 
		int32 Method4 () cil managed 
	{
		// Method begins at RVA 0x218c
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
	} // end of method AttributesCustomMetadataNativeEntry::Method4

	.method private hidebysig static 
		int32 Method5 () cil managed 
	{
		// Method begins at RVA 0x21a0
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
	} // end of method AttributesCustomMetadataNativeEntry::Method5

	.method private hidebysig static 
		int32 Method6 () cil managed 
	{
		// Method begins at RVA 0x21b4
		// Header size: 12
		// Code size: 53 (0x35)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldtoken [System.Runtime]System.Reflection.AssemblyDescriptionAttribute
		IL_0015: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_001a: call bool [System.Runtime]System.Reflection.CustomAttributeExtensions::IsDefined(class [System.Runtime]System.Reflection.Assembly, class [System.Runtime]System.Type)
		IL_001f: brtrue.s IL_0024

		IL_0021: ldc.i4.0
		IL_0022: br.s IL_0025

		IL_0024: ldc.i4.1

		IL_0025: stloc.0
		IL_0026: ldc.i4.0
		IL_0027: ldloc.0
		IL_0028: ldnull
		IL_0029: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_002e: nop
		IL_002f: ldloc.0
		IL_0030: stloc.1
		IL_0031: br.s IL_0033

		IL_0033: ldloc.1
		IL_0034: ret
	} // end of method AttributesCustomMetadataNativeEntry::Method6

	.method private hidebysig static 
		int32 Method7 () cil managed 
	{
		// Method begins at RVA 0x21f8
		// Header size: 12
		// Code size: 53 (0x35)
		.maxstack 3
		.locals init (
			[0] int32,
			[1] int32
		)

		IL_0000: nop
		IL_0001: ldtoken [System.Runtime]System.Byte
		IL_0006: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_000b: callvirt instance class [System.Runtime]System.Reflection.Assembly [System.Runtime]System.Type::get_Assembly()
		IL_0010: ldtoken [System.Runtime]System.Reflection.AssemblyDescriptionAttribute
		IL_0015: call class [System.Runtime]System.Type [System.Runtime]System.Type::GetTypeFromHandle(valuetype [System.Runtime]System.RuntimeTypeHandle)
		IL_001a: call bool [System.Runtime]System.Reflection.CustomAttributeExtensions::IsDefined(class [System.Runtime]System.Reflection.Assembly, class [System.Runtime]System.Type)
		IL_001f: brtrue.s IL_0024

		IL_0021: ldc.i4.0
		IL_0022: br.s IL_0025

		IL_0024: ldc.i4.1

		IL_0025: stloc.0
		IL_0026: ldc.i4.0
		IL_0027: ldloc.0
		IL_0028: ldnull
		IL_0029: call void [Chaos.TestFramework.Sdk]Chaos.TestFramework.Assert::Equal(int32, int32, string)
		IL_002e: nop
		IL_002f: ldloc.0
		IL_0030: stloc.1
		IL_0031: br.s IL_0033

		IL_0033: ldloc.1
		IL_0034: ret
	} // end of method AttributesCustomMetadataNativeEntry::Method7

	.method private hidebysig static 
		int32 Method8 () cil managed 
	{
		// Method begins at RVA 0x223c
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
	} // end of method AttributesCustomMetadataNativeEntry::Method8

} // end of class AttributesCustomMetadataNativeEntry

You are not using the latest version of the tool, please update.
Latest version is '10.0.1.8346' (yours is '9.1.0.7988')
