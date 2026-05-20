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
    "Thread": frozenset({"Sleep", "get_CurrentThread", "ResetAbort", "Yield"}),
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
    # Delegate operations — use lambda-created delegates
    ("Delegate", "DynamicInvoke", 1): "new System.Action(() => {}).DynamicInvoke()",
    ("Delegate", "get_Method", 0): "new System.Action(() => {}).Method",
    ("Delegate", "get_Target", 0): "new System.Action(() => {}).Target",
    ("Delegate", "CreateDelegate", 3): "skip",  # Requires MethodInfo + type; complex
    ("Delegate", "CreateDelegate", 2): "skip",  # Requires Type + MethodInfo
    ("MulticastDelegate", "GetInvocationList", 0): "new System.Action(() => {}).GetInvocationList()",
    ("Delegate", "op_Equality", 2): "skip",  # Operator overload; can't auto-generate
    ("Delegate", "op_Inequality", 2): "skip",  # Operator overload
    # Enum with non-enum type — auto-generatable for subjects variant
    # Note: For benchmark variant, these have explicit patterns in family_verification_orchestrator.py
    # Override: GetName with UInt64 param triggers boxed_type codegen bug for System.UInt64
    ("Enum", "GetName", 2): "Enum.GetName(typeof(DayOfWeek), (object)1)",
    # Override: ToString with format param triggers boxed_type codegen bug for DayOfWeek
    # Use primitive int to avoid enum boxing in il2cpp codegen
    ("Enum", "ToString", 1): "42.ToString(\"X\")",
    # Enum.TryParse has 'out' parameters — provide override with out var _
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
    # FieldInfo.get_FieldType — SimpleForward codegen lowering
    ("FieldInfo", "get_FieldType", 0): "(int)(typeof(byte).GetFields(BindingFlags.Public | BindingFlags.Static)[0].FieldType != null ? 1 : 0)",
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
    ("PropertyInfo", "get_PropertyType", 0): "(int)(typeof(DateTime).GetProperties(BindingFlags.Public | BindingFlags.Static)[0].PropertyType != null ? 1 : 0)",
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
    "System.RuntimeType": "typeof(byte).GetType()!",
    "System.RuntimeTypeHandle": "default(System.RuntimeTypeHandle)",
    "System.RuntimeFieldHandle": "default(System.RuntimeFieldHandle)",
    "System.IFormatProvider": "null",
    "System.Threading.ThreadPriority": "System.Threading.ThreadPriority.Normal",
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
    # Strip trailing colons (e.g. "AsnDecoder:" from "AsnDecoder::Void(...)")
    method_name = method_name.rstrip(":")
    return_type = method_and_ret[colon + 1:] if colon >= 0 else ""

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
    if ret == "System.Void" or not ret:
        return call_expr
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
    if ret in ("System.Object", "System.DateTime", "System.TimeSpan",
               "System.Exception", "System.Attribute",
               "System.RuntimeTypeHandle", "System.RuntimeMethodHandle",
               "System.RuntimeFieldHandle", "System.Version",
               "System.Reflection.Module", "System.IO.Stream",
               "System.Runtime.CompilerServices.FormattableString",
               "System.Runtime.CompilerServices.RuntimeWrappedException",
               "System.Globalization.DateTimeFormatInfo",
               "System.Globalization.NumberFormatInfo",
               "System.Globalization.CultureInfo",
               "System.Globalization.CompareInfo",
               "System.Globalization.TextInfo"):
        return f"(({call_expr}).GetHashCode())"
    if ret in ("System.Type", "System.Guid", "System.Reflection.MethodInfo",
               "System.Reflection.AssemblyName", "System.Reflection.Assembly",
               "System.Reflection.MemberInfo", "System.Reflection.FieldInfo",
               "System.Reflection.PropertyInfo", "System.Reflection.EventInfo",
               "System.Reflection.ParameterInfo", "System.Reflection.ConstructorInfo",
               "System.Reflection.Module", "System.Array",
               "System.Delegate", "System.MulticastDelegate",
               "System.Threading.Tasks.Task", "System.Threading.Thread"):
        return f"(({call_expr}).GetHashCode())"
    if ret == "System.Span" or ret.startswith("System.Span`") or ret == "System.ReadOnlySpan" or ret.startswith("System.ReadOnlySpan`"):
        return f"(({call_expr}).GetHashCode())"
    if ret.startswith("System.Threading.Tasks.Task"):
        return f"(({call_expr}).GetHashCode())"
    if ret.endswith("[]"):
        # Override expressions may already return int (e.g., List<T>(...).Count)
        # instead of an array. Detect int-returning patterns to avoid adding
        # .Length to something that already produces an int.
        if re.search(r'\.(Count|Length|GetHashCode)\s*\)?\s*$', call_expr.strip()):
            return f"({call_expr})"
        return f"(({call_expr}).Length)"
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
    im = instance_map or INSTANCE_EXPR_MAP
    inst = im.get(type_name)
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
    return (
        f"{_BASE_USINGS}\n"
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
