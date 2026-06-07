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
    private int _bridgeCounter;

    public BridgeAotCompiler(LinkedWorldModel linkedWorld, CodeRegistrationArtifact codeRegistration)
    {
        _linkedWorld = linkedWorld;
        _codeRegistration = codeRegistration;
        _existingSubjectIds = new HashSet<string>(
            linkedWorld.Methods.Select(m => m.SubjectId), StringComparer.Ordinal);
    }

    /// <summary>
    /// Compile bridged callees and integrate them into the AOT IR.
    /// Bridge methods get unique chaos_bridge_ symbols, their instructions are
    /// patched for Direct/ExternalRuntime dispatch, and they're added to
    /// aotCoreIr.Methods so the main emitter generates real function bodies.
    /// Returns the updated AotCoreIrArtifact and a redirect map.
    /// </summary>
    public (AotCoreIrArtifact UpdatedIr, Dictionary<string, string> RedirectMap) CompileAndIntegrate(
        AotCoreIrArtifact aotCoreIr)
    {
        // LCAC: BridgeAOT disabled
        return (aotCoreIr, new Dictionary<string, string>());
#pragma warning disable 0162
        var seen = new HashSet<string>(StringComparer.Ordinal);
        var redirectMap = new Dictionary<string, string>(StringComparer.Ordinal);
        var calleeToMethod = new Dictionary<string, AotCoreIrMethodArtifact>(StringComparer.Ordinal);
        int okCount = 0, failCount = 0;

        // Phase 1: Compile bridged callees — scan only ORIGINAL subject methods,
        // NOT bridge methods (which would transitively expand to thousands).
        var originalSubjectMethods = aotCoreIr.Methods.Where(m => !m.NativeSymbol.StartsWith("chaos_bridge_", StringComparison.Ordinal)).ToList();
        foreach (var method in originalSubjectMethods)
        {
            foreach (var instr in method.Instructions ?? [])
            {
                var callee = instr.Callee;
                if (string.IsNullOrEmpty(callee)) continue;
                if (_existingSubjectIds.Contains(callee)) continue;
                if (!seen.Add(callee)) continue;
                if (callee.Contains("<>c__DisplayClass") || callee.Contains("<>9__")) continue;
                if (callee.Contains("/<unknown>::", StringComparison.Ordinal) || callee.Contains("ThrowHelper::", StringComparison.Ordinal)) continue;

                try
                {
                    var cm = CompileSingleMethod(callee);
                    if (cm != null)
                    {
                        var bridgeSymbol = $"chaos_bridge_{_bridgeCounter++}_{ExtractMethodNameForSymbol(callee)}_{(uint)callee.GetHashCode():X4}";
                        // Clear shared generic flags: bridge methods are closed-form instantiations
                        // that don't need the runtime generic context parameter. Without this, the
                        // emitter generates dual declarations (with and without generic_context),
                        // causing C2733.
                        cm = cm with
                        {
                            NativeSymbol = bridgeSymbol,
                            OpenDefinitionSubjectId = null,
                            SharedGenericBodyId = null,
                        };
                        redirectMap[callee] = bridgeSymbol;
                        calleeToMethod[callee] = cm;
                        okCount++;
                    }
                    else { failCount++; }
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine($"[BRIDGE-AOT] FAIL: {callee} — {ex.Message}");
                    failCount++;
                }
            }
        }

        if (calleeToMethod.Count == 0)
        {
            Console.Error.WriteLine($"[BRIDGE-AOT] No methods compiled — {okCount} OK, {failCount} FAIL");
            return (aotCoreIr, redirectMap);
        }

        // Phase 2: Patch bridge methods' instructions
        // Bridge-to-bridge callee → Direct dispatch (chaos_bridge_ symbol)
        // Other callee → ExternalRuntime dispatch (resolved by planner's dispatch table)
        var compiledList = new List<AotCoreIrMethodArtifact>();
        foreach (var kvp in calleeToMethod)
        {
            var compiled = kvp.Value;
            var patchedInstrs = compiled.Instructions
                .Select(instr =>
                {
                    if (string.IsNullOrEmpty(instr.Callee))
                        return instr with { TargetReference = null };
                    if (redirectMap.TryGetValue(instr.Callee, out var targetSymbol))
                        return instr with { DispatchKindCode = HybridDispatchKind.Direct, TargetSymbol = targetSymbol };
                    return instr with { DispatchKindCode = HybridDispatchKind.ExternalRuntime };
                })
                .ToList();
            compiledList.Add(compiled with { Instructions = patchedInstrs });
        }

        // Phase 3: Add bridge methods to AOT IR (main emitter generates real bodies)
        var updatedMethods = aotCoreIr.Methods.Concat(compiledList).ToList();
        Console.Error.WriteLine($"[BRIDGE-AOT] Integrated: {okCount} OK, {failCount} FAIL — added {compiledList.Count} methods");
        return (aotCoreIr with { Methods = updatedMethods }, redirectMap);
    }

    private static string ExtractMethodNameForSymbol(string subjectId)
    {
        // SubjectId: "Assembly/Type::MethodName:ReturnType(Params)"
        var sep = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (sep < 0) return "unknown";
        var after = subjectId[(sep + 2)..];
        var colon = after.IndexOf(':', StringComparison.Ordinal);
        var paren = after.IndexOf('(', StringComparison.Ordinal);
        var end = colon >= 0 && colon < paren ? colon : (paren >= 0 ? paren : after.Length);
        if (end <= 0) return "unknown";
        var name = after[..end];
        return SanitizeForSymbol(name);
    }

    private static string SanitizeForSymbol(string input)
    {
        var sb = new System.Text.StringBuilder(input.Length);
        foreach (var c in input)
            sb.Append(char.IsLetterOrDigit(c) ? c : '_');
        return sb.ToString();
    }

    private AotCoreIrMethodArtifact? CompileSingleMethod(string subjectId)
    {
        var asmName = ExtractAssemblyName(subjectId);
        if (asmName == null) { Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — ExtractAssemblyName failed"); return null; }
        var dllPath = ResolveAssemblyPath(asmName);
        if (dllPath == null) { Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — DLL not found for assembly '{asmName}'"); return null; }

        using var stream = File.OpenRead(dllPath);
        using var peReader = new PEReader(stream);
        var meta = peReader.GetMetadataReader();

        var mh = FindMethodDefinition(meta, subjectId);
        // Fallback: some SubjectIds use the wrong assembly prefix (e.g. System.Collections
        // instead of System.Private.CoreLib for EqualityComparer<T>). Try corelib.
        if (mh == null && !string.Equals(asmName, "System.Private.CoreLib", StringComparison.Ordinal))
        {
            var corelibDll = ResolveAssemblyPath("System.Private.CoreLib");
            if (corelibDll != null && !string.Equals(corelibDll, dllPath, StringComparison.OrdinalIgnoreCase))
            {
                using var cs = File.OpenRead(corelibDll);
                using var cpr = new PEReader(cs);
                var cm = cpr.GetMetadataReader();
                mh = FindMethodDefinition(cm, subjectId);
                if (mh != null)
                {
                    try
                    {
                        var cmd = cm.GetMethodDefinition(mh.Value);
                        if (cmd.RelativeVirtualAddress != 0)
                        {
                            var cbody = cpr.GetMethodBody(cmd.RelativeVirtualAddress);
                            var cil = cbody.GetILBytes();
                            if (cil.Length > 0)
                                return CompileFromBody(cil, cm, subjectId, asmName, cmd, mh.Value);
                        }
                    }
                    catch
                    {
                        // Abstract/runtime method with no readable IL body
                        mh = null;
                    }
                }
            }
        }

        if (mh == null) { Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — FindMethodDefinition failed"); return null; }

        var md = meta.GetMethodDefinition(mh.Value);
        int rva;
        try
        {
            rva = md.RelativeVirtualAddress;
        }
        catch
        {
            // Handle invalid in main assembly — retry with corelib fallback
            Console.Error.WriteLine($"[BRIDGE-AOT] RETRY: {subjectId} — MethodDefinitionHandle invalid in {asmName}, retrying corelib");
            if (!string.Equals(asmName, "System.Private.CoreLib", StringComparison.Ordinal))
            {
                var corelibDll = ResolveAssemblyPath("System.Private.CoreLib");
                if (corelibDll != null && !string.Equals(corelibDll, dllPath, StringComparison.OrdinalIgnoreCase))
                {
                    using var cs = File.OpenRead(corelibDll);
                    using var cpr = new PEReader(cs);
                    var cm = cpr.GetMetadataReader();
                    var cmh = FindMethodDefinition(cm, subjectId);
                    if (cmh != null)
                    {
                        try
                        {
                            var cmd = cm.GetMethodDefinition(cmh.Value);
                            if (cmd.RelativeVirtualAddress != 0)
                            {
                                var cbody = cpr.GetMethodBody(cmd.RelativeVirtualAddress);
                                var cil = cbody.GetILBytes();
                                if (cil.Length > 0)
                                    return CompileFromBody(cil, cm, subjectId, asmName, cmd, cmh.Value);
                            }
                        }
                        catch { }
                    }
                }
            }
            Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — MethodDefinitionHandle invalid in assembly");
            return null;
        }
        if (rva == 0) { Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — RVA is 0 (no IL body)"); return null; }
        byte[] ilBytes;
        try
        {
            var body = peReader.GetMethodBody(rva);
            ilBytes = body.GetILBytes();
        }
        catch
        {
            Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — IL body unreadable at RVA 0x{rva:X}");
            return null;
        }

        return CompileFromBody(ilBytes, meta, subjectId, asmName, md, mh.Value);
    }

    private AotCoreIrMethodArtifact? CompileFromBody(byte[] ilBytes, MetadataReader meta,
        string subjectId, string asmName, MethodDefinition md, MethodDefinitionHandle mh)
    {
        var comp = ParseSubjectId(subjectId);
        if (comp == null) { Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — ParseSubjectId failed"); return null; }

        IReadOnlyList<ManagedInstructionModel> ilInstrs;
        try
        {
            ilInstrs = IlBytecodeDecoder.DecodeWithMetadata(ilBytes, meta, comp.AssemblyName);
        }
        catch
        {
            Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} — IL decode failed ({ilBytes.Length} bytes)");
            return null;
        }

        var sig = md.DecodeSignature(new SigTypeProvider(meta, comp.AssemblyName), default);
        var parameters = new List<ManagedParameterModel>();
        for (int pi = 0; pi < sig.ParameterTypes.Length; pi++)
        {
            parameters.Add(new ManagedParameterModel
            {
                Name = $"p{pi}",
                Type = sig.ParameterTypes[pi],
            });
        }

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
            MetadataToken = MetadataTokens.GetToken(mh),
            Parameters = parameters,
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
                    // Filter unresolvable Callee values from IlBytecodeDecoder — SubjectIds
                    // like '<22:0x...>' or 'Assembly/<unknown>::Method' would crash the
                    // emitter downstream (GetMethodDeclaringTypeSubjectId, ExternalRuntime stubs).
                    Callee = FilterCallee(i.Callee),
                    Reference = i.Reference,
                }).ToList(),
            }).ToList(),
        };
    }

    private static string? FilterCallee(string? callee)
    {
        if (string.IsNullOrEmpty(callee))
            return null;
        if (callee.StartsWith("<", StringComparison.Ordinal))
            return null;
        if (callee.Contains("/<unknown>::", StringComparison.Ordinal))
            return null;
        return callee;
    }

    private static IReadOnlyDictionary<string, GenericInstantiationDemandModel> BuildGenericDemandLookup(
        GenericInstantiationDemandGraphModel? graph)
    {
        var genericDemandLookup = new Dictionary<string, GenericInstantiationDemandModel>(StringComparer.Ordinal);
        if (graph?.Demands is not { Count: > 0 } demands)
            return genericDemandLookup;

        // Multiple assemblies may demand the same generic instantiation.
        // All entries for the same SubjectId produce the same
        // RuntimeGenericContextArtifact — keep the first.
        foreach (var demand in demands)
        {
            if (genericDemandLookup.ContainsKey(demand.SubjectId))
                continue;
            genericDemandLookup[demand.SubjectId] = demand;
        }

        return genericDemandLookup;
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
        if (comp == null) { Console.Error.WriteLine($"[BRIDGE-AOT] NULL: {subjectId} - ParseSubjectId failed"); return null; }
        var searchTypeName = comp.DeclaringTypeName;
        var genericBracketType = searchTypeName.IndexOf('<', StringComparison.Ordinal);
        if (genericBracketType > 0)
            searchTypeName = searchTypeName[..genericBracketType];
        MethodDefinitionHandle? fallback = null;
        foreach (var mh in reader.MethodDefinitions)
        {
            var md = reader.GetMethodDefinition(mh);
            var mdName = reader.GetString(md.Name);
            var searchName = comp.MethodName;
            var genericBracket = searchName.IndexOf('<', StringComparison.Ordinal);
            if (genericBracket > 0)
                searchName = searchName[..genericBracket];
            if (!string.Equals(mdName, searchName, StringComparison.Ordinal)) continue;
            var td = reader.GetTypeDefinition(md.GetDeclaringType());
            var ns = reader.GetString(td.Namespace);
            var tn = reader.GetString(td.Name);
            var backtick = tn.IndexOf('`', StringComparison.Ordinal);
            if (backtick > 0) tn = tn[..backtick];
            var ftn = string.IsNullOrEmpty(ns) ? tn : $"{ns}.{tn}";
            if (!string.Equals(ftn, searchTypeName, StringComparison.Ordinal) &&
                !string.Equals(tn, searchTypeName, StringComparison.Ordinal) &&
                !ftn.EndsWith("." + searchTypeName, StringComparison.Ordinal))
                continue;
            try
            {
                var sig = md.DecodeSignature(new SigTypeProvider(reader, comp.AssemblyName), default);
                if (sig.ParameterTypes.Length == comp.ParameterTypes.Count)
                {
                    if (md.RelativeVirtualAddress != 0)
                        return mh;
                    fallback ??= mh;
                    continue;
                }
                fallback ??= mh;
            }
            catch
            {
                // Signature decode failed (generic params like !!0) - skip this method
            }
        }
        return fallback;
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

        // Search runtime directory first
        var rd = Path.GetDirectoryName(typeof(object).Assembly.Location);
        if (rd != null)
        {
            var cand = Path.Combine(rd, $"{assemblyName}.dll");
            if (File.Exists(cand)) { _assemblyCache[assemblyName] = cand; return cand; }
        }

        // Search the subjects directory alongside CombinedSubjects.dll for test framework DLLs.
        var baseDir = AppContext.BaseDirectory;
        if (baseDir != null)
        {
            // Try multiple relative paths from the TPG output directory
            // (src/tools/Chaos.IL2CPP.Tools.TestProjectGenerator/bin/Debug/net8.0/)
            foreach (var relPath in new[] {
                Path.Combine("..", "..", "..", "..", "..", "..", "testing", "foundation-dll", "System.Text.Json", "chunks", "text-json", "managed", "subjects"),
                Path.Combine("..", "..", "..", "..", "..", "testing", "foundation-dll", "System.Text.Json", "chunks", "text-json", "managed", "subjects"),
            })
            {
                var fullDir = Path.GetFullPath(Path.Combine(baseDir, relPath));
                if (Directory.Exists(fullDir))
                {
                    var cand = Path.Combine(fullDir, $"{assemblyName}.dll");
                    if (File.Exists(cand)) { _assemblyCache[assemblyName] = cand; return cand; }
                }
            }
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
