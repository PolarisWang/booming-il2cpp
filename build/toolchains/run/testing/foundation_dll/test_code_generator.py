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
    "Thread": frozenset({"Sleep", "get_CurrentThread"}),
    "CultureInfo": frozenset({"GetCultureInfo", "get_CurrentCulture", "get_InvariantCulture", "get_CurrentUICulture"}),
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
}

# Method overrides keyed by (type_name, method_name, param_count) → "skip" or custom expression
_METHOD_OVERRIDES: dict[tuple[str, str, int], str] = {
    ("Span", "ToArray", 1): "skip",     # Span.ToArray() takes no args; contract has erroneous param
    ("Span", "get_Empty", 0): "Span<byte>.Empty",
    ("ReadOnlySpan", "get_Empty", 0): "ReadOnlySpan<byte>.Empty",
    # Attribute methods need real MemberInfo/Assembly
    ("Attribute", "GetCustomAttribute", 2): "typeof(byte).Assembly.GetCustomAttribute(typeof(AssemblyDescriptionAttribute))",
    ("Attribute", "GetCustomAttributes", 2): "typeof(byte).Assembly.GetCustomAttributes(typeof(AssemblyDescriptionAttribute))",
    ("Attribute", "GetCustomAttributes", 1): "typeof(byte).Assembly.GetCustomAttributes()",
    ("Attribute", "IsDefined", 2): "typeof(byte).Assembly.IsDefined(typeof(AssemblyDescriptionAttribute))",
    # Array.Sort with null comparer is ambiguous between IComparer<T> and Comparison<T>
    ("Array", "Sort", 2): "Array.Sort<byte>(new byte[1], (IComparer<byte>)null!)",
    # Parsing methods - "hello" is not valid input
    ("DateTime", "Parse", 1): "skip",
    ("TimeSpan", "Parse", 1): "skip",
    ("Guid", "Parse", 1): "skip",
    ("Guid", ".ctor", 1): "skip",           # Guid(string) needs valid format
    ("Boolean", "Parse", 1): "skip",
    ("Byte", "Parse", 1): "skip",
    ("SByte", "Parse", 1): "skip",
    ("Int16", "Parse", 1): "skip",
    ("Int32", "Parse", 1): "skip",
    ("Int64", "Parse", 1): "skip",
    ("Single", "Parse", 1): "skip",
    ("Double", "Parse", 1): "skip",
    ("Decimal", "Parse", 1): "skip",
    ("UInt16", "Parse", 1): "skip",
    ("UInt32", "Parse", 1): "skip",
    ("UInt64", "Parse", 1): "skip",
    # Convert.ToXxx(string) - "hello" is not valid
    ("Convert", "ToByte", 1): "skip",
    ("Convert", "ToSByte", 1): "skip",
    ("Convert", "ToInt16", 1): "skip",
    ("Convert", "ToUInt16", 1): "skip",
    ("Convert", "ToInt32", 1): "skip",
    ("Convert", "ToUInt32", 1): "skip",
    ("Convert", "ToInt64", 1): "skip",
    ("Convert", "ToUInt64", 1): "skip",
    ("Convert", "ToSingle", 1): "skip",
    ("Convert", "ToDouble", 1): "skip",
    ("Convert", "ToDecimal", 1): "skip",
    ("Convert", "ToBoolean", 1): "skip",
    # Convert.ToXxx(string, IFormatProvider) - same issue
    ("Convert", "ToChar", 2): "skip",      # with IFormatProvider
    # Array index-out-of-range with empty arrays
    ("Array", "GetLength", 1): "new int[4, 4].GetLength(0)",
    ("Array", "GetValue", 1): "new int[1].GetValue(0)",
    ("Array", "Copy", 3): "Array.Copy(new byte[4], new byte[4], 4)",
    ("Array", "Copy", 5): "Array.Copy(new byte[4], 0, new byte[4], 0, 4)",
    ("Array", "Clear", 3): "Array.Clear(new byte[4], 0, 4)",
    ("Array", "BinarySearch", 2): "Array.BinarySearch(new byte[4], (byte)42)",
    ("Array", "BinarySearch", 4): "Array.BinarySearch(new byte[4], 0, 4, (byte)42)",
    ("Array", "IndexOf", 2): "Array.IndexOf(new byte[4], (byte)42)",
    ("Array", "LastIndexOf", 2): "Array.LastIndexOf(new byte[4], (byte)42)",
    # Buffer methods with empty arrays
    ("Buffer", "BlockCopy", 5): "skip",    # Array.Empty -> out of range
    ("Buffer", "GetByte", 2): "skip",
    ("Buffer", "SetByte", 3): "skip",
    # Span/ReadOnlySpan/Memory on default (empty) - index out of range
    ("Span", "get_Item", 1): "skip",
    ("Span", "Slice", 1): "skip",
    ("Span", "Slice", 2): "skip",
    ("ReadOnlySpan", "get_Item", 1): "skip",
    ("ReadOnlySpan", "Slice", 1): "skip",
    ("ReadOnlySpan", "Slice", 2): "skip",
    ("Memory", "Slice", 1): "skip",
    # Collections on empty instances
    ("List", "RemoveAt", 1): "skip",
    # Thread on already-running thread
    ("Thread", "Start", 0): "skip",
    # Monitor needs proper sync
    ("Monitor", "Enter", 1): "skip",
    ("Monitor", "Exit", 1): "skip",
    ("Monitor", "Pulse", 1): "skip",
    ("Monitor", "PulseAll", 1): "skip",
    ("Monitor", "Wait", 1): "skip",
    # Delegate operations on null/default delegate
    ("Delegate", "DynamicInvoke", 1): "skip",
    ("Delegate", "CreateDelegate", 3): "skip",
    ("Delegate", "CreateDelegate", 2): "skip",
    ("Delegate", "get_Target", 0): "skip",
    ("Delegate", "get_Method", 0): "skip",
    ("MulticastDelegate", "GetInvocationList", 0): "skip",
    # Enum with non-enum type
    ("Enum", "Parse", 2): "skip",
    ("Enum", "Parse", 3): "skip",
    ("Enum", "IsDefined", 2): "skip",
    ("Enum", "GetValues", 1): "skip",
    ("Enum", "GetNames", 1): "skip",
    ("Enum", "GetName", 2): "skip",
    ("Enum", "GetName", 2): "skip",
    ("Enum", "Format", 3): "skip",
    ("Enum", "ToString", 1): "skip",
    # Type.GetType with unresolvable name
    ("Type", "GetType", 1): "skip",
    ("Type", "GetType", 2): "skip",
    ("Type", "GetType", 3): "skip",
    # Nullable with no value
    ("Nullable", "get_Value", 0): "skip",
    # RuntimeHelpers with invalid handles
    ("RuntimeHelpers", "RunClassConstructor", 1): "skip",
    ("RuntimeHelpers", "InitializeArray", 2): "skip",
    # String operations with out-of-range index
    ("String", "Substring", 1): "\"hello\".Substring(1)",
    ("String", "Substring", 2): "\"hello\".Substring(1, 2)",
    ("String", "Compare", 6): "string.Compare(\"hello\", 0, \"world\", 0, 3, StringComparison.OrdinalIgnoreCase)",
    ("string", "Compare", 6): "string.Compare(\"hello\", 0, \"world\", 0, 3, StringComparison.OrdinalIgnoreCase)",
    ("String", "Compare", 5): "string.Compare(\"hello\", 0, \"world\", 0, 3)",
    ("string", "Compare", 5): "string.Compare(\"hello\", 0, \"world\", 0, 3)",
    # BinaryReader/Writer on default streams
    ("BinaryReader", "ReadString", 0): "skip",
    ("BinaryReader", "ReadInt32", 0): "skip",
    ("BinaryReader", "ReadDouble", 0): "skip",
    ("BinaryWriter", "Write", 1): "skip",  # param count 1 for Write(T)
    ("BinaryWriter", "Write", 1): "skip",  # string overload
    # Stream operations on default MemoryStream
    ("Stream", "Read", 3): "skip",
    ("Stream", "Write", 3): "skip",
    ("Stream", "CopyTo", 1): "skip",
    # CultureInfo with invalid name
    ("CultureInfo", "GetCultureInfo", 1): "skip",
    # DateTime with invalid parameters
    ("DateTime", "DaysInMonth", 2): "DateTime.DaysInMonth(2024, 2)",
    ("DateTime", ".ctor", 3): "new DateTime(2024, 1, 1)",
    ("DateTime", ".ctor", 6): "new DateTime(2024, 1, 1, 0, 0, 0)",
    # Task operations that actually run
    ("Task", "Run", 1): "skip",
    ("Task", "ContinueWith", 1): "skip",
    ("Task", "WhenAny", 1): "skip",
    ("Task", "WhenAll", 1): "skip",
    # Reflection on default(MemberInfo) etc.
    ("MemberInfo", "get_Name", 0): "skip",
    ("MemberInfo", "get_MemberType", 0): "skip",
    ("MemberInfo", "get_DeclaringType", 0): "skip",
    ("MethodBase", "Invoke", 2): "skip",
    ("MethodInfo", "GetParameters", 0): "skip",
    ("MethodInfo", "get_ReturnType", 0): "skip",
    ("ConstructorInfo", "Invoke", 1): "skip",
    ("FieldInfo", "GetValue", 1): "skip",
    ("FieldInfo", "SetValue", 2): "skip",
    ("FieldInfo", "get_FieldType", 0): "skip",
    ("PropertyInfo", "GetValue", 1): "skip",
    ("PropertyInfo", "GetValue", 2): "skip",
    ("PropertyInfo", "SetValue", 2): "skip",
    ("PropertyInfo", "get_PropertyType", 0): "skip",
    # Exception with no stack trace
    ("Exception", "get_StackTrace", 0): "skip",
    # Math with invalid precision
    ("Math", "Round", 2): "Math.Round(42.0)",
    # BitConverter with empty array
    ("BitConverter", "ToDouble", 2): "skip",
    ("BitConverter", "ToInt32", 2): "skip",
    # Guid constructor with byte[] array that might be wrong format
    ("Guid", "ctor", 1): "skip",
    # Task operations that throw
    ("Task", "Run", 1): "skip",
    ("Interlocked", "CompareExchange", 3): "skip",
    ("Interlocked", "Exchange", 2): "skip",
    ("Interlocked", "Increment", 1): "skip",
    ("Interlocked", "Decrement", 1): "skip",
    ("Interlocked", "Add", 2): "skip",
    # Attribute.get_TypeId on default null
    ("Attribute", "get_TypeId", 0): "skip",
    # NullReferenceException from default(Delegate)! etc.
    ("Delegate", "Combine", 2): "skip",
    ("Delegate", "Remove", 2): "skip",
    # Thread operations that fail
    ("Thread", "get_ManagedThreadId", 0): "Thread.CurrentThread.ManagedThreadId",
    # RuntimeWrappedException on new object is fine, but WrappedException should work
    # Collections
    ("List", "Remove", 1): "skip",
    ("Dictionary", "get_Count", 0): "new Dictionary<string, int>().Count",
    ("Dictionary", "ContainsKey", 1): "skip",
    ("Dictionary", "Remove", 1): "skip",
    ("HashSet", "Add", 1): "skip",
    ("HashSet", "Contains", 1): "skip",
    ("HashSet", "Remove", 1): "skip",
    # Object.Equals on default object - actually works
    # But Object.GetHashCode/ToString/GetType on new object() work fine
    # Task.FromResult with generic param - skip (has generic param issue)
    ("Task", "FromResult", 1): "skip",
    # Thread.Sleep with 42 works fine
    # Thread.get_CurrentThread works fine
    # Task.Run with Func-1, 2 generic - skip
    ("Task", "Run", 1): "skip",
    # Wait on completed task - fine
    # Wait(bool, int) - fine
    # IsCompleted - fine
    # Status - fine
}

# Generic arity → concrete type arguments for generated code.
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
    "System.TimeSpan": "TimeSpan.Zero",
    "System.DateTime": "DateTime.UtcNow",
    "System.DateOnly": "new DateOnly(2024, 1, 1)",
    "System.TimeOnly": "new TimeOnly(0, 0)",
    "System.Uri": 'new Uri("http://localhost")',
    "System.Version": "new Version(1, 0)",
    "System.Object": "42",
    "System.Enum": "DayOfWeek.Monday",
    "System.ValueType": "42",
    "System.Span`1": "default(Span<byte>)",
    "System.ReadOnlySpan`1": "default(ReadOnlySpan<byte>)",
    "System.Memory`1": "default(Memory<byte>)",
    "System.Collections.Generic.List`1": "new List<int>()",
    "System.Collections.Generic.Dictionary`2": "new Dictionary<string, int>()",
    "System.IO.SeekOrigin": "System.IO.SeekOrigin.Begin",
    "System.Globalization.CompareOptions": "System.Globalization.CompareOptions.None",
    "System.RuntimeTypeHandle": "default(System.RuntimeTypeHandle)",
    "System.RuntimeFieldHandle": "default(System.RuntimeFieldHandle)",
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

    method_and_ret = sig[:paren]
    params_part = sig[paren + 1:-1]  # strip parens

    # Return type is after the last ':' before the paren
    colon = method_and_ret.rfind(":")
    method_name = method_and_ret[:colon] if colon >= 0 else method_and_ret
    return_type = method_and_ret[colon + 1:] if colon >= 0 else ""

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
        # Delegate-like types
        if bare in ("System.Delegate", "System.MulticastDelegate",
                     "System.EventHandler", "System.EventHandler`1"):
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
    """Generate default expression using an alternative type map."""
    tm = type_map or TYPE_DEFAULT_MAP
    bare = csharp_type.rstrip("&*?").strip()
    if bare in tm:
        return tm[bare]
    if bare.endswith("[]"):
        return f"Array.Empty<{bare[:-2]}>()"
    return "null!"


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
            bare = pt.rstrip("&").strip()
            default_val = _default_expr_for_type(bare, tm)
            local_name = f"refLocal_{i}"
            prelude_lines.append(f"    var {local_name} = {default_val};")
            call_args.append(f"ref {local_name}")
        else:
            call_args.append(_default_expr_for_type(pt, tm))

    prelude = "\n".join(prelude_lines)
    call_expr = _build_call_expr_with_args(parsed, ", ".join(call_args), im)
    return (prelude, call_expr)


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


def _cast_return_to_int(ret: str, call_expr: str) -> str:
    """Cast a method's return value to int for checksum return."""
    ret = ret.strip()
    if ret == "System.Int32":
        return call_expr
    if ret in ("System.Int64", "System.UInt64", "System.UInt32"):
        return f"(int)({call_expr})"
    if ret in ("System.Byte", "System.SByte", "System.Int16",
               "System.UInt16", "System.Char", "System.Boolean"):
        return f"(int)({call_expr})"
    if ret in ("System.Single", "System.Double", "System.Decimal"):
        return f"(int)({call_expr})"
    if ret in ("System.IntPtr", "System.UIntPtr"):
        return f"(int)({call_expr})"
    return f"(int)({call_expr})"


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


def _build_call_expr(parsed: dict[str, Any]) -> str:
    """Build a C# method call expression, e.g. ``Convert.ToChar(42)``.

    Handles:
      - Constructors: ``new DateTime(42, 42, 42)``
      - Property accessors: ``inst.Length``, ``inst[42]``
      - Instance methods: ``inst.ToString()``
      - Static methods: ``Convert.ToInt32("42")``
    """
    type_name = parsed["type_name"]
    method_name = parsed["method_name"]
    args = ", ".join(_default_expr(pt) for pt in parsed["param_types"])

    # Check override map first (for known problematic signatures)
    param_count = len(parsed["param_types"])
    override = _METHOD_OVERRIDES.get((type_name, method_name, param_count))
    if override is not None and override != "skip":
        return override

    # Constructor: Type..ctor(...) → new Type(...)
    if method_name in (".ctor", ".cctor"):
        return _build_ctor_expr(type_name, args)

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
    inst = INSTANCE_EXPR_MAP.get(type_name)
    if inst is not None:
        return f"{inst}.{method_name}({args})"

    # Fallback: assume static
    return f"{type_name}.{method_name}({args})"


def _build_ctor_expr(type_name: str, args: str) -> str:
    """Build constructor expression, handling generic types.

    e.g. ``Span`1`` → ``new Span<byte>(42)``
    """
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
            return f"{inst}[{args}]"
        inst = INSTANCE_EXPR_MAP.get(type_name)
        if inst is not None:
            return f"{inst}.{prop} = {args}"
        return f"{type_name}.{prop} = {args}"
    return None


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

    if _has_unsafe_param(parsed["param_types"]):
        return (
            "    // TODO: needs-manual — ref/pointer/unsafe parameter requires unsafe context",
            False,
            "needs-manual — ref/pointer parameter requires unsafe context",
        )

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
            f"    var result = {call_expr};\n"
            f"    Xunit.Assert.NotNull((object)result);",
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


def _class_name(family_id: str) -> str:
    tail = _family_slug(family_id).split("-")
    return "".join(part.capitalize() for part in tail) + "Tests"


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


def _member_name(prefix: str, method_subject_id: str) -> str:
    sanitized = re.sub(r"[^A-Za-z0-9]+", "_", method_subject_id).strip("_")
    sanitized = sanitized[:120] if sanitized else "Placeholder"
    return f"{prefix}_{sanitized}"


def _handwritten_source(family_id: str, class_name: str) -> str:
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "using Xunit;\n"
        "\n"
        f"[CapabilityTest(\"{family_id}\", IncludeBenchmark = true, IncludeHotUpdate = true)]\n"
        f"public partial class {class_name}\n"
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
    if not method_subject_ids:
        members = (
            "    [Fact(Skip = \"Auto-generated skeleton placeholder. Replace with concrete coverage assertions.\")]\n"
            "    public void TODO_ImplementGeneratedCoverage()\n"
            "    {\n"
            "    }\n"
        )
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = _parse_method_subject_id(method_subject_id)
            body, is_simple, skip_reason = _test_body(parsed)
            method_name = _member_name("Method", method_subject_id)
            skip_attr = f'\n    [Fact(Skip = "{skip_reason}")]' if skip_reason else "\n    [Fact]"
            lines = (
                f'    [MethodSubjectId("{method_subject_id}")]'
                f'\n    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]'
                f'\n    [VerificationRoute(VerificationRoute.Native)]'
                f'{skip_attr}'
                f'\n    public void {method_name}()'
                f'\n    {{'
                f'\n{body}'
                f'\n    }}'
            )
            parts.append(lines)
        members = "\n\n".join(parts)
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n"
        "using Xunit;\n"
        "\n"
        f"// Auto-generated skeleton for {display_name} ({family_id}).\n"
        f"public partial class {class_name}\n"
        "{\n"
        f"{members}"
        "}\n"
    )


def _benchmark_generated_source(
    class_name: str,
    *,
    method_subject_ids: list[str],
    capability_family_enum: str,
) -> str:
    benchmark_class_name = class_name.replace("Tests", "Benchmarks")
    if not method_subject_ids:
        members = "    public static void Placeholder() { }\n"
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = _parse_method_subject_id(method_subject_id)
            call_expr = _build_call_expr(parsed)
            member_name = _member_name("Benchmark", method_subject_id)
            purpose_comment = f"// Purpose: Benchmark native-runtime performance of {parsed['type_name']}.{parsed['method_name']} with typical input"
            if _has_unsafe_param(parsed["param_types"]):
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [BenchmarkSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    f"    public static void {member_name}() {{ }}"
                )
            else:
                ret = parsed["return_type"]
                body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [BenchmarkSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    "    [BenchmarkRoute(BenchmarkRoute.Native)]\n"
                    "    [BenchmarkProfile(BenchmarkProfile.Default)]\n"
                    f"    public static void {member_name}() {{ {body}; }}"
                )
        members = "\n".join(parts)
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n\n"
        f"// Auto-generated benchmark skeletons for {benchmark_class_name}.\n"
        f"// Framework handles timing — body only needs to invoke the method under measurement.\n"
        f"public static partial class {benchmark_class_name}\n"
        "{\n"
        f"{members}"
        "}\n"
    )


def _hotupdate_generated_source(
    class_name: str,
    *,
    method_subject_ids: list[str],
    capability_family_enum: str,
    direction: str = "HostToPatch",
) -> str:
    hotupdate_class_name = class_name.replace("Tests", "HotUpdate")
    if not method_subject_ids:
        members = "    public static void Placeholder() { }\n"
    else:
        parts: list[str] = []
        for method_subject_id in method_subject_ids:
            parsed = _parse_method_subject_id(method_subject_id)
            call_expr = _build_call_expr(parsed)
            member_name = _member_name("HotUpdate", method_subject_id)
            if direction == "PatchToHost":
                purpose_comment = f"// Purpose: Verify {parsed['type_name']}.{parsed['method_name']} executes correctly from the patch side back to the host"
            else:
                purpose_comment = f"// Purpose: Verify {parsed['type_name']}.{parsed['method_name']} executes correctly after hot-update patch (host side)"
            direction_attr = f"HotUpdateDirection(HotUpdateDirection.{direction})"
            if not _is_auto_callable(parsed) or _has_unsafe_param(parsed["param_types"]):
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [HotUpdateSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    f"    [{direction_attr}]\n"
                    f"    public static void {member_name}() {{ }}"
                )
            else:
                ret = parsed["return_type"]
                body = f"_ = {call_expr}" if ret not in ("System.Void", "") else f"{call_expr}"
                parts.append(
                    f"    {purpose_comment}\n"
                    f'    [HotUpdateSubjectId("{method_subject_id}")]\n'
                    f"    [CapabilityFamilyId(CapabilityFamilyId.{capability_family_enum})]\n"
                    f"    [{direction_attr}]\n"
                    f"    public static void {member_name}() {{ {body}; }}"
                )
        members = "\n".join(parts)
    return (
        f"{_BASE_USINGS}\n"
        "using Chaos.TestFramework;\n\n"
        f"// Auto-generated hot-update skeletons for {hotupdate_class_name}.\n"
        f"// Each method exercises a method from the {direction} direction.\n"
        f"public static partial class {hotupdate_class_name}\n"
        "{\n"
        f"{members}"
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


def _project_source(class_name: str, project_reference_path: str) -> str:
    return (
        "<Project Sdk=\"Microsoft.NET.Sdk\">\n"
        "  <PropertyGroup>\n"
        "    <TargetFramework>net8.0</TargetFramework>\n"
        "    <Nullable>enable</Nullable>\n"
        "    <ImplicitUsings>enable</ImplicitUsings>\n"
        "    <IsTestProject>true</IsTestProject>\n"
        f"    <AssemblyName>{class_name}</AssemblyName>\n"
        "  </PropertyGroup>\n"
        "  <ItemGroup>\n"
        f"    <ProjectReference Include=\"{project_reference_path}\" />\n"
        "  </ItemGroup>\n"
        "  <ItemGroup>\n"
        "    <PackageReference Include=\"Microsoft.NET.Test.Sdk\" Version=\"17.11.1\" />\n"
        "    <PackageReference Include=\"xunit\" Version=\"2.9.0\" />\n"
        "    <PackageReference Include=\"xunit.runner.visualstudio\" Version=\"2.8.2\">\n"
        "      <PrivateAssets>all</PrivateAssets>\n"
        "      <IncludeAssets>runtime; build; native; contentfiles; analyzers; buildtransitive</IncludeAssets>\n"
        "    </PackageReference>\n"
        "  </ItemGroup>\n"
        "</Project>\n"
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
    test_dir = family_root / "test"
    test_dir.mkdir(parents=True, exist_ok=True)

    class_name = _class_name(family_id)
    method_subject_ids = [str(item) for item in list(family.get("methodSubjectIds") or []) if str(item)]
    capability_family_enum = _enum_name(family)
    handwritten_path = test_dir / f"{class_name}.cs"
    generated_path = test_dir / f"{class_name}.AutoGenerated.cs"
    project_path = test_dir / f"{class_name}.csproj"
    readme_path = family_root / "README.md"
    verification_test_generated_path = test_dir / f"{class_name}.AutoGenerated.cs"
    benchmark_generated_path = family_root / "benchmark" / f"{class_name.replace('Tests', 'Benchmarks')}.AutoGenerated.cs"
    host_generated_path = family_root / "host" / f"{class_name.replace('Tests', 'HotUpdate')}.AutoGenerated.cs"
    patch_generated_path = family_root / "patch" / f"{class_name.replace('Tests', 'Patch')}.AutoGenerated.cs"

    if not handwritten_path.exists():
        handwritten_path.write_text(_handwritten_source(family_id, class_name), encoding="utf-8")

    generated_source = _generated_source(
        family_id,
        display_name,
        class_name,
        method_subject_ids=method_subject_ids,
        capability_family_enum=capability_family_enum,
    )
    generated_path.write_text(generated_source, encoding="utf-8")
    benchmark_generated_path.parent.mkdir(parents=True, exist_ok=True)
    benchmark_generated_path.write_text(
        _benchmark_generated_source(
            class_name,
            method_subject_ids=method_subject_ids,
            capability_family_enum=capability_family_enum,
        ),
        encoding="utf-8",
    )
    host_generated_path.parent.mkdir(parents=True, exist_ok=True)
    host_generated_path.write_text(
        _hotupdate_generated_source(
            class_name,
            method_subject_ids=method_subject_ids,
            capability_family_enum=capability_family_enum,
        ),
        encoding="utf-8",
    )
    patch_generated_path.parent.mkdir(parents=True, exist_ok=True)
    patch_generated_path.write_text(
        _patch_generated_source(
            class_name,
            method_subject_ids=method_subject_ids,
            capability_family_enum=capability_family_enum,
        ),
        encoding="utf-8",
    )

    readme_path.write_text(_readme_source(family_id, display_name), encoding="utf-8")

    return {
        "familyId": family_id,
        "outputRoot": _relative(repo_root, test_dir),
        "artifacts": [
            _relative(repo_root, handwritten_path),
            _relative(repo_root, generated_path),
            _relative(repo_root, project_path),
            _relative(repo_root, readme_path),
            _relative(repo_root, verification_test_generated_path),
            _relative(repo_root, benchmark_generated_path),
            _relative(repo_root, host_generated_path),
            _relative(repo_root, patch_generated_path),
        ],
    }
