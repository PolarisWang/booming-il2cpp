// Auto-generated managed benchmark harness
// Family: boxing-unboxing-casts, Assembly: System.Private.CoreLib
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text.Json;


// _ReflectionRunner — generic reflection-based benchmark invocation.
// Handles method signatures that cannot be expressed as direct C# calls.
// Uses runtime type resolution and Delegate.CreateDelegate for performance.
static class _ReflectionRunner
{
    static readonly Dictionary<string, InvokeEntry> _cache = new();

    struct InvokeEntry
    {
        public System.Reflection.MethodBase Method;
        public object? Instance;
        public System.Reflection.ParameterInfo[] Parameters;
    }

    public static void Invoke(string mid, int i)
    {
        if (!_cache.TryGetValue(mid, out var entry))
        {
            entry = ParseEntry(mid);
            _cache[mid] = entry;
        }

        var args = new object[entry.Parameters.Length];
        for (int p = 0; p < args.Length; p++)
            args[p] = CreateDefault(entry.Parameters[p].ParameterType, i);

        try { entry.Method.Invoke(entry.Instance, args); }
        catch { /* benign — benchmark may throw on edge inputs */ }
    }

    static InvokeEntry ParseEntry(string mid)
    {
        // mid format: "Assembly/Type::Method:RetType(Param1,Param2,...)"
        var assemblySep = mid.IndexOf('/');
        if (assemblySep < 0) throw new ArgumentException($"Invalid mid: {mid}");
        var rest = mid.Substring(assemblySep + 1);

        var methodSep = rest.LastIndexOf("::", StringComparison.Ordinal);
        if (methodSep < 0) throw new ArgumentException($"Invalid mid (no ::): {mid}");
        var typeName = rest.Substring(0, methodSep);

        var sigPart = rest.Substring(methodSep + 2);
        var colonSep = sigPart.IndexOf(':');
        if (colonSep < 0) throw new ArgumentException($"Invalid mid (no :): {mid}");
        var methodName = sigPart.Substring(0, colonSep);

        var retAndParams = sigPart.Substring(colonSep + 1);
        var parenOpen = retAndParams.IndexOf('(');
        var parenClose = retAndParams.LastIndexOf(')');
        var paramStr = parenOpen >= 0 && parenClose > parenOpen
            ? retAndParams.Substring(parenOpen + 1, parenClose - parenOpen - 1)
            : "";

        var type = Type.GetType(typeName, throwOnError: false);
        if (type == null)
        {
            // Fallback: try with "System.Private.CoreLib" assembly
            type = Type.GetType($"{typeName}, System.Private.CoreLib", throwOnError: false);
        }
        if (type == null)
        {
            // Fallback: try with "System.Runtime" assembly
            type = Type.GetType($"{typeName}, System.Runtime", throwOnError: false);
        }
        if (type == null)
        {
            // Fallback: try with "System.Collections" assembly
            type = Type.GetType($"{typeName}, System.Collections", throwOnError: false);
        }
        if (type == null)
            throw new ArgumentException($"Cannot resolve type: {typeName} from mid: {mid}");

        var paramTypeNames = string.IsNullOrEmpty(paramStr)
            ? Array.Empty<string>()
            : paramStr.Split(',').Select(p => p.Trim()).ToArray();
        var paramTypes = paramTypeNames
            .Select(n => Type.GetType(n, throwOnError: false)
                       ?? Type.GetType($"{n}, System.Private.CoreLib", throwOnError: false)
                       ?? typeof(object))
            .ToArray();

        // Handle get_ / set_ / .ctor prefixes
        System.Reflection.MethodBase? method = type.GetMethod(methodName, paramTypes);
        if (method == null && methodName.StartsWith("get_"))
        {
            var prop = type.GetProperty(methodName.Substring(4));
            method = prop?.GetGetMethod();
        }
        if (method == null && methodName.StartsWith("set_"))
        {
            var prop = type.GetProperty(methodName.Substring(4));
            method = prop?.GetSetMethod();
        }
        if (method == null && methodName == ".ctor")
        {
            method = type.GetConstructors().FirstOrDefault(c =>
                c.GetParameters().Length == paramTypes.Length);
        }
        if (method == null)
            throw new ArgumentException($"Cannot resolve method {methodName}({paramStr}) on {typeName}");

        var isStatic = method.IsStatic;
        object? instance = null;
        if (!isStatic)
        {
            try { instance = Activator.CreateInstance(type); }
            catch { instance = null; }
        }

        return new InvokeEntry { Method = method, Instance = instance, Parameters = method.GetParameters() };
    }

    static object CreateDefault(Type t, int i)
    {
        if (t == typeof(int)) return i & 0xFF;
        if (t == typeof(long)) return (long)(i & 0xFF);
        if (t == typeof(short)) return (short)(i & 0xFF);
        if (t == typeof(byte)) return (byte)(i & 0xFF);
        if (t == typeof(char)) return (char)(i & 0xFF);
        if (t == typeof(bool)) return (i & 1) == 0;
        if (t == typeof(float)) return (float)(i & 0xFF);
        if (t == typeof(double)) return (double)(i & 0xFF);
        if (t == typeof(string)) return (i & 1) == 0 ? "A" : "B";
        if (t == typeof(Type)) return typeof(int);
        if (t == typeof(object)) return (object)(i & 0xFF);
        if (t.IsEnum) return Enum.ToObject(t, i & 7);
        if (t.IsArray) return Array.CreateInstance(t.GetElementType()!, 3);
        if (t.IsValueType) try { return Activator.CreateInstance(t)!; } catch { }
        return null;
    }
}

class ManagedBenchmarkHarness
{
    static volatile int _g;

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_0(int i)
{
    System.Convert.ChangeType((object)(i & 0xFF), typeof(int));
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_1(int i)
{
    System.Convert.ChangeType((object)((i + 1) & 0xFF), typeof(int), null);
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_2(int i)
{
    typeof(int).IsAssignableFrom(typeof(int));
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_3(int i)
{
    typeof(int).IsInstanceOfType((object)((i + 3) & 0xFF));
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_4(int i)
{
    typeof(int).IsSubclassOf(typeof(int));
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_5(int i)
{
    typeof(int).IsAssignableTo(typeof(int));
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_6(int i)
{
    try { _ReflectionRunner.Invoke("System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()", i); return false; } catch { return true; }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_7(int i)
{
    System.Runtime.CompilerServices.RuntimeHelpers.GetObjectValue((object)((i + 7) & 0xFF));
    _g++;
    return false;
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_8(int i)
{
    try { System.Array.CreateInstance(typeof(int), (i + 8)); return false; } catch { return true; }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_9(int i)
{
    try { System.Array.CreateInstance(typeof(int), (i + 9), (i + 9)); return false; } catch { return true; }
}

[System.Runtime.CompilerServices.MethodImpl(System.Runtime.CompilerServices.MethodImplOptions.NoInlining)]
static bool H_10(int i)
{
    try { _ReflectionRunner.Invoke("System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)", i); return false; } catch { return true; }
}
    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_0(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_0(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [1] System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_1(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_1(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Private.CoreLib/System.Convert::ChangeType:System.Object(System.Object,System.Type,System.IFormatProvider)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [2] System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_2(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_2(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::IsAssignableFrom:System.Boolean(System.Type)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [3] System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_3(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_3(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::IsInstanceOfType:System.Boolean(System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [4] System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_4(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_4(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::IsSubclassOf:System.Boolean(System.Type)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [5] System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_5(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_5(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Private.CoreLib/System.Type::IsAssignableTo:System.Boolean(System.Type)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [6] System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_6(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_6(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Private.CoreLib/System.Object::MemberwiseClone:System.Object()",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [7] System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_7(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_7(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Private.CoreLib/System.Runtime.CompilerServices.RuntimeHelpers::GetObjectValue:System.Object(System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [8] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_8(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_8(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [9] System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_9(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_9(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Private.CoreLib/System.Array::CreateInstance:System.Array(System.Type,System.Int32,System.Int32)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
            { // [10] System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)
                bool threw = false;
                for (int i = 0; i < 100000; i++) {
                    if (H_10(i)) threw = true;
                }
                double bestMs = double.MaxValue;
                for (int r = 0; r < 3; r++) {
                    var sw = System.Diagnostics.Stopwatch.StartNew();
                    for (int i = 0; i < 100000; i++) {
                    if (H_10(i)) threw = true;
                    }
                    sw.Stop();
                    double ms = sw.Elapsed.TotalMilliseconds;
                    if (ms < bestMs) bestMs = ms;
                }
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Private.CoreLib/System.ValueType::Equals:System.Boolean(System.Object)",
                    ElapsedMilliseconds = bestMs,
                    Iterations = 100000,
                    IsBodyReal = true,
                    IsException = threw,
                });
            }
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
