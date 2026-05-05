using System.Text.Json;

namespace Chaos.TestFramework.Runtime;

/// <summary>
/// Minimal JSONL trace writer for the TestFramework Runtime.
/// Reads CHAOS_TRACE_PATH and CHAOS_TRACE_ID from environment (set by Python orchestration).
/// This is a standalone copy to avoid a dependency from reference/ to managed/ projects.
/// </summary>
internal static class RuntimeTrace
{
    private static readonly string? _tracePath;
    private static readonly string _traceId;
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        WriteIndented = false,
    };

    static RuntimeTrace()
    {
        _tracePath = Environment.GetEnvironmentVariable("CHAOS_TRACE_PATH");
        _traceId = Environment.GetEnvironmentVariable("CHAOS_TRACE_ID") ?? "";

        if (_tracePath is not null)
        {
            var dir = Path.GetDirectoryName(_tracePath);
            if (!string.IsNullOrEmpty(dir))
                Directory.CreateDirectory(dir);
        }
    }

    public static bool IsEnabled => _tracePath is not null;

    public static void Point(string operation, string stage = "", object? data = null)
    {
        if (_tracePath is null)
            return;

        var record = new Dictionary<string, object?>
        {
            ["t"] = DateTime.UtcNow.ToString("yyyy'-'MM'-'dd'T'HH':'mm':'ss'Z'"),
            ["l"] = "cs",
            ["o"] = operation,
            ["traceId"] = _traceId,
        };
        if (!string.IsNullOrEmpty(stage))
            record["s"] = stage;

        if (data is not null)
        {
            if (data is Dictionary<string, object?> dict)
            {
                foreach (var (k, v) in dict)
                    record[k] = v;
            }
            else
            {
                record["data"] = data;
            }
        }

        var json = JsonSerializer.Serialize(record, JsonOptions);
        try
        {
            File.AppendAllText(_tracePath, json + Environment.NewLine);
        }
        catch
        {
            // Silently ignore IO errors
        }
    }

    public static void TraceCall(string operation, string stage, Action action)
    {
        Point($"{operation}:enter", stage);
        var sw = System.Diagnostics.Stopwatch.StartNew();
        try
        {
            action();
            sw.Stop();
            Point($"{operation}:exit", stage, new Dictionary<string, object?>
            {
                ["dur_ms"] = Math.Round(sw.Elapsed.TotalMilliseconds, 1),
            });
        }
        catch (Exception ex)
        {
            sw.Stop();
            Point($"{operation}:exception", stage, new Dictionary<string, object?>
            {
                ["exception"] = ex.GetType().Name,
                ["message"] = ex.Message,
                ["dur_ms"] = Math.Round(sw.Elapsed.TotalMilliseconds, 1),
            });
            throw;
        }
    }
}
