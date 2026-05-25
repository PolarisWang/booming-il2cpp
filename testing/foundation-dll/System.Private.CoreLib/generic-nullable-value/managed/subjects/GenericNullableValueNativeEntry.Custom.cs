// Handwritten custom entry implementations for Generic Nullable Value.
//
// Method 3 (GetValueOrDefault with default value) has a generic parameter
// that the auto-generator cannot assign.
//
// This file is a READ-ONLY source for the pipeline. Pipeline copies it into
// il2cpp_dist/entrypoint/ before invoking generate_and_build().

public static partial class GenericNullableValueNativeEntry
{
    public static int _exitCode;

    // [3] System.Nullable`1::GetValueOrDefault(T)
    public static void CustomEntryMethod3()
    {
        int? val = 42;
        if (val.GetValueOrDefault(0) != 42) _exitCode = 1;
    }
}
