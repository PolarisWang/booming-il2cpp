
public class Program
{
    static int Main()
    {
        int failures = 0;
        PropertyChangeNotificationsNativeEntry._exitCode = 0; PropertyChangeNotificationsNativeEntry.Method0(); failures += PropertyChangeNotificationsNativeEntry._exitCode << 0;
        PropertyChangeNotificationsNativeEntry._exitCode = 0; PropertyChangeNotificationsNativeEntry.Method1(); failures += PropertyChangeNotificationsNativeEntry._exitCode << 1;
        PropertyChangeNotificationsNativeEntry._exitCode = 0; PropertyChangeNotificationsNativeEntry.Method2(); failures += PropertyChangeNotificationsNativeEntry._exitCode << 2;
        PropertyChangeNotificationsNativeEntry._exitCode = 0; PropertyChangeNotificationsNativeEntry.Method3(); failures += PropertyChangeNotificationsNativeEntry._exitCode << 3;
        return failures;
    }
}
