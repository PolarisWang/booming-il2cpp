using System.Reflection.Metadata;
using System.Reflection.PortableExecutable;
using System.Text.Json;

namespace Chaos.IL2CPP.Tools.FoundationPack.Analysis;

internal static class ILScanCommand
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        WriteIndented = true,
    };

    public static int Execute(IReadOnlyDictionary<string, string> options)
    {
        var dllPath = GetRequiredOption(options, "--dll-path");
        var outputPath = GetRequiredOption(options, "--output");

        Console.Error.WriteLine($"Scanning IL for: {dllPath}");

        using var stream = File.OpenRead(dllPath);
        using var peReader = new PEReader(stream);

        if (!peReader.HasMetadata)
        {
            Console.Error.WriteLine("No metadata in assembly.");
            return 1;
        }

        var metadataReader = peReader.GetMetadataReader();
        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

        var methodResults = new List<MethodIlScanResult>();
        var totalMethods = 0;
        var methodsWithBody = 0;
        var allUniqueOpcodes = new HashSet<string>();

        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            var typeDef = metadataReader.GetTypeDefinition(typeHandle);
            var typeName = FullTypeName(metadataReader, typeHandle);

            foreach (var methodHandle in typeDef.GetMethods())
            {
                totalMethods++;
                var methodDef = metadataReader.GetMethodDefinition(methodHandle);
                var methodName = metadataReader.GetString(methodDef.Name);

                var methodSubjectId = $"{assemblyName}/{typeName}::{methodName}";
                var hasBody = methodDef.RelativeVirtualAddress != 0;

                if (!hasBody)
                {
                    methodResults.Add(new MethodIlScanResult
                    {
                        MethodSubjectId = methodSubjectId,
                        HasBody = false,
                        IlOpcodes = [],
                        UniqueOpcodes = [],
                        ParameterCount = CountParameters(metadataReader, methodDef),
                    });
                    continue;
                }

                methodsWithBody++;
                var body = peReader.GetMethodBody(methodDef.RelativeVirtualAddress);
                var ilReader = body.GetILReader();
                var opcodes = new List<string>();

                while (ilReader.RemainingBytes > 0)
                {
                    var opCode = ReadOpCode(ref ilReader);
                    var name = GetOpCodeName(opCode);
                    opcodes.Add(name);

                    // Skip operand based on opcode type
                    SkipOperand(ref ilReader, opCode);
                }

                var unique = opcodes.Distinct().OrderBy(x => x).ToArray();
                foreach (var u in unique) allUniqueOpcodes.Add(u);

                methodResults.Add(new MethodIlScanResult
                {
                    MethodSubjectId = methodSubjectId,
                    HasBody = true,
                    IlOpcodes = opcodes.ToArray(),
                    UniqueOpcodes = unique,
                    ParameterCount = CountParameters(metadataReader, methodDef),
                });
            }
        }

        var result = new IlScanResult
        {
            AssemblyName = assemblyName,
            TotalMethods = totalMethods,
            MethodsWithBody = methodsWithBody,
            MethodsWithoutBody = totalMethods - methodsWithBody,
            UniqueOpcodes = allUniqueOpcodes.OrderBy(x => x).ToArray(),
            Methods = methodResults.OrderBy(m => m.MethodSubjectId).ToArray(),
        };

        File.WriteAllText(outputPath, JsonSerializer.Serialize(result, JsonOptions));
        Console.Error.WriteLine($"Output written to: {outputPath}");
        Console.Error.WriteLine($"  Total methods: {totalMethods}");
        Console.Error.WriteLine($"  With body: {methodsWithBody}");
        Console.Error.WriteLine($"  Unique opcodes: {allUniqueOpcodes.Count}");

        return 0;
    }

    private static string FullTypeName(MetadataReader reader, TypeDefinitionHandle handle)
    {
        var type = reader.GetTypeDefinition(handle);
        var name = reader.GetString(type.Name);
        var ns = reader.GetString(type.Namespace);
        var declaring = type.GetDeclaringType();
        if (!declaring.IsNil)
            return $"{FullTypeName(reader, declaring)}+{name}";
        return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
    }

    private static int CountParameters(MetadataReader reader, MethodDefinition method)
    {
        var count = 0;
        foreach (var param in method.GetParameters())
        {
            var p = reader.GetParameter(param);
            if (p.SequenceNumber > 0) count++;
        }
        return count;
    }

    private static ILOpCode ReadOpCode(ref BlobReader reader)
    {
        var code = reader.ReadByte();
        return code == 0xFE
            ? (ILOpCode)(code << 8 | reader.ReadByte())
            : (ILOpCode)code;
    }

    private static void SkipOperand(ref BlobReader reader, ILOpCode opCode)
    {
        switch (opCode)
        {
            // Branch targets (4 bytes)
            case ILOpCode.Br:
            case ILOpCode.Brfalse:
            case ILOpCode.Brtrue:
            case ILOpCode.Beq:
            case ILOpCode.Bge:
            case ILOpCode.Bgt:
            case ILOpCode.Ble:
            case ILOpCode.Blt:
            case ILOpCode.Bne_un:
            case ILOpCode.Bge_un:
            case ILOpCode.Bgt_un:
            case ILOpCode.Ble_un:
            case ILOpCode.Blt_un:
            case ILOpCode.Leave:
                reader.ReadInt32(); break;

            // Short branch targets (1 byte)
            case ILOpCode.Br_s:
            case ILOpCode.Brfalse_s:
            case ILOpCode.Brtrue_s:
            case ILOpCode.Beq_s:
            case ILOpCode.Bge_s:
            case ILOpCode.Bgt_s:
            case ILOpCode.Ble_s:
            case ILOpCode.Blt_s:
            case ILOpCode.Bne_un_s:
            case ILOpCode.Bge_un_s:
            case ILOpCode.Bgt_un_s:
            case ILOpCode.Ble_un_s:
            case ILOpCode.Blt_un_s:
            case ILOpCode.Leave_s:
                reader.ReadByte(); break;

            // Method/field/type tokens (4 bytes)
            case ILOpCode.Call:
            case ILOpCode.Callvirt:
            case ILOpCode.Newobj:
            case ILOpCode.Ldftn:
            case ILOpCode.Ldvirtftn:
            case ILOpCode.Ldstr:
            case ILOpCode.Ldtoken:
            case ILOpCode.Ldfld:
            case ILOpCode.Ldsfld:
            case ILOpCode.Ldflda:
            case ILOpCode.Ldsflda:
            case ILOpCode.Stfld:
            case ILOpCode.Stsfld:
            case ILOpCode.Castclass:
            case ILOpCode.Isinst:
            case ILOpCode.Box:
            case ILOpCode.Newarr:
            case ILOpCode.Unbox:
            case ILOpCode.Unbox_any:
            case ILOpCode.Ldelema:
            case ILOpCode.Ldelem:
            case ILOpCode.Stelem:
            case ILOpCode.Ldobj:
            case ILOpCode.Stobj:
            case ILOpCode.Initobj:
            case ILOpCode.Sizeof:
            case ILOpCode.Calli:
            case ILOpCode.Mkrefany:
            case ILOpCode.Refanyval:
            case ILOpCode.Refanytype:
            case ILOpCode.Ldarg:
            case ILOpCode.Ldarga:
            case ILOpCode.Starg:
            case ILOpCode.Ldloc:
            case ILOpCode.Ldloca:
            case ILOpCode.Stloc:
                reader.ReadInt32(); break;

            // Switch (4 bytes count + variable)
            case ILOpCode.Switch:
                var switchCount = reader.ReadInt32();
                for (var i = 0; i < switchCount; i++) reader.ReadInt32(); break;

            // Short argument/local (1 byte)
            case ILOpCode.Ldarg_s:
            case ILOpCode.Ldarga_s:
            case ILOpCode.Starg_s:
            case ILOpCode.Ldloc_s:
            case ILOpCode.Ldloca_s:
            case ILOpCode.Stloc_s:
            case ILOpCode.Ldc_i4_s:
                reader.ReadByte(); break;

            // Int32 constant
            case ILOpCode.Ldc_i4:
                reader.ReadInt32(); break;

            // Int64 constant
            case ILOpCode.Ldc_i8:
                reader.ReadInt64(); break;

            // Float32 constant
            case ILOpCode.Ldc_r4:
                reader.ReadSingle(); break;

            // Float64 constant
            case ILOpCode.Ldc_r8:
                reader.ReadDouble(); break;

            // Constrained prefix (1 byte type token afterwards... actually it's a 4-byte type token)
            case ILOpCode.Constrained:
                reader.ReadInt32(); break;

            // No operand
            default:
                break;
        }
    }

    /// <summary>
    /// Maps ILOpCode enum values to conventional opcode name strings.
    /// Only the opcodes used in the emitter switch-case are explicitly named;
    /// others fall back to the enum value string.
    /// </summary>
    private static string GetOpCodeName(ILOpCode opCode)
    {
        return opCode switch
        {
            // Load constant
            ILOpCode.Ldc_i4 => "ldc.i4",
            ILOpCode.Ldc_i4_s => "ldc.i4.s",
            ILOpCode.Ldc_i4_0 => "ldc.i4.0",
            ILOpCode.Ldc_i4_1 => "ldc.i4.1",
            ILOpCode.Ldc_i4_2 => "ldc.i4.2",
            ILOpCode.Ldc_i4_3 => "ldc.i4.3",
            ILOpCode.Ldc_i4_4 => "ldc.i4.4",
            ILOpCode.Ldc_i4_5 => "ldc.i4.5",
            ILOpCode.Ldc_i4_6 => "ldc.i4.6",
            ILOpCode.Ldc_i4_7 => "ldc.i4.7",
            ILOpCode.Ldc_i4_8 => "ldc.i4.8",
            ILOpCode.Ldc_i4_m1 => "ldc.i4.m1",
            ILOpCode.Ldc_i8 => "ldc.i8",
            ILOpCode.Ldc_r4 => "ldc.r4",
            ILOpCode.Ldc_r8 => "ldc.r8",
            ILOpCode.Ldnull => "ldnull",
            ILOpCode.Ldstr => "ldstr",
            ILOpCode.Ldtoken => "ldtoken",

            // Argument
            ILOpCode.Ldarg_0 => "ldarg.0",
            ILOpCode.Ldarg_1 => "ldarg.1",
            ILOpCode.Ldarg_2 => "ldarg.2",
            ILOpCode.Ldarg_3 => "ldarg.3",
            ILOpCode.Ldarg_s => "ldarg.s",
            ILOpCode.Ldarg => "ldarg",
            ILOpCode.Ldarga_s => "ldarga.s",
            ILOpCode.Ldarga => "ldarga",

            // Local
            ILOpCode.Ldloc_0 => "ldloc.0",
            ILOpCode.Ldloc_1 => "ldloc.1",
            ILOpCode.Ldloc_2 => "ldloc.2",
            ILOpCode.Ldloc_3 => "ldloc.3",
            ILOpCode.Ldloc_s => "ldloc.s",
            ILOpCode.Ldloc => "ldloc",
            ILOpCode.Ldloca_s => "ldloca.s",
            ILOpCode.Ldloca => "ldloca",
            ILOpCode.Stloc_0 => "stloc.0",
            ILOpCode.Stloc_1 => "stloc.1",
            ILOpCode.Stloc_2 => "stloc.2",
            ILOpCode.Stloc_3 => "stloc.3",
            ILOpCode.Stloc_s => "stloc.s",
            ILOpCode.Stloc => "stloc",

            // Stack
            ILOpCode.Dup => "dup",
            ILOpCode.Pop => "pop",

            // Arithmetic
            ILOpCode.Add => "add",
            ILOpCode.Add_ovf => "add.ovf",
            ILOpCode.Sub => "sub",
            ILOpCode.Sub_ovf => "sub.ovf",
            ILOpCode.Mul => "mul",
            ILOpCode.Mul_ovf => "mul.ovf",
            ILOpCode.Div => "div",
            ILOpCode.Div_un => "div.un",
            ILOpCode.Rem => "rem",
            ILOpCode.Rem_un => "rem.un",
            ILOpCode.Neg => "neg",

            // Bitwise
            ILOpCode.Shl => "shl",
            ILOpCode.Shr => "shr",
            ILOpCode.Shr_un => "shr.un",
            ILOpCode.Not => "not",
            ILOpCode.And => "and",
            ILOpCode.Or => "or",
            ILOpCode.Xor => "xor",

            // Conversion
            ILOpCode.Conv_i4 => "conv.i4",
            ILOpCode.Conv_i1 => "conv.i1",
            ILOpCode.Conv_i2 => "conv.i2",
            ILOpCode.Conv_i8 => "conv.i8",
            ILOpCode.Conv_u8 => "conv.u8",
            ILOpCode.Conv_r4 => "conv.r4",
            ILOpCode.Conv_r8 => "conv.r8",
            ILOpCode.Conv_u => "conv.u",
            ILOpCode.Conv_u1 => "conv.u1",
            ILOpCode.Conv_u2 => "conv.u2",
            ILOpCode.Conv_ovf_i1 => "conv.ovf.i1",
            ILOpCode.Conv_ovf_u1 => "conv.ovf.u1",
            ILOpCode.Conv_ovf_i2 => "conv.ovf.i2",
            ILOpCode.Conv_ovf_u2 => "conv.ovf.u2",
            ILOpCode.Conv_ovf_i4 => "conv.ovf.i4",
            ILOpCode.Conv_ovf_u4 => "conv.ovf.u4",
            ILOpCode.Conv_ovf_i8 => "conv.ovf.i8",
            ILOpCode.Conv_ovf_u8 => "conv.ovf.u8",
            ILOpCode.Conv_r_un => "conv.r.un",

            // Comparison
            ILOpCode.Ceq => "ceq",
            ILOpCode.Cgt => "cgt",
            ILOpCode.Cgt_un => "cgt.un",
            ILOpCode.Clt => "clt",
            ILOpCode.Clt_un => "clt.un",

            // Branch
            ILOpCode.Br_s => "br.s",
            ILOpCode.Br => "br",
            ILOpCode.Brfalse_s => "brfalse.s",
            ILOpCode.Brfalse => "brfalse",
            ILOpCode.Brtrue_s => "brtrue.s",
            ILOpCode.Brtrue => "brtrue",
            ILOpCode.Beq_s => "beq.s",
            ILOpCode.Beq => "beq",
            ILOpCode.Bne_un_s => "bne.un.s",
            ILOpCode.Bne_un => "bne.un",
            ILOpCode.Bge_s => "bge.s",
            ILOpCode.Bge => "bge",
            ILOpCode.Bge_un_s => "bge.un.s",
            ILOpCode.Bge_un => "bge.un",
            ILOpCode.Bgt_s => "bgt.s",
            ILOpCode.Bgt => "bgt",
            ILOpCode.Ble_s => "ble.s",
            ILOpCode.Ble => "ble",
            ILOpCode.Blt_s => "blt.s",
            ILOpCode.Blt => "blt",
            ILOpCode.Leave_s => "leave.s",
            ILOpCode.Leave => "leave",
            ILOpCode.Switch => "switch",

            // Call
            ILOpCode.Call => "call",
            ILOpCode.Callvirt => "callvirt",
            ILOpCode.Calli => "calli",
            ILOpCode.Ldftn => "ldftn",
            ILOpCode.Ldvirtftn => "ldvirtftn",
            ILOpCode.Newobj => "newobj",
            ILOpCode.Ret => "ret",
            ILOpCode.Throw => "throw",
            ILOpCode.Rethrow => "rethrow",

            // Field
            ILOpCode.Ldfld => "ldfld",
            ILOpCode.Ldsfld => "ldsfld",
            ILOpCode.Ldflda => "ldflda",
            ILOpCode.Ldsflda => "ldsflda",
            ILOpCode.Stfld => "stfld",
            ILOpCode.Stsfld => "stsfld",

            // Array
            ILOpCode.Newarr => "newarr",
            ILOpCode.Ldlen => "ldlen",
            ILOpCode.Ldelema => "ldelema",
            ILOpCode.Ldelem_i4 => "ldelem.i4",
            ILOpCode.Ldelem_i1 => "ldelem.i1",
            ILOpCode.Ldelem_i2 => "ldelem.i2",
            ILOpCode.Ldelem_i8 => "ldelem.i8",
            ILOpCode.Ldelem_r4 => "ldelem.r4",
            ILOpCode.Ldelem_r8 => "ldelem.r8",
            ILOpCode.Ldelem_ref => "ldelem.ref",
            ILOpCode.Ldelem_u1 => "ldelem.u1",
            ILOpCode.Ldelem_u2 => "ldelem.u2",
            ILOpCode.Ldelem_u4 => "ldelem.u4",
            ILOpCode.Ldelem => "ldelem",
            ILOpCode.Stelem_i4 => "stelem.i4",
            ILOpCode.Stelem_i1 => "stelem.i1",
            ILOpCode.Stelem_i2 => "stelem.i2",
            ILOpCode.Stelem_i8 => "stelem.i8",
            ILOpCode.Stelem_r4 => "stelem.r4",
            ILOpCode.Stelem_r8 => "stelem.r8",
            ILOpCode.Stelem_ref => "stelem.ref",
            ILOpCode.Stelem => "stelem",

            // Type
            ILOpCode.Castclass => "castclass",
            ILOpCode.Isinst => "isinst",
            ILOpCode.Box => "box",
            ILOpCode.Unbox => "unbox",
            ILOpCode.Unbox_any => "unbox.any",
            ILOpCode.Initobj => "initobj",
            ILOpCode.Ldobj => "ldobj",
            ILOpCode.Stobj => "stobj",
            ILOpCode.Sizeof => "sizeof",
            ILOpCode.Constrained => "constrained",

            // Indirect load
            ILOpCode.Ldind_i4 => "ldind.i4",
            ILOpCode.Ldind_i1 => "ldind.i1",
            ILOpCode.Ldind_i2 => "ldind.i2",
            ILOpCode.Ldind_i8 => "ldind.i8",
            ILOpCode.Ldind_r4 => "ldind.r4",
            ILOpCode.Ldind_r8 => "ldind.r8",
            ILOpCode.Ldind_ref => "ldind.ref",
            ILOpCode.Ldind_u1 => "ldind.u1",
            ILOpCode.Ldind_u2 => "ldind.u2",
            ILOpCode.Ldind_u4 => "ldind.u4",

            // Indirect store
            ILOpCode.Stind_i4 => "stind.i4",
            ILOpCode.Stind_i1 => "stind.i1",
            ILOpCode.Stind_i2 => "stind.i2",
            ILOpCode.Stind_i8 => "stind.i8",
            ILOpCode.Stind_r4 => "stind.r4",
            ILOpCode.Stind_r8 => "stind.r8",
            ILOpCode.Stind_ref => "stind.ref",

            // Memory
            ILOpCode.Localloc => "localloc",
            ILOpCode.Cpblk => "cpblk",
            ILOpCode.Initblk => "initblk",
            ILOpCode.Cpobj => "cpobj",

            // Prefixes
            ILOpCode.Volatile => "volatile",
            ILOpCode.Readonly => "readonly.",
            ILOpCode.Tail => "tail.",

            // Exception
            ILOpCode.Endfinally => "endfinally",
            ILOpCode.Endfilter => "endfilter",

            // Misc
            ILOpCode.Nop => "nop",
            ILOpCode.Mkrefany => "mkrefany",
            ILOpCode.Refanyval => "refanyval",
            ILOpCode.Refanytype => "refanytype",
            ILOpCode.Arglist => "arglist",
            ILOpCode.Break => "break",

            _ => opCode.ToString(),
        };
    }

    private static string GetRequiredOption(IReadOnlyDictionary<string, string> options, string key)
    {
        if (!options.TryGetValue(key, out var value) || string.IsNullOrWhiteSpace(value))
            throw new ArgumentException($"missing required option: {key}");
        return value;
    }
}

internal sealed record IlScanResult
{
    public string AssemblyName { get; init; } = "";
    public int TotalMethods { get; init; }
    public int MethodsWithBody { get; init; }
    public int MethodsWithoutBody { get; init; }
    public string[] UniqueOpcodes { get; init; } = [];
    public MethodIlScanResult[] Methods { get; init; } = [];
}

internal sealed record MethodIlScanResult
{
    public string MethodSubjectId { get; init; } = "";
    public bool HasBody { get; init; }
    public string[] IlOpcodes { get; init; } = [];
    public string[] UniqueOpcodes { get; init; } = [];
    public int ParameterCount { get; init; }
}
