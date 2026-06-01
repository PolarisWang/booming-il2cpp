namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Platform abstraction for native library build/packaging in the chaos SDK.
///
/// Each platform defines its static library naming convention and whether
/// prebuilt libraries are available (Windows SDK) or must be built from
/// source (Linux, macOS, etc.).
/// </summary>
public interface INativePlatform
{
    /// <summary>Static library filename prefix: "" on Windows, "lib" on Unix.</summary>
    string StaticLibPrefix { get; }

    /// <summary>Static library filename suffix: ".lib" on Windows, ".a" on Unix.</summary>
    string StaticLibSuffix { get; }

    /// <summary>Full static library file name for a given base name.</summary>
    string GetStaticLibFileName(string baseName);

    /// <summary>
    /// True when prebuilt native libraries exist in the artifacts/presets/ tree
    /// (Windows with CI-built .lib files).  False on platforms where the SDK
    /// must build native libs from source (Linux via cmake).
    /// </summary>
    bool HasPrebuiltLibraries { get; }

    /// <summary>
    /// Build native runtime libraries from the repo source tree into the SDK
    /// lib directory.  Only called when <see cref="HasPrebuiltLibraries"/> is false.
    /// Returns true if all expected libraries were built successfully.
    /// </summary>
    bool BuildNativeLibs(string repoRoot, string libDir, string configTier);
}
