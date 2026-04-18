using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text.Json;
using Chaos.TestFramework.Runtime;

namespace Chaos.IL2CPP.Tools.Benchmark.WorkloadEntry.PerfHarness;

internal static class Program
{
    private static int Main(string[] args)
    {
        var options = HarnessOptions.Parse(args);
        Trace($"parsed options mode={options.Mode} collection={options.CollectionPath} entryIndex={options.EntryIndex} host={options.HostAssemblyPath}");
        if (options.HasCollectionSelection)
        {
            ChaosTestCollectionLoader.EnsureEntryExists(options.CollectionPath, ChaosManagedHostKind.Benchmark, options.EntryIndex);
        }

        var descriptor = ResolveWorkloadDescriptor(options);
        Trace($"resolved descriptor subject={descriptor.SubjectId} workload={descriptor.WorkloadEntry}");
        var workload = LoadWorkload(options, descriptor);
        Trace("loaded workload delegate");

        var stopwatch = Stopwatch.StartNew();
        long checksum = 0;
        for (var index = 0; index < options.Iterations; index += 1)
        {
            Trace($"invoke iteration={index}");
            checksum += CoerceChecksum(workload());
            Trace($"completed iteration={index}");
        }
        stopwatch.Stop();

        var payload = new
        {
            harness = "benchmark-workload-entry",
            mode = options.Mode,
            subjectId = descriptor.SubjectId,
            workloadEntry = descriptor.WorkloadEntry,
            iterations = options.Iterations,
            elapsedMilliseconds = stopwatch.Elapsed.TotalMilliseconds,
            opsPerSecond = options.Iterations / Math.Max(stopwatch.Elapsed.TotalSeconds, 0.000001d),
            checksum = Math.Abs(checksum % 10000),
        };

        Console.WriteLine(JsonSerializer.Serialize(payload));
        return 0;
    }

    private static WorkloadDescriptor ResolveWorkloadDescriptor(HarnessOptions options)
    {
        if (options.HasCollectionSelection)
        {
            return ResolveCollectionWorkloadDescriptor(options);
        }

        if (!string.IsNullOrWhiteSpace(options.DeclaringType) && !string.IsNullOrWhiteSpace(options.MethodName))
        {
            var declaredMethodSignature = string.IsNullOrWhiteSpace(options.MethodSignature)
                ? options.MethodName + "()"
                : options.MethodSignature;
            var declaredSubjectId = !string.IsNullOrWhiteSpace(options.AssemblyName)
                ? options.AssemblyName
                : options.DeclaringType;
            var declaredWorkloadEntry = BuildWorkloadEntry(options.AssemblyName, options.DeclaringType, declaredMethodSignature);
            return new WorkloadDescriptor(
                declaredSubjectId,
                declaredWorkloadEntry,
                options.DeclaringType,
                options.MethodName,
                declaredMethodSignature,
                options.AssemblyName);
        }

        if (string.IsNullOrWhiteSpace(options.WorkloadEntry))
        {
            throw new ArgumentException("missing --workload-entry or declared benchmark metadata");
        }

        var workloadEntry = options.WorkloadEntry;
        var separatorIndex = workloadEntry.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0 || separatorIndex >= workloadEntry.Length - 2)
        {
            throw new ArgumentException($"invalid workload entry: {workloadEntry}");
        }

        var typeName = workloadEntry[..separatorIndex].Replace("/", ".", StringComparison.Ordinal);
        var methodSignature = workloadEntry[(separatorIndex + 2)..];
        var signatureIndex = methodSignature.IndexOf("(", StringComparison.Ordinal);
        var methodName = signatureIndex >= 0 ? methodSignature[..signatureIndex] : methodSignature;
        var subjectId = workloadEntry.Split('/', 2, StringSplitOptions.RemoveEmptyEntries)[0];
        return new WorkloadDescriptor(subjectId, workloadEntry, typeName, methodName, methodSignature, subjectId);
    }

    private static WorkloadDescriptor ResolveCollectionWorkloadDescriptor(HarnessOptions options)
    {
        var entry = ResolveDeclaredBenchmarkEntry(options.CollectionPath, options.EntryIndex);
        var workloadEntry = !string.IsNullOrWhiteSpace(options.WorkloadEntry)
            ? options.WorkloadEntry
            : BuildWorkloadEntry(entry.AssemblyName, entry.DeclaringType, entry.MethodSignature);
        var subjectId = !string.IsNullOrWhiteSpace(workloadEntry)
            ? workloadEntry.Split('/', 2, StringSplitOptions.RemoveEmptyEntries)[0]
            : entry.AssemblyName;
        return new WorkloadDescriptor(
            subjectId,
            workloadEntry,
            entry.DeclaringType,
            entry.MethodName,
            entry.MethodSignature,
            entry.AssemblyName);
    }

    private static string BuildWorkloadEntry(string assemblyName, string declaringType, string methodSignature)
    {
        if (string.IsNullOrWhiteSpace(assemblyName) || string.IsNullOrWhiteSpace(declaringType) || string.IsNullOrWhiteSpace(methodSignature))
        {
            return string.Empty;
        }

        var typeName = declaringType[(declaringType.LastIndexOf('.') + 1)..];
        return $"{assemblyName}/{typeName}::{methodSignature}";
    }

    private static string ResolveWorkloadAssemblyPath(HarnessOptions options, WorkloadDescriptor descriptor)
    {
        if (options.HasCollectionSelection)
        {
            if (string.IsNullOrWhiteSpace(options.HostAssemblyPath))
            {
                throw new ArgumentException("missing --host-assembly");
            }

            var hostAssemblyDirectory = Path.GetDirectoryName(options.HostAssemblyPath) ?? string.Empty;
            if (!string.IsNullOrWhiteSpace(descriptor.AssemblyName))
            {
                var siblingAssemblyPath = Path.Combine(hostAssemblyDirectory, descriptor.AssemblyName + ".dll");
                if (File.Exists(siblingAssemblyPath))
                {
                    return siblingAssemblyPath;
                }
            }

            return options.HostAssemblyPath;
        }

        if (string.IsNullOrWhiteSpace(options.AssemblyPath))
        {
            throw new ArgumentException("missing --assembly");
        }

        return options.AssemblyPath;
    }

    private static Func<object?> LoadWorkload(HarnessOptions options, WorkloadDescriptor descriptor)
    {
        if (options.HasCollectionSelection && !string.IsNullOrWhiteSpace(options.BindingManifestPath))
        {
            return LoadHostWorkload(options);
        }

        return LoadReflectedWorkload(ResolveWorkloadAssemblyPath(options, descriptor), descriptor);
    }

    private static Func<object?> LoadHostWorkload(HarnessOptions options)
    {
        if (string.IsNullOrWhiteSpace(options.HostAssemblyPath))
        {
            throw new ArgumentException("missing --host-assembly");
        }

        var hostAssembly = Assembly.LoadFrom(options.HostAssemblyPath);
        Trace($"loaded host assembly path={options.HostAssemblyPath}");
        var hostType = ResolveHostEntryType(hostAssembly);
        Trace($"resolved host type={hostType.FullName}");
        var invokeMethod = ResolveHostInvokeWorkloadMethod(hostType);
        if (invokeMethod is not null)
        {
            var invokeArguments = BuildHostInvokeArguments(options, invokeMethod);
            Trace($"resolved host invoke method={invokeMethod}");
            return () =>
            {
                Trace("invoking host InvokeWorkload");
                var result = invokeMethod.Invoke(null, invokeArguments);
                Trace("host InvokeWorkload returned");
                return result;
            };
        }

        var mainMethod = ResolveHostMainMethod(hostAssembly, hostType);
        Trace($"resolved host main method={mainMethod}");
        return () =>
        {
            Trace("invoking host Main");
            var result = mainMethod.Invoke(null, [BuildHostArguments(options)]);
            Trace("host Main returned");
            return result;
        };
    }

    private static string[] BuildHostArguments(HarnessOptions options)
    {
        if (!options.HasCollectionSelection)
        {
            throw new ArgumentException("collection-backed benchmark host execution requires collection selection.");
        }

        var arguments = new List<string>
        {
            ChaosManagedHostArguments.CollectionPathPrefix + options.CollectionPath,
        };
        if (!string.IsNullOrWhiteSpace(options.BindingManifestPath))
        {
            arguments.Add(ChaosHotUpdateHostArguments.BindingManifestPathPrefix + options.BindingManifestPath);
        }

        arguments.Add(ChaosManagedHostArguments.EntryIndexPrefix + options.EntryIndex);
        return arguments.ToArray();
    }

    private static object?[] BuildHostInvokeArguments(HarnessOptions options, MethodInfo invokeMethod)
    {
        var parameters = invokeMethod.GetParameters();
        if (parameters.Length == 3
            && parameters[0].ParameterType == typeof(string)
            && parameters[1].ParameterType == typeof(string)
            && parameters[2].ParameterType == typeof(int))
        {
            return [options.CollectionPath, options.BindingManifestPath, options.EntryIndex];
        }

        if (parameters.Length == 2
            && parameters[0].ParameterType == typeof(string)
            && parameters[1].ParameterType == typeof(int))
        {
            return [options.CollectionPath, options.EntryIndex];
        }

        if (parameters.Length == 1 && parameters[0].ParameterType == typeof(int))
        {
            return [options.EntryIndex];
        }

        throw new InvalidOperationException(
            $"generated benchmark host InvokeWorkload has unsupported signature: {invokeMethod}.");
    }

    private static MethodInfo? ResolveHostInvokeWorkloadMethod(Type hostType)
    {
        return ResolveHostStaticMethod(hostType, "InvokeWorkload", [typeof(string), typeof(string), typeof(int)])
            ?? ResolveHostStaticMethod(hostType, "InvokeWorkload", [typeof(string), typeof(int)])
            ?? ResolveHostStaticMethod(hostType, "InvokeWorkload", [typeof(int)]);
    }

    private static MethodInfo ResolveHostMainMethod(Assembly hostAssembly, Type hostType)
    {
        return hostAssembly.EntryPoint
            ?? ResolveHostStaticMethod(hostType, "Main", [typeof(string[])])
            ?? throw new InvalidOperationException("generated benchmark host is missing Main(string[] args).");
    }

    private static Type ResolveHostEntryType(Assembly hostAssembly)
    {
        return hostAssembly.EntryPoint?.DeclaringType
            ?? throw new InvalidOperationException("generated benchmark host entry point is missing.");
    }

    private static MethodInfo? ResolveHostStaticMethod(Type hostType, string methodName, Type[] parameterTypes)
    {
        return hostType.GetMethod(
            methodName,
            BindingFlags.Public | BindingFlags.Static,
            binder: null,
            types: parameterTypes,
            modifiers: null);
    }

    private static Func<object?> LoadReflectedWorkload(string assemblyPath, WorkloadDescriptor descriptor)
    {
        if (string.IsNullOrWhiteSpace(assemblyPath))
        {
            throw new ArgumentException("missing workload assembly path");
        }

        var assembly = Assembly.LoadFrom(assemblyPath);
        var type = assembly.GetType(descriptor.DeclaringType, throwOnError: true, ignoreCase: false)
            ?? throw new InvalidOperationException($"unable to resolve workload type: {descriptor.DeclaringType}");
        var method = ResolveMethod(type, descriptor.MethodName, descriptor.MethodSignature, descriptor.WorkloadEntry);
        if (method.GetParameters().Length != 0)
        {
            throw new InvalidOperationException($"workload method must be parameterless: {descriptor.WorkloadEntry}");
        }

        return () => method.Invoke(null, null);
    }

    private static MethodInfo ResolveMethod(Type type, string methodName, string methodSignature, string descriptorLabel)
    {
        var hasSignature = !string.IsNullOrWhiteSpace(methodSignature);
        MethodInfo? fallback = null;
        foreach (var candidate in type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static))
        {
            if (!string.Equals(candidate.Name, methodName, StringComparison.Ordinal))
            {
                continue;
            }

            if (hasSignature && string.Equals(BuildMethodSignature(candidate), methodSignature, StringComparison.Ordinal))
            {
                return candidate;
            }

            if (fallback is null)
            {
                fallback = candidate;
                continue;
            }

            if (!hasSignature)
            {
                throw new InvalidOperationException($"ambiguous workload method: {descriptorLabel}");
            }
        }

        if (hasSignature)
        {
            throw new InvalidOperationException($"unable to resolve workload method: {descriptorLabel}");
        }

        return fallback ?? throw new InvalidOperationException($"unable to resolve workload method: {descriptorLabel}");
    }

    private static string BuildMethodSignature(MethodInfo method)
    {
        var parameterList = string.Join(", ", method.GetParameters().Select(parameter => FormatParameterType(parameter.ParameterType)));
        return $"{method.Name}({parameterList})";
    }

    private static string FormatParameterType(Type parameterType)
    {
        if (parameterType.IsByRef)
        {
            return FormatParameterType(parameterType.GetElementType() ?? typeof(void)) + "&";
        }

        if (parameterType.IsPointer)
        {
            return FormatParameterType(parameterType.GetElementType() ?? typeof(void)) + "*";
        }

        if (parameterType.IsArray)
        {
            return FormatParameterType(parameterType.GetElementType() ?? typeof(void)) + "[]";
        }

        if (parameterType.IsGenericType)
        {
            var genericTypeName = parameterType.GetGenericTypeDefinition().FullName ?? parameterType.Name;
            var tickIndex = genericTypeName.IndexOf('`');
            if (tickIndex >= 0)
            {
                genericTypeName = genericTypeName[..tickIndex];
            }

            return $"{genericTypeName}<{string.Join(", ", parameterType.GetGenericArguments().Select(FormatParameterType))}>";
        }

        return parameterType.FullName ?? parameterType.Name;
    }

    private static long CoerceChecksum(object? result)
    {
        if (result is null)
        {
            return 0;
        }

        return result switch
        {
            byte value => value,
            sbyte value => value,
            short value => value,
            ushort value => value,
            int value => value,
            uint value => value,
            long value => value,
            ulong value => unchecked((long)value),
            bool value => value ? 1 : 0,
            _ => result.GetHashCode(),
        };
    }

    private static void Trace(string message)
    {
        if (!string.Equals(Environment.GetEnvironmentVariable("CHAOS_PERF_HARNESS_TRACE"), "1", StringComparison.Ordinal))
        {
            return;
        }

        Console.Error.WriteLine("[perf-harness] " + message);
    }

    private static DeclaredBenchmarkEntryDescriptor ResolveDeclaredBenchmarkEntry(string collectionPath, int entryIndex)
    {
        if (entryIndex < 0)
        {
            throw new ArgumentOutOfRangeException(nameof(entryIndex), entryIndex, "entry index must be non-negative.");
        }

        var collection = ChaosTestCollectionLoader.Load(collectionPath);
        var entry = collection.DeclaredBenchmarks.FirstOrDefault(candidate => candidate.EntryIndex == entryIndex)
            ?? throw new InvalidOperationException(
                $"collection '{collectionPath}' does not contain benchmark entry index {entryIndex}.");

        return new DeclaredBenchmarkEntryDescriptor(
            entry.AssemblyName,
            entry.DeclaringType,
            entry.MethodName,
            entry.MethodSignature);
    }

    private sealed record HarnessOptions(
        int Iterations,
        string AssemblyPath,
        string HostAssemblyPath,
        string CollectionPath,
        int EntryIndex,
        string BindingManifestPath,
        string WorkloadEntry,
        string AssemblyName,
        string DeclaringType,
        string MethodName,
        string MethodSignature,
        string Mode,
        string SubjectId)
    {
        public bool HasCollectionSelection => !string.IsNullOrWhiteSpace(CollectionPath) && EntryIndex >= 0;

        public static HarnessOptions Parse(string[] args)
        {
            var iterations = 1;
            if (args.Length > 0 && int.TryParse(args[0], out var parsedIterations) && parsedIterations > 0)
            {
                iterations = parsedIterations;
            }

            string assemblyPath = string.Empty;
            string hostAssemblyPath = string.Empty;
            string collectionPath = string.Empty;
            var entryIndex = -1;
            string bindingManifestPath = string.Empty;
            string workloadEntry = string.Empty;
            string assemblyName = string.Empty;
            string declaringType = string.Empty;
            string methodName = string.Empty;
            string methodSignature = string.Empty;
            var mode = "managed";
            for (var index = 1; index < args.Length; index += 2)
            {
                if (index + 1 >= args.Length)
                {
                    break;
                }

                var name = args[index];
                var value = args[index + 1];
                if (string.Equals(name, "--assembly", StringComparison.Ordinal))
                {
                    assemblyPath = value;
                    continue;
                }

                if (string.Equals(name, "--host-assembly", StringComparison.Ordinal))
                {
                    hostAssemblyPath = value;
                    continue;
                }

                if (string.Equals(name, "--collection-path", StringComparison.Ordinal))
                {
                    collectionPath = value;
                    continue;
                }

                if (string.Equals(name, "--entry-index", StringComparison.Ordinal))
                {
                    _ = int.TryParse(value, out entryIndex);
                    continue;
                }

                if (string.Equals(name, "--binding-manifest-path", StringComparison.Ordinal))
                {
                    bindingManifestPath = value;
                    continue;
                }

                if (string.Equals(name, "--workload-entry", StringComparison.Ordinal))
                {
                    workloadEntry = value;
                    continue;
                }

                if (string.Equals(name, "--assembly-name", StringComparison.Ordinal))
                {
                    assemblyName = value;
                    continue;
                }

                if (string.Equals(name, "--declaring-type", StringComparison.Ordinal))
                {
                    declaringType = value;
                    continue;
                }

                if (string.Equals(name, "--method-name", StringComparison.Ordinal))
                {
                    methodName = value;
                    continue;
                }

                if (string.Equals(name, "--method-signature", StringComparison.Ordinal))
                {
                    methodSignature = value;
                    continue;
                }

                if (string.Equals(name, "--mode", StringComparison.Ordinal))
                {
                    mode = value;
                }
            }

            var subjectId = !string.IsNullOrWhiteSpace(workloadEntry)
                ? workloadEntry.Split('/', 2, StringSplitOptions.RemoveEmptyEntries)[0]
                : assemblyName;
            return new HarnessOptions(
                iterations,
                assemblyPath,
                hostAssemblyPath,
                collectionPath,
                entryIndex,
                bindingManifestPath,
                workloadEntry,
                assemblyName,
                declaringType,
                methodName,
                methodSignature,
                mode,
                subjectId);
        }
    }

    private sealed record WorkloadDescriptor(
        string SubjectId,
        string WorkloadEntry,
        string DeclaringType,
        string MethodName,
        string MethodSignature,
        string AssemblyName);

    private sealed record DeclaredBenchmarkEntryDescriptor(
        string AssemblyName,
        string DeclaringType,
        string MethodName,
        string MethodSignature);
}
