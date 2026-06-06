using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Phase L2: On-demand IL body loading and AOT compilation for bridged methods.
/// After CollectBridgeImportThunks identifies methods that would go through the
/// interpreter, this resolver loads their IL bodies from the assembly DLLs and
/// attempts AOT compilation. Successfully compiled methods replace bridge thunks;
/// failures are preserved as bridge thunks (Phase L3 natural fallback).
/// </summary>
public sealed partial class NativeAotLoweringPlanner
{
    private sealed class BridgeMethodResolver
    {
        private readonly NativeAotLoweringPlanner _planner;
        private readonly string _sdkDir;
        private readonly string _projectRoot;

        // Cache: assembly name → DLL path
        private readonly Dictionary<string, string> _assemblyPathCache = new(StringComparer.Ordinal);

        public BridgeMethodResolver(NativeAotLoweringPlanner planner, string sdkDir, string projectRoot)
        {
            _planner = planner;
            _sdkDir = sdkDir;
            _projectRoot = projectRoot;
        }

        /// <summary>
        /// Resolve bridge thunks by loading IL bodies from assembly DLLs and
        /// attempting AOT compilation. Returns list of successfully compiled methods.
        /// </summary>
        public List<AotCoreIrMethodArtifact> ResolveBridgedMethods(
            Dictionary<string, BridgeImportThunkDefinition> bridgeThunks,
            List<AotCoreIrMethodArtifact> existingMethods)
        {
            var compiledMethods = new List<AotCoreIrMethodArtifact>();
            var existingSubjectIds = new HashSet<string>(
                existingMethods.Select(m => m.SubjectId ?? string.Empty),
                StringComparer.Ordinal);

            foreach (var (subjectId, thunk) in bridgeThunks)
            {
                if (string.IsNullOrEmpty(subjectId)) continue;
                if (existingSubjectIds.Contains(subjectId)) continue;

                try
                {
                    var compiledMethod = CompileMethod(subjectId);
                    if (compiledMethod != null)
                    {
                        compiledMethods.Add(compiledMethod);
                        Console.Error.WriteLine($"[BRIDGE-RESOLVE] Success: {subjectId}");
                    }
                    else
                    {
                        Console.Error.WriteLine($"[BRIDGE-RESOLVE] Skipped (cannot compile): {subjectId}");
                    }
                }
                catch (Exception ex)
                {
                    Console.Error.WriteLine($"[BRIDGE-RESOLVE] Failed: {subjectId} — {ex.Message}");
                    // L3 natural fallback: keep bridge thunk
                }
            }

            return compiledMethods;
        }

        private AotCoreIrMethodArtifact? CompileMethod(string subjectId)
        {
            // Extract assembly name from SubjectId prefix
            var assemblyName = ExtractAssemblyName(subjectId);
            if (assemblyName == null) return null;

            var dllPath = ResolveAssemblyPath(assemblyName);
            if (dllPath == null || !File.Exists(dllPath)) return null;

            using var stream = File.OpenRead(dllPath);
            using var peReader = new PEReader(stream);
            var metadataReader = peReader.GetMetadataReader();

            // TODO: Match SubjectId → MethodDefinitionHandle
            // TODO: Load IL body
            // TODO: Create AotCoreIrMethodArtifact

            // Stub: return null until full implementation
            return null;
        }

        private static string? ExtractAssemblyName(string subjectId)
        {
            var slashIndex = subjectId.IndexOf('/', StringComparison.Ordinal);
            return slashIndex > 0 ? subjectId[..slashIndex] : null;
        }

        private string? ResolveAssemblyPath(string assemblyName)
        {
            if (_assemblyPathCache.TryGetValue(assemblyName, out var cached))
                return cached;

            // Probe: .NET runtime shared framework first
            var runtimeDir = Path.GetDirectoryName(typeof(object).Assembly.Location);
            if (runtimeDir != null)
            {
                var candidate = Path.Combine(runtimeDir, $"{assemblyName}.dll");
                if (File.Exists(candidate))
                {
                    _assemblyPathCache[assemblyName] = candidate;
                    return candidate;
                }
            }

            // Probe: project root / testing / foundation-dll / ... / managed / subjects
            var subjectsDir = Path.Combine(_projectRoot, "testing", "foundation-dll");
            if (Directory.Exists(subjectsDir))
            {
                var files = Directory.GetFiles(subjectsDir, $"{assemblyName}.dll", SearchOption.AllDirectories);
                if (files.Length > 0)
                {
                    _assemblyPathCache[assemblyName] = files[0];
                    return files[0];
                }
            }

            return null;
        }
    }
}
