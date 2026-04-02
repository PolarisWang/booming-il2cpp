using System.Collections.ObjectModel;
using System.Reflection;
using System.Runtime.InteropServices;

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
    private static int Main()
    {
        HostEmbeddingSession session = new();
        session.Start();
        session.AttachThread("main");
        session.CompleteRegistrationFlow();
        session.InitializeBootstrap();

        string callbackResult = session.InvokeManagedEntry(static payload => $"managed:{payload}");
        HostEnvironmentSnapshot snapshot = session.CaptureEnvironment();
        string reflectionSummary = session.RequestReflectionSummary();
        string guardSummary = VerifyLifecycleGuards();

        session.DetachThread("main");
        session.Stop();

        Console.WriteLine($"state={session.State}");
        Console.WriteLine(callbackResult);
        Console.WriteLine($"env={snapshot.WorkingDirectoryAvailable}|{snapshot.TempDirectoryName}|{snapshot.FilePayload}");
        Console.WriteLine($"capabilities={snapshot.LocalClockReadable}|{snapshot.TimeZoneReadable}|{snapshot.ProbeLibraryResolved}");
        Console.WriteLine(reflectionSummary);
        Console.WriteLine(guardSummary);
        Console.WriteLine(string.Join(",", session.Trace));
        return 0;
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
