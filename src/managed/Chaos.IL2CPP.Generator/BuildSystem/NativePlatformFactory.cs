using System.Runtime.InteropServices;

namespace Chaos.IL2CPP.Generator.BuildSystem;

/// <summary>
/// Creates the platform-specific <see cref="INativePlatform"/> implementation
/// based on the current OS.
/// </summary>
public static class NativePlatformFactory
{
    /// <summary>
    /// Create the appropriate platform implementation for the current OS.
    /// </summary>
    public static INativePlatform Create()
    {
        if (RuntimeInformation.IsOSPlatform(OSPlatform.Windows))
            return new WindowsNativePlatform();
        return new LinuxNativePlatform();
    }
}
