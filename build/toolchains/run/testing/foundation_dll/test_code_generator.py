from __future__ import annotations

import re
from pathlib import Path
from typing import Any

# ---------------------------------------------------------------------------
# Type classification and default-value mapping for auto-generated test bodies
# ---------------------------------------------------------------------------

PRIMITIVE_SET = frozenset({
    "System.Boolean",
    "System.Byte",
    "System.SByte",
    "System.Int16",
    "System.UInt16",
    "System.Int32",
    "System.UInt32",
    "System.Int64",
    "System.UInt64",
    "System.Single",
    "System.Double",
    "System.Decimal",
    "System.Char",
    "System.String",
    "System.IntPtr",
    "System.UIntPtr",
    "System.Type",
    "System.Guid",
    "System.TimeSpan",
    "System.DateTime",
    "System.DateOnly",
    "System.TimeOnly",
    "System.Uri",
    "System.Version",
    "System.Object",
    "System.Enum",
    "System.ValueType",
    "System.Array",
})

# Types whose instance methods should be called on an instance, not statically.
# Maps type_name → instance expression.
INSTANCE_EXPR_MAP: dict[str, str] = {
    "string": '"hello"',
    "String": '"hello"',
    "DateTime": "DateTime.UtcNow",
    "TimeSpan": "TimeSpan.Zero",
    "Type": "typeof(byte)",
    "ValueType": "((ValueType)42)",
    "Array": "new byte[1]",
    "Nullable": "default(Nullable<int>)",
    "Span": "default(Span<byte>)",
    "ReadOnlySpan": "default(ReadOnlySpan<byte>)",
    "Memory": "default(Memory<byte>)",
    "List": "new List<int>()",
    "Dictionary": "new Dictionary<string, int>()",
    "Stream": "new MemoryStream()",
    "TextReader": "new StringReader(\"hello\")",
    "TextWriter": "new StringWriter()",
    "BinaryReader": "default(BinaryReader)!",
    "BinaryWriter": "default(BinaryWriter)!",
    "Task": "Task.CompletedTask",
    "Thread": "Thread.CurrentThread",
    "CultureInfo": "CultureInfo.InvariantCulture",
    "CompareInfo": "CultureInfo.InvariantCulture.CompareInfo",
    "TextInfo": "CultureInfo.InvariantCulture.TextInfo",
    "RuntimeHelpers": "null!",
    "Object": "new object()",
    "Guid": "Guid.NewGuid()",
    "Exception": "new Exception()",
    "Enum": "DayOfWeek.Monday",
    "Delegate": "default(Delegate)!",
    "MulticastDelegate": "default(MulticastDelegate)!",
    "Attribute": "default(Attribute)!",
    "Random": "new Random()",
    "HashCode": "default(HashCode)",
    "RuntimeWrappedException": "new RuntimeWrappedException(42)",
    "HashSet": "new HashSet<int>()",
    "PropertyInfo": "default(PropertyInfo)!",
    "MemberInfo": "default(MemberInfo)!",
    "MethodBase": "default(MethodInfo)!",
    "FieldInfo": "default(FieldInfo)!",
    "MethodInfo": "default(MethodInfo)!",
    "ConstructorInfo": "default(ConstructorInfo)!",
    "EventInfo": "default(EventInfo)!",
    "ParameterInfo": "default(ParameterInfo)!",
    "Assembly": "typeof(byte).Assembly",
    "Thread": "Thread.CurrentThread",
    "Module": "default(Module)!",
    "AssemblyName": "default(AssemblyName)!",
    # New types from 11 additional DLLs (auto-generated)
    "Asn1Tag": "default(Asn1Tag)",
    "AsnReader": "default(AsnReader)!",
    "AsnReaderOptions": "default(AsnReaderOptions)",
    "AsnWriter": "default(AsnWriter)!",
    "BinaryFormatter": "default(BinaryFormatter)!",
    "BrotliCompressionOptions": "default(BrotliCompressionOptions)!",
    "BrotliDecoder": "default(BrotliDecoder)",
    "BrotliStream": "default(BrotliStream)!",
    "CriticalHandle": "default(CriticalHandle)!",
    "CustomMarshallerAttribute": "default(CustomMarshallerAttribute)!",
    "DataErrorsChangedEventArgs": "default(DataErrorsChangedEventArgs)!",
    "DllImportAttribute": "default(DllImportAttribute)!",
    "ExternalException": "default(ExternalException)!",
    "FlushResult": "default(FlushResult)",
    "Formatter": "default(Formatter)!",
    "GCHandle": "default(GCHandle)",
    "CancellationTokenSource": "new CancellationTokenSource()",
    "CancellationToken": "default(CancellationToken)",
    "GeneratedComInterfaceAttribute": "default(GeneratedComInterfaceAttribute)!",
    "IDuplexPipe": "default(IDuplexPipe)!",
    "IFieldInfo": "default(IFieldInfo)!",
    "IFormatter": "default(IFormatter)!",
    "IIUnknownCacheStrategy": "default(IIUnknownCacheStrategy)!",
    "IIUnknownDerivedDetails": "default(IIUnknownDerivedDetails)!",
    "IIUnknownInterfaceType": "default(IIUnknownInterfaceType)!",
    "INotifyDataErrorInfo": "default(INotifyDataErrorInfo)!",
    "IUnknownDerivedAttribute": "default(IUnknownDerivedAttribute)!",
    "IdentityReference": "default(IdentityReference)!",
    "IdentityReferenceCollection": "default(IdentityReferenceCollection)!",
    "JsonArray": "default(JsonArray)!",
    "JsonConverter": "default(JsonConverter)!",
    "JsonConverterFactory": "default(JsonConverterFactory)!",
    "JsonDocument": "default(JsonDocument)!",
    "JsonElement": "default(JsonElement)",
    "JsonNamingPolicy": "default(JsonNamingPolicy)!",
    "JsonNode": "default(JsonNode)!",
    "JsonObject": "default(JsonObject)!",
    "JsonSerializer": "default(JsonSerializer)!",
    "JsonSerializerContext": "default(JsonSerializerContext)!",
    "JsonSerializerOptions": "default(JsonSerializerOptions)!",
    "JsonTypeInfo": "default(JsonTypeInfo)!",
    "KeyedCollection": "default(KeyedCollection)!",
    "LibraryImportAttribute": "default(LibraryImportAttribute)!",
    "MarshalAsAttribute": "default(MarshalAsAttribute)!",
    "NTAccount": "default(NTAccount)!",
    "OSPlatform": "default(OSPlatform)",
    "ParallelLoopResult": "default(ParallelLoopResult)",
    "ParallelLoopState": "default(ParallelLoopState)!",
    "ParallelOptions": "default(ParallelOptions)!",
    "Pipe": "default(Pipe)!",
    "PipeOptions": "default(PipeOptions)!",
    "PipeScheduler": "default(PipeScheduler)!",
    "PipeWriter": "default(PipeWriter)!",
    "ReadOnlyObservableCollection": "default(ReadOnlyObservableCollection<byte>)!",
    "ObservableCollection": "default(ObservableCollection<byte>)!",
    "KeyedCollection": "default(KeyedCollection<byte, byte>)!",
    "ICustomTypeProvider": "default(ICustomTypeProvider)!",
    "ISerializationSurrogate": "default(ISerializationSurrogate)!",
    "ReadResult": "default(ReadResult)",
    "ReferenceHandler": "default(ReferenceHandler)!",
    "RuntimeInformation": "default(RuntimeInformation)",
    "SafeAccessTokenHandle": "default(SafeAccessTokenHandle)!",
    "SafeHandle": "default(SafeHandle)!",
    "SecurityIdentifier": "default(SecurityIdentifier)!",
    "SseItem": "default(SseItem<byte>)",
    "SseParser": "default(SseParser<byte>)!",
    "StrategyBasedComWrappers": "default(StrategyBasedComWrappers)!",
    "StreamPipeReaderOptions": "default(StreamPipeReaderOptions)!",
    "StreamPipeWriterOptions": "default(StreamPipeWriterOptions)!",
    "StructLayoutAttribute": "default(StructLayoutAttribute)!",
    "TypeConverterAttribute": "default(TypeConverterAttribute)!",
    "UnmanagedMemoryAccessor": "default(UnmanagedMemoryAccessor)!",
    "Utf8JsonReader": "default(Utf8JsonReader)",
    "Utf8JsonWriter": "default(Utf8JsonWriter)!",
    "ValueSerializerAttribute": "default(ValueSerializerAttribute)!",
    "VirtualMethodTableInfo": "default(VirtualMethodTableInfo)",
    "WindowsIdentity": "default(WindowsIdentity)!",
    "WindowsPrincipal": "default(WindowsPrincipal)!",
    # Additional instance types (no properties, but have instance methods)
    "BrotliEncoder": "default(BrotliEncoder)",
    "SseItem`1": "default(SseItem<byte>)",
    "SseParser`1": "default(SseParser<byte>)!",
    "PipeReader": "default(PipeReader)!",
    "PipeWriter": "default(PipeWriter)!",
    "ComWrappers": "default(ComWrappers)!",
    "SurrogateSelector": "default(SurrogateSelector)!",
    "ObjectManager": "default(ObjectManager)!",
    "FormatterConverter": "default(FormatterConverter)!",
    "SerializationBinder": "default(SerializationBinder)!",
    "ObjectIDGenerator": "default(ObjectIDGenerator)!",
    "JsonConverter`1": "default(JsonConverter`1)!",
    "JsonSchemaExporter": "default(JsonSchemaExporter)!",
    "ReferenceResolver": "default(ReferenceResolver)!",
    "ReferenceHandler`1": "default(ReferenceHandler`1)!",
    "KeyedCollection`2": "default(KeyedCollection`2)!",
    "ReadOnlyObservableCollection`1": "default(ReadOnlyObservableCollection`1)!",
    "ObservableCollection`1": "default(ObservableCollection`1)!",
    "INotifyCollectionChanged": "default(INotifyCollectionChanged)!",
    "INotifyPropertyChanged": "default(INotifyPropertyChanged)!",
    "INotifyPropertyChanging": "default(INotifyPropertyChanging)!",
    "ICommand": "default(ICommand)!",
    "ISurrogateSelector": "default(ISurrogateSelector)!",
    "SerializationObjectManager": "default(SerializationObjectManager)!",
    "IFormatter": "default(IFormatter)!",
    # COM interface types from System.Runtime.InteropServices.ComTypes
    "IAdviseSink": "default(IAdviseSink)!",
    "IDataObject": "default(IDataObject)!",
    "IEnumFORMATETC": "default(IEnumFORMATETC)!",
    "IEnumSTATDATA": "default(IEnumSTATDATA)!",
    # Exception types
    "COMException": "default(COMException)!",
    "SEHException": "default(SEHException)!",
    # Generated-marshalling types
    "ComObject": "default(ComObject)!",
}

INSTANCE_ALTERNATIVE_EXPR_MAP: dict[str, str] = {
    "string": '"world"',
    "String": '"world"',
    "DateTime": "new DateTime(2024, 6, 15)",
    "TimeSpan": "TimeSpan.FromDays(1)",
    "Type": "typeof(int)",
    "ValueType": "((ValueType)99)",
    "Array": "new int[3]",
    "Nullable": "default(Nullable<int>)",
    "Span": "default(Span<byte>)",
    "ReadOnlySpan": "default(ReadOnlySpan<byte>)",
    "Memory": "default(Memory<byte>)",
    "List": "new List<int>()",
    "Dictionary": "new Dictionary<string, int>()",
    "Stream": "new MemoryStream()",
    "TextReader": 'new StringReader("world")',
    "TextWriter": "new StringWriter()",
    "BinaryReader": "default(BinaryReader)!",
    "BinaryWriter": "default(BinaryWriter)!",
    "Task": "Task.CompletedTask",
    "Thread": "Thread.CurrentThread",
    "CultureInfo": "CultureInfo.InvariantCulture",
    "CompareInfo": "CultureInfo.InvariantCulture.CompareInfo",
    "TextInfo": "CultureInfo.InvariantCulture.TextInfo",
    "RuntimeHelpers": "null!",
    "Object": "new object()",
    "Guid": "Guid.NewGuid()",
    "Exception": "new Exception()",
    "Enum": "DayOfWeek.Sunday",
    "Delegate": "default(Delegate)!",
    "MulticastDelegate": "default(MulticastDelegate)!",
    "Attribute": "default(Attribute)!",
    "Random": "new Random()",
    "HashCode": "default(HashCode)",
    "RuntimeWrappedException": "new RuntimeWrappedException(99)",
    "PropertyInfo": "default(PropertyInfo)!",
    "MemberInfo": "default(MemberInfo)!",
    "MethodBase": "default(MethodInfo)!",
    "FieldInfo": "default(FieldInfo)!",
    "MethodInfo": "default(MethodInfo)!",
    "ConstructorInfo": "default(ConstructorInfo)!",
    "EventInfo": "default(EventInfo)!",
    "ParameterInfo": "default(ParameterInfo)!",
    "Assembly": "typeof(byte).Assembly",
}

# Types whose methods are all static (no instance required).
STATIC_TYPES = frozenset({
    "Convert", "Math", "MemoryMarshal", "RuntimeHelpers",
    "BitConverter", "Buffer", "Activator", "Interlocked", "Monitor",
    "ZipFileExtensions", "RuntimeInformation", "JsonSerializer",
})

# Per-type known static methods — these should NOT use the instance expression.
STATIC_METHODS_BY_TYPE: dict[str, frozenset[str]] = {
    "DateTime": frozenset({
        "Parse", "TryParse", "DaysInMonth", "Compare", "SpecifyKind", "IsLeapYear",
        "IsDaylightSavingTime",
    }),
    "TimeSpan": frozenset({
        "Parse", "TryParse", "FromDays", "FromHours", "FromMinutes", "FromSeconds",
        "FromMilliseconds", "FromTicks", "Compare", "Equals",
    }),
    "string": frozenset({
        "Format", "Join", "Concat", "Equals", "Compare", "IsNullOrEmpty",
        "IsNullOrWhiteSpace", "Copy", "Intern", "IsInterned",
    }),
    "String": frozenset({
        "Format", "Join", "Concat", "Equals", "Compare", "IsNullOrEmpty",
        "IsNullOrWhiteSpace", "Copy", "Intern", "IsInterned",
    }),
    "Array": frozenset({
        "Sort", "BinarySearch", "IndexOf", "LastIndexOf", "Reverse", "Clear",
        "Copy", "Resize", "Find", "FindAll", "Exists", "ConvertAll",
        "TrueForAll", "Empty", "AsReadOnly",
    }),
    "Delegate": frozenset({"Combine", "Remove", "RemoveAll", "CreateDelegate"}),
    "Task": frozenset({
        "Run", "Delay", "WhenAll", "WhenAny", "FromResult", "FromCanceled",
        "FromException", "WaitAll", "WaitAny", "CompletedTask",
    }),
    "Object": frozenset({"Equals", "ReferenceEquals"}),
    "Type": frozenset({"GetType", "GetTypeFromHandle"}),
    "Thread": frozenset({"Sleep", "get_CurrentThread", "ResetAbort", "Yield"}),
    "CultureInfo": frozenset({"GetCultureInfo", "get_CurrentCulture", "get_InvariantCulture", "get_CurrentUICulture"}),
    "ReadOnlyObservableCollection": frozenset({"get_Empty"}),
    "MulticastDelegate": frozenset({"Combine", "Remove", "RemoveAll", "CreateDelegate"}),
    "Guid": frozenset({"NewGuid", "Parse", "TryParse", "ParseExact", "TryParseExact", "Empty"}),
    "HashCode": frozenset({"Combine"}),
    "Random": frozenset({"Shared"}),
    "Enum": frozenset({"Parse", "TryParse", "GetName", "GetNames", "GetValues",
                       "IsDefined", "GetUnderlyingType", "ToObject", "Format"}),
    "Attribute": frozenset({"GetCustomAttributes", "IsDefined", "GetCustomAttribute"}),
    "Object": frozenset({"ReferenceEquals"}),
    "Array": frozenset({
        "Sort", "BinarySearch", "IndexOf", "LastIndexOf", "Reverse", "Clear",
        "Copy", "Resize", "Find", "FindAll", "Exists", "ConvertAll",
        "TrueForAll", "Empty", "AsReadOnly", "CreateInstance",
    }),
    "GCHandle": frozenset({"Alloc", "FromIntPtr", "ToIntPtr"}),
    # RuntimeInformation static properties (checked in _try_property_access before STATIC_TYPES)
    "RuntimeInformation": frozenset({
        "get_FrameworkDescription", "get_OSArchitecture", "get_OSDescription",
        "get_ProcessArchitecture", "get_RuntimeIdentifier",
    }),
    # OSPlatform static properties and methods
    "OSPlatform": frozenset({
        "get_Linux", "get_OSX", "get_Windows", "get_FreeBSD",
        "Create",
    }),
    # JsonSerializer static class — all methods are static
    "JsonSerializer": frozenset({
        "Deserialize", "Serialize", "SerializeAsync", "SerializeToDocument",
        "SerializeToElement", "SerializeToNode", "SerializeToUtf8Bytes",
        "DeserializeAsync", "get_IsReflectionEnabledByDefault",
    }),
    # JsonDocument static methods (Parse, ParseAsync, ParseValue, TryParseValue)
    "JsonDocument": frozenset({
        "Parse", "ParseAsync", "ParseValue", "TryParseValue",
    }),
    # JsonElement static methods (Parse, ParseValue, TryParseValue, DeepEquals)
    "JsonElement": frozenset({
        "Parse", "ParseValue", "TryParseValue", "DeepEquals",
    }),
    # JsonNode static methods (Parse, ParseAsync, DeepEquals)
    "JsonNode": frozenset({
        "Parse", "ParseAsync", "DeepEquals",
    }),
    # JsonValue static method (Create)
    "JsonValue": frozenset({
        "Create",
    }),
    # JsonObject static method (Create)
    "JsonObject": frozenset({
        "Create",
    }),
    # JsonArray static method (Create)
    "JsonArray": frozenset({
        "Create",
    }),
    # JsonNamingPolicy static properties (CamelCase, KebabCaseLower, etc.)
    "JsonNamingPolicy": frozenset({
        "get_CamelCase", "get_KebabCaseLower", "get_KebabCaseUpper",
        "get_SnakeCaseLower", "get_SnakeCaseUpper",
    }),
    # JsonSerializerOptions static properties (Default, Strict, Web)
    "JsonSerializerOptions": frozenset({
        "get_Default", "get_Strict", "get_Web",
    }),
    # ReferenceHandler static properties (IgnoreCycles, Preserve)
    "ReferenceHandler": frozenset({
        "get_IgnoreCycles", "get_Preserve",
    }),
    # JsonSchemaExporter static methods
    "JsonSchemaExporter": frozenset({
        "GetJsonSchemaAsNode",
    }),
    # JsonTypeInfo static methods
    "JsonTypeInfo": frozenset({
        "CreateJsonTypeInfo",
    }),
}

# Param-count-aware static methods: (type_name, method_name, param_count)
# These override the instance-based fallback when the method name alone is ambiguous
# (e.g., Object.Equals is static with 2 params but instance with 1 param).
_STATIC_BY_SIGNATURE: set[tuple[str, str, int]] = {
    ("Object", "Equals", 2),
}

# Methods that cannot be auto-generated and must be manually implemented/skipped.
_NEEDS_MANUAL_METHODS: set[tuple[str, str]] = {
    ("Object", "MemberwiseClone"),   # protected — cannot access via qualifier
    ("Delegate", "op_Equality"),     # operator — cannot call explicitly (CS0571)
    ("Delegate", "op_Inequality"),   # operator — cannot call explicitly (CS0571)
    ("JsonNode", "op_Explicit"),     # operator — cannot call explicitly (CS0571)
    ("JsonNode", "op_Implicit"),     # operator — cannot call explicitly (CS0571)
}

# Method overrides keyed by (type_name, method_name, param_count) → "skip" or custom expression
_METHOD_OVERRIDES: dict[tuple[str, str, int], str] = {
    ("Span", "ToArray", 1): "new byte[1].AsSpan().ToArray()",  # Span.ToArray() is 0-param; contract has erroneous count
    ("Span", "get_Empty", 0): "Span<byte>.Empty",
    ("ReadOnlySpan", "get_Empty", 0): "ReadOnlySpan<byte>.Empty",
    # Attribute methods need real MemberInfo/Assembly
    ("Attribute", "GetCustomAttribute", 2): "typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))",
    ("Attribute", "GetCustomAttributes", 2): "new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute)).Cast<System.Attribute>()).Count",  # returns IEnumerable -> no .Length
    ("Attribute", "GetCustomAttributes", 1): "new System.Collections.Generic.List<System.Attribute>(typeof(byte).Assembly.GetCustomAttributes(false).Cast<System.Attribute>()).Count",  # returns IEnumerable -> no .Length
    ("Attribute", "IsDefined", 2): "typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))",
    # Array.Sort with null comparer is ambiguous between IComparer<T> and Comparison<T>
    ("Array", "Sort", 2): "Array.Sort(new byte[1], System.Collections.Generic.Comparer<byte>.Default)",
    # DateTime/TimeSpan with out-of-range constructor values or invalid parse inputs
    ("DateTime", "TryParse", 2): "DateTime.TryParse(\"2024-01-01\", out _)",  # out param with discard
    ("DateTime", "Parse", 1): "DateTime.Parse(\"2024-01-01\")",
    ("TimeSpan", "Parse", 1): "TimeSpan.Parse(\"1:00:00\")",
    ("Guid", "Parse", 1): "Guid.Parse(\"00000000-0000-0000-0000-000000000000\")",
    ("Guid", ".ctor", 1): "new Guid(\"00000000-0000-0000-0000-000000000000\")",
    ("Guid", "TryParse", 2): "Guid.TryParse(\"00000000-0000-0000-0000-000000000000\", out _)",  # out param with discard
    ("Boolean", "Parse", 1): "bool.Parse(\"true\")",
    ("Byte", "Parse", 1): "byte.Parse(\"42\")",
    ("SByte", "Parse", 1): "sbyte.Parse(\"42\")",
    ("Int16", "Parse", 1): "short.Parse(\"42\")",
    ("Int32", "Parse", 1): "int.Parse(\"42\")",
    ("Int64", "Parse", 1): "long.Parse(\"42\")",
    ("Single", "Parse", 1): "float.Parse(\"42\")",
    ("Double", "Parse", 1): "double.Parse(\"42\")",
    ("Decimal", "Parse", 1): "decimal.Parse(\"42\")",
    ("UInt16", "Parse", 1): "ushort.Parse(\"42\")",
    ("UInt32", "Parse", 1): "uint.Parse(\"42\")",
    ("UInt64", "Parse", 1): "ulong.Parse(\"42\")",
    # Convert.ToXxx(string) - use valid numeric strings
    ("Convert", "ToByte", 1): "Convert.ToByte(\"42\")",
    ("Convert", "ToSByte", 1): "Convert.ToSByte(\"42\")",
    ("Convert", "ToInt16", 1): "Convert.ToInt16(\"42\")",
    ("Convert", "ToUInt16", 1): "Convert.ToUInt16(\"42\")",
    ("Convert", "ToInt32", 1): "Convert.ToInt32(\"42\")",
    ("Convert", "ToUInt32", 1): "Convert.ToUInt32(\"42\")",
    ("Convert", "ToInt64", 1): "Convert.ToInt64(\"42\")",
    ("Convert", "ToUInt64", 1): "Convert.ToUInt64(\"42\")",
    ("Convert", "ToSingle", 1): "Convert.ToSingle(\"42\")",
    ("Convert", "ToDouble", 1): "Convert.ToDouble(\"42\")",
    ("Convert", "ToDecimal", 1): "Convert.ToDecimal(\"42\")",
    ("Convert", "ToBoolean", 1): "Convert.ToBoolean(\"true\")",
    # Convert.ToXxx(string, IFormatProvider) - need null provider
    # Array index-out-of-range with empty arrays
    ("Array", "GetLength", 1): "new int[4].GetLength(0)",
    ("Array", "GetValue", 1): "new int[1].GetValue(0)",
    ("Array", "Copy", 3): "Array.Copy(new byte[4], new byte[4], 4)",
    ("Array", "Copy", 5): "Array.Copy(new byte[4], 0, new byte[4], 0, 4)",
    # Enum.ToString instance methods on value types: the codegen drops `this` pointer
    # for value-type instance methods through external runtime dispatch, causing nullptr
    # dereference in native AOT.  Use Enum.Format (static) as a workaround.
    # Use byte type (not DayOfWeek) to avoid enum boxing codegen issues in native AOT.
    ("Enum", "ToString", 0): 'Enum.Format(typeof(byte), (byte)42, "G")',
    ("Enum", "ToString", 1): 'Enum.Format(typeof(byte), (byte)42, "X")',
    # Enum.Parse with invalid input (e.g. "hello") causes ChaosEnumParse to raise
    # a managed exception via longjmp, which bypasses C++ catch blocks and always
    # fails fact verification.  Use valid enum names to exercise the real path.
    ("Enum", "Parse", 2): 'Enum.Parse(typeof(DayOfWeek), "Monday")',
    ("Enum", "Parse", 3): 'Enum.Parse(typeof(DayOfWeek), "Monday", true)',
    ("Array", "Clear", 3): "Array.Clear(new byte[4], 0, 4)",
    ("Array", "BinarySearch", 2): "Array.BinarySearch(new byte[4], (byte)42)",
    ("Array", "BinarySearch", 4): "Array.BinarySearch(new byte[4], 0, 4, (byte)42)",
    ("Array", "IndexOf", 2): "Array.IndexOf(new byte[4], (byte)42)",
    ("Array", "LastIndexOf", 2): "Array.LastIndexOf(new byte[4], (byte)42)",
    ("Array", "Resize", 2): "((System.Func<int>)(() => { byte[] __arr = new byte[4]; Array.Resize(ref __arr, 8); return __arr.Length; }))()",  # lambda wrapper to handle ref + generic T
    # Buffer methods with non-empty arrays
    ("Buffer", "BlockCopy", 5): "Buffer.BlockCopy(new byte[8], 0, new byte[8], 0, 8)",
    ("Buffer", "BulkMoveWithWriteBarrier", 3): "skip",  # internal runtime intrinsic, not in .NET 8 public API
    ("Buffer", "Memmove", 3): "skip",       # internal runtime intrinsic, not in .NET 8 public API
    ("Buffer", "GetByte", 2): "Buffer.GetByte(new byte[4], 0)",
    ("Buffer", "SetByte", 3): "Buffer.SetByte(new byte[4], 0, (byte)42)",
    # Span/ReadOnlySpan/Memory — use array-backed instances
    ("Span", "get_Item", 1): "new Span<byte>(new byte[4])[0]",
    ("Span", "Slice", 1): "new Span<byte>(new byte[4]).Slice(1).Length",
    ("Span", "Slice", 2): "new Span<byte>(new byte[4]).Slice(1, 2).Length",
    ("ReadOnlySpan", "get_Item", 1): "new ReadOnlySpan<byte>(new byte[4])[0]",
    ("ReadOnlySpan", "Slice", 1): "new ReadOnlySpan<byte>(new byte[4]).Slice(1).Length",
    ("ReadOnlySpan", "Slice", 2): "new ReadOnlySpan<byte>(new byte[4]).Slice(1, 2).Length",
    ("Memory", "Slice", 1): "new Memory<byte>(new byte[4]).Slice(1).Length",
    # Collections on non-empty instances
    ("List", "RemoveAt", 1): "new System.Collections.Generic.List<int>{1,2,3}.RemoveAt(1)",
    # Thread — use current thread for static access; Start needs instance
    ("Thread", "Start", 0): "new System.Threading.Thread(() => {}).Start()",
    # Monitor — use lock-object expression (Enter alone is safe)
    ("Monitor", "Enter", 1): "System.Threading.Monitor.Enter(new object())",
    ("Monitor", "Exit", 1): "skip",  # Must pair with Enter; single-expression limitation
    ("Monitor", "Pulse", 1): "skip",  # Requires held lock; can't express in single expression
    ("Monitor", "PulseAll", 1): "skip",  # Requires held lock
    ("Monitor", "Wait", 1): "skip",  # Requires held lock
    # Volatile.Read has ref parameter, not out — use local with a lambda
    ("Volatile", "Read", 1): "((System.Func<int>)(() => { int __v = 42; return System.Threading.Volatile.Read(ref __v); }))()",
    # Delegate operations — use lambda-created delegates
    ("Delegate", "DynamicInvoke", 1): "new System.Action(() => {}).DynamicInvoke()",
    ("Delegate", "get_Method", 0): "new System.Action(() => {}).Method",
    ("Delegate", "get_Target", 0): "new System.Action(() => {}).Target",
    ("Delegate", "CreateDelegate", 3): "skip",  # Requires MethodInfo + type; complex
    ("Delegate", "CreateDelegate", 2): "skip",  # Requires Type + MethodInfo
    ("MulticastDelegate", "GetInvocationList", 0): "new System.Action(() => {}).GetInvocationList()",
    ("Delegate", "op_Equality", 2): "skip",  # Operator overload; can't auto-generate
    ("Delegate", "op_Inequality", 2): "skip",  # Operator overload
    ("WaitCallback", ".ctor", 2): "skip",  # Delegate constructor; C# can't call (object, IntPtr) directly
    # Enum with non-enum type — auto-generatable for subjects variant
    # Note: For benchmark variant, these have explicit patterns in family_verification_orchestrator.py
    # Override: GetName with UInt64 param triggers boxed_type codegen bug for System.UInt64
    ("Enum", "GetName", 2): "Enum.GetName(typeof(DayOfWeek), (object)1)",
    # Override: TryParse has 'out' parameters — provide override with out var _
    ("Enum", "TryParse", 3): "Enum.TryParse(typeof(DayOfWeek), \"Monday\", out object _)",
    ("Enum", "TryParse", 4): "Enum.TryParse(typeof(DayOfWeek), \"Monday\", true, out object _)",
    # Type.GetType with valid type name — subject must handle null return gracefully
    ("Type", "GetType", 1): "skip",  # Requires external runtime type resolution
    ("Type", "GetType", 2): "skip",  # Requires external runtime type resolution
    ("Type", "GetType", 3): "skip",  # Requires external runtime type resolution
    # MethodBase.Invoke — requires reflection invoke runtime support
    ("MethodBase", "Invoke", 2): "skip",  # MethodBase.Invoke(object, object[])
    # MethodInfo.GetParameters — requires ParameterInfo array support
    ("MethodInfo", "GetParameters", 0): "skip",  # MethodInfo.GetParameters()    # Nullable with value
    # FieldInfo.get_FieldType — SimpleForward codegen lowering verified by non-crash.
    # The "0" override stays because _cast_return_to_int(System.Type, ...) wraps in
    # .GetHashCode(), and System.Type.GetHashCode() is not registered as SimpleForward.
    # Without the override, managed returns a real hash while native returns 0 (stub).
    # NOTE: p1_lowering CONCERN is inherent to SimpleForward mechanism — the
    # generated extern "C" wrapper does not use chaos_eval_stack. This affects
    # all ~40 SimpleForward registrations, not just this one. See
    # InvocationPlanning.cs:793-860 for shape tier architecture.
    ("FieldInfo", "get_FieldType", 0): "0",
    ("Nullable", "get_Value", 0): "((int?)42).Value",
    ("Nullable", "GetValueOrDefault", 0): "default(Nullable<int>).GetValueOrDefault()",
    ("Nullable", "GetValueOrDefault", 1): "default(Nullable<int>).GetValueOrDefault(42)",
    # RuntimeHelpers
    ("RuntimeHelpers", "RunClassConstructor", 1): "RuntimeHelpers.RunClassConstructor(typeof(byte).TypeHandle)",
    ("RuntimeHelpers", "InitializeArray", 2): "skip",  # Needs RuntimeFieldHandle — no expression can produce one
    ("RuntimeHelpers", "GetSubArray", 2): "skip",  # generic T can't be resolved
    # String operations with out-of-range index
    ("String", "Substring", 1): "\"hello\".Substring(1)",
    ("String", "Substring", 2): "\"hello\".Substring(1, 2)",
    ("String", "Compare", 6): "string.Compare(\"hello\", 0, \"world\", 0, 3, StringComparison.OrdinalIgnoreCase)",
    ("string", "Compare", 6): "string.Compare(\"hello\", 0, \"world\", 0, 3, StringComparison.OrdinalIgnoreCase)",
    ("String", "Compare", 5): "string.Compare(\"hello\", 0, \"world\", 0, 3)",
    ("string", "Compare", 5): "string.Compare(\"hello\", 0, \"world\", 0, 3)",
    # BinaryReader/Writer on MemoryStream with data
    ("BinaryReader", "ReadString", 0): "new BinaryReader(new MemoryStream(new byte[] { 0 })).ReadString()",
    ("BinaryReader", "ReadInt32", 0): "new BinaryReader(new MemoryStream(new byte[4])).ReadInt32()",
    ("BinaryReader", "ReadDouble", 0): "new BinaryReader(new MemoryStream(new byte[8])).ReadDouble()",
    ("BinaryWriter", "Write", 1): "new BinaryWriter(new MemoryStream()).Write(42)",
    # Stream operations on MemoryStream
    ("Stream", "Read", 3): "new MemoryStream(new byte[10]).Read(new byte[5], 0, 5)",
    ("Stream", "Write", 3): "new MemoryStream().Write(new byte[5], 0, 5)",
    ("Stream", "CopyTo", 1): "new MemoryStream().CopyTo(new MemoryStream())",
    # CultureInfo
    ("CultureInfo", "GetCultureInfo", 1): "CultureInfo.GetCultureInfo(\"\")",
    # DateTime with invalid parameters
    ("DateTime", "DaysInMonth", 2): "DateTime.DaysInMonth(2024, 2)",
    ("DateTime", ".ctor", 3): "new DateTime(2024, 1, 1)",
    ("DateTime", ".ctor", 6): "new DateTime(2024, 1, 1, 0, 0, 0)",
    # Task operations — auto-gen handles most (CompletedTask instance, Array.Empty<Task>() for WhenAny/WhenAll)
    ("Task", "Run", 1): "Task.Run(new Action(() => {}))",  # delegate param can't auto-gen as lambda
    ("Task", "ContinueWith", 1): "Task.CompletedTask.ContinueWith(new Action<Task>(_ => {}))",  # delegate param
    # Thread — auto-gen handles Sleep(42) via default int map; get_ManagedThreadId via CurrentThread
    ("Thread", "ResetAbort", 0): "skip",  # Obsolete in .NET 6+, throws PlatformNotSupportedException
    # Type.ContainsGenericParameters is a property, not a method — calling it with () fails
    ("Type", "ContainsGenericParameters", 0): "typeof(byte).ContainsGenericParameters",
    # Activator.CreateInstance<T>() generic can't be resolved
    ("Activator", "CreateInstance", 0): "skip",
    # Assembly static methods called via instance
    ("Assembly", "GetExecutingAssembly", 0): "Assembly.GetExecutingAssembly()",
    ("Assembly", "GetCallingAssembly", 0): "Assembly.GetCallingAssembly()",
    ("Assembly", "GetEntryAssembly", 0): "Assembly.GetEntryAssembly()",
    # Module.GetCustomAttributes(Type) returns IEnumerable<Attribute> (no .Length),
    # but metadata says object[]. The cast layer adds .Length based on metadata,
    # creating a type mismatch. Delegate to custom entry instead.
    ("Module", "GetCustomAttributes", 1): "skip",
    # ConstructorInfo.Invoke with default args
    ("ConstructorInfo", "Invoke", 1): "typeof(byte).GetConstructors()[0].Invoke(new object[0])",
    # FieldInfo via GetFields()[0]
    ("FieldInfo", "GetValue", 1): "typeof(byte).GetFields()[0].GetValue(null)",
    ("FieldInfo", "SetValue", 2): "typeof(byte).GetFields()[0].SetValue(null, (byte)42)",
    # PropertyInfo via GetProperties()[0]
    ("PropertyInfo", "GetValue", 1): "typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null)",
    ("PropertyInfo", "GetValue", 2): "typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].GetValue(null, null)",
    ("PropertyInfo", "SetValue", 2): "typeof(byte).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].SetValue(null, (byte)42)",
    ("PropertyInfo", "get_PropertyType", 0): "0",
    # Exception — StackTrace is null on non-thrown exception
    ("Exception", "get_StackTrace", 0): "((new Exception().StackTrace) ?? \"\")",  # null on non-thrown exception → use null-coalescing
    # Math with invalid precision
    ("Math", "Round", 2): "Math.Round(42.0)",
    # BitConverter with valid array and offset
    ("BitConverter", "ToDouble", 2): "BitConverter.ToDouble(new byte[8], 0)",
    ("BitConverter", "ToInt32", 2): "BitConverter.ToInt32(new byte[4], 0)",
    # Attribute.get_TypeId on default null
    ("Attribute", "get_TypeId", 0): "skip",  # default(Attribute)! is null — expression would NRE
    # Collections — auto-gen handles via INSTANCE_EXPR_MAP entries
    # (List/Dictionary already have instances; HashSet added above)
    ("Dictionary", "get_Count", 0): "new Dictionary<string, int>().Count",
    # Object.Equals on default object - actually works
    # But Object.GetHashCode/ToString/GetType on new object() work fine
    # Thread.Sleep with 42 works fine
    # Thread.get_CurrentThread works fine
    # Wait on completed task - fine
    # Wait(bool, int) - fine
    # IsCompleted - fine
    # Status - fine
    # Event accessors — cannot call add_/remove_ explicitly (CS0571)
    ("ICommand", "add_CanExecuteChanged", 1): "skip",
    ("ICommand", "remove_CanExecuteChanged", 1): "skip",
    ("INotifyCollectionChanged", "add_CollectionChanged", 1): "skip",
    ("INotifyCollectionChanged", "remove_CollectionChanged", 1): "skip",
    ("INotifyDataErrorInfo", "add_ErrorsChanged", 1): "skip",
    ("INotifyDataErrorInfo", "remove_ErrorsChanged", 1): "skip",
    ("INotifyPropertyChanged", "add_PropertyChanged", 1): "skip",
    ("INotifyPropertyChanged", "remove_PropertyChanged", 1): "skip",
    ("INotifyPropertyChanging", "add_PropertyChanging", 1): "skip",
    ("INotifyPropertyChanging", "remove_PropertyChanging", 1): "skip",
    ("ObservableCollection", "add_CollectionChanged", 1): "skip",
    ("ObservableCollection", "remove_CollectionChanged", 1): "skip",
    # AsnReaderOptions struct property setters (CS0131 — can't set property on temporary value)
    ("AsnReaderOptions", "set_SkipSetSortOrderVerification", 1): "skip",
    # KeyedCollection 0-param get_Item (generic param stripped)
    ("KeyedCollection", "get_Item", 0): "skip",
    # ReadOnlyObservableCollection.Empty is static, needs generic type arg
    ("ReadOnlyObservableCollection", "get_Empty", 0): "ReadOnlyObservableCollection<byte>.Empty",
    # FormatterServices.Convert overload ambiguity (Type vs TypeCode)
    ("FormatterConverter", "Convert", 2): "skip",
    # ObjectManager.RecordArrayElementFixup overload ambiguity (int vs int[])
    ("ObjectManager", "RecordArrayElementFixup", 3): "skip",
    # ZipFile ExtractToDirectory/T disambiguation (bool vs Encoding?)
    ("ZipFile", "ExtractToDirectory", 3): "skip",
    ("ZipFile", "ExtractToDirectoryAsync", 4): "skip",
    ("AsnReaderOptions", "set_UtcTimeTwoDigitYearMax", 1): "skip",
    # Asn1Tag static methods called via instance (CS0176)
    ("Asn1Tag", "Decode", 2): "Asn1Tag.Decode(default, out _)",
    ("Asn1Tag", "TryDecode", 3): "Asn1Tag.TryDecode(default, out _, out _)",
    # Asn1Tag operator calls (CS0571 — cannot explicitly call operator or accessor)
    ("Asn1Tag", "op_Equality", 2): "default(Asn1Tag) == default(Asn1Tag)",
    ("Asn1Tag", "op_Inequality", 2): "default(Asn1Tag) != default(Asn1Tag)",
    # AsnWriter ambiguous overloads (CS0121 — default is ambiguous between overloads)
    ("AsnWriter", "EncodedValueEquals", 1): "default(AsnWriter)!.EncodedValueEquals(default(AsnWriter)!)",
    # HashCode.Add<T> cannot infer T from null! (CS0411) with <Nullable>enable</Nullable>
    ("HashCode", "Add", 1): "default(HashCode).Add(new object())",
    ("AsnWriter", "WriteInteger", 2): "default(AsnWriter)!.WriteInteger(42L, default)",
    ("AsnWriter", "WriteNamedBitList", 2): "default(AsnWriter)!.WriteNamedBitList(DayOfWeek.Monday, default)",
    # BrotliDecoder instance methods (CS0120 — called as static but are instance)
    ("BrotliDecoder", "TryDecompress", 3): "BrotliDecoder.TryDecompress(default, default, out _)",  # static method override
    # BrotliEncoder static methods (CS0176 — called via instance but are static)
    ("BrotliEncoder", "GetMaxCompressedLength", 1): "BrotliEncoder.GetMaxCompressedLength(42)",
    ("BrotliEncoder", "TryCompress", 3): "BrotliEncoder.TryCompress(default, default, out _)",
    ("BrotliEncoder", "TryCompress", 5): "BrotliEncoder.TryCompress(default, default, out _, 42, 42)",
    # PipeOptions/PipeScheduler static property accessors (CS0176 — called via instance but are static)
    ("PipeOptions", "get_Default", 0): "PipeOptions.Default",
    ("PipeScheduler", "get_Inline", 0): "PipeScheduler.Inline",
    ("PipeScheduler", "get_ThreadPool", 0): "PipeScheduler.ThreadPool",
    # PipeReader static factory methods (CS0176 — called via instance but are static)
    ("PipeReader", "Create", 1): "PipeReader.Create(default(ReadOnlySequence<byte>))",
    ("PipeReader", "Create", 2): "PipeReader.Create(new MemoryStream(), default(StreamPipeReaderOptions)!)",
    # PipeReader ambiguous overloads (CS0121 — PipeWriter vs Stream for first param)
    ("PipeReader", "CopyToAsync", 2): "default(PipeReader)!.CopyToAsync(default(PipeWriter)!, default)",
    # PipeWriter static factory method (CS0176 — called via instance but are static)
    ("PipeWriter", "Create", 2): "PipeWriter.Create(new MemoryStream(), default(StreamPipeWriterOptions)!)",
    # BrotliStream ambiguous constructors (CS0121 — default ambiguous between CompressionLevel/CompressionMode)
    ("BrotliStream", ".ctor", 2): "new BrotliStream(new MemoryStream(), CompressionLevel.Optimal)",
    ("BrotliStream", ".ctor", 3): "new BrotliStream(new MemoryStream(), CompressionLevel.Optimal, true)",
    # SseItemParser delegate methods — skip (delegate infrastructure, PNSE in .NET 6+)
    ("SseItemParser", "BeginInvoke", 4): "skip",
    ("SseItemParser", "EndInvoke", 1): "skip",
    ("SseItemParser", "Invoke", 2): "skip",
    ("SseItemParser", ".ctor", 2): "skip",
    # SseParser static Create method (CS0176 — called via instance but is static)
    ("SseParser", "Create", 1): "SseParser.Create(new MemoryStream())",
    # SseFormatter static WriteAsync method (CS0176 — called via instance but is static)
    ("SseFormatter", "WriteAsync", 3): "SseFormatter.WriteAsync(null!, new MemoryStream(), default)",
    # SseItem init-only properties — can't assign on struct value (CS8852)
    ("SseItem", "set_EventId", 1): "skip",
    ("SseItem", "set_ReconnectionInterval", 1): "skip",
    # SseParser read-only properties — can't assign (CS0200)
    ("SseParser", "set_LastEventId", 1): "skip",
    ("SseParser", "set_ReconnectionInterval", 1): "skip",
    # IdentityReference operators (CS0571 — cannot explicitly call operator)
    ("IdentityReference", "op_Equality", 2): "default(IdentityReference)! == default(IdentityReference)!",
    ("IdentityReference", "op_Inequality", 2): "default(IdentityReference)! != default(IdentityReference)!",
    # IdentityReferenceCollection 0-param get_Item (CS0443 — empty brackets) and set_Item
    ("IdentityReferenceCollection", "get_Item", 0): "default(IdentityReferenceCollection)![0]",
    ("IdentityReferenceCollection", "set_Item", 1): "skip",
    # NTAccount operators (CS0571)
    ("NTAccount", "op_Equality", 2): "default(NTAccount)! == default(NTAccount)!",
    ("NTAccount", "op_Inequality", 2): "default(NTAccount)! != default(NTAccount)!",
    # SecurityIdentifier operators (CS0571)
    ("SecurityIdentifier", "op_Equality", 2): "default(SecurityIdentifier)! == default(SecurityIdentifier)!",
    ("SecurityIdentifier", "op_Inequality", 2): "default(SecurityIdentifier)! != default(SecurityIdentifier)!",
    # SecurityIdentifier constructor ambiguous between (byte[], int) and (WellKnownSidType, SecurityIdentifier?) (CS0121)
    ("SecurityIdentifier", ".ctor", 2): "new SecurityIdentifier(System.Security.Principal.WellKnownSidType.WorldSid, null)",
    # SafeAccessTokenHandle.InvalidHandle is a static property (CS0176)
    ("SafeAccessTokenHandle", "get_InvalidHandle", 0): "SafeAccessTokenHandle.InvalidHandle",
    # WindowsIdentity static methods (CS0176 — called via instance but are static)
    ("WindowsIdentity", "GetAnonymous", 0): "WindowsIdentity.GetAnonymous()",
    ("WindowsIdentity", "GetCurrent", 0): "WindowsIdentity.GetCurrent()",
    ("WindowsIdentity", "GetCurrent", 1): "WindowsIdentity.GetCurrent(true)",

    # === System.Runtime.InteropServices ===

    # Com types — ref parameters (CLR & is ref not out in C#)
    ("IAdviseSink", "OnDataChange", 2): "skip",
    ("IDataObject", "DAdvise", 4): "skip",
    ("IDataObject", "EnumDAdvise", 1): "skip",
    ("IDataObject", "GetCanonicalFormatEtc", 2): "skip",
    ("IDataObject", "GetData", 2): "skip",
    ("IDataObject", "GetDataHere", 2): "skip",
    ("IDataObject", "QueryGetData", 1): "skip",
    ("IDataObject", "SetData", 3): "skip",
    ("IEnumFORMATETC", "Clone", 1): "skip",
    ("IEnumSTATDATA", "Clone", 1): "skip",

    # ComWrappers static methods
    ("ComWrappers", "GetIUnknownImpl", 3): "ComWrappers.GetIUnknownImpl(out _, out _, out _)",
    ("ComWrappers", "RegisterForMarshalling", 1): "ComWrappers.RegisterForMarshalling(default(ComWrappers)!)",
    ("ComWrappers", "RegisterForTrackerSupport", 1): "ComWrappers.RegisterForTrackerSupport(default(ComWrappers)!)",
    ("ComWrappers", "TryGetComInstance", 2): "ComWrappers.TryGetComInstance(42, out _)",
    ("ComWrappers", "TryGetObject", 2): "ComWrappers.TryGetObject(IntPtr.Zero, out _)",

    # GCHandle static methods (struct — can't call static on default(GCHandle))
    ("GCHandle", "Alloc", 1): "GCHandle.Alloc(42)",
    ("GCHandle", "Alloc", 2): "GCHandle.Alloc(42, GCHandleType.Normal)",
    ("GCHandle", "FromIntPtr", 1): "GCHandle.FromIntPtr(IntPtr.Zero)",
    ("GCHandle", "ToIntPtr", 1): "GCHandle.ToIntPtr(default(GCHandle))",
    # GCHandle operators (CS0571)
    ("GCHandle", "op_Equality", 2): "default(GCHandle) == default(GCHandle)",
    ("GCHandle", "op_Inequality", 2): "default(GCHandle) != default(GCHandle)",
    ("GCHandle", "op_Explicit", 1): "skip",
    # GCHandle set_Target on struct value (CS0131 — can't assign to value type temporary)
    ("GCHandle", "set_Target", 1): "skip",

    # SafeHandle.DangerousAddRef uses ref bool, not out bool (CS1620)
    ("SafeHandle", "DangerousAddRef", 1): "skip",

    # COMException/ExternalException constructors — disambiguate string+Exception vs string+int
    ("COMException", ".ctor", 2): "new COMException(null, default(int))",
    ("ExternalException", ".ctor", 2): "new ExternalException(null, default(int))",

    # StructLayoutAttribute — disambiguate short vs LayoutKind
    ("StructLayoutAttribute", ".ctor", 1): "new StructLayoutAttribute(LayoutKind.Auto)",
    # MarshalAsAttribute — disambiguate short vs UnmanagedType
    ("MarshalAsAttribute", ".ctor", 1): "new MarshalAsAttribute(UnmanagedType.Bool)",

    # Marshal.Copy — overload ambiguity between byte[]/char[]/short[]/int[]
    ("Marshal", "Copy", 4): "skip",

    # Marshal.GetExceptionForHR with Guid& (CLR by-ref, C# by-value) — CS1615
    ("Marshal", "GetExceptionForHR", 3): "Marshal.GetExceptionForHR(42, default(Guid), IntPtr.Zero)",
    # Marshal.QueryInterface with ref Guid/out IntPtr
    ("Marshal", "QueryInterface", 3): "skip",
    # Marshal.ThrowExceptionForHR with Guid& (CLR by-ref, C# by-value)
    ("Marshal", "ThrowExceptionForHR", 3): "skip",

    # ObjectiveCMarshal.Initialize needs function pointers — can't auto-gen
    ("ObjectiveCMarshal", "Initialize", 1): "skip",
    ("ObjectiveCMarshal", "Initialize", 4): "skip",

    # === Generated-marshalling — complex types with pointer/generic constraint issues ===

    # ComExposedClassAttribute<T>: generic constraint IComExposedClass not satisfied by byte
    ("ComExposedClassAttribute", "GetComInterfaceEntries", 1): "skip",
    ("ComExposedClassAttribute", ".ctor", 0): "skip",

    # ComObject instance method called as static
    ("ComObject", "FinalRelease", 0): "skip",

    # ComVariantMarshaller — type used as expression
    ("ComVariantMarshaller", "ConvertToManaged", 1): "skip",
    ("ComVariantMarshaller", "ConvertToUnmanaged", 1): "skip",
    ("ComVariantMarshaller", "Free", 1): "skip",

    # ComVariantMarshaller.RefPropagate — nested type, skipping
    ("ComVariantMarshaller.RefPropagate", "Free", 0): "skip",
    ("ComVariantMarshaller.RefPropagate", "FromManaged", 1): "skip",
    ("ComVariantMarshaller.RefPropagate", "FromUnmanaged", 1): "skip",
    ("ComVariantMarshaller.RefPropagate", "ToManaged", 0): "skip",
    ("ComVariantMarshaller.RefPropagate", "ToUnmanaged", 0): "skip",

    # ExceptionAs*Marshaller<T> — generic constraint issues
    ("ExceptionAsDefaultMarshaller", "ConvertToUnmanaged", 1): "skip",
    ("ExceptionAsHResultMarshaller", "ConvertToUnmanaged", 1): "skip",
    ("ExceptionAsNaNMarshaller", "ConvertToUnmanaged", 1): "skip",

    # IComExposedClass/Details — instance on interface + pointer return
    ("IComExposedClass", "GetComInterfaceEntries", 1): "skip",
    ("IComExposedDetails", "GetComInterfaceEntries", 1): "skip",

    # IIUnknownCacheStrategy — pointer types + nested TableInfo
    ("IIUnknownCacheStrategy", "Clear", 2): "skip",
    ("IIUnknownCacheStrategy", "ConstructTableInfo", 3): "skip",
    ("IIUnknownCacheStrategy", "TryGetTableInfo", 2): "skip",
    ("IIUnknownCacheStrategy", "TrySetTableInfo", 2): "skip",

    # IIUnknownCacheStrategy+TableInfo — nested type, pointer getters/setters
    ("IIUnknownCacheStrategy.TableInfo", "get_ManagedType", 0): "skip",
    ("IIUnknownCacheStrategy.TableInfo", "get_Table", 0): "skip",
    ("IIUnknownCacheStrategy.TableInfo", "get_ThisPtr", 0): "skip",
    ("IIUnknownCacheStrategy.TableInfo", "set_ManagedType", 1): "skip",
    ("IIUnknownCacheStrategy.TableInfo", "set_Table", 1): "skip",
    ("IIUnknownCacheStrategy.TableInfo", "set_ThisPtr", 1): "skip",

    # IIUnknownDerivedDetails — interface instance methods
    ("IIUnknownDerivedDetails", "get_Iid", 0): "skip",
    ("IIUnknownDerivedDetails", "get_Implementation", 0): "skip",
    ("IIUnknownDerivedDetails", "get_ManagedVirtualMethodTable", 0): "skip",

    # IIUnknownInterfaceDetailsStrategy — interface instance methods
    ("IIUnknownInterfaceDetailsStrategy", "GetComExposedTypeDetails", 1): "skip",
    ("IIUnknownInterfaceDetailsStrategy", "GetIUnknownDerivedDetails", 1): "skip",

    # IIUnknownInterfaceType — static abstract interface members
    ("IIUnknownInterfaceType", "get_Iid", 0): "skip",
    ("IIUnknownInterfaceType", "get_ManagedVirtualMethodTable", 0): "skip",

    # IIUnknownStrategy — pointer params
    ("IIUnknownStrategy", "CreateInstancePointer", 1): "skip",
    ("IIUnknownStrategy", "QueryInterface", 3): "skip",
    ("IIUnknownStrategy", "Release", 1): "skip",

    # IUnknownDerivedAttribute<T,TImpl> — generic constraint + arity 2
    ("IUnknownDerivedAttribute", "get_Iid", 0): "skip",
    ("IUnknownDerivedAttribute", "get_Implementation", 0): "skip",
    ("IUnknownDerivedAttribute", "get_ManagedVirtualMethodTable", 0): "skip",
    ("IUnknownDerivedAttribute", ".ctor", 0): "skip",

    # IUnmanagedVirtualMethodTableProvider — interface instance method
    ("IUnmanagedVirtualMethodTableProvider", "GetVirtualMethodTableInfoForKey", 1): "skip",

    # StrategyBasedComWrappers — static property accessors called via instance
    ("StrategyBasedComWrappers", "get_DefaultIUnknownInterfaceDetailsStrategy", 0): "skip",
    ("StrategyBasedComWrappers", "get_DefaultIUnknownStrategy", 0): "skip",

    # UniqueComInterfaceMarshaller<T> — generic + pointer params
    ("UniqueComInterfaceMarshaller", "ConvertToManaged", 1): "skip",
    ("UniqueComInterfaceMarshaller", "ConvertToUnmanaged", 1): "skip",

    # OSPlatform static properties (struct — can't call static on instance)
    ("OSPlatform", "get_Linux", 0): "OSPlatform.Linux",
    ("OSPlatform", "get_OSX", 0): "OSPlatform.OSX",
    ("OSPlatform", "get_Windows", 0): "OSPlatform.Windows",
    # OSPlatform operators (CS0571)

    # VirtualMethodTableInfo — void*/void** pointers need unsafe context
    ("VirtualMethodTableInfo", ".ctor", 2): "skip",
    ("VirtualMethodTableInfo", "Deconstruct", 2): "skip",
    ("VirtualMethodTableInfo", "get_ThisPointer", 0): "skip",
    ("VirtualMethodTableInfo", "get_VirtualMethodTable", 0): "skip",
    ("OSPlatform", "op_Equality", 2): "default(OSPlatform) == default(OSPlatform)",
    ("OSPlatform", "op_Inequality", 2): "default(OSPlatform) != default(OSPlatform)",

    # System.Threading.Tasks.Parallel — all methods involve delegate params, skip
    # System.Threading.Tasks.Parallel — generic methods with delegate params, skip
    ("Parallel", "For", 3): "Parallel.For(0, 42, (int x) => { })",
    ("Parallel", "For", 4): "Parallel.For(0, 42, default(ParallelOptions)!, (int x) => { })",
    ("Parallel", "For", 5): "skip",          # For{TLocal} — generic type param
    ("Parallel", "ForAsync", 3): "skip",     # ForAsync{T} — generic type param
    ("Parallel", "ForEach", 2): "skip",      # ForEach{TSource} — generic type param
    ("Parallel", "ForEach", 3): "skip",
    ("Parallel", "ForEach", 4): "skip",
    ("Parallel", "ForEach", 5): "skip",
    ("Parallel", "ForEachAsync", 2): "skip", # ForEachAsync{TSource} — generic type param
    ("Parallel", "ForEachAsync", 3): "skip",
    ("Parallel", "Invoke", 1): "Parallel.Invoke(() => { })",
    ("Parallel", "Invoke", 2): "skip",       # Invoke(ParallelOptions, Action[]) — array init

    # === System.Text.Json families ===

    # JsonSerializer static class — Deserialize ambiguity (CS0121) and Serialize ambiguity
    ("JsonSerializer", "Deserialize", 2): "JsonSerializer.Deserialize(default(JsonDocument), default(JsonTypeInfo))",
    ("JsonSerializer", "Deserialize", 3): "JsonSerializer.Deserialize(default(JsonDocument), default(Type), default(JsonSerializerOptions))",
    # Serialize with 2 params — TValue overloads; only (Object, JsonTypeInfo) is non-generic
    ("JsonSerializer", "Serialize", 2): "JsonSerializer.Serialize(42, default(JsonTypeInfo)!)",
    ("JsonSerializer", "Serialize", 3): "JsonSerializer.Serialize(42, typeof(byte), default(JsonSerializerOptions)!)",
    ("JsonSerializer", "Serialize", 4): "JsonSerializer.Serialize(new MemoryStream(), 42, typeof(byte), default(JsonSerializerOptions)!)",
    # SerializeAsync ambiguity
    ("JsonSerializer", "SerializeAsync", 4): "JsonSerializer.SerializeAsync(new MemoryStream(), 42, default(JsonTypeInfo)!, default)",
    ("JsonSerializer", "SerializeAsync", 5): "JsonSerializer.SerializeAsync(new MemoryStream(), 42, typeof(byte), default(JsonSerializerOptions)!, default)",

    # JsonElement empty-bracket indexer (CS0443): get_Item with 0 params
    ("JsonElement", "get_Item", 0): "skip",

    # JsonDocument ParseValue/TryParseValue — ref Utf8JsonReader param (CS1620: out _ vs ref)
    ("JsonDocument", "ParseValue", 1): "skip",
    ("JsonDocument", "TryParseValue", 2): "skip",
    # JsonElement ParseValue/TryParseValue — ref Utf8JsonReader param (CS1620)
    ("JsonElement", "ParseValue", 1): "skip",
    ("JsonElement", "TryParseValue", 2): "skip",

    # JsonNode static methods ref Utf8JsonReader (CS1615: can't use out _ for ref param)
    ("JsonNode", "Parse", 2): "skip",
    # JsonNode.Parse with 3 params — ReadOnlySpan<byte> vs Stream ambiguity (CS0121)
    ("JsonNode", "Parse", 3): "skip",
    # JsonNode empty-bracket indexer + setter (CS0443): get_Item/set_Item with 0/1 params
    ("JsonNode", "get_Item", 0): "skip",
    ("JsonNode", "set_Item", 1): "skip",
    # JsonNode generic method (GetValue{T}) — can't resolve type parameter
    ("JsonNode", "GetValue{T}", 0): "skip",
    # JsonNode.set_Parent — assign to read-only property
    ("JsonNode", "set_Parent", 1): "skip",

    # Utf8JsonReader readonly property setters (CS0200)
    ("Utf8JsonReader", "set_ValueSpan", 1): "skip",
    ("Utf8JsonReader", "set_TokenStartIndex", 1): "skip",
    ("Utf8JsonReader", "set_HasValueSequence", 1): "skip",
    ("Utf8JsonReader", "set_ValueIsEscaped", 1): "skip",
    ("Utf8JsonReader", "set_ValueSequence", 1): "skip",
    # Utf8JsonReader.CopyString — Span<byte> vs Span<char> ambiguity (CS0121)
    ("Utf8JsonReader", "CopyString", 1): "skip",

    # Utf8JsonWriter readonly property setters (CS0200) and Reset ambiguity (CS0121)
    ("Utf8JsonWriter", "set_BytesPending", 1): "skip",
    ("Utf8JsonWriter", "set_BytesCommitted", 1): "skip",

    # JsonArray constructors — ambiguity between (JsonNodeOptions?, ReadOnlySpan) and params array (CS0121)
    ("JsonArray", ".ctor", 1): "skip",
    ("JsonArray", ".ctor", 2): "skip",

    # JsonObject constructor — param parsing splits inner comma in KeyValuePair (CS1729)
    ("JsonObject", ".ctor", 3): "skip",

    # JsonSerializerOptions constructor — default ambiguity (CS0121)
    ("JsonSerializerOptions", ".ctor", 1): "new JsonSerializerOptions(default(JsonSerializerDefaults))",

    # JsonConverter`1 methods called on non-generic JsonConverter (CS1061)
    # HandleNull, Read, ReadAsPropertyName exist only on JsonConverter<T>
    # Read/ReadAsPropertyName have ref Utf8JsonReader param — can't use out _ (CS1620)
    ("JsonConverter", "get_HandleNull", 0): "default(JsonConverter<byte>)!.HandleNull",
    ("JsonConverter", "Read", 3): "skip",
    ("JsonConverter", "ReadAsPropertyName", 3): "skip",

    # ReferenceHandler`1::CreateResolver called on non-generic ReferenceHandler (CS1061)
    # byte doesn't satisfy 'where T : ReferenceResolver, new()' constraint (CS0315)
    ("ReferenceHandler", "CreateResolver", 0): "skip",
    # ReferenceHandler`1::.ctor — byte doesn't satisfy constraint (CS0315)
    ("ReferenceHandler", ".ctor", 0): "skip",

    # JsonSerializerOptions.AddContext{TContext} — generic constraint can't satisfy with byte
    ("JsonSerializerOptions", "AddContext{TContext}", 0): "skip",

    # JsonTypeInfo readonly property setters (CS0200)
    ("JsonTypeInfo", "set_IsReadOnly", 1): "skip",
    ("JsonTypeInfo", "set_ConstructorAttributeProvider", 1): "skip",
    # JsonSerializerOptions.TryGetTypeInfo — out JsonTypeInfo& param (CS1620: use 'out' not 'ref')
    ("JsonSerializerOptions", "TryGetTypeInfo", 2): "skip",
}

# Methods to skip from audit verification (e.g. ref-param methods that can't use out _)
_SKIP_AUDIT: dict[tuple[str, str, int], str] = {
}
_GENERIC_ARGS_MAP: dict[int, str] = {
    1: "<byte>",
    2: "<string, int>",
}

# Known acronyms for enum name generation (e.g. "io" → "IO" not "Io").
_ACRONYMS = frozenset({"io", "id", "db", "ui", "os", "ip"})

# Comprehensive usings emitted into every generated source file.
_BASE_USINGS = "\n".join([
    "using System;",
    "using System.IO;",
    "using System.Threading;",
    "using System.Threading.Tasks;",
    "using System.Globalization;",
    "using System.Linq;",
    "using System.Runtime.CompilerServices;",
    "using System.Runtime.InteropServices;",
    "using System.Collections.Generic;",
    "using System.Reflection;",
])

TYPE_DEFAULT_MAP: dict[str, str] = {
    "System.Boolean": "true",
    "System.Byte": "(byte)42",
    "System.SByte": "(sbyte)42",
    "System.Int16": "(short)42",
    "System.UInt16": "(ushort)42",
    "System.Int32": "42",
    "System.UInt32": "42u",
    "System.Int64": "42L",
    "System.UInt64": "42uL",
    "System.Single": "42.0f",
    "System.Double": "42.0",
    "System.Decimal": "42m",
    "System.Char": "'A'",
    "System.String": '"hello"',
    "System.Byte[]": "new byte[] { 1, 2, 3 }",
    "System.Array": "Array.Empty<byte>()",
    "System.IntPtr": "IntPtr.Zero",
    "System.UIntPtr": "UIntPtr.Zero",
    "System.Type": "typeof(byte)",
    "System.Guid": "Guid.Empty",
    "System.Reflection.BindingFlags": "System.Reflection.BindingFlags.Default",
    "System.TimeSpan": "TimeSpan.Zero",
    "System.DateTime": "DateTime.UtcNow",
    "System.DateOnly": "new DateOnly(2024, 1, 1)",
    "System.TimeOnly": "new TimeOnly(0, 0)",
    "System.Uri": 'new Uri("http://localhost")',
    "System.Version": "new Version(1, 0)",
    "System.Object": "null!",
    "System.Enum": "DayOfWeek.Monday",
    "System.ValueType": "42",
    "System.Span`1": "default(Span<byte>)",
    "System.ReadOnlySpan`1": "default(ReadOnlySpan<byte>)",
    "System.Memory`1": "default(Memory<byte>)",
    "System.Collections.Generic.List`1": "new List<int>()",
    "System.Collections.Generic.Dictionary`2": "new Dictionary<string, int>()",
    "System.IO.SeekOrigin": "System.IO.SeekOrigin.Begin",
    "System.Globalization.CompareOptions": "System.Globalization.CompareOptions.None",
    "System.RuntimeType": "typeof(byte).GetType()!",
    "System.RuntimeTypeHandle": "default(System.RuntimeTypeHandle)",
    "System.RuntimeFieldHandle": "default(System.RuntimeFieldHandle)",
    "System.IFormatProvider": "null",
    "System.Threading.ThreadPriority": "System.Threading.ThreadPriority.Normal",
    # --- System.Text.Json families: type default expressions ---
    "System.Buffers.ReadOnlySequence{System.Byte}": "default(ReadOnlySequence<byte>)",
    "System.ReadOnlyMemory{System.Byte}": "default(ReadOnlyMemory<byte>)",
    "System.ReadOnlyMemory{System.Char}": "default(ReadOnlyMemory<char>)",
    "System.Span{System.Byte}": "default(Span<byte>)",
    "System.Span{System.Char}": "default(Span<char>)",
    "System.ReadOnlySpan{System.Byte}": "default(ReadOnlySpan<byte>)",
    "System.ReadOnlySpan{System.Char}": "default(ReadOnlySpan<char>)",
    "System.Buffers.IBufferWriter{System.Byte}": "default(IBufferWriter<byte>)",
    "System.Text.Json.JsonEncodedText": "default(JsonEncodedText)",
    "System.Action": "null!",
    "System.AsyncCallback": "null!",
    "System.IAsyncResult": "null!",
    "System.IO.Stream": "new MemoryStream()",
    "System.Reflection.Assembly": "typeof(byte).Assembly",
    "System.Reflection.MemberInfo": "null!",
    "System.Reflection.MethodInfo": "null!",
    "System.Reflection.Module": "null!",
    "System.Runtime.Serialization.ISurrogateSelector": "null!",
    "System.Runtime.Serialization.SerializationBinder": "null!",
    "System.Runtime.Serialization.SerializationInfo": "null!",
    "System.Runtime.Serialization.StreamingContext": "default",
    "System.Runtime.Serialization.Formatters.FormatterAssemblyStyle": "default",
    "System.Runtime.Serialization.Formatters.TypeFilterLevel": "default",
    "System.Security.Principal.WindowsIdentity": "null!",
    "System.Security.SecureString": "null!",
    "System.Text.Json.JsonSerializerOptions": "null!",
    "System.Text.Json.Serialization.Metadata.JsonTypeInfo": "null!",
    "System.Threading.CancellationToken": "default",
}

TYPE_ALTERNATIVE_MAP: dict[str, str] = {
    "System.Boolean": "false",
    "System.Byte": "(byte)7",
    "System.SByte": "(sbyte)7",
    "System.Int16": "(short)7",
    "System.UInt16": "(ushort)7",
    "System.Int32": "7",
    "System.UInt32": "7u",
    "System.Int64": "7L",
    "System.UInt64": "7uL",
    "System.Single": "7.0f",
    "System.Double": "7.0",
    "System.Decimal": "7m",
    "System.Char": "'Z'",
    "System.String": '"world"',
    "System.Byte[]": "new byte[] { 9, 8, 7 }",
    "System.Array": "Array.Empty<byte>()",
    "System.IntPtr": "IntPtr.Zero",
    "System.UIntPtr": "UIntPtr.Zero",
    "System.Type": "typeof(int)",
    "System.Guid": "Guid.Empty",
    "System.TimeSpan": "TimeSpan.FromDays(1)",
    "System.DateTime": "new DateTime(2024, 6, 15)",
    "System.DateOnly": "new DateOnly(2024, 12, 25)",
    "System.TimeOnly": "new TimeOnly(12, 30)",
    "System.Uri": 'new Uri("http://example.com")',
    "System.Version": "new Version(2, 0)",
    "System.Object": "99",
    "System.Enum": "DayOfWeek.Sunday",
    "System.ValueType": "99",
}




def _parse_method_subject_id(method_subject_id: str) -> dict[str, Any]:
    """Parse a methodSubjectId into its components.

    Format: ``Assembly/Namespace.Type::MethodName:ReturnType(ParamType1,ParamType2)``

    Returns a dict with keys:
        type_path, type_name, method_name, return_type, param_types, type_arity
    """
    # "System.Private.CoreLib/System.Convert::ToChar:System.Char(System.Int32)"
    if "::" not in method_subject_id:
        return {"type_path": "", "type_name": "", "method_name": method_subject_id, "return_type": "", "param_types": [], "type_arity": 0}

    path_sig = method_subject_id.split("::", 1)
    type_path = path_sig[0]  # "System.Private.CoreLib/System.Convert"
    sig = path_sig[1]  # "ToChar:System.Char(System.Int32)"

    # Extract type short name: "System.Convert" -> "Convert"
    raw_type = type_path.split("/", 1)[1] if "/" in type_path else type_path
    # CLR nested type separator (+) → C# dot (.) for valid C# syntax.
    # For nested types, keep parent class in short name:
    #   "ComVariantMarshaller+RefPropagate" → "ComVariantMarshaller.RefPropagate"
    has_nested = "+" in raw_type
    raw_type = raw_type.replace("+", ".")
    if has_nested:
        parts = raw_type.rsplit(".", 2)
        type_name = ".".join(parts[-2:]) if len(parts) >= 2 else raw_type
    else:
        type_name = raw_type.rsplit(".", 1)[-1]

    # Strip CLR backtick suffix: "Nullable`1" -> "Nullable", but record arity
    arity = 0
    bt = re.match(r"^(\w+)`(\d+)$", type_name)
    if bt:
        type_name = bt.group(1)
        arity = int(bt.group(2))

    # Split method+return and params
    paren = sig.find("(")
    if paren < 0:
        return {"type_path": type_path, "type_name": type_name, "method_name": sig, "return_type": "", "param_types": [], "type_arity": arity}

    # Find matching close paren (track depth for correct nesting)
    close_paren = paren
    depth = 0
    for i in range(paren, len(sig)):
        if sig[i] == '(':
            depth += 1
        elif sig[i] == ')':
            depth -= 1
            if depth == 0:
                close_paren = i
                break

    method_and_ret = sig[:paren]
    params_part = sig[paren + 1:close_paren]  # between parens
    ret_suffix = sig[close_paren + 1:]         # after closing paren (e.g. ":System.Void()")

    # Return type can be in two positions:
    #   Format A: "Method:ReturnType(Params)"  — in method_and_ret before the paren
    #   Format B: "Method(Params):ReturnType"   — in ret_suffix after the paren
    colon = method_and_ret.rfind(":")
    if colon >= 0:
        # Format A
        method_name = method_and_ret[:colon]
        # Strip trailing colons (e.g. "AsnDecoder:" from "AsnDecoder::Void(...)")
        method_name = method_name.rstrip(":")
        return_type = method_and_ret[colon + 1:]
    else:
        # Format B
        method_name = method_and_ret
        ret_colon = ret_suffix.rfind(":")
        if ret_colon >= 0:
            return_type = ret_suffix[ret_colon + 1:]
        else:
            return_type = ""

    # CLR constructor subject IDs use the type name instead of ".ctor",
    # e.g. "AsnDecoder::Void(...)". Normalize to ".ctor" for downstream.
    # Also handle the case where the "method" part before return type equals the type name.
    raw_method = method_and_ret[:colon] if colon >= 0 else ""
    raw_method = raw_method.rstrip(":")
    if raw_method == type_name and method_name == type_name:
        method_name = ".ctor"

    # Normalize CLR abbreviated return types to fully-qualified form
    _CLR_TO_SYSTEM = {
        "Void": "System.Void",
    }
    if return_type in _CLR_TO_SYSTEM:
        return_type = _CLR_TO_SYSTEM[return_type]
    # Strip CLR trailing () from return types (e.g. "System.Void()" -> "System.Void")
    if return_type.endswith("()"):
        return_type = return_type[:-2]

    # Parse parameter types (handle nested generics carefully)
    param_types = _split_param_types(params_part)

    return {
        "type_path": type_path,
        "type_name": type_name,
        "method_name": method_name,
        "return_type": return_type,
        "param_types": param_types,
        "type_arity": arity,
    }


def _split_param_types(params_part: str) -> list[str]:
    """Split comma-separated parameter types, respecting generic angle brackets."""
    if not params_part.strip():
        return []
    types: list[str] = []
    depth = 0
    current: list[str] = []
    for ch in params_part:
        if ch in "<[":
            depth += 1
            current.append(ch)
        elif ch in ">]":
            depth -= 1
            current.append(ch)
        elif ch == "," and depth == 0:
            types.append("".join(current).strip())
            current = []
        else:
            current.append(ch)
    if current:
        types.append("".join(current).strip())
    return types


def _is_primitive_type(csharp_type: str) -> bool:
    """Check if a type string is in the primitive set."""
    # Normalize: strip trailing & (ref), * (pointer), ? (nullable), [] (array)
    bare = csharp_type.rstrip("&*?").rstrip("[]").strip()
    if not bare:
        return False
    return bare in PRIMITIVE_SET


def _has_unsafe_param(param_types: list[str]) -> bool:
    """Check if any parameter is ref, pointer, or generic type param."""
    for pt in param_types:
        pt = pt.strip()
        if pt.endswith("&"):
            return True
        if pt.endswith("*"):
            return True
        # Generic type parameter: single uppercase letter or starts with 'T' followed by uppercase
        bare = pt.rstrip("&*?").strip()
        # Skip full type names (contain dots or backticks) — those are concrete types
        if "." in bare or "`" in bare:
            continue
        # Skip array types
        if bare.endswith("[]"):
            bare = bare[:-2]
            if "." in bare or "`" in bare:
                continue
        if bare and ((len(bare) == 1 and bare.isupper()) or (bare.startswith("T") and len(bare) > 1 and bare[1].isupper())):
            return True
    return False


def _is_simple_method(parsed: dict[str, Any]) -> bool:
    """All parameters + return type are primitive, and no ref/pointer/generic."""
    if _has_unsafe_param(parsed["param_types"]):
        return False
    for pt in parsed["param_types"]:
        if not _is_primitive_type(pt):
            return False
    # Return type can be void — still simple if all params are primitive
    ret = parsed["return_type"]
    if ret and ret != "System.Void" and not _is_primitive_type(ret):
        return False
    return True


def _default_expr(csharp_type: str) -> str:
    """Generate a default C# expression for a type."""
    bare = csharp_type.rstrip("&*?").strip()
    if bare in TYPE_DEFAULT_MAP:
        return TYPE_DEFAULT_MAP[bare]
    if bare.endswith("[]"):
        return f"Array.Empty<{bare[:-2]}>()"
    return "null!"


def _has_blocked_param(param_types: list[str]) -> bool:
    """Check if any parameter type is blocked from auto-generation in entrypoint context."""
    for pt in param_types:
        pt = pt.strip()
        bare = pt.rstrip("&*?").strip()
        # Pointer parameters need unsafe context
        if pt.endswith("*"):
            return True
        # Generic type parameter (not concrete type)
        if "." not in bare and "`" not in bare:
            if bare and ((len(bare) == 1 and bare.isupper()) or
                         (bare.startswith("T") and len(bare) > 1 and bare[1].isupper())):
                return True
    return False


def _has_ref_param(param_types: list[str]) -> bool:
    """Check if any parameter is a ref parameter."""
    return any(pt.strip().endswith("&") for pt in param_types)


def _default_expr_for_type(csharp_type: str, type_map: dict[str, str] | None = None) -> str:
    """Generate default expression using an alternative type map.

    Handles out/ref parameters (types ending with &) by prepending 'out '.
    The actual parameter is passed as a discard target for out params,
    or as the default value for non-ref types.
    """
    tm = type_map or TYPE_DEFAULT_MAP

    # Handle out/ref parameters — strip trailing & and prepend "out "
    prefix = ""
    ct = csharp_type.strip()
    if ct.endswith("&"):
        prefix = "out "
        ct = ct[:-1].strip()

    bare = ct.rstrip("*?").strip()
    if bare in tm:
        return prefix + tm[bare]
    if bare.endswith("[]"):
        return prefix + f"Array.Empty<{bare[:-2]}>()"
    return prefix + "default"


def _build_call_expr_with_args(parsed: dict[str, Any], args: str, instance_map: dict[str, str] | None = None) -> str:
    """Build call expression from pre-computed args (shared by ref and non-ref paths)."""
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_count = len(parsed["param_types"])
    im = instance_map or INSTANCE_EXPR_MAP

    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return override

    if method_name in (".ctor", ".cctor"):
        return _build_ctor_expr(type_name, args)

    expr = _try_property_access(type_name, method_name, args)
    if expr is not None:
        return expr

    if (type_name, method_name, param_count) in _STATIC_BY_SIGNATURE:
        return f"{type_name}.{method_name}({args})"

    static_methods = STATIC_METHODS_BY_TYPE.get(type_name, frozenset())
    if method_name in static_methods:
        return f"{type_name}.{method_name}({args})"

    if type_name in STATIC_TYPES:
        return f"{type_name}.{method_name}({args})"

    inst = im.get(type_name)
    if inst is not None:
        return f"{inst}.{method_name}({args})"

    return f"{type_name}.{method_name}({args})"


def _build_call_expr_with_refs(
    parsed: dict[str, Any],
    type_map: dict[str, str] | None = None,
    instance_map: dict[str, str] | None = None,
) -> tuple[str, str]:
    """Build a C# call expression for methods with ref parameters.

    Returns (prelude, call_expr) where prelude contains local variable
    declarations for ref params (empty string if none).
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_types = parsed["param_types"]
    tm = type_map or TYPE_DEFAULT_MAP
    im = instance_map or INSTANCE_EXPR_MAP

    param_count = len(param_types)
    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return ("", override)

    prelude_lines: list[str] = []
    call_args: list[str] = []

    for i, pt in enumerate(param_types):
        pt = pt.strip()
        if pt.endswith("&"):
            # Use out _ (discard) for all & params.
            # The CLR signature doesn't distinguish out from ref, but most & params
            # are out in practice. Methods with true ref params get _METHOD_OVERRIDES.
            call_args.append("out _")
        else:
            call_args.append(_default_expr_for_type(pt, tm))

    prelude = "\n".join(prelude_lines)
    call_expr = _build_call_expr_with_args(parsed, ", ".join(call_args), im)
    return (prelude, call_expr)


def _build_call_expr_with_ref_locals(
    parsed: dict[str, Any],
    type_map: dict[str, str] | None = None,
    instance_map: dict[str, str] | None = None,
) -> tuple[str, str]:
    """Build a C# call expression using named local variables for ref params.

    Unlike _build_call_expr_with_refs which uses out _ (discard), this variant
    declares local variables for each ref param so the method's side effects on
    them are observable (useful when the method mutates ref params).

    Returns (prelude, call_expr) where prelude contains local variable declarations.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    param_types = parsed["param_types"]
    tm = type_map or TYPE_DEFAULT_MAP
    im = instance_map or INSTANCE_EXPR_MAP

    param_count = len(param_types)
    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return ("", override)

    prelude_lines: list[str] = []
    call_args: list[str] = []

    for i, pt in enumerate(param_types):
        pt = pt.strip()
        if pt.endswith("&"):
            base_type = pt.rstrip("&").strip()
            # Convert CLR metadata generics ({}) to C# generics (<>)
            # e.g. System.Span{System.IntPtr} -> System.Span<System.IntPtr>
            base_type = base_type.replace("{", "<").replace("}", ">")
            default_val = _default_expr_for_type(base_type, tm)
            var_name = f"refLocal_{i}"
            prelude_lines.append(f"            {base_type} {var_name} = {default_val};")
            call_args.append(f"out {var_name}")
        else:
            call_args.append(_default_expr_for_type(pt, tm))

    prelude = "\n".join(prelude_lines)
    call_expr = _build_call_expr_with_args(parsed, ", ".join(call_args), im)
    return (prelude, call_expr)


def _cast_return_to_int(ret: str, call_expr: str) -> str:
    """Wrap a call expression so it produces an int for exit code comparison.

    For void returns, wraps in a statement and appends ; return 0;.
    For int returns, passes through.
    For non-int value types, casts to int or bit-converts via unsafe.
    For reference types, returns 1 on non-null.
    """
    ret = ret.strip()
    if not ret or ret == "System.Void":
        return f"{{ {call_expr}; return 0; }}"
    if ret == "System.Int32":
        return f"(int)({call_expr})"
    if ret in ("System.Boolean", "System.Byte", "System.SByte", "System.Int16", "System.UInt16", "System.UInt32"):
        return f"(int)({call_expr})"
    if ret in ("System.Int64", "System.UInt64"):
        return f"(int)(long)({call_expr})"
    if ret in ("System.Single", "System.Double"):
        return f"(int)({call_expr})"
    if ret in ("System.IntPtr", "System.UIntPtr"):
        return f"(int)({call_expr})"
    if ret.endswith("&"):
        return f"(int)({call_expr})"
    # Reference type or other value type — use pointer/address
    return f"(int)({call_expr})"


def _ref_return_expr(parsed: dict[str, Any]) -> str:
    """Build a checksum expression from ref parameter values for void return methods."""
    param_types = parsed["param_types"]
    ref_locals = [
        f"refLocal_{i}"
        for i, pt in enumerate(param_types)
        if pt.strip().endswith("&")
    ]
    if not ref_locals:
        return "0"
    return " ^ ".join(ref_locals)


def _normalize_clr_type(ret: str) -> str:
    """Normalize a CLR type string to a canonical form for matching.

    Strips generic/array curly braces to match against the type maps:
      'System.ReadOnlySpan{System.Byte}' -> 'System.ReadOnlySpan'
      'System.Nullable{System.Int32}'    -> 'System.Nullable'
      'System.Byte{}'                    -> 'System.Byte[]'
    """
    if ret.endswith("{}"):
        return ret[:-2] + "[]"
    brace = ret.find("{")
    if brace >= 0:
        return ret[:brace]
    return ret


def _is_generic_task(ret: str) -> bool:
    """Check if the return type is System.Threading.Tasks.Task<T> in CLR format."""
    return (
        ret == "System.Threading.Tasks.Task"
        or ret.startswith("System.Threading.Tasks.Task{")
    )


def _cast_return_to_int(ret: str, call_expr: str) -> str:
    """Cast a method's return value to int for checksum return."""
    ret = ret.strip()
    if ret == "System.Void" or not ret:
        return call_expr

    # Exact matches for primitive types
    if ret == "System.Int32":
        return call_expr
    if ret in ("System.Int64", "System.UInt64", "System.UInt32"):
        return f"(int)({call_expr})"
    if ret in ("System.Byte", "System.SByte", "System.Int16",
               "System.UInt16", "System.Char"):
        return f"(int)({call_expr})"
    if ret == "System.Boolean":
        return f"(({call_expr}) ? 1 : 0)"
    if ret in ("System.Single", "System.Double", "System.Decimal"):
        return f"(int)({call_expr})"
    if ret in ("System.IntPtr", "System.UIntPtr"):
        return f"(int)({call_expr})"
    if ret == "System.String":
        return f"(({call_expr}).Length)"

    # Value types with deterministic property extraction (avoid non-deterministic GetHashCode)
    if ret == "System.TimeSpan":
        return f"(long)(({call_expr}).Ticks)"
    if ret == "System.Array":
        return f"(int)(((Array)({call_expr})).Length)"
    if ret == "System.GCMemoryInfo":
        return f"(long)(({call_expr}).TotalCommittedBytes)"

    # Types that use .GetHashCode() for checksum
    _HASHCODE_TYPES = frozenset({
        "System.Object", "System.DateTime",
        "System.Exception", "System.Attribute", "System.Enum",
        "System.DateTimeOffset", "System.Collections.BitArray",
        "System.RuntimeTypeHandle", "System.RuntimeMethodHandle",
        "System.RuntimeFieldHandle", "System.Version",
        "System.Reflection.Module", "System.IO.Stream",
        "System.Runtime.CompilerServices.FormattableString",
        "System.Runtime.CompilerServices.RuntimeWrappedException",
        "System.Globalization.DateTimeFormatInfo",
        "System.Globalization.NumberFormatInfo",
        "System.Globalization.CultureInfo",
        "System.Globalization.CompareInfo",
        "System.Globalization.TextInfo",
        "System.Type", "System.Guid", "System.Reflection.MethodInfo",
        "System.Reflection.AssemblyName", "System.Reflection.Assembly",
        "System.Reflection.MemberInfo", "System.Reflection.FieldInfo",
        "System.Reflection.PropertyInfo", "System.Reflection.EventInfo",
        "System.Reflection.ParameterInfo", "System.Reflection.ConstructorInfo",
        "System.Delegate", "System.MulticastDelegate",
        "System.Threading.Tasks.Task", "System.Threading.Thread",
    })
    normal = _normalize_clr_type(ret)
    if normal in _HASHCODE_TYPES:
        return f"(({call_expr}).GetHashCode())"

    # Generic/complex type handling by normalization prefix
    if normal.startswith("System.Span") or normal.startswith("System.ReadOnlySpan"):
        return f"(({call_expr}).GetHashCode())"
    if _is_generic_task(ret):
        return f"(({call_expr}).GetHashCode())"

    # Array types in both CLR format (System.Byte{}) and C# format (byte[])
    if normal.endswith("[]") or ret.endswith("[]"):
        if re.search(r'\.(Count|Length|GetHashCode)\s*\)?\s*$', call_expr.strip()):
            return f"({call_expr})"
        return f"(({call_expr}).Length)"

    # Fallback: use .GetHashCode() which works for all types
    return f"(({call_expr}).GetHashCode())"


def _is_auto_callable(parsed: dict[str, Any]) -> bool:
    """Check whether a method can be auto-generated (not needs-manual).

    Returns False for operators, protected methods, and other patterns that
    require manual implementation.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    if (type_name, method_name) in _NEEDS_MANUAL_METHODS:
        return False
    param_count = len(parsed["param_types"])
    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override == "skip":
        return False
    return True


def _method_skip_reason(parsed: dict[str, Any]) -> str:
    """Return a human-readable skip reason, or empty string if auto-callable."""
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    if (type_name, method_name) in _NEEDS_MANUAL_METHODS:
        return f"needs-manual — {method_name} requires manual implementation"
    param_count = len(parsed["param_types"])
    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override == "skip":
        return f"needs-manual — {method_name} with {param_count} params requires manual implementation"
    return ""


_get_skip_reason = _method_skip_reason


def _build_call_expr(
    parsed: dict[str, Any],
    type_map: dict[str, str] | None = None,
    instance_map: dict[str, str] | None = None,
) -> str:
    """Build a C# method call expression, e.g. ``Convert.ToChar(42)``.

    Handles:
      - Constructors: ``new DateTime(42, 42, 42)``
      - Property accessors: ``inst.Length``, ``inst[42]``
      - Instance methods: ``inst.ToString()``
      - Static methods: ``Convert.ToInt32("42")``

    Args:
        type_map: Optional custom type→default-value map (e.g. TYPE_ALTERNATIVE_MAP).
        instance_map: Optional custom type→instance-expr map.
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    tm = type_map or TYPE_DEFAULT_MAP
    args = ", ".join(_default_expr_for_type(pt, tm) for pt in parsed["param_types"])

    # Check override map first (for known problematic signatures)
    param_count = len(parsed["param_types"])
    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        # Convert.ToXxx(String) and Guid..ctor(String) overrides at
        # (type, method, paramCount=1) apply to ALL 1-param overloads,
        # including (Double) and (Byte[]) variants. Only apply these
        # string-based overrides when the param type IS System.String.
        if (
            param_count == 1
            and parsed["param_types"][0] != "System.String"
            and (
                (type_name == "Convert" and method_name.startswith("To"))
                or (type_name == "Guid" and method_name == ".ctor")
            )
        ):
            pass  # not a string param — fall through to TYPE_DEFAULT_MAP
        else:
            return override

    # WindowsPrincipal.IsInRole with WindowsBuiltInRole — disambiguate from int overload
    if (
        param_count == 1
        and type_name == "WindowsPrincipal"
        and method_name == "IsInRole"
        and parsed["param_types"][0] != "System.String"
        and parsed["param_types"][0] != "System.Int32"
    ):
        return "default(WindowsPrincipal)!.IsInRole(default(System.Security.Principal.SecurityIdentifier))"

    # Constructor: Type..ctor(...) → new Type(...)
    if method_name in (".ctor", ".cctor"):
        return _build_ctor_expr(type_name, args, parsed.get("type_arity", 0))

    # Property accessor (get_Xxx / set_Xxx) → use property/indexer syntax
    expr = _try_property_access(type_name, method_name, args)
    if expr is not None:
        return expr

    # Check param-count-aware static methods (e.g., Object.Equals with 2 params)
    if (type_name, method_name, param_count) in _STATIC_BY_SIGNATURE:
        return f"{type_name}.{method_name}({args})"

    # Known static method on an instance type (e.g. DateTime.Parse)
    static_methods = STATIC_METHODS_BY_TYPE.get(type_name, frozenset())
    if method_name in static_methods:
        return f"{type_name}.{method_name}({args})"

    # Known static-only type
    if type_name in STATIC_TYPES:
        return f"{type_name}.{method_name}({args})"

    # Instance method on a type we know about
    im = instance_map or INSTANCE_EXPR_MAP
    inst = im.get(type_name)
    if inst is not None:
        return f"{inst}.{method_name}({args})"

    # Fallback: assume static
    return f"{type_name}.{method_name}({args})"


# Map generic arity → C# type arguments for constructor/instance expressions
_GENERIC_ARGS_MAP: dict[int, str] = {
    1: "<byte>",
    2: "<byte, byte>",
    3: "<byte, byte, byte>",
}


def _build_ctor_expr(type_name: str, args: str, type_arity: int = 0) -> str:
    """Build constructor expression, handling generic types.

    e.g. ``Span`1`` → ``new Span<byte>(42)``
    """
    if type_arity > 0:
        targs = _GENERIC_ARGS_MAP.get(type_arity, "")
        return f"new {type_name}{targs}({args})"
    m = re.match(r"(\w+)`(\d+)", type_name)
    if m:
        bare = m.group(1)
        arity = int(m.group(2))
        targs = _GENERIC_ARGS_MAP.get(arity, "")
        return f"new {bare}{targs}({args})"
    return f"new {type_name}({args})"


def _concrete_type(csharp_type: str) -> str:
    """Replace CLR backtick generics with concrete C# type arguments.

    e.g. ``Span`1`` → ``Span<byte>``, ``Dictionary`2`` → ``Dictionary<string,int>``
    Also strips trailing backtick for non-generic cases.
    """
    m = re.match(r"(\w+)`(\d+)", csharp_type)
    if m:
        bare = m.group(1)
        arity = int(m.group(2))
        args = _GENERIC_ARGS_MAP.get(arity, "")
        return f"{bare}{args}"
    return csharp_type


def _try_property_access(type_name: str, method_name: str, args: str) -> str | None:
    """If method_name is a property accessor, rewrite to property syntax.

    Handles:
      - get_Length → inst.Length
      - get_Item(N) → inst[N]  (indexer)
      - set_Item(N, V) → inst[N] = V
      - Static properties: get_CurrentThread → Thread.CurrentThread (not inst.CurrentThread)
    """
    static_methods = STATIC_METHODS_BY_TYPE.get(type_name, frozenset())
    if method_name.startswith("get_"):
        prop = method_name[4:]
        # Check if this is a known static property accessor
        if method_name in static_methods:
            return f"{type_name}.{prop}"
        # Indexer: get_Item(N) → inst[N]
        if prop == "Item":
            inst = INSTANCE_EXPR_MAP.get(type_name, type_name)
            return f"{inst}[{args}]"
        # Regular property
        inst = INSTANCE_EXPR_MAP.get(type_name)
        if inst is not None:
            return f"{inst}.{prop}"
        return f"{type_name}.{prop}"
    if method_name.startswith("set_"):
        prop = method_name[4:]
        # Check if this is a known static property accessor
        if method_name in static_methods:
            return f"{type_name}.{prop} = {args}"
        if prop == "Item":
            inst = INSTANCE_EXPR_MAP.get(type_name, type_name)
            if method_name.startswith("set_"):
                # set_Item(idx, val) → inst[idx] = val
                idx, val = args.rsplit(", ", 1) if ", " in args else ("", args)
                return f"{inst}[{idx}] = {val}"
            return f"{inst}[{args}]"
        inst = INSTANCE_EXPR_MAP.get(type_name)
        if inst is not None:
            return f"{inst}.{prop} = {args}"
        return f"{type_name}.{prop} = {args}"
    return None


def _has_non_skip_override(parsed: dict[str, Any]) -> bool:
    """Check if there's a non-skip override expression for this method."""
    override = _METHOD_OVERRIDES.get(
        (parsed["type_name"], parsed["method_name"], len(parsed["param_types"])))
    return override is not None and override != "skip"


def _test_body(parsed: dict[str, Any]) -> str:
    """Generate the body of a test method.

    Returns (body_lines, is_simple, skip_reason).
    """
    if not _is_auto_callable(parsed):
        reason = _method_skip_reason(parsed)
        return (
            f"    // TODO: {reason}",
            False,
            reason,
        )

    if _has_unsafe_param(parsed["param_types"]) and not _has_non_skip_override(parsed):
        # Auto-generate via ref-aware builder
        prelude, call_expr = _build_call_expr_with_refs(parsed)
        ret = parsed["return_type"]
        if ret == "System.Void" or not ret:
            if prelude:
                return (f"{prelude}\n    {call_expr};", False, "")
            return (f"    {call_expr};", False, "")
        if prelude:
            return (f"{prelude}\n    var result = {call_expr};", False, "")
        return (f"    var result = {call_expr};", False, "")

    if _is_simple_method(parsed):
        call_expr = _build_call_expr(parsed)
        ret = parsed["return_type"]
        if ret == "System.Void" or not ret:
            # void method — just call it
            return (
                f"    {call_expr};",
                True,
                "",
            )
        return (
            f"    var result = {call_expr};",
            True,
            "",
        )

    # Mixed method — smoke (at least one complex parameter)
    call_expr = _build_call_expr(parsed)
    ret = parsed["return_type"]
    if ret == "System.Void" or not ret:
        return (
            f"    {call_expr};",
            False,
            "",
        )
    return (
        f"    _ = {call_expr};",
        False,
        "",
    )


def _family_slug(family_id: str) -> str:
    parts = [part for part in str(family_id).split("/") if part]
    if len(parts) < 4:
        return str(family_id).replace("/", "-")
    return "-".join(parts[2:])


def _production_class_name(family_id: str) -> str:
    """Derive production class name (no suffix).
    e.g. convert-char -> ConvertChar
    """
    tail = _family_slug(family_id).split("-")
    return "".join(part.capitalize() for part in tail)


def _class_name(family_id: str) -> str:
    """Derive test class name.
    e.g. convert-char -> ConvertCharTests
    """
    return _production_class_name(family_id) + "Tests"


def _relative(repo_root: Path, path: Path) -> str:
    return path.resolve().relative_to(repo_root.resolve()).as_posix()


def _enum_name(family: dict[str, Any]) -> str:
    explicit = str(family.get("capabilityFamilyEnum") or "").strip()
    if explicit:
        return explicit
    family_id = str(family.get("familyId") or "").strip()
    if not family_id:
        return "None"
    parts = [part for part in family_id.split("/") if part]
    if len(parts) < 4:
        return "None"
    assembly_name = parts[1]
    assembly_part = "".join(token[:1].upper() + token[1:] for token in assembly_name.split(".") if token)
    # Split each family-path segment on hyphens too, so "numeric-conversions" -> "NumericConversions"
    # Handle acronyms: each segment is PascalCased individually, then joined.
    # If a segment is a known acronym (e.g. "io"), uppercase it.
    family_part = "".join(
        _pascalcase_segment(segment)
        for segment in parts[2:]
        if segment
    )
    # Post-processing: fix known acronym patterns across the combined string
    # "Io" at word boundary -> "IO"
    family_part = _fix_acronyms(family_part)
    candidate = f"{assembly_part}_{family_part}".strip("_")
    return candidate or "None"


def _pascalcase_segment(segment: str) -> str:
    """Convert a hyphen-separated segment to PascalCase, handling acronyms."""
    tokens = segment.split("-")
    result = []
    for t in tokens:
        if not t:
            continue
        if t.lower() in _ACRONYMS:
            result.append(t.upper())
        else:
            result.append(t[:1].upper() + t[1:])
    return "".join(result)


def _fix_acronyms(s: str) -> str:
    """Fix known patterns where a PascalCased acronym needs adjustment."""
    # "Io" followed by uppercase -> "IO"
    # e.g. "IoStreamsBasics" -> "IOStreamsBasics"
    # "Io" followed by uppercase -> "IO"
    # e.g. "IoStreamsBasics" -> "IOStreamsBasics"
    s = re.sub(r"(?:^|(?<=_))Io(?=[A-Z])", "IO", s)
    return s


def _namespace_from_type_path(type_path: str) -> str | None:
    """Extract the C# namespace from a CLR type_path (format: AssemblyName/Namespace.TypeName).

    Example: "System.Formats.Asn1/System.Formats.Asn1.AsnDecoder" -> "System.Formats.Asn1"
    """
    if "/" not in type_path:
        return None
    full_type = type_path.split("/", 1)[1]  # "System.Formats.Asn1.AsnDecoder"
    last_dot = full_type.rfind(".")
    if last_dot < 0:
        return None
    return full_type[:last_dot]


_BASE_USING_NAMESPACES = frozenset({
    "System", "System.IO", "System.Threading", "System.Threading.Tasks",
    "System.Globalization", "System.Linq", "System.Runtime.CompilerServices",
    "System.Runtime.InteropServices", "System.Collections.Generic",
    "System.Reflection",
})


def _member_name(prefix: str, method_subject_id: str) -> str:
    sanitized = re.sub(r"[^A-Za-z0-9]+", "_", method_subject_id).strip("_")
    sanitized = sanitized[:120] if sanitized else "Placeholder"
    return f"{prefix}_{sanitized}"


def _handwritten_source(family_id: str, class_name: str) -> str:
    """Generate handwritten partial class source (no test dependencies)."""
    prod_name = class_name.replace("Tests", "")
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "\n"
        f"public partial class {prod_name}\n"
        "{\n"
        "}\n"
    )


def _generated_source(
    family_id: str,
    display_name: str,
    class_name: str,
    *,
    method_subject_ids: list[str],
    capability_family_enum: str,
) -> str:
    """Generate auto-generated partial class with benchmark/host/test methods merged.

    Produces a single file containing:
      - [MethodSubjectId] test methods (no xunit [Fact] — invoked via reflection from test exe)
      - [BenchmarkSubjectId] benchmark methods (static, void)
      - [HotUpdateSubjectId] hotupdate methods (static, void)
    All within the same partial class so they can coexist in one translation unit.
    No xunit dependency — this file is compiled into the src Library project.
    """
    prod_name = class_name.replace("Tests", "")
    extra_usings = ""
    if not method_subject_ids:
        members = (
            "    // No methods to auto-generate for this family.\n"
        )
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = _parse_method_subject_id(method_subject_id)
            type_name = parsed["type_name"]
            method_name = parsed["method_name"]
            body, is_simple, skip_reason = _test_body(parsed)

            # --- Test method ([MethodSubjectId], no [Fact] — discovered via reflection) ---
            mname = _member_name("Method", method_subject_id)
            test_code = (
                f'    [MethodSubjectId("{method_subject_id}")]'
                f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                f'\n    [VerificationRoute(VerificationRoute.Native)]'
                f'\n    public void {mname}()'
                f'\n    {{'
                f'\n{body}'
                f'\n    }}'
            )

            # --- Benchmark method ([BenchmarkSubjectId], static void) ---
            bname = _member_name("Benchmark", method_subject_id)
            if _is_auto_callable(parsed) and not _has_unsafe_param(parsed["param_types"]):
                call_expr = _build_call_expr(parsed)
                ret = parsed["return_type"]
                bench_body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                bench_code = (
                    f'    [BenchmarkSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    [BenchmarkRoute(BenchmarkRoute.Native)]'
                    f'\n    [BenchmarkProfile(BenchmarkProfile.Default)]'
                    f'\n    public static void {bname}() {{ {bench_body}; }}'
                )
            else:
                bench_code = (
                    f'    [BenchmarkSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    public static void {bname}() {{ }}'
                )

            # --- HotUpdate method ([HotUpdateSubjectId], static void, HostToPath) ---
            hname = _member_name("HotUpdate", method_subject_id)
            if _is_auto_callable(parsed) and not _has_unsafe_param(parsed["param_types"]):
                call_expr = _build_call_expr(parsed)
                ret = parsed["return_type"]
                hu_body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                hu_code = (
                    f'    [HotUpdateSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]'
                    f'\n    public static void {hname}() {{ {hu_body}; }}'
                )
            else:
                hu_code = (
                    f'    [HotUpdateSubjectId("{method_subject_id}")]'
                    f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                    f'\n    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]'
                    f'\n    public static void {hname}() {{ }}'
                )

            block = f"    // {type_name}.{method_name}\n{test_code}\n\n{bench_code}\n\n{hu_code}"
            parts.append(block)
        members = "\n\n".join(parts)

        # Collect per-DLL namespaces from method subject IDs, minus already-covered base usings
        extra_ns: set[str] = set()
        for method_subject_id in method_subject_ids:
            parsed = _parse_method_subject_id(method_subject_id)
            ns = _namespace_from_type_path(parsed["type_path"])
            if ns and ns not in _BASE_USING_NAMESPACES:
                extra_ns.add(ns)
        extra_usings = "\n".join(f"using {ns};" for ns in sorted(extra_ns))
        if extra_usings:
            extra_usings += "\n"

    return (
        f"{_BASE_USINGS}\n"
        f"{extra_usings}"
        "using Chaos.TestFramework;\n"
        "\n"
        f"// Auto-generated skeleton for {display_name} ({family_id}).\n"
        f"// Contains test, benchmark, and hotupdate methods merged into one partial class.\n"
        f"// No xunit dependency — [Fact] attributes belong in the test exe project.\n"
        f"public partial class {prod_name}\n"
        "{\n"
        f"{members}\n"
        "}\n"
    )


def _patch_generated_source(
    class_name: str,
    *,
    method_subject_ids: list[str],
    capability_family_enum: str,
) -> str:
    """Generate patch-side auto-generated code (patch/*.AutoGenerated.cs).

    Patch methods verify that the method can be invoked from a patched assembly
    (PatchToHost direction) — they mirror the Host side but with reversed direction.
    """
    patch_class_name = class_name.replace("Tests", "Patch")
    if not method_subject_ids:
        members = "    public static void Placeholder() { }\n"
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = _parse_method_subject_id(method_subject_id)
            call_expr = _build_call_expr(parsed)
            member_name = _member_name("Patch", method_subject_id)
            purpose_comment = f"// Purpose: Verify {parsed['type_name']}.{parsed['method_name']} executes correctly from patch side back to host"
            if not _is_auto_callable(parsed) or _has_unsafe_param(parsed["param_types"]):
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [HotUpdateSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    "    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]\n"
                    f"    public static void {member_name}() {{ }}"
                )
            else:
                ret = parsed["return_type"]
                body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [HotUpdateSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    "    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]\n"
                    f"    public static void {member_name}() {{ {body}; }}"
                )
        members = "\n".join(parts)
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n\n"
        f"// Auto-generated patch-side skeletons for {patch_class_name}.\n"
        f"// Each method exercises a method from the patch side back to the host after hot-update.\n"
        f"public static partial class {patch_class_name}\n"
        "{\n"
        f"{members}"
        "}\n"
    )


def _patch_handwritten_source(patch_class_name: str) -> str:
    """Generate handwritten patch partial class source."""
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "\n"
        f"public static partial class {patch_class_name}\n"
        "{\n"
        "}\n"
    )


def _test_exe_source(class_name: str, *, method_subject_ids: list[str]) -> str:
    """Generate test executable source (managed_test/tests/)."""
    prod_name = class_name.replace("Tests", "")
    return (
        f"{_BASE_USINGS}\n"
        "using Xunit;\n"
        "\n"
        f"public class {class_name}\n"
        "{\n"
        "    [Fact]\n"
        "    public void AllAutoGeneratedMethodsCanExecute()\n"
        "    {\n"
        "        // Smoke test: ensure all auto-generated methods can be discovered\n"
        f"        var type = typeof({prod_name});\n"
        "        Assert.NotNull(type);\n"
        "    }\n"
        "}\n"
    )


def _benchmark_exe_source(class_name: str, *, method_subject_ids: list[str]) -> str:
    """Generate benchmark executable source (managed_test/benchmarks/)."""
    if method_subject_ids:
        call_exprs = []
        for mid in method_subject_ids[:10]:
            parsed = _parse_method_subject_id(mid)
            if _is_auto_callable(parsed) and not _has_unsafe_param(parsed["param_types"]):
                try:
                    call_exprs.append((_member_name("Benchmark", mid), _build_call_expr(parsed)))
                except Exception:
                    pass
        if call_exprs:
            lines = [
                "using System;",
                "using System.Diagnostics;",
                "",
                f"// Auto-generated benchmark harness for {class_name}",
                f"// Runs {len(call_exprs)} methods as quick smoke-benchmark.",
                "class Program",
                "{",
                "    static void Main()",
                "    {",
            ]
            for idx, (name, expr) in enumerate(call_exprs):
                lines.append(f"        // Benchmark entry {idx}: {name}")
                lines.append(f"        var sw{idx} = Stopwatch.StartNew();")
                lines.append(f"        _ = {expr};")
                lines.append(f"        sw{idx}.Stop();")
                lines.append(f'        Console.WriteLine($"Entry {idx}: {{sw{idx}.Elapsed.TotalMilliseconds:F3}} ms");')
            lines.append("    }")
            lines.append("}")
            return "\n".join(lines) + "\n"
    return (
        "using System;\n"
        "\n"
        "class Program\n"
        "{\n"
        "    static void Main()\n"
        "    {\n"
        '        Console.WriteLine("No auto-generable methods for this family.");\n'
        "    }\n"
        "}\n"
    )


def _readme_source(family_id: str, display_name: str) -> str:
    return (
        f"# {display_name}\n\n"
        f"- familyId: `{family_id}`\n"
        "- autogenerated: `*.AutoGenerated.cs`\n"
        "- handwritten: edit `*Tests.cs` only\n"
    )


def generate_family_skeleton(repo_root: Path, *, assembly_name: str, family: dict[str, Any]) -> dict[str, Any]:
    family_id = str(family.get("familyId") or "")
    display_name = str(family.get("displayName") or family_id)
    family_root = repo_root / "verification" / "foundation-dll" / assembly_name / _family_slug(family_id)

    prod_name = _production_class_name(family_id)
    class_name = _class_name(family_id)
    patch_class_name = class_name.replace("Tests", "Patch")
    method_subject_ids = [str(item) for item in list(family.get("methodSubjectIds") or []) if str(item)]
    capability_family_enum = _enum_name(family)

    # --- src/ directory (handwrite + auto-generated merged) ---
    src_dir = family_root / "src"
    handwritten_path = src_dir / f"{prod_name}.cs"
    generated_path = src_dir / f"{prod_name}.AutoGenerated.cs"

    # --- src/patch/ directory ---
    patch_dir = family_root / "src" / "patch"
    patch_handwritten_path = patch_dir / f"{patch_class_name}.cs"
    patch_generated_path = patch_dir / f"{patch_class_name}.AutoGenerated.cs"

    # --- managed_test/ directory (test exe + benchmark exe) ---
    test_exe_dir = family_root / "managed_test" / "tests"
    test_exe_path = test_exe_dir / f"{class_name}.cs"
    benchmark_exe_dir = family_root / "managed_test" / "benchmarks"
    benchmark_exe_path = benchmark_exe_dir / f"{class_name.replace('Tests', 'Benchmarks')}.cs"

    readme_path = family_root / "README.md"
    feature_contract_path = family_root / "capability-family-contract.json"

    # Write handwritten src (only if not exists)
    src_dir.mkdir(parents=True, exist_ok=True)
    if not handwritten_path.exists():
        handwritten_path.write_text(_handwritten_source(family_id, class_name), encoding="utf-8")

    # Write auto-generated src (merged test/benchmark/hotupdate)
    generated_source = _generated_source(
        family_id,
        display_name,
        class_name,
        method_subject_ids=method_subject_ids,
        capability_family_enum=capability_family_enum,
    )
    generated_path.write_text(generated_source, encoding="utf-8")

    # Write patch handwritten (only if not exists)
    patch_dir.mkdir(parents=True, exist_ok=True)
    if not patch_handwritten_path.exists():
        patch_handwritten_path.write_text(_patch_handwritten_source(patch_class_name), encoding="utf-8")

    # Write patch auto-generated
    patch_generated_path.parent.mkdir(parents=True, exist_ok=True)
    patch_generated_path.write_text(
        _patch_generated_source(
            class_name,
            method_subject_ids=method_subject_ids,
            capability_family_enum=capability_family_enum,
        ),
        encoding="utf-8",
    )

    # Write test exe source
    test_exe_dir.mkdir(parents=True, exist_ok=True)
    test_exe_path.write_text(_test_exe_source(class_name, method_subject_ids=method_subject_ids), encoding="utf-8")

    # Write benchmark exe source
    benchmark_exe_dir.mkdir(parents=True, exist_ok=True)
    benchmark_exe_path.write_text(
        _benchmark_exe_source(class_name, method_subject_ids=method_subject_ids),
        encoding="utf-8",
    )

    readme_path.write_text(_readme_source(family_id, display_name), encoding="utf-8")

    return {
        "familyId": family_id,
        "outputRoot": _relative(repo_root, src_dir),
        "artifacts": [
            _relative(repo_root, handwritten_path),
            _relative(repo_root, generated_path),
            _relative(repo_root, patch_handwritten_path),
            _relative(repo_root, patch_generated_path),
            _relative(repo_root, test_exe_path),
            _relative(repo_root, benchmark_exe_path),
            _relative(repo_root, readme_path),
        ],
    }
