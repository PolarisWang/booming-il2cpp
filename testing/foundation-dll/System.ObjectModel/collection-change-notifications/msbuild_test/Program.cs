
public class Program
{
    static int Main()
    {
        int failures = 0;
        CollectionChangeNotificationsNativeEntry._exitCode = 0; CollectionChangeNotificationsNativeEntry.Method0(); failures += CollectionChangeNotificationsNativeEntry._exitCode << 0;
        CollectionChangeNotificationsNativeEntry._exitCode = 0; CollectionChangeNotificationsNativeEntry.Method1(); failures += CollectionChangeNotificationsNativeEntry._exitCode << 1;
        return failures;
    }
}
