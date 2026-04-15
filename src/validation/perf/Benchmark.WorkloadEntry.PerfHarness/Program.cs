using System;
using System.Diagnostics;
using System.Linq;
using System.Reflection;
using System.Text.Json;

namespace Chaos.Benchmark.WorkloadEntry.PerfHarness;

internal static class Program
{
    private static int Main(string[] args)
    {
        var options = HarnessOptions.Parse(args);
        var descriptor = ResolveWorkloadDescriptor(options);
        var workload = LoadWorkload(options.AssemblyPath, descriptor);

        var stopwatch = Stopwatch.StartNew();
        long checksum = 0;
        for (var index = 0; index < options.Iterations; index += 1)
        {
            checksum += CoerceChecksum(workload());
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
                declaredMethodSignature);
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
        return new WorkloadDescriptor(subjectId, workloadEntry, typeName, methodName, methodSignature);
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

    private static Func<object?> LoadWorkload(string assemblyPath, WorkloadDescriptor descriptor)
    {
        if (string.IsNullOrWhiteSpace(assemblyPath))
        {
            throw new ArgumentException("missing --assembly");
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
            var genericTypeName = (parameterType.GetGenericTypeDefinition().FullName ?? parameterType.Name);
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

    private sealed record HarnessOptions(
        int Iterations,
        string AssemblyPath,
        string WorkloadEntry,
        string AssemblyName,
        string DeclaringType,
        string MethodName,
        string MethodSignature,
        string Mode,
        string SubjectId)
    {
        public static HarnessOptions Parse(string[] args)
        {
            var iterations = 1;
            if (args.Length > 0 && int.TryParse(args[0], out var parsedIterations) && parsedIterations > 0)
            {
                iterations = parsedIterations;
            }

            string assemblyPath = string.Empty;
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
        string MethodSignature);
}
