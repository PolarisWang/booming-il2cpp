
public class Program
{
    static int Main()
    {
        int failures = 0;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method0(); failures += ConvertersNativeEntry._exitCode << 0;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method1(); failures += ConvertersNativeEntry._exitCode << 1;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method2(); failures += ConvertersNativeEntry._exitCode << 2;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method3(); failures += ConvertersNativeEntry._exitCode << 3;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method4(); failures += ConvertersNativeEntry._exitCode << 4;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method5(); failures += ConvertersNativeEntry._exitCode << 5;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method6(); failures += ConvertersNativeEntry._exitCode << 6;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method7(); failures += ConvertersNativeEntry._exitCode << 7;
        ConvertersNativeEntry._exitCode = 0; ConvertersNativeEntry.Method8(); failures += ConvertersNativeEntry._exitCode << 8;
        return failures;
    }
}
