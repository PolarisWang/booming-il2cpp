using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Text.Json;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

public sealed class ProbeEmitter
{
    private readonly CSharpSerializer _serializer;
    private readonly CSharpExpressionBuilder _expressionBuilder;

    private static readonly HashSet<string> CastNeededTypes = new(StringComparer.Ordinal)
    {
        "System.Boolean", "System.Byte", "System.SByte", "System.Int16", "System.UInt16",
        "System.Int32", "System.UInt32", "System.Int64", "System.UInt64", "System.Single",
        "System.Double", "System.Decimal", "System.Char",
    };

    public ProbeEmitter(CSharpSerializer serializer, CSharpExpressionBuilder expressionBuilder)
    {
        _serializer = serializer;
        _expressionBuilder = expressionBuilder;
    }

    public IReadOnlyList<ProbeResult> Probe(
        string outputDir,
        string assemblyName,
        string typeFullName,
        IReadOnlyList<MethodSignature> methods,
        IReadOnlyList<IReadOnlyList<ValueSet>> allValueSets,
        string dllPath,
        string targetFramework = "net8.0")
    {
        var probeDir = Path.Combine(outputDir, ".probe");
        Directory.CreateDirectory(probeDir);

        var typeShortName = CSharpSerializer.MapToCSharpType(typeFullName);
        var probeSource = GenerateProbeSource(typeFullName, typeShortName, methods, allValueSets);
        File.WriteAllText(Path.Combine(probeDir, "Probe.cs"), probeSource);

        var probeCsproj = GenerateProbeCsproj(targetFramework, dllPath);
        File.WriteAllText(Path.Combine(probeDir, "Probe.csproj"), probeCsproj);

        Console.Write("  Building probe... ");
        if (!RunDotnetBuild(probeDir, out var buildOk))
        {
            Console.WriteLine("FAILED");
            return Array.Empty<ProbeResult>();
        }
        Console.WriteLine("OK");

        // Single-run probe: one invocation, no determinism check.
        // Three-run determinism detection was too expensive (147 types × 3 runs × 60s = 7+hrs).
        // Serializing with a single run + probing overloads is sufficient to classify
        // deterministic/non-deterministic behavior for the vast majority of methods.
        Console.Write("  Probing... ");
        var run = RunProbe(probeDir);
        Console.WriteLine($"{run.Count} results");

        return MergeResults(methods, allValueSets, run, Array.Empty<ProbeRunResult>(), Array.Empty<ProbeRunResult>());
    }

    private string GenerateProbeSource(
        string typeFullName,
        string typeShortName,
        IReadOnlyList<MethodSignature> methods,
        IReadOnlyList<IReadOnlyList<ValueSet>> allValueSets)
    {
        var sb = new StringBuilder();
        sb.AppendLine("// Auto-generated probe");
        sb.AppendLine("using System;");
        sb.AppendLine("using System.Buffers;");
        sb.AppendLine("using System.Collections;");
        sb.AppendLine("using System.Collections.Concurrent;");
        sb.AppendLine("using System.Collections.Frozen;");
        sb.AppendLine("using System.Collections.Generic;");
        sb.AppendLine("using System.Collections.Immutable;");
        sb.AppendLine("using System.Collections.ObjectModel;");
        sb.AppendLine("using System.ComponentModel;");
        sb.AppendLine("using System.Data;");
        sb.AppendLine("using System.Diagnostics;");
        sb.AppendLine("using System.Formats.Asn1;");
        sb.AppendLine("using System.Globalization;");
        sb.AppendLine("using System.IO;");
        sb.AppendLine("using System.IO.Compression;");
        sb.AppendLine("using System.Linq;");
        sb.AppendLine("using System.Linq.Expressions;");
        sb.AppendLine("using System.Net;");
        sb.AppendLine("using System.Net.Http;");
        sb.AppendLine("using System.Net.Http.Headers;");
        sb.AppendLine("using System.Net.Sockets;");
        sb.AppendLine("using System.Numerics;");
        sb.AppendLine("using System.Reflection;");
        sb.AppendLine("using System.Reflection.Emit;");
        sb.AppendLine("using System.Reflection.Metadata;");
        sb.AppendLine("using System.Runtime.CompilerServices;");
        sb.AppendLine("using System.Runtime.InteropServices;");
        sb.AppendLine("using System.Runtime.Serialization;");
        sb.AppendLine("using System.Security.Claims;");
        sb.AppendLine("using System.Security.Cryptography;");
        sb.AppendLine("using System.Security.Cryptography.X509Certificates;");
        sb.AppendLine("using System.Security.Principal;");
        sb.AppendLine("using System.Text;");
        sb.AppendLine("using System.Text.Json;");
        sb.AppendLine("using System.Text.Json.Nodes;");
        sb.AppendLine("using System.Text.Json.Serialization;");
        sb.AppendLine("using System.Text.Json.Serialization.Metadata;");
        sb.AppendLine("using System.Text.RegularExpressions;");
        sb.AppendLine("using System.Threading;");
        sb.AppendLine("using System.Threading.Channels;");
        sb.AppendLine("using System.Threading.Tasks;");
        sb.AppendLine("using System.Xml;");
        sb.AppendLine("using System.Xml.Schema;");
        sb.AppendLine("using Microsoft.Win32.SafeHandles;");
        sb.AppendLine();
        sb.AppendLine("namespace AutoGeneratedProbe;");
        sb.AppendLine();
        sb.AppendLine("public static class Probe");
        sb.AppendLine("{");
        sb.AppendLine("    public static unsafe void Main()");
        sb.AppendLine("    {");
        sb.AppendLine("        var results = new System.Collections.Generic.List<object>();");

        for (int mi = 0; mi < methods.Count; mi++)
        {
            var method = methods[mi];

            // Methods with ref struct parameters may resolve differently in C# compiler
            // (extension method overloads returning ReadOnlySpan<char> instead of string).
            // Treat the return as potentially unserializable to avoid CS9244.
            // Check both the declared ref struct return AND whether any parameter is a
            // ref struct type (which may cause the compiler to resolve to a ref-struct
            // returning extension method — e.g. string.Trim(ReadOnlySpan<char>) resolves
            // to MemoryExtensions.Trim which returns ReadOnlySpan<char>).
            var isUnsafeReturn = method.IsRefStructReturn ||
                                 HasRefStructParameter(method);

            var instanceExpr = _expressionBuilder.GetInstanceExpression(typeFullName, method.IsStatic);

            // Re-wrap typeShortName for the using-directive context (builder returns C# type name)
            sb.AppendLine($"        // --- {method.Name} ---");

            foreach (var set in allValueSets[mi])
            {
                // Generate variable declarations for out/ref params
                var prelude = new List<string>();
                var finalArgs = new List<string>();
                var outRefVarNames = new List<string>();
                var refStructRefVars = new HashSet<string>();
                int refVarCounter = 0;

                for (int pi = 0; pi < method.Parameters.Count; pi++)
                {
                    var param = method.Parameters[pi];
                    var argExpr = set.ArgumentExpressions[pi];

                    if (param.IsOut || param.IsRef)
                    {
                        var baseTypeName = param.TypeName.EndsWith('&')
                            ? param.TypeName[..^1].Trim()
                            : param.TypeName;
                        var csType = CSharpSerializer.MapToCSharpType(baseTypeName);
                        if (csType == "string") csType = baseTypeName;

                        var varName = $"__ref_{mi}_{set.SetIndex}_{refVarCounter++}";
                        prelude.Add($"            {csType} {varName} = default({csType});");
                        finalArgs.Add(param.IsOut ? $"out {varName}" : $"ref {varName}");
                        outRefVarNames.Add(varName);
                        if (param.IsRefStruct)
                            refStructRefVars.Add(varName);
                    }
                    else
                    {
                        finalArgs.Add(DisambiguateArg(param.TypeName, argExpr));
                    }
                }

                // Join prelude
                var preludeStr = prelude.Count > 0
                    ? string.Join("\n", prelude) + "\n"
                    : "";

                var argsStr = string.Join(", ", finalArgs);
                var genericSuffix = method.GenericTypeArgs is { Count: > 0 }
                    ? $"<{string.Join(", ", method.GenericTypeArgs.Select(CSharpSerializer.MapToCSharpType))}>"
                    : "";
                var callExpr = $"{instanceExpr}.{method.Name}{genericSuffix}({argsStr})";

                // Async unwrapping: Task/ValueTask return types must be awaited
                // so the probe captures the actual result, not the Task wrapper.
                // DllScanner.GetTypeName outputs generic types in angle-bracket format
                // (e.g. "System.Threading.Tasks.Task<System.Int32>"), so check
                // StartsWith("<") alongside the CLR backtick "`1" format.
                var asyncReturnType = method.ReturnTypeName;
                var isPlainAsync = asyncReturnType == "System.Threading.Tasks.Task" ||
                                   asyncReturnType == "System.Threading.Tasks.ValueTask";
                var isGenericAsync = asyncReturnType == "System.Threading.Tasks.Task`1" ||
                                     asyncReturnType == "System.Threading.Tasks.ValueTask`1" ||
                                     asyncReturnType.StartsWith("System.Threading.Tasks.Task<") ||
                                     asyncReturnType.StartsWith("System.Threading.Tasks.ValueTask<");
                if (isPlainAsync || isGenericAsync)
                    callExpr += ".GetAwaiter().GetResult()";

                sb.AppendLine("        try");
                sb.AppendLine("        {");

                // Emit variable declarations (before the method call)
                if (preludeStr.Length > 0)
                    sb.Append(preludeStr);

                if (method.IsVoid || isPlainAsync)
                {
                    sb.AppendLine($"            {callExpr};");

                    // After call: capture out/ref values (skip ref structs — can't serialize)
                    var serializableOutRefs = outRefVarNames.Where(v => !refStructRefVars.Contains(v)).ToList();
                    if (outRefVarNames.Count > 0)
                    {
                        if (serializableOutRefs.Count > 0)
                        {
                            sb.AppendLine("            var _outValues = new System.Collections.Generic.List<string>();");
                            foreach (var vn in serializableOutRefs)
                                sb.AppendLine($"            _outValues.Add(System.Text.Json.JsonSerializer.Serialize({vn}));");
                        }

                        sb.AppendLine("            results.Add(new { MethodIndex = " +
                                       $"{mi}, SetIndex = {set.SetIndex}, " +
                                       "HasException = false, ExceptionType = (string?)null, " +
                                       "ReturnValueJson = (string?)null, ReturnValueType = (string?)null, " +
                                       "ReturnValue = (long?)null, " +
                                       (serializableOutRefs.Count > 0
                                           ? "OutRefValues = (System.Collections.Generic.List<string>?)_outValues });"
                                           : "OutRefValues = (System.Collections.Generic.List<string>?)null });"));
                    }
                    else
                    {
                        sb.AppendLine("            results.Add(new { MethodIndex = " +
                                       $"{mi}, SetIndex = {set.SetIndex}, " +
                                       "HasException = false, ExceptionType = (string?)null, " +
                                       "ReturnValueJson = (string?)null, ReturnValueType = (string?)null, " +
                                       "ReturnValue = (long?)null, " +
                                       "OutRefValues = (System.Collections.Generic.List<string>?)null });");
                    }
                }
                else
                {
                    // Ref struct return (or ref struct param — compiler may resolve
                    // to extension overload with ref struct return): can't serialize
                    if (isUnsafeReturn)
                    {
                        sb.AppendLine($"            {callExpr};");

                        var nonVoidRSOutRefs = outRefVarNames.Where(v => !refStructRefVars.Contains(v)).ToList();
                        if (nonVoidRSOutRefs.Count > 0)
                        {
                            sb.AppendLine("            var _outValues = new System.Collections.Generic.List<string>();");
                            foreach (var vn in nonVoidRSOutRefs)
                                sb.AppendLine($"            _outValues.Add(System.Text.Json.JsonSerializer.Serialize({vn}));");
                        }

                        sb.AppendLine("            results.Add(new { MethodIndex = " +
                                       $"{mi}, SetIndex = {set.SetIndex}, " +
                                       "HasException = false, ExceptionType = (string?)null, " +
                                       "ReturnValueJson = (string?)null, ReturnValueType = (string?)null, " +
                                       "ReturnValue = (long?)null, " +
                                       (nonVoidRSOutRefs.Count > 0
                                           ? "OutRefValues = (System.Collections.Generic.List<string>?)_outValues });"
                                           : "OutRefValues = (System.Collections.Generic.List<string>?)null });"));
                    }
                    else
                    {
                        sb.AppendLine($"            var result = {callExpr};");

                        // After call: capture out/ref values (skip ref structs — can't serialize)
                        var nonVoidSerializableOutRefs = outRefVarNames.Where(v => !refStructRefVars.Contains(v)).ToList();
                        if (outRefVarNames.Count > 0)
                        {
                            if (nonVoidSerializableOutRefs.Count > 0)
                            {
                                sb.AppendLine("            var _outValues = new System.Collections.Generic.List<string>();");
                                foreach (var vn in nonVoidSerializableOutRefs)
                                    sb.AppendLine($"            _outValues.Add(System.Text.Json.JsonSerializer.Serialize({vn}));");
                            }

                            sb.AppendLine("            string _rvJson = System.Text.Json.JsonSerializer.Serialize(result);");
                            sb.AppendLine("            string _rvType = ((object)result)?.GetType().FullName ?? \"\";");
                            sb.AppendLine("            long _rvLong = ToLong(result);");
                            sb.AppendLine("            results.Add(new { MethodIndex = " +
                                           $"{mi}, SetIndex = {set.SetIndex}, " +
                                           "HasException = false, ExceptionType = (string?)null, " +
                                           "ReturnValueJson = _rvJson, ReturnValueType = _rvType, " +
                                           "ReturnValue = (long?)_rvLong, " +
                                           (nonVoidSerializableOutRefs.Count > 0
                                               ? "OutRefValues = (System.Collections.Generic.List<string>?)_outValues });"
                                               : "OutRefValues = (System.Collections.Generic.List<string>?)null });"));
                        }
                        else
                        {
                            sb.AppendLine("            string _rvJson = System.Text.Json.JsonSerializer.Serialize(result);");
                            sb.AppendLine("            string _rvType = ((object)result)?.GetType().FullName ?? \"\";");
                            sb.AppendLine("            long _rvLong = ToLong(result);");
                            sb.AppendLine("            results.Add(new { MethodIndex = " +
                                           $"{mi}, SetIndex = {set.SetIndex}, " +
                                           "HasException = false, ExceptionType = (string?)null, " +
                                           "ReturnValueJson = _rvJson, ReturnValueType = _rvType, " +
                                           "ReturnValue = (long?)_rvLong, " +
                                           "OutRefValues = (System.Collections.Generic.List<string>?)null });");
                        }
                    }
                }
                sb.AppendLine("        }");
                sb.AppendLine("        catch (Exception ex)");
                sb.AppendLine("        {");
                sb.AppendLine("            results.Add(new { MethodIndex = " +
                               $"{mi}, SetIndex = {set.SetIndex}, " +
                               "HasException = true, ExceptionType = ex.GetType().FullName, " +
                               "ReturnValueJson = (string?)null, ReturnValueType = (string?)null, " +
                               "ReturnValue = (long?)null, " +
                               "OutRefValues = (System.Collections.Generic.List<string>?)null });");
                sb.AppendLine("        }");
            }
        }

        sb.AppendLine();
        sb.AppendLine("        var json = System.Text.Json.JsonSerializer.Serialize(results, new JsonSerializerOptions { WriteIndented = false });");
        sb.AppendLine("        Console.WriteLine(\"[PROBE_RESULT]\" + json);");
        sb.AppendLine("    }");
        sb.AppendLine();
        sb.AppendLine("    private static long ToLong(object value)");
        sb.AppendLine("    {");
        sb.AppendLine("        if (value == null) return 0;");
        sb.AppendLine("        if (value is int i) return i;");
        sb.AppendLine("        if (value is long l) return l;");
        sb.AppendLine("        if (value is short s) return s;");
        sb.AppendLine("        if (value is byte b) return b;");
        sb.AppendLine("        if (value is sbyte sb) return sb;");
        sb.AppendLine("        if (value is uint ui) return ui;");
        sb.AppendLine("        if (value is ulong ul) return (long)ul;");
        sb.AppendLine("        if (value is ushort us) return us;");
        sb.AppendLine("        if (value is char c) return c;");
        sb.AppendLine("        if (value is bool bl) return bl ? 1 : 0;");
        sb.AppendLine("        if (value is float f) return (long)f;");
        sb.AppendLine("        if (value is double d) return (long)d;");
        sb.AppendLine("        if (value is decimal dm) return (long)dm;");
        sb.AppendLine("        if (value is string str) return str.Length;");
        sb.AppendLine("        return value.GetHashCode();");
        sb.AppendLine("    }");
        sb.AppendLine("}");

        return sb.ToString();
    }

    private static string GenerateProbeCsproj(string targetFramework, string dllPath)
    {
        var runtimeDir = Path.GetDirectoryName(dllPath) ?? "";
        // Add probing DLL's directory as a reference path so types from the
        // same assembly/package can be resolved even if not in implicit usings.
        // Skip explicit reference for System.Private.* and System.Private.CoreLib
        // — they're internal implementation assemblies. The framework already
        // provides the public types through facade/reference assemblies (e.g.
        // System.Private.Xml → System.Xml.ReaderWriter). Referencing both the
        // private assembly AND the framework facade causes CS0433 ambiguity.
        var dllName = Path.GetFileNameWithoutExtension(dllPath);
        var isNetCoreImpl = dllName == "System.Private.CoreLib" ||
                            dllName.StartsWith("System.Private.", StringComparison.Ordinal);
        var referenceItem = isNetCoreImpl
            ? ""
            : $"  <ItemGroup>\n" +
              $"    <Reference Include=\"{dllName}\" HintPath=\"{dllPath}\" />\n" +
              $"  </ItemGroup>\n";
        return
            "<Project Sdk=\"Microsoft.NET.Sdk\">\n" +
            "  <PropertyGroup>\n" +
            "    <GenerateProgramFile>false</GenerateProgramFile>\n" +
            "    <OutputType>Exe</OutputType>\n" +
            $"    <TargetFramework>{targetFramework}</TargetFramework>\n" +
            "    <ImplicitUsings>enable</ImplicitUsings>\n" +
            "    <Nullable>enable</Nullable>\n" +
            "    <AllowUnsafeBlocks>true</AllowUnsafeBlocks>\n" +
            "  </PropertyGroup>\n" +
           referenceItem +
            "</Project>\n";
    }

    private bool RunDotnetBuild(string probeDir, out bool success)
    {
        success = false;
        try
        {
            var psi = new ProcessStartInfo("dotnet", $"build \"{probeDir}\" -nologo -v q")
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            using var process = Process.Start(psi);
            if (process is null) return false;

            // Drain stdout/stderr FIRST, then wait for exit.
            // Calling WaitForExit before ReadToEnd can deadlock when the
            // child process fills a pipe buffer (default ~4KB) and blocks
            // on writing, preventing it from exiting.
            var output = process.StandardOutput.ReadToEnd();
            var error = process.StandardError.ReadToEnd();

            if (!process.WaitForExit(120_000))
            {
                process.Kill();
                process.WaitForExit(5_000);
            }

            success = process.ExitCode == 0;
            if (!success)
            {
                // Print only first 500 chars of build errors
                var brief = error.Length > 500 ? error[..500] + "..." : error;
                Console.Error.WriteLine(brief);
            }
            return success;
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"[Probe] Build error: {ex.Message}");
            return false;
        }
    }

    private IReadOnlyList<ProbeRunResult> RunProbe(string probeDir)
    {
        var results = new List<ProbeRunResult>();
        try
        {
            var psi = new ProcessStartInfo("dotnet", $"run --project \"{probeDir}\" -nologo")
            {
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
                CreateNoWindow = true,
            };
            using var process = Process.Start(psi);
            if (process is null) return results;

            // Drain stdout FIRST to avoid pipe buffer deadlock, then
            // wait for exit with timeout.
            var output = process.StandardOutput.ReadToEnd();
            var error = process.StandardError.ReadToEnd();

            if (!process.WaitForExit(15_000))
            {
                process.Kill();
                process.WaitForExit(5_000);
            }

            foreach (var line in output.Split('\n', StringSplitOptions.RemoveEmptyEntries))
            {
                var trimmed = line.Trim();
                if (trimmed.StartsWith("[PROBE_RESULT]"))
                {
                    var json = trimmed["[PROBE_RESULT]".Length..];
                    var parsed = JsonSerializer.Deserialize<List<ProbeEntry>>(json);
                    if (parsed is not null)
                    {
                        results.AddRange(parsed.Select((p, i) =>
                            new ProbeRunResult(i, p.MethodIndex, p.SetIndex,
                                p.HasException, p.ExceptionType,
                                p.ReturnValueJson, p.ReturnValueType, p.ReturnValue,
                                p.OutRefValues)));
                    }
                }
            }
        }
        catch (Exception ex)
        {
            Console.Error.WriteLine($"[Probe] Run error: {ex.Message}");
        }
        return results;
    }

    /// <summary>
    /// Check if any parameter of the method is a ref struct type (like ReadOnlySpan&lt;T&gt; or Span&lt;T&gt;).
    /// Such methods are unsafe to serialize because the C# compiler may resolve the call
    /// to a ref-struct-returning extension method (e.g. string.Trim(ReadOnlySpan&lt;char&gt;) resolves
    /// to MemoryExtensions.Trim which returns ReadOnlySpan&lt;char&gt; instead of System.String).
    /// </summary>
    private static bool HasRefStructParameter(MethodSignature method)
    {
        foreach (var p in method.Parameters)
        {
            var pt = p.TypeName.EndsWith('&') ? p.TypeName[..^1].Trim() : p.TypeName;
            // Check known ref struct base names in the parameter type
            var baseName = pt.Split('<')[0].Trim();
            var lastDot = baseName.LastIndexOf('.');
            if (lastDot >= 0) baseName = baseName[(lastDot + 1)..];
            if (baseName is "ReadOnlySpan" or "Span")
                return true;
        }
        return false;
    }

    /// <summary>
    /// Add explicit type cast for boundary/default values to disambiguate overloads.
    /// </summary>
    private static string DisambiguateArg(string paramType, string argExpr)
    {
        // Safety net: untyped null! creates ambiguity for overloaded reference types
        // (e.g. byte[] vs Stream). Cast to the parameter type to disambiguate.
        if (argExpr == "null!")
            return $"({CSharpSerializer.MapToCSharpType(paramType)})null!";

        if (!CastNeededTypes.Contains(paramType))
            return argExpr;

        if (argExpr.Length == 0) return argExpr;
        var first = argExpr[0];
        if (!char.IsDigit(first) && first != '-' && first != '\"')
            return argExpr;
        if (argExpr.StartsWith('('))
            return argExpr;

        var csType = CSharpSerializer.MapToCSharpType(paramType);
        return $"({csType}){argExpr}";
    }

    private static IReadOnlyList<ProbeResult> MergeResults(
        IReadOnlyList<MethodSignature> methods,
        IReadOnlyList<IReadOnlyList<ValueSet>> allValueSets,
        IReadOnlyList<ProbeRunResult> run1,
        IReadOnlyList<ProbeRunResult> run2,
        IReadOnlyList<ProbeRunResult> run3)
    {
        var results = new List<ProbeResult>();
        var run1Lookup = run1.ToDictionary(r => (r.MethodIndex, r.SetIndex));
        var run2Lookup = run2.ToDictionary(r => (r.MethodIndex, r.SetIndex));
        var run3Lookup = run3.ToDictionary(r => (r.MethodIndex, r.SetIndex));

        for (int mi = 0; mi < methods.Count; mi++)
        {
            var method = methods[mi];
            var sets = allValueSets[mi];

            foreach (var set in sets)
            {
                var r1 = run1Lookup.GetValueOrDefault((mi, set.SetIndex));
                var r2 = run2Lookup.GetValueOrDefault((mi, set.SetIndex));
                var r3 = run3Lookup.GetValueOrDefault((mi, set.SetIndex));

                if (r1 is null)
                {
                    results.Add(new ProbeResult(mi, set.SetIndex,
                        BuildSubjectId(method, set.SetIndex),
                        method.IsVoid, false, null, null, null, null, false, null));
                    continue;
                }

                // Single-run mode (run2/run3 empty): treat as deterministic with one caveat —
                // void-returning methods that didn't throw are non-deterministic (no observable
                // side effect to verify across runs). Non-void results and exceptions are treated
                // as deterministic since the single run captured a concrete outcome.
                if (run2.Count == 0 && run3.Count == 0)
                {
                    var isDeterministic = !(method.IsVoid && !r1.HasException);
                    results.Add(new ProbeResult(mi, set.SetIndex,
                        BuildSubjectId(method, set.SetIndex),
                        method.IsVoid,
                        r1.HasException, r1.ExceptionType,
                        r1.ReturnValue, r1.ReturnValueJson, r1.ReturnValueType,
                        isDeterministic, r1.OutRefValues));
                    continue;
                }

                // 3-way determinism: majority vote (2/3 must agree)
                var pairs = new[] { (r1, r2), (r1, r3), (r2, r3) };
                int agreeCount = 0;
                ProbeRunResult? agreedResult = null;
                foreach (var (a, b) in pairs)
                {
                    if (b is null) continue;
                    if (a.HasException == b.HasException &&
                        a.ExceptionType == b.ExceptionType &&
                        JsonValuesEqual(a.ReturnValueJson, b.ReturnValueJson) &&
                        OutRefValuesEqual(a.OutRefValues, b.OutRefValues))
                    {
                        agreeCount++;
                        agreedResult ??= a;
                    }
                }

                // 2/3 majority = deterministic; use the agreed result
                var deterministic = agreeCount >= 2 && agreedResult is not null;
                var chosenResult = agreedResult ?? r1;

                results.Add(new ProbeResult(mi, set.SetIndex,
                    BuildSubjectId(method, set.SetIndex),
                    method.IsVoid,
                    chosenResult.HasException, chosenResult.ExceptionType,
                    chosenResult.ReturnValue,
                    chosenResult.ReturnValueJson, chosenResult.ReturnValueType,
                    deterministic, chosenResult.OutRefValues));
            }
        }

        return results;
    }

    private static bool OutRefValuesEqual(
        System.Collections.Generic.List<string>? a,
        System.Collections.Generic.List<string>? b)
    {
        if (a is null && b is null) return true;
        if (a is null || b is null) return false;
        if (a.Count != b.Count) return false;
        for (int i = 0; i < a.Count; i++)
            if (!JsonValuesEqual(a[i], b[i])) return false;
        return true;
    }

    /// <summary>
    /// Compare two JSON strings structurally (order-independent for object keys).
    /// Falls back to string equality if parsing fails.
    /// </summary>
    private static bool JsonValuesEqual(string? a, string? b)
    {
        if (a == null && b == null) return true;
        if (a == null || b == null) return false;
        try
        {
            using var docA = JsonDocument.Parse(a);
            using var docB = JsonDocument.Parse(b);
            return JsonElementsEqual(docA.RootElement, docB.RootElement);
        }
        catch (JsonException)
        {
            return a == b;
        }
    }

    /// <summary>Recursive structural comparison of two JsonElement trees.</summary>
    private static bool JsonElementsEqual(JsonElement a, JsonElement b)
    {
        if (a.ValueKind != b.ValueKind) return false;
        switch (a.ValueKind)
        {
            case JsonValueKind.Object:
                var aProps = a.EnumerateObject().OrderBy(p => p.Name).ToList();
                var bProps = b.EnumerateObject().OrderBy(p => p.Name).ToList();
                if (aProps.Count != bProps.Count) return false;
                for (int i = 0; i < aProps.Count; i++)
                    if (aProps[i].Name != bProps[i].Name ||
                        !JsonElementsEqual(aProps[i].Value, bProps[i].Value))
                        return false;
                return true;
            case JsonValueKind.Array:
                var aArr = a.EnumerateArray().ToList();
                var bArr = b.EnumerateArray().ToList();
                if (aArr.Count != bArr.Count) return false;
                for (int i = 0; i < aArr.Count; i++)
                    if (!JsonElementsEqual(aArr[i], bArr[i]))
                        return false;
                return true;
            default:
                return a.ToString() == b.ToString();
        }
    }

    internal static string BuildSubjectId(MethodSignature method, int setIndex)
    {
        var paramPart = string.Join(",",
            method.Parameters.Select(p => p.TypeName));
        var returnPart = method.IsVoid ? "System.Void" : method.ReturnTypeName;

        return $"{method.DeclaringTypeFullName}::{method.Name}:{returnPart}({paramPart})__Set{setIndex}";
    }

    private sealed record ProbeEntry(
        int MethodIndex, int SetIndex,
        bool HasException, string? ExceptionType,
        string? ReturnValueJson, string? ReturnValueType, long? ReturnValue,
        System.Collections.Generic.List<string>? OutRefValues);

    private sealed record ProbeRunResult(
        int Index, int MethodIndex, int SetIndex,
        bool HasException, string? ExceptionType,
        string? ReturnValueJson, string? ReturnValueType, long? ReturnValue,
        System.Collections.Generic.List<string>? OutRefValues);
}
