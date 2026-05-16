
public class Program
{
    static int Main()
    {
        int failures = 0;
        GarbageCollectionPatchEntry._exitCode = 0; GarbageCollectionPatchEntry.Method0(); failures += GarbageCollectionPatchEntry._exitCode << 0;
        GarbageCollectionPatchEntry._exitCode = 0; GarbageCollectionPatchEntry.Method1(); failures += GarbageCollectionPatchEntry._exitCode << 1;
        GarbageCollectionPatchEntry._exitCode = 0; GarbageCollectionPatchEntry.Method2(); failures += GarbageCollectionPatchEntry._exitCode << 2;
        GarbageCollectionPatchEntry._exitCode = 0; GarbageCollectionPatchEntry.Method3(); failures += GarbageCollectionPatchEntry._exitCode << 3;
        GarbageCollectionPatchEntry._exitCode = 0; GarbageCollectionPatchEntry.Method4(); failures += GarbageCollectionPatchEntry._exitCode << 4;
        GarbageCollectionPatchEntry._exitCode = 0; GarbageCollectionPatchEntry.Method5(); failures += GarbageCollectionPatchEntry._exitCode << 5;
        return failures;
    }
}
