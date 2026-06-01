namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Windows native platform — uses prebuilt .lib files from CI artifacts.
/// No source build needed; libraries are copied from artifacts/presets/windows-x64-reference/.
/// </summary>
public sealed class WindowsNativePlatform : INativePlatform
{
    public string StaticLibPrefix => "";
    public string StaticLibSuffix => ".lib";
    public bool HasPrebuiltLibraries => true;

    public string GetStaticLibFileName(string baseName) => $"{baseName}.lib";

    public bool BuildNativeLibs(string repoRoot, string libDir, string configTier)
    {
        // Windows uses prebuilt .lib files — no source build needed.
        return true;
    }
}
