using System;
using System.Collections.Generic;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Minimal IL bytecode decoder. Converts raw IL bytes into ManagedInstructionModel
/// objects for the AOT codegen pipeline. Supports common opcodes needed by
/// BridgeAotCompiler for framework method compilation.
/// </summary>
internal static class IlBytecodeDecoder
{
    public static IReadOnlyList<ManagedInstructionModel> Decode(byte[] ilBytes, int baseOffset = 0)
    {
        var list = new List<ManagedInstructionModel>();
        int o = 0;
        while (o < ilBytes.Length)
        {
            int start = o;
            int op = ReadOp(ilBytes, ref o);
            if (op == 0) break;
            var (name, ot) = GetOp(op);
            object? operand = null;
            switch (ot)
            {
                case Op.ShortBrT: operand = o + 1 + (sbyte)ReadB(ilBytes, ref o); break;
                case Op.BrT: operand = o + 4 + ReadI4(ilBytes, ref o); break;
                case Op.ShortI: operand = (sbyte)ReadB(ilBytes, ref o); break;
                case Op.I: operand = ReadI4(ilBytes, ref o); break;
                case Op.ShortV: operand = ReadB(ilBytes, ref o); break;
                case Op.V: operand = ReadU2(ilBytes, ref o); break;
                case Op.I8: operand = ReadI8(ilBytes, ref o); break;
                case Op.ShortR: operand = BitConverter.ToSingle(ilBytes, o); o += 4; break;
                case Op.R: operand = BitConverter.ToDouble(ilBytes, o); o += 8; break;
                case Op.Method: case Op.Field: case Op.Type: case Op.String: case Op.Tok: case Op.Sig:
                    operand = ReadI4(ilBytes, ref o); break;
                case Op.Switch:
                    int n = ReadI4(ilBytes, ref o);
                    var t = new int[n];
                    for (int i = 0; i < n; i++) t[i] = o + i * 4 + ReadI4(ilBytes, ref o);
                    operand = t; break;
            }
            list.Add(new ManagedInstructionModel { Op = name, Operand = operand, IlOffset = baseOffset + start });
        }
        return list;
    }

    static int ReadOp(byte[] b, ref int o)
    {
        if (o >= b.Length) return 0;
        byte b1 = b[o++];
        if (b1 == 0xFE && o < b.Length) return 0xFE00 | b[o++];
        return b1;
    }
    static byte ReadB(byte[] b, ref int o) => b[o++];
    static ushort ReadU2(byte[] b, ref int o) { ushort v = (ushort)(b[o] | (b[o + 1] << 8)); o += 2; return v; }
    static int ReadI4(byte[] b, ref int o) { int v = b[o] | (b[o + 1] << 8) | (b[o + 2] << 16) | (b[o + 3] << 24); o += 4; return v; }
    static long ReadI8(byte[] b, ref int o)
    {
        uint lo = (uint)ReadI4(b, ref o); uint hi = (uint)ReadI4(b, ref o);
        return (long)lo | ((long)hi << 32);
    }

    static (string name, Op op) GetOp(int op)
    {
        if (op <= 13) return (new[]{"nop","break","ldarg.0","ldarg.1","ldarg.2","ldarg.3",
            "ldloc.0","ldloc.1","ldloc.2","ldloc.3","stloc.0","stloc.1","stloc.2","stloc.3"}[op], Op.None);
        if (op >= 16 && op <= 25) return ("ldc.i4." + (op - 16), Op.None);
        return op switch
        {
            0x0E => ("ldarg.s", Op.ShortV), 0x11 => ("ldloc.s", Op.ShortV), 0x13 => ("stloc.s", Op.ShortV),
            0x14 => ("ldnull", Op.None), 0x1F => ("ldc.i4.s", Op.ShortI), 0x20 => ("ldc.i4", Op.I),
            0x21 => ("ldc.i8", Op.I8), 0x22 => ("ldc.r4", Op.ShortR), 0x23 => ("ldc.r8", Op.R),
            0x25 => ("dup", Op.None), 0x26 => ("pop", Op.None), 0x28 => ("call", Op.Method),
            0x2A => ("ret", Op.None), 0x2B => ("br.s", Op.ShortBrT), 0x2C => ("brfalse.s", Op.ShortBrT),
            0x2D => ("brtrue.s", Op.ShortBrT), 0x38 => ("br", Op.BrT), 0x39 => ("brfalse", Op.BrT),
            0x3A => ("brtrue", Op.BrT),
            0x58 => ("add", Op.None), 0x59 => ("sub", Op.None), 0x5A => ("mul", Op.None),
            0x5F => ("and", Op.None), 0x60 => ("or", Op.None), 0x61 => ("xor", Op.None),
            0x67 => ("conv.i1", Op.None), 0x68 => ("conv.i2", Op.None), 0x69 => ("conv.i4", Op.None),
            0x6A => ("conv.i8", Op.None), 0x6B => ("conv.r4", Op.None), 0x6C => ("conv.r8", Op.None),
            0x6F => ("callvirt", Op.Method), 0x72 => ("ldstr", Op.String), 0x73 => ("newobj", Op.Method),
            0x7B => ("ldfld", Op.Field), 0x7D => ("stfld", Op.Field), 0x7E => ("ldsfld", Op.Field),
            0x80 => ("stsfld", Op.Field), 0x8C => ("box", Op.Type), 0x8D => ("newarr", Op.Type),
            0x8E => ("ldlen", Op.None), 0xC6 => ("ldftn", Op.Method),
            0xC9 => ("ldarg", Op.V), 0xCC => ("ldloc", Op.V), 0xCE => ("stloc", Op.V),
            0xFE01 => ("ceq", Op.None), 0xFE02 => ("cgt", Op.None), 0xFE04 => ("clt", Op.None),
            0xFE09 => ("ldarg", Op.ShortV), 0xFE0C => ("ldloc", Op.ShortV), 0xFE0E => ("stloc", Op.ShortV),
            _ => ("nop", Op.None),
        };
    }

    enum Op : byte { None, BrT, ShortBrT, I, ShortI, I8, R, ShortR, V, ShortV, String, Field, Method, Type, Tok, Sig, Switch }
}
