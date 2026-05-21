
public class Program
{
    static int Main()
    {
        int failures = 0;
        CollectionChangeNotificationsPatchEntry._exitCode = 0; CollectionChangeNotificationsPatchEntry.Method0(); failures += CollectionChangeNotificationsPatchEntry._exitCode << 0;
        CollectionChangeNotificationsPatchEntry._exitCode = 0; CollectionChangeNotificationsPatchEntry.Method1(); failures += CollectionChangeNotificationsPatchEntry._exitCode << 1;
        return failures;
    }
}
