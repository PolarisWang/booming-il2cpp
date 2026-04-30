using System.Text.Json;
using System.Runtime.CompilerServices;

namespace Chaos.IL2CPP.Diagnostics;

/// <summary>
/// Cross-language trace: writes JSONL to the session trace file.
/// Session path is set via the CHAOS_TRACE_PATH environment variable
/// (exported by the Python orchestration layer before invoking the driver).
/// </summary>
public static class ChaosTrace
{
    private static string? _tracePath;
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        WriteIndented = false,
    };

    /// <summary>
    /// Initialize trace output to a specific file path.
    /// </summary>
    public static void Init(string tracePath)
    {
        _tracePath = tracePath;
        var dir = Path.GetDirectoryName(tracePath);
        if (!string.IsNullOrEmpty(dir))
            Directory.CreateDirectory(dir);
    }

    /// <summary>
    /// Initialize from CHAOS_TRACE_PATH environment variable.
    /// No-op if the variable is not set.
    /// </summary>
    public static void InitFromEnv()
    {
        var path = Environment.GetEnvironmentVariable("CHAOS_TRACE_PATH");
        if (!string.IsNullOrEmpty(path))
            Init(path);
    }

    /// <summary>
    /// Write a single trace point.
    /// </summary>
    /// <param name="operation">Operation name, e.g. "driver.run_convert"</param>
    /// <param name="stage">Stage name, e.g. "codegen"</param>
    /// <param name="data">Optional key-value pairs to include in the trace record</param>
    /// <param name="file">Auto-populated caller file path</param>
    /// <param name="line">Auto-populated caller line number</param>
    public static void Point(
        string operation,
        string stage = "",
        object? data = null,
        [CallerFilePath] string file = "",
        [CallerLineNumber] int line = 0)
    {
        if (_tracePath is null)
            return;

        var record = new Dictionary<string, object?>
        {
            ["t"] = DateTime.UtcNow.ToString("yyyy'-'MM'-'dd'T'HH':'mm':'ss'Z'"),
            ["l"] = "cs",
            ["o"] = operation,
        };
        if (!string.IsNullOrEmpty(stage))
            record["s"] = stage;

        // Shorten file path relative to repo root for readability
        if (!string.IsNullOrEmpty(file))
        {
            var relPath = file;
            var repoMarker = "booming-il2cpp";
            var idx = file.IndexOf(repoMarker, StringComparison.Ordinal);
            if (idx >= 0)
                relPath = file[(idx + repoMarker.Length + 1)..];
            record["f"] = $"{relPath}:{line}";
        }

        // Merge data dictionary or attach single data value
        if (data is not null)
        {
            if (data is Dictionary<string, object?> dict)
            {
                foreach (var (k, v) in dict)
                    record[k] = v;
            }
            else if (data is IEnumerable<KeyValuePair<string, object?>> kvps)
            {
                foreach (var (k, v) in kvps)
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
            // Silently ignore IO errors (trace must never crash the driver)
        }
    }

    /// <summary>
    /// Wrap an action with entry/exit/exception trace points and duration.
    /// </summary>
    public static void TraceCall(
        string operation,
        string stage,
        object? entryData,
        Action action)
    {
        Point($"{operation}:enter", stage, entryData);
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
