
public class Program
{
    static int Main()
    {
        int failures = 0;
        ConverterFactoryPatchEntry._exitCode = 0; ConverterFactoryPatchEntry.Method0(); failures += ConverterFactoryPatchEntry._exitCode << 0;
        ConverterFactoryPatchEntry._exitCode = 0; ConverterFactoryPatchEntry.Method1(); failures += ConverterFactoryPatchEntry._exitCode << 1;
        return failures;
    }
}
