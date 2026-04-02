using System.Collections.ObjectModel;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using System.Text.Json.Serialization;

internal enum HostLifecycleState
{
    Created,
    Started,
    Stopped,
}

internal sealed record HostEnvironmentSnapshot(
    bool WorkingDirectoryAvailable,
    string TempDirectoryName,
    bool LocalClockReadable,
    bool TimeZoneReadable,
    bool ProbeLibraryResolved,
    string FilePayload);

internal sealed record TraceExportRequest(
    string Platform,
    string OutputPath);

internal sealed record WarmupTraceDocument(
    string FormatVersion,
    string TraceName,
    string TargetPlatform,
    WarmupTraceSample[] Samples,
    string? TraceSource = null,
    string[]? SessionTrace = null);

internal sealed record WarmupTraceSample(
    string SampleId,
    string Scenario,
    WarmupTraceEvent[] Events);

internal sealed record WarmupTraceEvent(
    string EventName,
    string Phase,
    string SubjectKind,
    string SubjectId,
    int Order,
    string Status,
    string? Trigger = null);

internal sealed class WarmupPlayer
{
    public int LastDamage { get; private set; }

    public void TakeDamage(int amount)
    {
        LastDamage = amount;
    }
}

internal sealed class WarmupInventory
{
    public delegate void ItemAddedHandler(string item);

    public string? LastItem { get; private set; }

    public void AddItem<T>(T item)
    {
        LastItem = item?.ToString();
    }
}

internal sealed class BootstrapStateMachine
{
    private readonly List<string> _trace;

    public BootstrapStateMachine(List<string> trace)
    {
        _trace = trace;
    }

    public bool RegistrationComplete { get; private set; }

    public bool MetadataCacheInitialized { get; private set; }

    public bool GcDependentLateInitComplete { get; private set; }

    public bool ClassWorldWarmupComplete { get; private set; }

    public void CompleteRegistration()
    {
        RegistrationComplete = true;
        _trace.Add("bootstrap:registration-complete");
    }

    public void InitializeMetadataCache()
    {
        Ensure(RegistrationComplete, "registration must complete first");
        MetadataCacheInitialized = true;
        _trace.Add("bootstrap:metadata-cache-initialize");
    }

    public void CompleteGcDependentLateInit()
    {
        Ensure(MetadataCacheInitialized, "metadata cache must initialize first");
        GcDependentLateInitComplete = true;
        _trace.Add("bootstrap:gc-dependent-late-init");
    }

    public void WarmClassWorldAndReflectionCache()
    {
        Ensure(GcDependentLateInitComplete, "GC-dependent init must complete first");

        if (ClassWorldWarmupComplete)
        {
            return;
        }

        ClassWorldWarmupComplete = true;
        _trace.Add("bootstrap:class-world-reflection-cache-warmup");
    }

    private static void Ensure(bool condition, string message)
    {
        if (!condition)
        {
            throw new InvalidOperationException(message);
        }
    }
}

internal sealed class HostEmbeddingSession
{
    private readonly List<string> _trace = new();
    private readonly BootstrapStateMachine _bootstrap;
    private int _attachedThreads;

    public HostEmbeddingSession()
    {
        _bootstrap = new BootstrapStateMachine(_trace);
    }

    public HostLifecycleState State { get; private set; } = HostLifecycleState.Created;

    public ReadOnlyCollection<string> Trace => _trace.AsReadOnly();

    public void Start()
    {
        if (State != HostLifecycleState.Created)
        {
            throw new InvalidOperationException("host can only start from created state");
        }

        State = HostLifecycleState.Started;
        _trace.Add("host:start");
    }

    public void Stop()
    {
        EnsureStarted(nameof(Stop));

        if (_attachedThreads != 0)
        {
            throw new InvalidOperationException("all attached threads must detach before stop");
        }

        State = HostLifecycleState.Stopped;
        _trace.Add("host:stop");
    }

    public void CompleteRegistrationFlow()
    {
        EnsureStarted(nameof(CompleteRegistrationFlow));
        _trace.Add("registration:assemblies");
        _trace.Add("registration:managed-exports");
        _trace.Add("registration:callbacks");
        _bootstrap.CompleteRegistration();
    }

    public void InitializeBootstrap()
    {
        EnsureStarted(nameof(InitializeBootstrap));
        _bootstrap.InitializeMetadataCache();
        _bootstrap.CompleteGcDependentLateInit();
    }

    public void AttachThread(string logicalThreadName)
    {
        EnsureStarted(nameof(AttachThread));
        _attachedThreads++;
        _trace.Add($"thread:attach:{logicalThreadName}:{_attachedThreads}");
    }

    public void DetachThread(string logicalThreadName)
    {
        EnsureStarted(nameof(DetachThread));

        if (_attachedThreads == 0)
        {
            throw new InvalidOperationException("cannot detach thread when no threads are attached");
        }

        _attachedThreads--;
        _trace.Add($"thread:detach:{logicalThreadName}:{_attachedThreads}");
    }

    public string InvokeManagedEntry(Func<string, string> callback)
    {
        EnsureAttachedThread(nameof(InvokeManagedEntry));
        _trace.Add("managed-entry:begin");
        string callbackResult = callback("host-callback-payload");
        _trace.Add("managed-entry:end");
        return callbackResult;
    }

    public HostEnvironmentSnapshot CaptureEnvironment()
    {
        EnsureAttachedThread(nameof(CaptureEnvironment));

        string workingDirectory = Environment.CurrentDirectory;
        string tempRoot = Path.Combine(Path.GetTempPath(), "roadmap0-host-embedding-lite");
        Directory.CreateDirectory(tempRoot);

        string payloadPath = Path.Combine(tempRoot, "payload.txt");
        File.WriteAllText(payloadPath, "host-embedding-lite");
        string payload = File.ReadAllText(payloadPath);
        DateTimeOffset localTime = DateTimeOffset.Now;
        TimeZoneInfo localTimeZone = TimeZoneInfo.Local;

        bool loaded = NativeLibrary.TryLoad(ResolveLibraryName(), out nint handle);
        if (loaded)
        {
            NativeLibrary.Free(handle);
        }

        _trace.Add("filesystem:payload-roundtrip");
        _trace.Add("time:local-clock-and-zone");
        _trace.Add($"dynamic-library:{loaded}");

        return new HostEnvironmentSnapshot(
            Directory.Exists(workingDirectory),
            Path.GetFileName(tempRoot.TrimEnd(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar)),
            localTime != default,
            !string.IsNullOrWhiteSpace(localTimeZone.Id),
            loaded,
            payload);
    }

    public string RequestReflectionSummary()
    {
        EnsureAttachedThread(nameof(RequestReflectionSummary));
        _trace.Add("reflection:requested");
        _bootstrap.WarmClassWorldAndReflectionCache();
        Type entryType = typeof(HostEmbeddingSession);
        MethodInfo managedEntry = entryType.GetMethod(nameof(InvokeManagedEntry))!;
        return $"{entryType.Name}:{managedEntry.Name}:{_bootstrap.ClassWorldWarmupComplete}";
    }

    public void RecordWarmupTraceEvent(string sampleId, WarmupTraceEvent traceEvent)
    {
        EnsureAttachedThread(nameof(RecordWarmupTraceEvent));
        string encodedSubjectId = Convert.ToBase64String(Encoding.UTF8.GetBytes(traceEvent.SubjectId));
        _trace.Add(
            $"warmup-trace|sampleId={sampleId}|order={traceEvent.Order}|eventName={traceEvent.EventName}|phase={traceEvent.Phase}|subjectKind={traceEvent.SubjectKind}|subjectId={encodedSubjectId}|status={traceEvent.Status}");
    }

    private void EnsureStarted(string operationName)
    {
        if (State != HostLifecycleState.Started)
        {
            throw new InvalidOperationException($"{operationName} requires started host");
        }
    }

    private void EnsureAttachedThread(string operationName)
    {
        EnsureStarted(operationName);

        if (_attachedThreads == 0)
        {
            throw new InvalidOperationException($"{operationName} requires an attached thread");
        }
    }

    private static string ResolveLibraryName()
    {
        if (OperatingSystem.IsWindows())
        {
            return "kernel32.dll";
        }

        if (OperatingSystem.IsLinux())
        {
            return "libc.so.6";
        }

        return "libSystem.B.dylib";
    }
}

internal static class Program
{
    private static int Main(string[] args)
    {
        TraceExportRequest? traceRequest = TryGetTraceExportRequest(args);
        HostEmbeddingSession session = new();
        session.Start();
        session.AttachThread("main");
        session.CompleteRegistrationFlow();
        session.InitializeBootstrap();

        string callbackResult = session.InvokeManagedEntry(static payload => $"managed:{payload}");
        HostEnvironmentSnapshot snapshot = session.CaptureEnvironment();
        string reflectionSummary = session.RequestReflectionSummary();
        WarmupTraceDocument? traceDocument = traceRequest is null
            ? null
            : BuildWarmupTraceDocument(traceRequest.Platform, session);
        string guardSummary = VerifyLifecycleGuards();

        session.DetachThread("main");
        session.Stop();
        string? traceSummary = TryWriteWarmupTrace(traceRequest, traceDocument, session.Trace);

        Console.WriteLine($"state={session.State}");
        Console.WriteLine(callbackResult);
        Console.WriteLine($"env={snapshot.WorkingDirectoryAvailable}|{snapshot.TempDirectoryName}|{snapshot.FilePayload}");
        Console.WriteLine($"capabilities={snapshot.LocalClockReadable}|{snapshot.TimeZoneReadable}|{snapshot.ProbeLibraryResolved}");
        Console.WriteLine(reflectionSummary);
        Console.WriteLine(guardSummary);

        if (traceSummary is not null)
        {
            Console.WriteLine(traceSummary);
        }

        Console.WriteLine(string.Join(",", session.Trace));
        return 0;
    }

    private static TraceExportRequest? TryGetTraceExportRequest(IReadOnlyList<string> args)
    {
        string? tracePlatform = TryGetOption(args, "--trace-platform");
        string? traceOutput = TryGetOption(args, "--trace-output");

        if (tracePlatform is null && traceOutput is null)
        {
            return null;
        }

        if (tracePlatform is null || traceOutput is null)
        {
            throw new ArgumentException("trace platform and trace output must be supplied together");
        }

        return new TraceExportRequest(tracePlatform, Path.GetFullPath(traceOutput));
    }

    private static string? TryWriteWarmupTrace(
        TraceExportRequest? traceRequest,
        WarmupTraceDocument? traceDocument,
        IReadOnlyCollection<string> sessionTrace)
    {
        if (traceRequest is null)
        {
            return null;
        }

        if (traceDocument is null)
        {
            throw new InvalidOperationException("trace document must exist when trace export is requested");
        }

        WarmupTraceDocument runtimeTraceDocument = new(
            traceDocument.FormatVersion,
            traceDocument.TraceName,
            traceDocument.TargetPlatform,
            traceDocument.Samples,
            "host-embedding-session",
            sessionTrace.ToArray());

        string outputPath = traceRequest.OutputPath;
        string? outputDirectory = Path.GetDirectoryName(outputPath);

        if (!string.IsNullOrEmpty(outputDirectory))
        {
            Directory.CreateDirectory(outputDirectory);
        }

        JsonSerializerOptions options = new()
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
            WriteIndented = true,
        };

        File.WriteAllText(outputPath, JsonSerializer.Serialize(runtimeTraceDocument, options));
        return $"trace={runtimeTraceDocument.TargetPlatform}|{Path.GetFileName(outputPath)}";
    }

    private static WarmupTraceDocument BuildWarmupTraceDocument(string tracePlatform, HostEmbeddingSession session)
    {
        return tracePlatform switch
        {
            "windows" => BuildWindowsWarmupTrace(session),
            "macos" => BuildMacosWarmupTrace(session),
            _ => throw new ArgumentOutOfRangeException(nameof(tracePlatform), tracePlatform, "unsupported trace platform"),
        };
    }

    private static void AddWarmupTraceEvent(
        List<WarmupTraceEvent> events,
        HostEmbeddingSession session,
        string sampleId,
        WarmupTraceEvent traceEvent)
    {
        events.Add(traceEvent);
        session.RecordWarmupTraceEvent(sampleId, traceEvent);
    }

    private static WarmupTraceDocument BuildWindowsWarmupTrace(HostEmbeddingSession session)
    {
        const string sampleId = "windows-lazy-method-cache";
        const string typeSubjectId = "Game.Core/Game.Player";
        const string methodSubjectId = "Game.Core/Game.Player::TakeDamage(System.Int32)";

        WarmupPlayer player = new();
        Type playerType = typeof(WarmupPlayer);
        Dictionary<string, MethodInfo> methodCache = new();
        List<WarmupTraceEvent> events = new();

        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.type.requested", "requested", "type", typeSubjectId, 10, "ok", "lazy-touch"));
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.type.started", "started", "type", typeSubjectId, 11, "ok"));
        _ = playerType.FullName ?? throw new InvalidOperationException("warmup player type name missing");
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.type.completed", "completed", "type", typeSubjectId, 12, "ok"));

        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.requested", "requested", "method", methodSubjectId, 13, "ok", "lazy-touch"));

        if (!methodCache.TryGetValue(methodSubjectId, out MethodInfo? method))
        {
            AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.started", "started", "method", methodSubjectId, 14, "ok"));
            method = playerType.GetMethod(nameof(WarmupPlayer.TakeDamage), BindingFlags.Instance | BindingFlags.Public)
                ?? throw new InvalidOperationException("warmup player method missing");
            method.Invoke(player, [7]);
            methodCache[methodSubjectId] = method;
            AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.completed", "completed", "method", methodSubjectId, 15, "ok"));
        }

        MethodInfo cachedMethod = methodCache[methodSubjectId];
        cachedMethod.Invoke(player, [3]);
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.hit", "cached", "method", methodSubjectId, 16, "cached"));

        return new WarmupTraceDocument(
            "v0",
            "warmupTrace",
            "windows",
            [
                new WarmupTraceSample(
                    sampleId,
                    "首次懒访问触发类型与方法预热，第二次访问命中缓存。",
                    events.ToArray()),
            ]);
    }

    private static WarmupTraceDocument BuildMacosWarmupTrace(HostEmbeddingSession session)
    {
        const string sampleId = "macos-lazy-generic-replay";
        const string genericContextSubjectId = "Game.Core/Game.Inventory::AddItem<System.String>(System.String)";
        const string methodSubjectId = "Game.Core/Game.Inventory::AddItem<System.String>(System.String)";
        const string delegateSubjectId = "Game.Core/Game.Inventory+ItemAddedHandler::.ctor(System.Object,System.IntPtr) => Game.Core/Game.Inventory::AddItem<System.String>(System.String)";

        WarmupInventory inventory = new();
        MethodInfo openMethod = typeof(WarmupInventory).GetMethod(nameof(WarmupInventory.AddItem), BindingFlags.Instance | BindingFlags.Public)
            ?? throw new InvalidOperationException("warmup inventory method missing");
        Dictionary<string, MethodInfo> genericContextCache = new();
        List<WarmupTraceEvent> events = new();

        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("generic.context.requested", "requested", "genericContext", genericContextSubjectId, 20, "ok", "lazy-touch"));
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("generic.context.started", "started", "genericContext", genericContextSubjectId, 21, "ok"));

        MethodInfo closedMethod = openMethod.MakeGenericMethod(typeof(string));
        genericContextCache[genericContextSubjectId] = closedMethod;
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("generic.context.completed", "completed", "genericContext", genericContextSubjectId, 22, "ok"));

        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.requested", "requested", "method", methodSubjectId, 23, "ok", "lazy-touch"));
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.started", "started", "method", methodSubjectId, 24, "ok"));
        closedMethod.Invoke(inventory, ["rope"]);
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("warmup.method.completed", "completed", "method", methodSubjectId, 25, "ok"));

        MethodInfo replayMethod = genericContextCache[genericContextSubjectId];
        WarmupInventory.ItemAddedHandler boundDelegate =
            (WarmupInventory.ItemAddedHandler)Delegate.CreateDelegate(typeof(WarmupInventory.ItemAddedHandler), inventory, replayMethod);
        boundDelegate("potion");
        AddWarmupTraceEvent(events, session, sampleId, new WarmupTraceEvent("delegate.stub.bind", "replayed", "delegate", delegateSubjectId, 26, "ok"));

        return new WarmupTraceDocument(
            "v0",
            "warmupTrace",
            "macos",
            [
                new WarmupTraceSample(
                    sampleId,
                    "首次懒访问闭包泛型方法完成预热，后续委托绑定走 replay 路径。",
                    events.ToArray()),
            ]);
    }

    private static string? TryGetOption(IReadOnlyList<string> args, string optionName)
    {
        for (int index = 0; index < args.Count - 1; index++)
        {
            if (string.Equals(args[index], optionName, StringComparison.Ordinal))
            {
                return args[index + 1];
            }
        }

        return null;
    }

    private static string VerifyLifecycleGuards()
    {
        bool invalidDetachRejected = VerifyInvalidDetachGuard();
        bool doubleStartRejected = VerifyDoubleStartGuard();
        bool unattachedEntryRejected = VerifyUnattachedEntryGuard();
        return $"guards=invalid-detach:{invalidDetachRejected}|double-start:{doubleStartRejected}|unattached-entry:{unattachedEntryRejected}";
    }

    private static bool VerifyInvalidDetachGuard()
    {
        HostEmbeddingSession session = new();
        session.Start();

        try
        {
            session.DetachThread("orphan");
            return false;
        }
        catch (InvalidOperationException)
        {
            return true;
        }
        finally
        {
            session.Stop();
        }
    }

    private static bool VerifyDoubleStartGuard()
    {
        HostEmbeddingSession session = new();
        session.Start();

        try
        {
            session.Start();
            return false;
        }
        catch (InvalidOperationException)
        {
            return true;
        }
        finally
        {
            session.Stop();
        }
    }

    private static bool VerifyUnattachedEntryGuard()
    {
        HostEmbeddingSession session = new();
        session.Start();
        session.CompleteRegistrationFlow();
        session.InitializeBootstrap();

        try
        {
            session.InvokeManagedEntry(static payload => payload);
            return false;
        }
        catch (InvalidOperationException)
        {
            return true;
        }
        finally
        {
            session.Stop();
        }
    }
}
