// Auto-generated partial: NativeReferenceProofEmitter.AssemblyMetadataCache
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
    private sealed class AssemblyMetadataCache
    {
        private readonly string assemblyName;
        private readonly FileStream metadataStream;
        private readonly PEReader peReader;
        private readonly MetadataReader metadataReader;
        private readonly Dictionary<string, TypeDefinitionHandle> typeDefinitionHandlesBySubjectId;
        private readonly Dictionary<string, ExternalMetadataTokenResolution> resolutionsByMethodSubjectId =
            new(StringComparer.Ordinal);
        private readonly object resolutionGate = new();

        public AssemblyMetadataCache(
            string assemblyName,
            FileStream metadataStream,
            PEReader peReader,
            MetadataReader metadataReader,
            Dictionary<string, TypeDefinitionHandle> typeDefinitionHandlesBySubjectId)
        {
            this.assemblyName = assemblyName;
            this.metadataStream = metadataStream;
            this.peReader = peReader;
            this.metadataReader = metadataReader;
            this.typeDefinitionHandlesBySubjectId = typeDefinitionHandlesBySubjectId;
        }

        public bool TryResolveMethodAndDeclaringTypeToken(
            string methodSubjectId,
            out ExternalMetadataTokenResolution resolution)
        {
            lock (resolutionGate)
            {
                if (resolutionsByMethodSubjectId.TryGetValue(methodSubjectId, out resolution!))
                {
                    return true;
                }

                var declaringTypeSubjectId = GetDeclaringTypeSubjectId(methodSubjectId);
                if (!typeDefinitionHandlesBySubjectId.TryGetValue(declaringTypeSubjectId, out var typeDefinitionHandle))
                {
                    resolution = null!;
                    return false;
                }

                var metadataMethodName = GetMetadataMethodName(methodSubjectId);
                var parameterCount = GetMethodParameterTypesFromSubjectId(methodSubjectId).Count;
                var typeDefinition = metadataReader.GetTypeDefinition(typeDefinitionHandle);
                foreach (var candidateHandle in typeDefinition.GetMethods())
                {
                    var candidateDefinition = metadataReader.GetMethodDefinition(candidateHandle);
                    if (!string.Equals(
                            metadataReader.GetString(candidateDefinition.Name),
                            metadataMethodName,
                            StringComparison.Ordinal) ||
                        GetMethodParameterCount(metadataReader, candidateDefinition) != parameterCount)
                    {
                        continue;
                    }

                    resolution = new ExternalMetadataTokenResolution(
                        assemblyName,
                        unchecked((uint)MetadataTokens.GetToken(typeDefinitionHandle)),
                        unchecked((uint)MetadataTokens.GetToken(candidateHandle)));
                    resolutionsByMethodSubjectId[methodSubjectId] = resolution;
                    return true;
                }

                resolution = null!;
                return false;
            }
        }
    }

    private sealed record RuntimeSkeletonStubBuildContext(
        NativeReferenceLoweringPlanArtifact LoweringPlan,
        ManagedClosureManifestArtifact ClosureManifest,
        AssemblyMetadataTokenResolver ExternalMetadataTokenResolver,
        MetadataRegistrationArtifact MetadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> MethodPointers,
        IReadOnlyList<TypedIlMethodArtifact> Methods,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> MethodsBySubjectId,
        IReadOnlyDictionary<string, string> MethodStubNamesBySubjectId,
        RuntimeSkeletonPageSupportBuilder PageSupportBuilder,
        string StubName,
        string SubjectId);

}
