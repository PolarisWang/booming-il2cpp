from __future__ import annotations

from typing import Any


TEXT_JSON_MARKER_RULES: list[tuple[str, str, str, tuple[str, ...]]] = [
    ("document-element", "JSON Document Element", "JsonDocument and JsonElement parse, enumerate, clone, query, and disposal behavior.", ("System.Text.Json.JsonDocument", "System.Text.Json.JsonElement")),
    ("serializer", "JSON Serializer", "JsonSerializer serialize, deserialize, async, stream, DOM, type, and generic overload behavior.", ("System.Text.Json.JsonSerializer",)),
    ("serializer-options", "JSON Serializer Options", "JsonSerializerOptions configuration, resolver, naming policy, ignore, number handling, and freezing behavior.", ("System.Text.Json.JsonSerializerOptions", "System.Text.Json.JsonNamingPolicy")),
    ("reader", "JSON Reader", "Utf8JsonReader tokenization, state, options, comments, and numeric/string reads.", ("System.Text.Json.Utf8JsonReader",)),
    ("writer", "JSON Writer", "Utf8JsonWriter writing, options, escaping, validation, and flushing behavior.", ("System.Text.Json.Utf8JsonWriter",)),
    ("converters", "JSON Converters", "JsonConverter, built-in converters, enum/string/number handling, and custom converter contracts.", ("System.Text.Json.Serialization.JsonConverter",)),
    ("converter-factory", "JSON Converter Factory", "JsonConverterFactory creation, can-convert, caching, and generic converter behavior.", ("System.Text.Json.Serialization.JsonConverterFactory",)),
    ("attributes", "JSON Attributes", "JsonAttribute, constructor/include/ignore/name/order/required/unmapped attributes and metadata effects.", ("System.Text.Json.Serialization.JsonAttribute",)),
    ("source-generation-context", "JSON Source Generation Context", "JsonSerializerContext, JsonSerializableAttribute, JsonSourceGenerationOptionsAttribute, and generated metadata surface.", ("System.Text.Json.Serialization.JsonSerializerContext",)),
    ("metadata-typeinfo", "JSON Metadata TypeInfo", "JsonTypeInfo, JsonPropertyInfo, JsonParameterInfo, resolver, metadata services, and object info values.", ("System.Text.Json.Serialization.Metadata.JsonTypeInfo",)),
    ("nodes", "JSON Nodes", "JsonNode, JsonObject, JsonArray, JsonValue, mutation, conversion, and serialization behavior.", ("System.Text.Json.Nodes.JsonNode", "System.Text.Json.Nodes.JsonObject", "System.Text.Json.Nodes.JsonArray", "System.Text.Json.Nodes.JsonValue")),
    ("schema", "JSON Schema", "JsonSchemaExporter, schema exporter options, and schema generation behavior.", ("System.Text.Json.Schema.JsonSchemaExporter",)),
    ("polymorphism-reference", "JSON Polymorphism And Reference", "Polymorphic attributes/options, derived types, ReferenceHandler, and ReferenceResolver behavior.", ("System.Text.Json.Serialization.ReferenceHandler", "System.Text.Json.Serialization.ReferenceResolver")),
]

RUNTIME_INTEROPSERVICES_MARKER_RULES: list[tuple[str, str, str, tuple[str, ...]]] = [
    ("marshalling-attributes", "Marshalling Attributes", "StructLayout, MarshalAs, In, Out, Optional, PreserveSig, and related metadata attributes.", ("System.Runtime.InteropServices.StructLayoutAttribute", "System.Runtime.InteropServices.MarshalAsAttribute", "System.Runtime.InteropServices.InAttribute", "System.Runtime.InteropServices.OutAttribute", "System.Runtime.InteropServices.OptionalAttribute", "System.Runtime.InteropServices.PreserveSigAttribute")),
    ("pinvoke-dllimport", "PInvoke DllImport", "DllImport, LibraryImport-adjacent metadata, entry points, calling convention, and char set behavior.", ("System.Runtime.InteropServices.DllImportAttribute", "System.Runtime.InteropServices.LibraryImportAttribute")),
    ("native-memory-pointers", "Native Memory And Pointers", "Marshal allocation, pointer read/write, offset operations, unmanaged memory accessors, and IntPtr helpers.", ("System.Runtime.InteropServices.Marshal", "System.IO.UnmanagedMemoryAccessor")),
    ("handles-safehandle-gchandle", "Handles SafeHandle GCHandle", "SafeHandle, CriticalHandle, GCHandle, handle types, allocation, release, and lifetime behavior.", ("System.Runtime.InteropServices.SafeHandle", "System.Runtime.InteropServices.CriticalHandle", "System.Runtime.InteropServices.GCHandle")),
    ("com-types", "COM Types", "ComTypes interfaces, IStream, IEnumVARIANT, FILETIME, BIND_OPTS, and related COM structs.", ("System.Runtime.InteropServices.ComTypes.",)),
    ("com-wrappers", "COM Wrappers", "ComWrappers, wrappers strategy, vtable pointer helpers, and COM object identity behavior.", ("System.Runtime.InteropServices.ComWrappers",)),
    ("generated-marshalling", "Generated Marshalling", "Generated marshalling attributes, marshaller shape, native marshalling, and source-generated contracts.", ("System.Runtime.InteropServices.Marshalling.",)),
    ("custom-marshaller-contracts", "Custom Marshaller Contracts", "CustomMarshallerAttribute, marshal modes, element marshalling, and collection marshalling contracts.", ("System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute",)),
    ("function-pointers", "Function Pointers", "UnmanagedCallersOnly, delegates to function pointers, callbacks, and native invocation contracts.", ("System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute",)),
    ("unmanaged-calling-conventions", "Unmanaged Calling Conventions", "CallingConvention, CallingConventionAttribute, SuppressGCTransition, and convention metadata.", ("System.Runtime.InteropServices.CallingConvention", "System.Runtime.InteropServices.SuppressGCTransitionAttribute")),
    ("objective-c-interop", "Objective-C Interop", "ObjectiveCMarshal, Objective-C tracked type contracts, and platform-specific interop behavior.", ("System.Runtime.InteropServices.ObjectiveC.",)),
    ("exception-errors", "Interop Exceptions And Errors", "COMException, ExternalException, SEHException, DllNotFoundException, HRESULT, and last-error behavior.", ("System.Runtime.InteropServices.COMException", "System.Runtime.InteropServices.ExternalException", "System.Runtime.InteropServices.SEHException", "System.DllNotFoundException")),
    ("secure-string-marshal", "SecureString Marshal", "SecureStringMarshal and secure string to native memory conversion behavior.", ("System.Security.SecureStringMarshal",)),
    ("runtime-interop-services", "Runtime Interop Services", "RuntimeInformation, OSPlatform, Architecture, MemoryMarshal-adjacent interop surface, and utility contracts.", ("System.Runtime.InteropServices.RuntimeInformation", "System.Runtime.InteropServices.OSPlatform", "System.Runtime.InteropServices.Architecture")),
]

LINQ_ANCHOR_TYPES = {
    "System.Linq.Enumerable",
    "System.Linq.IGrouping",
    "System.Linq.ILookup",
    "System.Linq.IOrderedEnumerable",
    "System.Linq.Lookup",
}

LINQ_FAMILY_BUNDLE: list[tuple[str, str, str]] = [
    ("filtering", "LINQ Filtering", "Where, OfType, Cast, DistinctBy-adjacent filters, and predicate enumeration behavior."),
    ("projection", "LINQ Projection", "Select, SelectMany, index-aware selectors, and iterator shape behavior."),
    ("partitioning", "LINQ Partitioning", "Skip, Take, Range, Chunk, ElementAt, First, Last, Single, and default variants."),
    ("ordering", "LINQ Ordering", "OrderBy, ThenBy, Reverse, comparer handling, and stable ordering behavior."),
    ("grouping-lookup", "LINQ Grouping Lookup", "GroupBy, ToLookup, IGrouping, ILookup, and grouped projection behavior."),
    ("joins", "LINQ Joins", "Join, GroupJoin, key comparison, and joined projection behavior."),
    ("set-operations", "LINQ Set Operations", "Distinct, Union, Intersect, Except, sequence equality, and comparer behavior."),
    ("aggregation", "LINQ Aggregation", "Aggregate, Count, LongCount, Any, All, Contains, and non-enumerated count behavior."),
    ("numeric-aggregation", "LINQ Numeric Aggregation", "Sum, Average, Min, Max, MinBy, MaxBy, nullable numeric, and selector overload behavior."),
    ("materialization", "LINQ Materialization", "ToArray, ToList, ToDictionary, ToHashSet, and collection conversion behavior."),
    ("generation-concat", "LINQ Generation And Concatenation", "Range, Repeat, Empty, Append, Prepend, Concat, and default-if-empty behavior."),
    ("shuffle-index", "LINQ Shuffle Index", "Index, Shuffle, randomization helpers, and index-producing enumeration behavior."),
]


RULE_REGISTRY: dict[str, dict[str, Any]] = {
    "System.Private.CoreLib": {
        "mode": "defer-to-ledger",
    },
    "System.Collections.Immutable": {
        "mode": "marker-rules",
        "rules": [
            ("immutable-array", "Immutable Array", "ImmutableArray construction, builders, enumeration, mutation-like transforms, and interop.", ("System.Collections.Immutable.ImmutableArray",)),
            ("immutable-list", "Immutable List", "ImmutableList nodes, builders, indexing, range, mutation-like transforms, and enumeration.", ("System.Collections.Immutable.ImmutableList",)),
            ("immutable-dictionary", "Immutable Dictionary", "ImmutableDictionary lookup, add, set, remove, builders, and enumeration.", ("System.Collections.Immutable.ImmutableDictionary",)),
            ("immutable-sorted-dictionary", "Immutable Sorted Dictionary", "ImmutableSortedDictionary comparers, ordering, builders, lookup, and mutation-like transforms.", ("System.Collections.Immutable.ImmutableSortedDictionary",)),
            ("immutable-hash-set", "Immutable Hash Set", "ImmutableHashSet equality comparers, set operations, builders, and enumeration.", ("System.Collections.Immutable.ImmutableHashSet",)),
            ("immutable-sorted-set", "Immutable Sorted Set", "ImmutableSortedSet ordering, range views, set operations, builders, and enumeration.", ("System.Collections.Immutable.ImmutableSortedSet",)),
            ("immutable-queue-stack", "Immutable Queue And Stack", "ImmutableQueue and ImmutableStack push, pop, peek, clear, and enumeration behavior.", ("System.Collections.Immutable.ImmutableQueue", "System.Collections.Immutable.ImmutableStack")),
            ("immutable-interlocked", "Immutable Interlocked", "ImmutableInterlocked atomic update, add, remove, and compare-exchange helper behavior.", ("System.Collections.Immutable.ImmutableInterlocked",)),
            ("frozen-collections", "Frozen Collections", "FrozenDictionary and FrozenSet lookup, alternate lookup, enumeration, and construction.", ("System.Collections.Frozen.FrozenDictionary", "System.Collections.Frozen.FrozenSet")),
            ("extensions-marshal", "Immutable Extensions And Marshal", "ImmutableArrayExtensions and ImmutableCollectionsMarshal interop and projection behavior.", ("System.Linq.ImmutableArrayExtensions", "System.Runtime.InteropServices.ImmutableCollectionsMarshal")),
        ],
    },
    "System.Text.Json": {
        "mode": "marker-rules",
        "rules": TEXT_JSON_MARKER_RULES,
    },
    "System.Runtime.InteropServices": {
        "mode": "marker-rules",
        "rules": RUNTIME_INTEROPSERVICES_MARKER_RULES,
    },
    "System.Linq": {
        "mode": "anchor-bundle",
        "anchorTypes": LINQ_ANCHOR_TYPES,
        "familyBundle": LINQ_FAMILY_BUNDLE,
    },
    "System.Formats.Asn1": {
        "mode": "marker-rules",
        "rules": [
            ("tag-model", "ASN.1 Tag Model", "Asn1Tag, TagClass, UniversalTagNumber, and tag equality/encoding behavior.", ("System.Formats.Asn1.Asn1Tag", "System.Formats.Asn1.TagClass", "System.Formats.Asn1.UniversalTagNumber")),
            ("encoding-rules", "ASN.1 Encoding Rules", "BER, CER, DER rule selection and validation behavior.", ("System.Formats.Asn1.AsnEncodingRules",)),
            ("reader", "ASN.1 Reader", "AsnReader navigation, scope handling, primitive reads, and validation.", ("System.Formats.Asn1.AsnReader",)),
            ("writer", "ASN.1 Writer", "AsnWriter construction, scope handling, primitive writes, and final encoding.", ("System.Formats.Asn1.AsnWriter",)),
            ("decoder", "ASN.1 Decoder", "AsnDecoder static decode helpers for primitive and structured values.", ("System.Formats.Asn1.AsnDecoder",)),
            ("options-errors", "ASN.1 Options And Errors", "AsnReaderOptions, AsnContentException, and malformed input behavior.", ("System.Formats.Asn1.AsnReaderOptions", "System.Formats.Asn1.AsnContentException")),
        ],
    },
    "System.IO.Compression.Brotli": {
        "mode": "marker-rules",
        "rules": [
            ("stream", "Brotli Stream", "BrotliStream read, write, flush, dispose, and async behavior.", ("System.IO.Compression.BrotliStream",)),
            ("encoder", "Brotli Encoder", "BrotliEncoder encode, flush, compression level, and state behavior.", ("System.IO.Compression.BrotliEncoder",)),
            ("decoder", "Brotli Decoder", "BrotliDecoder decode, state, status, and reset behavior.", ("System.IO.Compression.BrotliDecoder",)),
            ("options", "Brotli Options", "BrotliCompressionOptions validation and mapping behavior.", ("System.IO.Compression.BrotliCompressionOptions",)),
        ],
    },
    "System.IO.Compression.ZipFile": {
        "mode": "marker-rules",
        "rules": [
            ("zip-file-apis", "ZipFile APIs", "ZipFile create, open, extract, and directory archive behavior.", ("System.IO.Compression.ZipFile",)),
            ("zip-file-extensions", "ZipFile Extensions", "ZipArchiveEntry file extraction and creation extension behavior.", ("System.IO.Compression.ZipFileExtensions",)),
        ],
    },
    "System.IO.Pipelines": {
        "mode": "marker-rules",
        "rules": [
            ("pipe-core", "Pipe Core", "Pipe construction, reset, completion, pause/resume, and coordination behavior.", ("System.IO.Pipelines.Pipe", "System.IO.Pipelines.IDuplexPipe")),
            ("reader-writer", "Pipe Reader Writer", "PipeReader and PipeWriter read, write, advance, cancel, and completion behavior.", ("System.IO.Pipelines.PipeReader", "System.IO.Pipelines.PipeWriter")),
            ("read-flush-results", "Read And Flush Results", "ReadResult and FlushResult state, cancellation, and completion behavior.", ("System.IO.Pipelines.ReadResult", "System.IO.Pipelines.FlushResult")),
            ("options-scheduler", "Options And Scheduler", "PipeOptions, reader/writer stream options, PipeScheduler, and threshold behavior.", ("System.IO.Pipelines.PipeOptions", "System.IO.Pipelines.PipeScheduler", "System.IO.Pipelines.StreamPipeReaderOptions", "System.IO.Pipelines.StreamPipeWriterOptions")),
            ("stream-adapters", "Stream Adapters", "StreamPipeExtensions and stream-backed pipe reader/writer behavior.", ("System.IO.Pipelines.StreamPipeExtensions",)),
        ],
    },
    "System.ObjectModel": {
        "mode": "marker-rules",
        "rules": [
            ("object-model-collections", "ObjectModel Collections", "Collection, ReadOnlyCollection, KeyedCollection, and protected mutation behavior.", ("System.Collections.ObjectModel.Collection", "System.Collections.ObjectModel.ReadOnlyCollection", "System.Collections.ObjectModel.KeyedCollection")),
            ("observable-collections", "Observable Collections", "ObservableCollection and ReadOnlyObservableCollection change behavior.", ("System.Collections.ObjectModel.ObservableCollection", "System.Collections.ObjectModel.ReadOnlyObservableCollection")),
            ("collection-change-notifications", "Collection Change Notifications", "INotifyCollectionChanged, event args, actions, and event handlers.", ("System.Collections.Specialized.INotifyCollectionChanged", "System.Collections.Specialized.NotifyCollectionChanged")),
            ("property-change-notifications", "Property Change Notifications", "INotifyPropertyChanged, INotifyPropertyChanging, event args, and handlers.", ("System.ComponentModel.INotifyPropertyChanged", "System.ComponentModel.INotifyPropertyChanging", "System.ComponentModel.PropertyChanged", "System.ComponentModel.PropertyChanging")),
            ("data-errors", "Data Errors", "INotifyDataErrorInfo and DataErrorsChangedEventArgs behavior.", ("System.ComponentModel.INotifyDataErrorInfo", "System.ComponentModel.DataErrorsChangedEventArgs")),
            ("command-type-metadata", "Command And Type Metadata", "ICommand, ICustomTypeProvider, type converter attributes, and value serializer attributes.", ("System.Windows.Input.ICommand", "System.Reflection.ICustomTypeProvider", "System.ComponentModel.TypeConverterAttribute", "System.Windows.Markup.ValueSerializerAttribute")),
        ],
    },
    "System.Net.ServerSentEvents": {
        "mode": "marker-rules",
        "rules": [
            ("sse-item", "SSE Item Model", "SseItem value, event type, id, retry, comments, and generic payload model.", ("System.Net.ServerSentEvents.SseItem",)),
            ("parser", "SSE Parser", "SseParser stream parsing, frame boundaries, cancellation, and malformed input behavior.", ("System.Net.ServerSentEvents.SseParser",)),
            ("item-parser", "SSE Item Parser", "SseItemParser parsing callbacks, typed payload conversion, and error handling.", ("System.Net.ServerSentEvents.SseItemParser",)),
            ("formatter", "SSE Formatter", "SseFormatter output, field escaping, comments, retry, and typed payload formatting.", ("System.Net.ServerSentEvents.SseFormatter",)),
        ],
    },
    "System.Runtime.Serialization.Formatters": {
        "mode": "marker-rules",
        "rules": [
            ("formatter-base", "Formatter Base", "Formatter, IFormatter, formatter context, and base serialization contract behavior.", ("System.Runtime.Serialization.Formatter", "System.Runtime.Serialization.IFormatter")),
            ("binary-formatter", "Binary Formatter", "BinaryFormatter serialize, deserialize, binder, and compatibility behavior.", ("System.Runtime.Serialization.Formatters.Binary.BinaryFormatter",)),
            ("binder-type-style", "Binder And Type Style", "SerializationBinder, FormatterAssemblyStyle, FormatterTypeStyle, and TypeFilterLevel behavior.", ("System.Runtime.Serialization.SerializationBinder", "System.Runtime.Serialization.Formatters.FormatterAssemblyStyle", "System.Runtime.Serialization.Formatters.FormatterTypeStyle", "System.Runtime.Serialization.Formatters.TypeFilterLevel")),
            ("surrogate-selection", "Surrogate Selection", "ISerializationSurrogate, ISurrogateSelector, and SurrogateSelector behavior.", ("System.Runtime.Serialization.ISerializationSurrogate", "System.Runtime.Serialization.ISurrogateSelector", "System.Runtime.Serialization.SurrogateSelector")),
            ("object-manager-id-generator", "Object Manager And ID Generator", "ObjectManager, ObjectIDGenerator, fixups, and deserialization graph behavior.", ("System.Runtime.Serialization.ObjectManager", "System.Runtime.Serialization.ObjectIDGenerator")),
            ("formatter-services-converter", "Formatter Services And Converter", "FormatterServices, FormatterConverter, IFieldInfo, and SerializationObjectManager behavior.", ("System.Runtime.Serialization.FormatterServices", "System.Runtime.Serialization.FormatterConverter", "System.Runtime.Serialization.Formatters.IFieldInfo", "System.Runtime.Serialization.SerializationObjectManager")),
        ],
    },
    "System.Security.Principal.Windows": {
        "mode": "marker-rules",
        "rules": [
            ("identity-reference", "Identity Reference", "IdentityReference, IdentityReferenceCollection, translation, and equality behavior.", ("System.Security.Principal.IdentityReference", "System.Security.Principal.IdentityReferenceCollection")),
            ("nt-account", "NT Account", "NTAccount construction, validation, equality, and SID translation behavior.", ("System.Security.Principal.NTAccount",)),
            ("security-identifier", "Security Identifier", "SecurityIdentifier construction, binary form, account domain SID, compare, and translation behavior.", ("System.Security.Principal.SecurityIdentifier",)),
            ("token-access", "Token Access", "TokenAccessLevels, SafeAccessTokenHandle, and token handle lifetime behavior.", ("System.Security.Principal.TokenAccessLevels", "Microsoft.Win32.SafeHandles.SafeAccessTokenHandle")),
            ("windows-identity", "Windows Identity", "WindowsIdentity construction, impersonation, claims, token, and name behavior.", ("System.Security.Principal.WindowsIdentity",)),
            ("windows-principal-roles", "Windows Principal Roles", "WindowsPrincipal, WindowsBuiltInRole, WindowsAccountType, and role checks.", ("System.Security.Principal.WindowsPrincipal", "System.Security.Principal.WindowsBuiltInRole", "System.Security.Principal.WindowsAccountType")),
        ],
    },
    "System.Threading.Tasks.Parallel": {
        "mode": "marker-rules",
        "rules": [
            ("parallel-apis", "Parallel APIs", "Parallel.Invoke, For, ForEach, async-aware overloads, and scheduling behavior.", ("System.Threading.Tasks.Parallel",)),
            ("parallel-options", "Parallel Options", "ParallelOptions cancellation, scheduler, degree of parallelism, and configuration behavior.", ("System.Threading.Tasks.ParallelOptions",)),
            ("loop-state", "Parallel Loop State", "ParallelLoopState break, stop, exceptional, and low/highest iteration behavior.", ("System.Threading.Tasks.ParallelLoopState",)),
            ("loop-result", "Parallel Loop Result", "ParallelLoopResult completion and lowest break iteration behavior.", ("System.Threading.Tasks.ParallelLoopResult",)),
        ],
    },
}


def get_rule_config(assembly_name: str) -> dict[str, Any] | None:
    return RULE_REGISTRY.get(assembly_name)
