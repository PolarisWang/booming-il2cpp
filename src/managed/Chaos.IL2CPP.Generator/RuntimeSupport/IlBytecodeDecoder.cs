using System;
using System.Collections.Generic;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
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
        => DecodeInternal(ilBytes, null, null, baseOffset);

    /// <summary>
    /// Decode IL with metadata token resolution. When a MetadataReader is provided,
    /// method/field/type tokens in call/callvirt/newobj/ldfld/stfld instructions are
    /// resolved to SubjectId strings and set as the Callee property.
    /// </summary>
    public static IReadOnlyList<ManagedInstructionModel> DecodeWithMetadata(
        byte[] ilBytes, MetadataReader reader, string assemblyName, int baseOffset = 0)
        => DecodeInternal(ilBytes, reader, assemblyName, baseOffset);

    private static IReadOnlyList<ManagedInstructionModel> DecodeInternal(
        byte[] ilBytes, MetadataReader? reader, string? assemblyName, int baseOffset)
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
                    int token = ReadI4(ilBytes, ref o);
                    if (reader != null && !string.IsNullOrEmpty(assemblyName))
                    {
                        // ldstr: resolve token to actual string value
                        if (string.Equals(name, "ldstr", StringComparison.Ordinal))
                        {
                            var ush = MetadataTokens.Handle(token);
                            if (ush.Kind == HandleKind.UserString)
                            {
                                var userString = reader.GetUserString((UserStringHandle)ush);
                                list.Add(new ManagedInstructionModel
                                {
                                    Op = name, Operand = userString ?? "", IlOffset = baseOffset + start,
                                });
                                continue;
                            }
                        }
                        // Only resolve Callee for method-bearing instructions (call, callvirt, newobj, ldftn)
                        // Field/type/string tokens set Operand only — Callee would cause emitter errors.
                        if (IsMethodInstruction(name))
                        {
                            var resolved = ResolveToken(reader, assemblyName, token);
                            list.Add(new ManagedInstructionModel
                            {
                                Op = name, Operand = token, IlOffset = baseOffset + start,
                                Callee = resolved.SubjectId,
                            });
                            continue;
                        }
                        // Resolve type SubjectId for type-bearing instructions (box, newarr, isinst)
                        if (IsTypeInstruction(name))
                        {
                            var typeId = ResolveTypeToken(reader, assemblyName, token);
                            list.Add(new ManagedInstructionModel
                            {
                                Op = name, Operand = token, IlOffset = baseOffset + start,
                                Reference = new ManagedInstructionReference
                                {
                                    AssemblyName = assemblyName,
                                    SubjectKind = "type",
                                    SubjectId = typeId,
                                },
                            });
                            continue;
                        }
                        // Resolve field SubjectId for field-bearing instructions (ldfld, stsfld, etc.)
                        if (IsFieldInstruction(name))
                        {
                            var fieldId = ResolveToken(reader, assemblyName, token);
                            list.Add(new ManagedInstructionModel
                            {
                                Op = name, Operand = token, IlOffset = baseOffset + start,
                                Reference = new ManagedInstructionReference
                                {
                                    AssemblyName = assemblyName,
                                    SubjectKind = "field",
                                    SubjectId = fieldId.SubjectId,
                                },
                            });
                            continue;
                        }
                        operand = token; break;
                    }
                    operand = token; break;
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

    static bool IsMethodInstruction(string op) => op is "call" or "callvirt" or "newobj" or "ldftn" or "ldvirtftn";
    static bool IsTypeInstruction(string op) => op is "box" or "newarr" or "isinst" or "castclass" or "unbox" or "unbox.any";
    static bool IsFieldInstruction(string op) => op is "ldfld" or "stfld" or "ldsfld" or "stsfld" or "ldflda";

    private static string ResolveTypeToken(MetadataReader reader, string assemblyName, int token)
    {
        var handle = MetadataTokens.Handle(token);
        switch (handle.Kind)
        {
            case HandleKind.TypeDefinition:
            {
                var td = reader.GetTypeDefinition((TypeDefinitionHandle)handle);
                var ns = reader.GetString(td.Namespace);
                var name = reader.GetString(td.Name);
                return $"{assemblyName}/{(string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}")}";
            }
            case HandleKind.TypeReference:
            {
                var tr = reader.GetTypeReference((TypeReferenceHandle)handle);
                var ns = reader.GetString(tr.Namespace);
                var name = reader.GetString(tr.Name);
                return $"{assemblyName}/{(string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}")}";
            }
            default:
                return $"<{handle.Kind}:0x{token:X8}>";
        }
    }

    private sealed record TokenResult(string SubjectId);

    private static TokenResult ResolveToken(MetadataReader reader, string assemblyName, int token)
    {
        var handle = MetadataTokens.Handle(token);
        switch (handle.Kind)
        {
            case HandleKind.MethodDefinition:
                return ResolveMethodDef(reader, assemblyName, (MethodDefinitionHandle)handle);
            case HandleKind.MemberReference:
                return ResolveMemberRef(reader, assemblyName, (MemberReferenceHandle)handle);
            case HandleKind.MethodSpecification:
                return ResolveMethodSpec(reader, assemblyName, (MethodSpecificationHandle)handle);
            default:
                return new TokenResult($"<{handle.Kind}:0x{token:X8}>");
        }
    }

    private static TokenResult ResolveMethodDef(MetadataReader reader, string assemblyName, MethodDefinitionHandle h)
    {
        var md = reader.GetMethodDefinition(h);
        var typeName = GetTypeFullName(reader, md.GetDeclaringType());
        var methodName = reader.GetString(md.Name);
        var sig = md.DecodeSignature(new DummySigProvider(), default);
        var returnType = sig.ReturnType;
        var paramTypes = sig.ParameterTypes.ToArray();
        var declaringTypeSubjectId = $"{assemblyName}/{typeName}";
        var sid = ManagedNaming.CreateMethodSubjectId(declaringTypeSubjectId, methodName, returnType, paramTypes);
        return new TokenResult(sid);
    }

    private static TokenResult ResolveMemberRef(MetadataReader reader, string assemblyName, MemberReferenceHandle h)
    {
        var mr = reader.GetMemberReference(h);
        var methodName = reader.GetString(mr.Name);
        var typeName = ResolveTypeFromParent(reader, mr.Parent);
        var sig = mr.DecodeMethodSignature(new DummySigProvider(), default);
        var returnType = sig.ReturnType;
        var paramTypes = sig.ParameterTypes.ToArray();
        var declaringTypeSubjectId = $"{assemblyName}/{typeName}";
        var sid = ManagedNaming.CreateMethodSubjectId(declaringTypeSubjectId, methodName, returnType, paramTypes);
        return new TokenResult(sid);
    }

    private static TokenResult ResolveMethodSpec(MetadataReader reader, string assemblyName, MethodSpecificationHandle h)
    {
        var ms = reader.GetMethodSpecification(h);
        // MethodSpec wraps a method + generic args. Resolve the underlying method first.
        var genericMethod = ResolveToken(reader, assemblyName, MetadataTokens.GetToken(ms.Method));
        return genericMethod;
    }

    private static string GetTypeFullName(MetadataReader reader, TypeDefinitionHandle h)
    {
        var td = reader.GetTypeDefinition(h);
        var ns = reader.GetString(td.Namespace);
        var name = reader.GetString(td.Name);
        return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
    }

    private static string ResolveTypeFromParent(MetadataReader reader, EntityHandle parent)
    {
        if (parent.Kind == HandleKind.TypeDefinition)
        {
            var td = reader.GetTypeDefinition((TypeDefinitionHandle)parent);
            var ns = reader.GetString(td.Namespace);
            var name = reader.GetString(td.Name);
            return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
        }
        if (parent.Kind == HandleKind.TypeReference)
        {
            var tr = reader.GetTypeReference((TypeReferenceHandle)parent);
            var ns = reader.GetString(tr.Namespace);
            var name = reader.GetString(tr.Name);
            return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
        }
        return "<unknown>";
    }

    /// <summary>Minimal signature type provider that returns type name strings.</summary>
    private sealed class DummySigProvider : ISignatureTypeProvider<string, object?>
    {
        public string GetPrimitiveType(PrimitiveTypeCode t) => t switch
        {
            PrimitiveTypeCode.Boolean => "System.Boolean", PrimitiveTypeCode.Byte => "System.Byte",
            PrimitiveTypeCode.Int32 => "System.Int32", PrimitiveTypeCode.Int64 => "System.Int64",
            PrimitiveTypeCode.Single => "System.Single", PrimitiveTypeCode.Double => "System.Double",
            PrimitiveTypeCode.String => "System.String", PrimitiveTypeCode.Void => "System.Void",
            _ => t.ToString(),
        };
        public string GetTypeFromDefinition(MetadataReader r, TypeDefinitionHandle h, byte k) => GetTypeFullName(r, h);
        public string GetTypeFromReference(MetadataReader r, TypeReferenceHandle h, byte k)
        {
            var tr = r.GetTypeReference(h);
            var ns = r.GetString(tr.Namespace);
            var name = r.GetString(tr.Name);
            return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
        }
        public string GetSZArrayType(string e) => $"{e}[]";
        public string GetArrayType(string e, ArrayShape s) => $"{e}[{new string(',', s.Rank - 1)}]";
        public string GetByReferenceType(string e) => $"{e}&";
        public string GetPointerType(string e) => $"{e}*";
        public string GetPinnedType(string e) => e;
        public string GetGenericInstantiation(string g, System.Collections.Immutable.ImmutableArray<string> a) => g;
        public string GetGenericMethodParameter(object? c, int i) => $"!!{i}";
        public string GetGenericTypeParameter(object? c, int i) => $"!{i}";
        public string GetFunctionPointerType(MethodSignature<string> s) => "fnptr";
        public string GetModifiedType(string m, string u, bool r) => u;
        public string GetTypeFromSpecification(MetadataReader r, object? c, TypeSpecificationHandle h, byte k) => "spec";
    }
}
