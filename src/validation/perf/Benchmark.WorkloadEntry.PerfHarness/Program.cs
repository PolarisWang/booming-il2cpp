using System;
using System.Diagnostics;
using System.Reflection;
using System.Text.Json;

namespace Chaos.Benchmark.WorkloadEntry.PerfHarness;

internal static class Program
{
    private static int Main(string[] args)
    {
        var options = HarnessOptions.Parse(args);
        var workload = LoadWorkload(options.AssemblyPath, options.WorkloadEntry);

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
            subjectId = options.SubjectId,
            workloadEntry = options.WorkloadEntry,
            iterations = options.Iterations,
            elapsedMilliseconds = stopwatch.Elapsed.TotalMilliseconds,
            opsPerSecond = options.Iterations / Math.Max(stopwatch.Elapsed.TotalSeconds, 0.000001d),
            checksum = Math.Abs(checksum % 10000),
        };

        Console.WriteLine(JsonSerializer.Serialize(payload));
        return 0;
    }

    private static Func<object?> LoadWorkload(string assemblyPath, string workloadEntry)
    {
        if (string.IsNullOrWhiteSpace(assemblyPath))
        {
            throw new ArgumentException("missing --assembly");
        }

        if (string.IsNullOrWhiteSpace(workloadEntry))
        {
            throw new ArgumentException("missing --workload-entry");
        }

        var separatorIndex = workloadEntry.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0 || separatorIndex >= workloadEntry.Length - 2)
        {
            throw new ArgumentException($"invalid workload entry: {workloadEntry}");
        }

        var typeName = workloadEntry[..separatorIndex].Replace("/", ".", StringComparison.Ordinal);
        var methodName = workloadEntry[(separatorIndex + 2)..];
        var signatureIndex = methodName.IndexOf("(", StringComparison.Ordinal);
        if (signatureIndex >= 0)
        {
            methodName = methodName[..signatureIndex];
        }

        var assembly = Assembly.LoadFrom(assemblyPath);
        var type = assembly.GetType(typeName, throwOnError: true, ignoreCase: false)
            ?? throw new InvalidOperationException($"unable to resolve workload type: {typeName}");
        var method = type.GetMethod(methodName, BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static)
            ?? throw new InvalidOperationException($"unable to resolve workload method: {workloadEntry}");
        if (method.GetParameters().Length != 0)
        {
            throw new InvalidOperationException($"workload method must be parameterless: {workloadEntry}");
        }

        return () => method.Invoke(null, null);
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

                if (string.Equals(name, "--mode", StringComparison.Ordinal))
                {
                    mode = value;
                }
            }

            var subjectId = workloadEntry.Split('/', 2, StringSplitOptions.RemoveEmptyEntries)[0];
            return new HarnessOptions(iterations, assemblyPath, workloadEntry, mode, subjectId);
        }
    }
}
