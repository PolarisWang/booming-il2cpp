// Auto-generated partial: MetadataRegistrationLookup
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
    private sealed class MetadataRegistrationLookup
    {
        private static readonly IReadOnlyList<MetadataRegistrationEntry> EmptyRegistrations = [];
        private readonly Dictionary<string, List<MetadataRegistrationEntry>> registrationsByKind;
        private readonly Dictionary<string, Dictionary<string, MetadataRegistrationEntry>> registrationsByKindAndSubjectId;
        private readonly Dictionary<string, Dictionary<string, uint>> metadataTokensByKindAndSubjectId;
        private readonly HashSet<string> concreteTypeSubjectIds;

        public MetadataRegistrationLookup(IReadOnlyList<MetadataRegistrationEntry> registrations)
        {
            registrationsByKind = new Dictionary<string, List<MetadataRegistrationEntry>>(StringComparer.Ordinal);
            registrationsByKindAndSubjectId = new Dictionary<string, Dictionary<string, MetadataRegistrationEntry>>(StringComparer.Ordinal);
            metadataTokensByKindAndSubjectId = new Dictionary<string, Dictionary<string, uint>>(StringComparer.Ordinal);
            concreteTypeSubjectIds = new HashSet<string>(StringComparer.Ordinal);

            foreach (var registration in registrations)
            {
                if (!registrationsByKind.TryGetValue(registration.RegistrationKind, out var registrationsForKind))
                {
                    registrationsForKind = [];
                    registrationsByKind.Add(registration.RegistrationKind, registrationsForKind);
                }

                registrationsForKind.Add(registration);

                if (!registrationsByKindAndSubjectId.TryGetValue(registration.RegistrationKind, out var registrationsForKindBySubjectId))
                {
                    registrationsForKindBySubjectId = new Dictionary<string, MetadataRegistrationEntry>(StringComparer.Ordinal);
                    registrationsByKindAndSubjectId.Add(registration.RegistrationKind, registrationsForKindBySubjectId);
                }

                registrationsForKindBySubjectId[registration.SubjectId] = registration;

                if (string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal))
                {
                    concreteTypeSubjectIds.Add(registration.SubjectId);
                }

                if (!TryGetMetadataTokenPrefix(registration.RegistrationKind, out var tokenPrefix))
                {
                    continue;
                }

                if (!metadataTokensByKindAndSubjectId.TryGetValue(registration.RegistrationKind, out var tokensForKind))
                {
                    tokensForKind = new Dictionary<string, uint>(StringComparer.Ordinal);
                    metadataTokensByKindAndSubjectId.Add(registration.RegistrationKind, tokensForKind);
                }

                tokensForKind[registration.SubjectId] = tokenPrefix | (uint)registration.Slot;
            }
        }

        public IReadOnlyList<MetadataRegistrationEntry> GetRegistrationsForKind(string registrationKind)
        {
            return registrationsByKind.TryGetValue(registrationKind, out var registrationsForKind)
                ? registrationsForKind
                : EmptyRegistrations;
        }

        public bool TryGetRegistration(
            string registrationKind,
            string subjectId,
            out MetadataRegistrationEntry registration)
        {
            if (registrationsByKindAndSubjectId.TryGetValue(registrationKind, out var registrationsForKindBySubjectId) &&
                registrationsForKindBySubjectId.TryGetValue(subjectId, out var resolvedRegistration))
            {
                registration = resolvedRegistration;
                return true;
            }

            registration = default!;
            return false;
        }

        public uint GetRequiredToken(
            string registrationKind,
            string subjectId)
        {
            if (metadataTokensByKindAndSubjectId.TryGetValue(registrationKind, out var tokensForKind) &&
                tokensForKind.TryGetValue(subjectId, out var token))
            {
                return token;
            }

            throw new InvalidOperationException(
                $"missing required '{registrationKind}' metadata registration for '{subjectId}'");
        }

        public bool HasConcreteType(string subjectId)
        {
            return concreteTypeSubjectIds.Contains(subjectId);
        }
    }
}
