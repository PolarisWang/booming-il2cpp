
public class Program
{
    static int Main()
    {
        int failures = 0;
        ObjectModelCollectionsNativeEntry._exitCode = 0; ObjectModelCollectionsNativeEntry.Method0(); failures += ObjectModelCollectionsNativeEntry._exitCode << 0;
        ObjectModelCollectionsNativeEntry._exitCode = 0; ObjectModelCollectionsNativeEntry.Method1(); failures += ObjectModelCollectionsNativeEntry._exitCode << 1;
        ObjectModelCollectionsNativeEntry._exitCode = 0; ObjectModelCollectionsNativeEntry.Method2(); failures += ObjectModelCollectionsNativeEntry._exitCode << 2;
        ObjectModelCollectionsNativeEntry._exitCode = 0; ObjectModelCollectionsNativeEntry.Method3(); failures += ObjectModelCollectionsNativeEntry._exitCode << 3;
        ObjectModelCollectionsNativeEntry._exitCode = 0; ObjectModelCollectionsNativeEntry.Method4(); failures += ObjectModelCollectionsNativeEntry._exitCode << 4;
        ObjectModelCollectionsNativeEntry._exitCode = 0; ObjectModelCollectionsNativeEntry.Method5(); failures += ObjectModelCollectionsNativeEntry._exitCode << 5;
        return failures;
    }
}
