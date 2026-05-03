"""
Known IL opcodes supported by the NativeAot emitter switch-case.
Source: src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs
"""

# Current 110 opcodes handled by the emitter switch-case (case "xxx":)
EMITTER_KNOWN_OPCODES = {
    # Constants
    "ldc.i4", "ldc.i8", "ldc.r4", "ldc.r8", "ldnull",
    # Arguments / Locals
    "ldarg", "ldarga", "ldloc", "ldloca", "stloc",
    # Strings / Tokens
    "ldstr", "ldtoken",
    # Stack
    "dup", "pop",
    # Conversions
    "conv.i4", "conv.i1", "conv.i2", "conv.i8", "conv.u8",
    "conv.r4", "conv.r8", "conv.u",
    "conv.u1", "conv.u2",
    "conv.ovf.i1", "conv.ovf.u1",
    # Comparison
    "cgt.un", "ceq", "cgt", "clt",
    # Arithmetic
    "add", "add.ovf", "sub", "sub.ovf", "mul", "mul.ovf", "div", "rem",
    # Bitwise / Shift
    "shl", "shr", "shr.un", "not", "and", "or", "xor",
    # Calls
    "call", "callvirt", "calli", "ldftn", "newobj",
    # Exception
    "throw",
    # Arrays
    "newarr", "ldlen", "ldelema", "ldelem", "ldelem.ref", "stelem", "stelem.ref",
    # Type checking
    "castclass", "isinst",
    # Fields
    "ldfld", "ldsfld", "ldflda", "ldsflda", "stfld", "stsfld",
    # Indirect load (ldind.*)
    "ldind.i4", "ldind.u1", "ldind.i1", "ldind.u2", "ldind.i2",
    "ldind.u4", "ldind.i8", "ldind.r4", "ldind.r8", "ldind.ref",
    # Indirect store (stind.*)
    "stind.i4", "stind.i1", "stind.i2", "stind.i8",
    "stind.r4", "stind.r8", "stind.ref",
    # Boxing / ValueType
    "box", "unbox", "unbox.any", "initobj", "ldobj", "stobj",
    # Branch
    "br", "brtrue", "brfalse", "leave",
    "beq", "blt", "bgt", "ble", "bge",
    "bge.un", "bne.un",
    "switch",
    # Return
    "ret",
    # Memory
    "localloc", "cpblk",
    # Newly added opcodes (Phase 1 expansion)
    "nop", "neg", "div.un", "rem.un", "clt.un", "conv.r.un",
    "endfinally", "rethrow", "endfilter",
    "sizeof", "ldvirtftn", "starg.s", "refanytype",
    # Phase 1 expansion: native int indirect + unsigned branch + checked conversions
    "ldind.i", "stind.i",
    "bgt.un", "ble.un", "blt.un",
    "conv.ovf.i2", "conv.ovf.i4", "conv.ovf.i8",
    "conv.ovf.u2", "conv.ovf.u4", "conv.ovf.u8",
    # Phase 1 expansion: native int / unsigned int32 conversion
    "conv.i", "conv.u4",
    # Phase 1 expansion: checked native int conversion
    "conv.ovf.i", "conv.ovf.u",
    # Phase 1 expansion: checked unsigned-input conversion (.un variants)
    "conv.ovf.i.un", "conv.ovf.u.un",
    "conv.ovf.i1.un", "conv.ovf.i2.un", "conv.ovf.i4.un", "conv.ovf.i8.un",
    "conv.ovf.u1.un", "conv.ovf.u2.un", "conv.ovf.u4.un",
    # Phase 1 expansion: overflow-checked unsigned arithmetic
    "add.ovf.un", "sub.ovf.un", "mul.ovf.un",
}

# Additional opcodes with short-form variants handled before switch
EMITTER_SHORT_FORM_HANDLED = {
    # ldc.i4.s gets normalized to ldc.i4
    # ldarg.0/1/2/3 get normalized to ldarg
    # ldloc.0/1/2/3 get normalized to ldloc
    # stloc.0/1/2/3 get normalized to stloc
    # br.s gets normalized to br
    # leave.s gets normalized to leave
    # beq.s, bge.s, bgt.s, ble.s, blt.s get normalized
}

# Opcodes handled/resolved in the Loader stage (never reach emitter switch)
LOADER_HANDLED_OPCODES = {
    # Prefixes: loader skips them (returns null instruction)
    "constrained",   # constrained. prefix — loader reads type token and drops it; callvirt follows with dispatch
    "readonly.",      # readonly. prefix — loader drops it; verification hint, no codegen impact
    # Typed array element load/store: loader normalizes to "ldelem" / "stelem"
    "ldelem.i4",     # normalized to ldelem + Operand=System.Int32
    "stelem.i4",     # normalized to stelem + Operand=System.Int32
}

# Combined set for comparison
EMITTER_ALL_KNOWN = EMITTER_KNOWN_OPCODES | LOADER_HANDLED_OPCODES | {
    # Short forms that emitter handles via normalization
    "ldc.i4.s", "ldc.i4.0", "ldc.i4.1", "ldc.i4.2", "ldc.i4.3",
    "ldc.i4.4", "ldc.i4.5", "ldc.i4.6", "ldc.i4.7", "ldc.i4.8", "ldc.i4.m1",
    "ldarg.0", "ldarg.1", "ldarg.2", "ldarg.3", "ldarg.s",
    "ldloc.0", "ldloc.1", "ldloc.2", "ldloc.3", "ldloc.s",
    "stloc.0", "stloc.1", "stloc.2", "stloc.3", "stloc.s",
    "br.s", "brtrue.s", "brfalse.s",
    "leave.s",
    "beq.s", "bgt.s", "bge.s", "ble.s", "blt.s",
    "bne.un.s", "bge.un.s", "bgt.un.s", "ble.un.s", "blt.un.s",
    "ldarga.s", "ldloca.s",
}
