using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed class BridgeAotCompiler
{
    private readonly LinkedWorldModel _linkedWorld;
    private readonly CodeRegistrationArtifact _codeRegistration;
    private readonly HashSet<string> _existingSubjectIds;
    private readonly Dictionary<string, string> _assemblyCache = new(StringComparer.Ordinal);

    public BridgeAotCompiler(LinkedWorldModel linkedWorld, CodeRegistrationArtifact codeRegistration)
    {
        _linkedWorld = linkedWorld;
        _codeRegistration = codeRegistration;
        _existingSubjectIds = new HashSet<string>(
            linkedWorld.Methods.Select(m => m.SubjectId), StringComparer.Ordinal);
    }

    public List<AotCoreIrMethodArtifact> CompileBridgedMethods(AotCoreIrArtifact aotCoreIr)
    {
        var compiled = new List<AotCoreIrMethodArtifact>();
        var seen = new HashSet<string>(StringComparer.Ordinal);

        foreach (var method in aotCoreIr.Methods)
        {
            foreach (var instr in method.Instructions ?? [])
            {
                var callee = instr.Callee;
                if (string.IsNullOrEmpty(callee)) continue;
                if (_existingSubjectIds.Contains(callee)) continue;
                if (!seen.Add(callee)) continue;
                if (callee.Contains("<>c__DisplayClass") || callee.Contains("<>9__")) continue;

                // Diagnostic: count ALL unique callees
                Console.Error.WriteLine($"[BRIDGE-AOT] SCANALL: {callee}");

                try
                {
                    var cm = CompileSingleMethod(callee);
                    if (cm != null) { compiled.Add(cm); Console.Error.WriteLine($"[BRIDGE-AOT] OK: {callee}"); }
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine($"[BRIDGE-AOT] FAIL: {callee} — {ex.Message}");
                }
            }
        }
        return compiled;
    }

    private AotCoreIrMethodArtifact? CompileSingleMethod(string subjectId)
    {
        var asmName = ExtractAssemblyName(subjectId);
        if (asmName == null) return null;
        var dllPath = ResolveAssemblyPath(asmName);
        if (dllPath == null) return null;

        using var stream = File.OpenRead(dllPath);
        using var peReader = new PEReader(stream);
        var meta = peReader.GetMetadataReader();

        var mh = FindMethodDefinition(meta, subjectId);
        if (mh == null) return null;

        var md = meta.GetMethodDefinition(mh.Value);
        var rva = md.RelativeVirtualAddress;
        if (rva == 0) return null;
        var body = peReader.GetMethodBody(rva);
        var ilBytes = body.GetILBytes();

        var comp = ParseSubjectId(subjectId);
        if (comp == null) return null;

        var ilInstrs = IlBytecodeDecoder.DecodeWithMetadata(ilBytes, meta, comp.AssemblyName);

        var mm = new ManagedMethodModel
        {
            AssemblyName = comp.AssemblyName,
            DeclaringTypeSubjectId = comp.DeclaringTypeSubjectId,
            DeclaringTypeDisplayName = comp.DeclaringTypeName,
            Name = comp.MethodName,
            ReturnType = comp.ReturnType,
            SubjectId = subjectId,
            DefinitionSubjectId = subjectId,
            Signature = $"({string.Join(",", comp.ParameterTypes)})",
            IsStatic = (md.Attributes & MethodAttributes.Static) != 0,
            IsVirtual = (md.Attributes & MethodAttributes.Virtual) != 0,
            MetadataToken = MetadataTokens.GetToken(mh.Value),
            Parameters = Array.Empty<ManagedParameterModel>(),
            Body = new ManagedMethodBodyModel
            {
                ExceptionRegions = Array.Empty<ManagedExceptionRegionModel>(),
                Blocks = new List<ManagedBlockModel>
                {
                    new() { BlockId = "0", Instructions = ilInstrs.ToList() },
                },
            },
        };

        var tm = ToTypedIl(mm);

        var targetSymbols = _codeRegistration.Modules
            .SelectMany(m => m.Registrations)
            .Where(r => string.Equals(r.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .GroupBy(r => r.SubjectId)
            .ToDictionary(g => g.Key, g => g.First().Symbol, StringComparer.Ordinal);

        var mTypes = _linkedWorld.Types.ToDictionary(t => t.SubjectId, StringComparer.Ordinal);
        var mFields = _linkedWorld.Fields.ToDictionary(f => f.SubjectId, StringComparer.Ordinal);
        var mMethods = _linkedWorld.Methods.ToDictionary(m => m.SubjectId, StringComparer.Ordinal);
        var genericDemand = BuildGenericDemandLookup(_linkedWorld.GenericInstantiationDemandGraph);

        var result = AotCoreIrLowering.TryCreateMethod(mm, tm, mTypes, mFields, mMethods, targetSymbols, genericDemand);
        if (result == null)
            Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — TryCreateMethod returned null");
        return result;
    }

    private static TypedIlMethodArtifact ToTypedIl(ManagedMethodModel method)
    {
        return new TypedIlMethodArtifact
        {
            MethodId = method.SubjectId,
            SubjectId = method.SubjectId,
            Signature = method.Signature,
            Identity = new ManagedMethodIdentityArtifact
            {
                AssemblyName = method.AssemblyName,
                DeclaringTypeSubjectId = method.DeclaringTypeSubjectId,
                DefinitionSubjectId = method.DefinitionSubjectId,
                SubjectId = method.SubjectId,
                MethodId = method.SubjectId,
                Signature = method.Signature,
            },
            MethodRole = "regular",
            BodyAvailability = "native-generated",
            BodyAvailabilityCode = BodyAvailabilityCode.NativeGenerated,
            Capabilities = Array.Empty<string>(),
            Parameters = method.Parameters.Select(p => new TypedIlParameterArtifact
            {
                Name = p.Name,
                Type = p.Name ?? "System.Object",
            }).ToList(),
            Blocks = method.Body.Blocks.Select(b => new TypedIlBlockArtifact
            {
                BlockId = b.BlockId,
                Instructions = b.Instructions.Select(i => new TypedIlInstructionArtifact
                {
                    Op = i.Op,
                    Operand = i.Operand,
                }).ToList(),
            }).ToList(),
        };
    }

    private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(
        GenericInstantiationDemandGraphModel? graph)
    {
        if (graph?.Demands == null) return new Dictionary<string, GenericInstantiationDemandModel>();
        return graph.Demands.ToDictionary(d => d.SubjectId, d => d, StringComparer.Ordinal);
    }

    // ── SubjectId parsing ─────────────────────────────────────
    private sealed record SubjectIdComponents(
        string AssemblyName, string DeclaringTypeSubjectId, string DeclaringTypeName,
        string MethodName, string ReturnType, List<string> ParameterTypes);

    private static SubjectIdComponents? ParseSubjectId(string subjectId)
    {
        var slash = subjectId.IndexOf('/', StringComparison.Ordinal);
        if (slash <= 0) return null;
        var asm = subjectId[..slash];
        var rest = subjectId[(slash + 1)..];
        var sep = rest.IndexOf("::", StringComparison.Ordinal);
        if (sep < 0) return null;
        var type = rest[..sep];
        var after = rest[(sep + 2)..];
        var paren = after.IndexOf('(', StringComparison.Ordinal);
        if (paren < 0) return null;
        var colon = after.IndexOf(':', StringComparison.Ordinal);
        var mEnd = colon >= 0 && colon < paren ? colon : paren;
        var mName = after[..mEnd];
        // Extract return type if present (MethodName:ReturnType format)
        var retType = "System.Void";
        if (mEnd < paren && after[mEnd] == ':')
            retType = after[(mEnd + 1)..paren];
        var pPart = after[(paren + 1)..^1];
        var pTypes = string.IsNullOrWhiteSpace(pPart) ? new List<string>() : pPart.Split(',').Select(p => p.Trim()).ToList();
        return new SubjectIdComponents(asm, $"{asm}/{type}", type, mName, retType, pTypes);
    }

    // ── Method definition matching ────────────────────────────
    private static MethodDefinitionHandle? FindMethodDefinition(MetadataReader reader, string subjectId)
    {
        var comp = ParseSubjectId(subjectId);
        if (comp == null) return null;
        foreach (var mh in reader.MethodDefinitions)
        {
            var md = reader.GetMethodDefinition(mh);
            if (!string.Equals(reader.GetString(md.Name), comp.MethodName, StringComparison.Ordinal)) continue;
            var td = reader.GetTypeDefinition(md.GetDeclaringType());
            var ns = reader.GetString(td.Namespace);
            var tn = reader.GetString(td.Name);
            var ftn = string.IsNullOrEmpty(ns) ? tn : $"{ns}.{tn}";
            if (!string.Equals(ftn, comp.DeclaringTypeName, StringComparison.Ordinal)) continue;
            var sig = md.DecodeSignature(new SigTypeProvider(reader, comp.AssemblyName), default);
            if (sig.ParameterTypes.Length != comp.ParameterTypes.Count) continue;
            return mh;
        }
        return null;
    }

    // ── Assembly resolution ──────────────────────────────────
    private static string? ExtractAssemblyName(string subjectId)
    {
        var slash = subjectId.IndexOf('/', StringComparison.Ordinal);
        return slash > 0 ? subjectId[..slash] : null;
    }

    private string? ResolveAssemblyPath(string assemblyName)
    {
        if (_assemblyCache.TryGetValue(assemblyName, out var c)) return c;
        var rd = Path.GetDirectoryName(typeof(object).Assembly.Location);
        if (rd != null)
        {
            var cand = Path.Combine(rd, $"{assemblyName}.dll");
            if (File.Exists(cand)) { _assemblyCache[assemblyName] = cand; return cand; }
        }
        return null;
    }

    // ── Minimal signature type provider ──────────────────────
    private sealed class SigTypeProvider : ISignatureTypeProvider<string, object?>
    {
        private readonly MetadataReader _r;
        private readonly string _a;
        public SigTypeProvider(MetadataReader r, string a) { _r = r; _a = a; }
        public string GetPrimitiveType(PrimitiveTypeCode t) => t.ToString();
        public string GetTypeFromDefinition(MetadataReader r, TypeDefinitionHandle h, byte k)
        {
            var t = r.GetTypeDefinition(h); var ns = r.GetString(t.Namespace); var n = r.GetString(t.Name);
            return string.IsNullOrEmpty(ns) ? n : $"{ns}.{n}";
        }
        public string GetTypeFromReference(MetadataReader r, TypeReferenceHandle h, byte k)
        {
            var t = r.GetTypeReference(h); var ns = r.GetString(t.Namespace); var n = r.GetString(t.Name);
            return string.IsNullOrEmpty(ns) ? n : $"{ns}.{n}";
        }
        public string GetSZArrayType(string e) => $"{e}[]";
        public string GetArrayType(string e, ArrayShape s) => $"{e}[{new string(',', s.Rank - 1)}]";
        public string GetByReferenceType(string e) => $"{e}&";
        public string GetPointerType(string e) => $"{e}*";
        public string GetPinnedType(string e) => e;
        public string GetGenericInstantiation(string g, ImmutableArray<string> a) => g;
        public string GetGenericMethodParameter(object? c, int i) => $"!!{i}";
        public string GetGenericTypeParameter(object? c, int i) => $"!{i}";
        public string GetFunctionPointerType(MethodSignature<string> s) => "fnptr";
        public string GetModifiedType(string m, string u, bool r) => u;
        public string GetTypeFromSpecification(MetadataReader r, object? c, TypeSpecificationHandle h, byte k) => "spec";
    }
}
