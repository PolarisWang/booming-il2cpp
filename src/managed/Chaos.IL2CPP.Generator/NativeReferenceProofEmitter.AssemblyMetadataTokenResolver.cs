// Auto-generated partial: NativeReferenceProofEmitter.AssemblyMetadataTokenResolver
using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private sealed class AssemblyMetadataTokenResolver
    {
        private readonly IReadOnlyDictionary<string, string> assemblyPathsByName;
        private readonly Dictionary<string, AssemblyMetadataCache> cacheByAssemblyName = new(StringComparer.Ordinal);
        private readonly object cacheGate = new();

        public AssemblyMetadataTokenResolver(IReadOnlyDictionary<string, string> assemblyPathsByName)
        {
            this.assemblyPathsByName = assemblyPathsByName;
        }

        public bool TryResolveMethodAndDeclaringTypeToken(
            string assemblyName,
            string methodSubjectId,
            out ExternalMetadataTokenResolution resolution)
        {
            resolution = null!;
            if (!assemblyPathsByName.TryGetValue(assemblyName, out var assemblyPath))
            {
                return false;
            }

            AssemblyMetadataCache cache;
            lock (cacheGate)
            {
                if (!cacheByAssemblyName.TryGetValue(assemblyName, out cache!))
                {
                    if (!TryCreateAssemblyMetadataCache(assemblyName, assemblyPath, out cache))
                    {
                        return false;
                    }

                    cacheByAssemblyName.Add(assemblyName, cache);
                }
            }

            return cache.TryResolveMethodAndDeclaringTypeToken(methodSubjectId, out resolution);
        }
    }

}
