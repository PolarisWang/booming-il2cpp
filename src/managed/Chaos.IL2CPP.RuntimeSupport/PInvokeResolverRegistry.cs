using System.Reflection;
using System.Runtime.InteropServices;

namespace Chaos.IL2CPP.RuntimeSupport;

/// <summary>
/// Registry for per-assembly DllImportResolver callbacks.
///
/// Usage:
/// <code>
/// PInvokeResolverRegistry.SetDllImportResolver(typeof(MyClass).Assembly, MyResolver);
///
/// static IntPtr MyResolver(string libraryName, Assembly assembly, DllImportSearchPath? searchPath)
/// {
///     if (libraryName == "my_native.dll")
///         return NativeLibrary.Load("my_native_v2.dll");
///     return IntPtr.Zero;
/// }
/// </code>
///
/// The [UnmanagedCallersOnly] entry point is automatically registered with the
/// native runtime via a static constructor, so the codegen-emitted
/// TryResolveDllImport check in every P/Invoke wrapper will call back into
/// managed code to resolve overrides.
/// </summary>
public static unsafe class PInvokeResolverRegistry
{
    // Keyed by assembly name (short name, no version/token).
    private static readonly Dictionary<string, (Assembly Assembly, DllImportResolver Resolver)> s_resolvers = new();
    private static readonly object s_lock = new();

    static PInvokeResolverRegistry()
    {
        // Register the [UnmanagedCallersOnly] callback so that the native
        // TryResolveDllImport function can dispatch back into managed code.
        delegate* unmanaged<IntPtr, IntPtr, IntPtr> callback = &TryResolveCallback;
        RegisterPInvokeResolverCallback((IntPtr)callback);
    }

    /// <summary>
    /// Register a DllImportResolver for the given assembly.
    /// Thread-safe.  Subsequent registrations for the same assembly overwrite
    /// the previous resolver.
    /// </summary>
    public static void SetDllImportResolver(Assembly assembly, DllImportResolver resolver)
    {
        ArgumentNullException.ThrowIfNull(assembly);
        ArgumentNullException.ThrowIfNull(resolver);

        string name = assembly.GetName().Name
            ?? throw new ArgumentException("Assembly must have a short name", nameof(assembly));

        lock (s_lock)
        {
            s_resolvers[name] = (assembly, resolver);
        }
    }

    /// <summary>
    /// Native callback invoked by TryResolveDllImport.
    /// assemblyNameUtf8 and libraryNameUtf8 are null-terminated UTF-8 C strings.
    /// Returns a native library handle (HMODULE / dlopen pointer) or
    /// IntPtr.Zero to fall through to the default LoadLibrary path.
    /// </summary>
    [UnmanagedCallersOnly]
    public static IntPtr TryResolveCallback(IntPtr assemblyNameUtf8, IntPtr libraryNameUtf8)
    {
        string assemblyName = Marshal.PtrToStringUTF8(assemblyNameUtf8) ?? string.Empty;
        string libraryName = Marshal.PtrToStringUTF8(libraryNameUtf8) ?? string.Empty;

        (Assembly Assembly, DllImportResolver Resolver)? entry;
        lock (s_lock)
        {
            if (!s_resolvers.TryGetValue(assemblyName, out var pair))
                return IntPtr.Zero;
            entry = pair;
        }

        // searchPath is null because this is a runtime resolution (not a
        // specific DllImportSearchPath directive).
        return entry.Value.Resolver(libraryName, entry.Value.Assembly, null);
    }

    [DllImport("__Internal")]
    private static extern void RegisterPInvokeResolverCallback(IntPtr callback);
}
