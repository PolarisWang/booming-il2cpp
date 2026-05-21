
public class Program
{
    static int Main()
    {
        int failures = 0;
        ConverterFactoryNativeEntry._exitCode = 0; ConverterFactoryNativeEntry.Method0(); failures += ConverterFactoryNativeEntry._exitCode << 0;
        ConverterFactoryNativeEntry._exitCode = 0; ConverterFactoryNativeEntry.Method1(); failures += ConverterFactoryNativeEntry._exitCode << 1;
        return failures;
    }
}
