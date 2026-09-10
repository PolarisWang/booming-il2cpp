from __future__ import annotations

from tests._support.module_loading import load_module


from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[5]
SYNTHESIS_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "family_synthesis.py"
MATCHING_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "family_matching.py"
REGISTRY_MODULE_PATH = REPO_ROOT / "build" / "toolchains" / "run" / "testing" / "foundation_dll" / "rule_registry.py"


def test_rule_registry_contains_current_synthesis_trials() -> None:
    registry_module = load_module(REGISTRY_MODULE_PATH, "chaos_foundation_dll_rule_registry")

    expected_assemblies = {
        "System.Private.CoreLib",
        "System.Collections.Immutable",
        "System.Formats.Asn1",
        "System.IO.Compression.Brotli",
        "System.IO.Compression.ZipFile",
        "System.IO.Pipelines",
        "System.Linq",
        "System.Net.ServerSentEvents",
        "System.ObjectModel",
        "System.Runtime.InteropServices",
        "System.Runtime.Serialization.Formatters",
        "System.Security.Principal.Windows",
        "System.Text.Json",
        "System.Threading.Tasks.Parallel",
    }
    assert expected_assemblies.issubset(set(registry_module.RULE_REGISTRY.keys()))
    assert registry_module.RULE_REGISTRY["System.Text.Json"]["mode"] == "marker-rules"
    assert registry_module.RULE_REGISTRY["System.Runtime.InteropServices"]["mode"] == "marker-rules"
    assert registry_module.RULE_REGISTRY["System.Linq"]["mode"] == "anchor-bundle"
    assert registry_module.RULE_REGISTRY["System.Private.CoreLib"]["mode"] == "defer-to-ledger"


def test_synthesize_text_json_families_from_surface_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis")

    surface_types = {
        "System.Text.Json.JsonDocument": {},
        "System.Text.Json.JsonException": {},
        "System.Text.Json.JsonNamingPolicy": {},
        "System.Text.Json.JsonSerializer": {},
        "System.Text.Json.JsonSerializerOptions": {},
        "System.Text.Json.Utf8JsonWriter": {},
        "System.Text.Json.Serialization.JsonConverter": {},
        "System.Text.Json.Serialization.JsonConverterFactory": {},
        "System.Text.Json.Serialization.JsonAttribute": {},
        "System.Text.Json.Serialization.JsonSerializerContext": {},
        "System.Text.Json.Serialization.Metadata.JsonTypeInfo": {},
        "System.Text.Json.Nodes.JsonObject": {},
        "System.Text.Json.Schema.JsonSchemaExporter": {},
        "System.Text.Json.Serialization.ReferenceHandler": {},
    }

    result = synthesis_module.synthesize_families_for_assembly("System.Text.Json", surface_types)
    family_ids = [item["familyId"] for item in result]

    assert "family/System.Text.Json/document-element" in family_ids
    assert "family/System.Text.Json/serializer" in family_ids
    assert "family/System.Text.Json/serializer-options" in family_ids
    assert "family/System.Text.Json/writer" in family_ids
    assert "family/System.Text.Json/converters" in family_ids
    assert "family/System.Text.Json/converter-factory" in family_ids
    assert "family/System.Text.Json/attributes" in family_ids
    assert "family/System.Text.Json/source-generation-context" in family_ids
    assert "family/System.Text.Json/metadata-typeinfo" in family_ids
    assert "family/System.Text.Json/nodes" in family_ids
    assert "family/System.Text.Json/schema" in family_ids
    assert "family/System.Text.Json/polymorphism-reference" in family_ids


def test_match_synthesized_families_against_existing_confirmed_text_json_ledger() -> None:
    matching_module = load_module(MATCHING_MODULE_PATH, "chaos_foundation_dll_family_matching")

    synthesized = [
        {"familyId": "family/System.Text.Json/serializer", "displayName": "JSON Serializer"},
        {"familyId": "family/System.Text.Json/writer", "displayName": "JSON Writer"},
        {"familyId": "family/System.Text.Json/nodes", "displayName": "JSON Nodes"},
    ]
    existing = [
        {"familyId": "family/System.Text.Json/serializer", "displayName": "JSON Serializer"},
        {"familyId": "family/System.Text.Json/writer", "displayName": "JSON Writer"},
        {"familyId": "family/System.Text.Json/schema", "displayName": "JSON Schema"},
    ]

    result = matching_module.match_family_candidates(synthesized, existing)

    assert result["matchedFamilyIds"] == [
        "family/System.Text.Json/serializer",
        "family/System.Text.Json/writer",
    ]
    assert result["newCandidateFamilyIds"] == ["family/System.Text.Json/nodes"]
    assert result["orphanConfirmedFamilyIds"] == ["family/System.Text.Json/schema"]


def test_synthesize_runtime_interopservices_families_from_surface_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis_interop")

    surface_types = {
        "System.Runtime.InteropServices.Marshal": {},
        "System.Runtime.InteropServices.StructLayoutAttribute": {},
        "System.Runtime.InteropServices.MarshalAsAttribute": {},
        "System.Runtime.InteropServices.DllImportAttribute": {},
        "System.Runtime.InteropServices.LibraryImportAttribute": {},
        "System.Runtime.InteropServices.SafeHandle": {},
        "System.Runtime.InteropServices.CriticalHandle": {},
        "System.Runtime.InteropServices.GCHandle": {},
        "System.Runtime.InteropServices.ComWrappers": {},
        "System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute": {},
        "System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute": {},
        "System.Runtime.InteropServices.ComTypes.IStream": {},
        "System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal": {},
        "System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute": {},
        "System.Runtime.InteropServices.SuppressGCTransitionAttribute": {},
        "System.Runtime.InteropServices.COMException": {},
        "System.Runtime.InteropServices.ExternalException": {},
        "System.Security.SecureStringMarshal": {},
        "System.Runtime.InteropServices.RuntimeInformation": {},
    }

    result = synthesis_module.synthesize_families_for_assembly("System.Runtime.InteropServices", surface_types)
    family_ids = [item["familyId"] for item in result]

    assert "family/System.Runtime.InteropServices/marshalling-attributes" in family_ids
    assert "family/System.Runtime.InteropServices/pinvoke-dllimport" in family_ids
    assert "family/System.Runtime.InteropServices/native-memory-pointers" in family_ids
    assert "family/System.Runtime.InteropServices/handles-safehandle-gchandle" in family_ids
    assert "family/System.Runtime.InteropServices/com-types" in family_ids
    assert "family/System.Runtime.InteropServices/com-wrappers" in family_ids
    assert "family/System.Runtime.InteropServices/generated-marshalling" in family_ids
    assert "family/System.Runtime.InteropServices/function-pointers" in family_ids
    assert "family/System.Runtime.InteropServices/unmanaged-calling-conventions" in family_ids
    assert "family/System.Runtime.InteropServices/objective-c-interop" in family_ids
    assert "family/System.Runtime.InteropServices/exception-errors" in family_ids
    assert "family/System.Runtime.InteropServices/secure-string-marshal" in family_ids
    assert "family/System.Runtime.InteropServices/runtime-interop-services" in family_ids


def test_match_synthesized_families_against_existing_confirmed_interop_ledger() -> None:
    matching_module = load_module(MATCHING_MODULE_PATH, "chaos_foundation_dll_family_matching_interop")

    synthesized = [
        {"familyId": "family/System.Runtime.InteropServices/com-wrappers"},
        {"familyId": "family/System.Runtime.InteropServices/com-types"},
        {"familyId": "family/System.Runtime.InteropServices/generated-marshalling"},
    ]
    existing = [
        {"familyId": "family/System.Runtime.InteropServices/com-wrappers"},
        {"familyId": "family/System.Runtime.InteropServices/generated-marshalling"},
        {"familyId": "family/System.Runtime.InteropServices/runtime-interop-services"},
    ]

    result = matching_module.match_family_candidates(synthesized, existing)

    assert result["matchedFamilyIds"] == [
        "family/System.Runtime.InteropServices/com-wrappers",
        "family/System.Runtime.InteropServices/generated-marshalling",
    ]
    assert result["newCandidateFamilyIds"] == ["family/System.Runtime.InteropServices/com-types"]
    assert result["orphanConfirmedFamilyIds"] == ["family/System.Runtime.InteropServices/runtime-interop-services"]


def test_synthesize_linq_families_from_enumerable_anchor_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis_linq")

    surface_types = {
        "System.Linq.Enumerable": {},
        "System.Linq.IGrouping": {},
        "System.Linq.ILookup": {},
        "System.Linq.IOrderedEnumerable": {},
        "System.Linq.Lookup": {},
    }

    result = synthesis_module.synthesize_families_for_assembly("System.Linq", surface_types)
    family_ids = [item["familyId"] for item in result]

    assert "family/System.Linq/filtering" in family_ids
    assert "family/System.Linq/projection" in family_ids
    assert "family/System.Linq/partitioning" in family_ids
    assert "family/System.Linq/ordering" in family_ids
    assert "family/System.Linq/grouping-lookup" in family_ids
    assert "family/System.Linq/joins" in family_ids
    assert "family/System.Linq/set-operations" in family_ids
    assert "family/System.Linq/aggregation" in family_ids
    assert "family/System.Linq/numeric-aggregation" in family_ids
    assert "family/System.Linq/materialization" in family_ids
    assert "family/System.Linq/generation-concat" in family_ids
    assert "family/System.Linq/shuffle-index" in family_ids


def test_match_synthesized_families_against_existing_confirmed_linq_ledger() -> None:
    matching_module = load_module(MATCHING_MODULE_PATH, "chaos_foundation_dll_family_matching_linq")

    synthesized = [
        {"familyId": "family/System.Linq/filtering"},
        {"familyId": "family/System.Linq/projection"},
        {"familyId": "family/System.Linq/materialization"},
    ]
    existing = [
        {"familyId": "family/System.Linq/filtering"},
        {"familyId": "family/System.Linq/projection"},
        {"familyId": "family/System.Linq/ordering"},
    ]

    result = matching_module.match_family_candidates(synthesized, existing)

    assert result["matchedFamilyIds"] == [
        "family/System.Linq/filtering",
        "family/System.Linq/projection",
    ]
    assert result["newCandidateFamilyIds"] == ["family/System.Linq/materialization"]
    assert result["orphanConfirmedFamilyIds"] == ["family/System.Linq/ordering"]


def test_synthesize_remaining_dll_families_from_marker_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis_remaining")

    cases = {
        "System.Collections.Immutable": (
            {
                "System.Collections.Immutable.ImmutableArray": {},
                "System.Collections.Immutable.ImmutableList": {},
                "System.Collections.Immutable.ImmutableDictionary": {},
                "System.Collections.Immutable.ImmutableSortedDictionary": {},
                "System.Collections.Immutable.ImmutableHashSet": {},
                "System.Collections.Immutable.ImmutableSortedSet": {},
                "System.Collections.Immutable.ImmutableQueue": {},
                "System.Collections.Immutable.ImmutableInterlocked": {},
                "System.Collections.Frozen.FrozenDictionary": {},
                "System.Linq.ImmutableArrayExtensions": {},
            },
            {
                "family/System.Collections.Immutable/immutable-array",
                "family/System.Collections.Immutable/immutable-list",
                "family/System.Collections.Immutable/immutable-dictionary",
                "family/System.Collections.Immutable/immutable-sorted-dictionary",
                "family/System.Collections.Immutable/immutable-hash-set",
                "family/System.Collections.Immutable/immutable-sorted-set",
                "family/System.Collections.Immutable/immutable-queue-stack",
                "family/System.Collections.Immutable/immutable-interlocked",
                "family/System.Collections.Immutable/frozen-collections",
                "family/System.Collections.Immutable/extensions-marshal",
            },
        ),
        "System.IO.Compression.Brotli": (
            {
                "System.IO.Compression.BrotliStream": {},
                "System.IO.Compression.BrotliEncoder": {},
                "System.IO.Compression.BrotliDecoder": {},
                "System.IO.Compression.BrotliCompressionOptions": {},
            },
            {
                "family/System.IO.Compression.Brotli/stream",
                "family/System.IO.Compression.Brotli/encoder",
                "family/System.IO.Compression.Brotli/decoder",
                "family/System.IO.Compression.Brotli/options",
            },
        ),
        "System.IO.Compression.ZipFile": (
            {
                "System.IO.Compression.ZipFile": {},
                "System.IO.Compression.ZipFileExtensions": {},
            },
            {
                "family/System.IO.Compression.ZipFile/zip-file-apis",
                "family/System.IO.Compression.ZipFile/zip-file-extensions",
            },
        ),
        "System.Net.ServerSentEvents": (
            {
                "System.Net.ServerSentEvents.SseItem": {},
                "System.Net.ServerSentEvents.SseParser": {},
                "System.Net.ServerSentEvents.SseItemParser": {},
                "System.Net.ServerSentEvents.SseFormatter": {},
            },
            {
                "family/System.Net.ServerSentEvents/sse-item",
                "family/System.Net.ServerSentEvents/parser",
                "family/System.Net.ServerSentEvents/item-parser",
                "family/System.Net.ServerSentEvents/formatter",
            },
        ),
        "System.Runtime.Serialization.Formatters": (
            {
                "System.Runtime.Serialization.Formatter": {},
                "System.Runtime.Serialization.Formatters.Binary.BinaryFormatter": {},
                "System.Runtime.Serialization.SerializationBinder": {},
                "System.Runtime.Serialization.SurrogateSelector": {},
                "System.Runtime.Serialization.ObjectManager": {},
                "System.Runtime.Serialization.FormatterServices": {},
            },
            {
                "family/System.Runtime.Serialization.Formatters/formatter-base",
                "family/System.Runtime.Serialization.Formatters/binary-formatter",
                "family/System.Runtime.Serialization.Formatters/binder-type-style",
                "family/System.Runtime.Serialization.Formatters/surrogate-selection",
                "family/System.Runtime.Serialization.Formatters/object-manager-id-generator",
                "family/System.Runtime.Serialization.Formatters/formatter-services-converter",
            },
        ),
        "System.Security.Principal.Windows": (
            {
                "System.Security.Principal.IdentityReference": {},
                "System.Security.Principal.NTAccount": {},
                "System.Security.Principal.SecurityIdentifier": {},
                "Microsoft.Win32.SafeHandles.SafeAccessTokenHandle": {},
                "System.Security.Principal.WindowsIdentity": {},
                "System.Security.Principal.WindowsPrincipal": {},
            },
            {
                "family/System.Security.Principal.Windows/identity-reference",
                "family/System.Security.Principal.Windows/nt-account",
                "family/System.Security.Principal.Windows/security-identifier",
                "family/System.Security.Principal.Windows/token-access",
                "family/System.Security.Principal.Windows/windows-identity",
                "family/System.Security.Principal.Windows/windows-principal-roles",
            },
        ),
        "System.Threading.Tasks.Parallel": (
            {
                "System.Threading.Tasks.Parallel": {},
                "System.Threading.Tasks.ParallelOptions": {},
                "System.Threading.Tasks.ParallelLoopState": {},
                "System.Threading.Tasks.ParallelLoopResult": {},
            },
            {
                "family/System.Threading.Tasks.Parallel/parallel-apis",
                "family/System.Threading.Tasks.Parallel/parallel-options",
                "family/System.Threading.Tasks.Parallel/loop-state",
                "family/System.Threading.Tasks.Parallel/loop-result",
            },
        ),
    }

    for assembly_name, (surface_types, expected_family_ids) in cases.items():
        result = synthesis_module.synthesize_families_for_assembly(assembly_name, surface_types)
        assert expected_family_ids.issubset({item["familyId"] for item in result}), assembly_name


def test_synthesize_formats_asn1_families_from_surface_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis_asn1")

    surface_types = {
        "System.Formats.Asn1.Asn1Tag": {},
        "System.Formats.Asn1.AsnEncodingRules": {},
        "System.Formats.Asn1.AsnReader": {},
        "System.Formats.Asn1.AsnWriter": {},
        "System.Formats.Asn1.AsnDecoder": {},
        "System.Formats.Asn1.AsnReaderOptions": {},
        "System.Formats.Asn1.AsnContentException": {},
    }

    result = synthesis_module.synthesize_families_for_assembly("System.Formats.Asn1", surface_types)
    family_ids = [item["familyId"] for item in result]

    assert "family/System.Formats.Asn1/tag-model" in family_ids
    assert "family/System.Formats.Asn1/encoding-rules" in family_ids
    assert "family/System.Formats.Asn1/reader" in family_ids
    assert "family/System.Formats.Asn1/writer" in family_ids
    assert "family/System.Formats.Asn1/decoder" in family_ids
    assert "family/System.Formats.Asn1/options-errors" in family_ids


def test_synthesize_io_pipelines_families_from_surface_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis_pipelines")

    surface_types = {
        "System.IO.Pipelines.Pipe": {},
        "System.IO.Pipelines.PipeReader": {},
        "System.IO.Pipelines.PipeWriter": {},
        "System.IO.Pipelines.ReadResult": {},
        "System.IO.Pipelines.FlushResult": {},
        "System.IO.Pipelines.PipeOptions": {},
        "System.IO.Pipelines.PipeScheduler": {},
        "System.IO.Pipelines.StreamPipeExtensions": {},
    }

    result = synthesis_module.synthesize_families_for_assembly("System.IO.Pipelines", surface_types)
    family_ids = [item["familyId"] for item in result]

    assert "family/System.IO.Pipelines/pipe-core" in family_ids
    assert "family/System.IO.Pipelines/reader-writer" in family_ids
    assert "family/System.IO.Pipelines/read-flush-results" in family_ids
    assert "family/System.IO.Pipelines/options-scheduler" in family_ids
    assert "family/System.IO.Pipelines/stream-adapters" in family_ids


def test_synthesize_objectmodel_families_from_surface_types() -> None:
    synthesis_module = load_module(SYNTHESIS_MODULE_PATH, "chaos_foundation_dll_family_synthesis_objectmodel")

    surface_types = {
        "System.Collections.ObjectModel.KeyedCollection": {},
        "System.Collections.ObjectModel.ObservableCollection": {},
        "System.Collections.Specialized.INotifyCollectionChanged": {},
        "System.ComponentModel.INotifyPropertyChanged": {},
        "System.ComponentModel.INotifyDataErrorInfo": {},
        "System.Windows.Input.ICommand": {},
        "System.ComponentModel.TypeConverterAttribute": {},
    }

    result = synthesis_module.synthesize_families_for_assembly("System.ObjectModel", surface_types)
    family_ids = [item["familyId"] for item in result]

    assert "family/System.ObjectModel/object-model-collections" in family_ids
    assert "family/System.ObjectModel/observable-collections" in family_ids
    assert "family/System.ObjectModel/collection-change-notifications" in family_ids
    assert "family/System.ObjectModel/property-change-notifications" in family_ids
    assert "family/System.ObjectModel/data-errors" in family_ids
    assert "family/System.ObjectModel/command-type-metadata" in family_ids
