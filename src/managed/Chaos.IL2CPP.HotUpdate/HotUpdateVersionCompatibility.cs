using System.Globalization;

namespace Chaos.IL2CPP.HotUpdate;

public readonly record struct HotUpdateSemanticVersion(int Major, int Minor, int Patch)
{
    public static bool TryParse(string? value, out HotUpdateSemanticVersion version)
    {
        version = default;
        if (string.IsNullOrWhiteSpace(value))
        {
            return false;
        }

        var segments = value.Split('.', StringSplitOptions.TrimEntries);
        if (segments.Length != 3)
        {
            return false;
        }

        if (!TryParseSegment(segments[0], out var major) ||
            !TryParseSegment(segments[1], out var minor) ||
            !TryParseSegment(segments[2], out var patch))
        {
            return false;
        }

        version = new HotUpdateSemanticVersion(major, minor, patch);
        return true;
    }

    public bool IsCompatibleWith(HotUpdateSemanticVersion runtimeVersion)
    {
        return Major == runtimeVersion.Major && Minor == runtimeVersion.Minor;
    }

    private static bool TryParseSegment(string value, out int parsed)
    {
        return int.TryParse(value, NumberStyles.None, CultureInfo.InvariantCulture, out parsed) && parsed >= 0;
    }
}
