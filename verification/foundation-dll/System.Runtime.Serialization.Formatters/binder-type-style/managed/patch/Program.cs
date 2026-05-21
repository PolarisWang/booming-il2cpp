
public class Program
{
    static int Main()
    {
        int failures = 0;
        BinderTypeStylePatchEntry._exitCode = 0; BinderTypeStylePatchEntry.Method0(); failures += BinderTypeStylePatchEntry._exitCode << 0;
        BinderTypeStylePatchEntry._exitCode = 0; BinderTypeStylePatchEntry.Method1(); failures += BinderTypeStylePatchEntry._exitCode << 1;
        return failures;
    }
}
