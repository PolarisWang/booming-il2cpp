
public class Program
{
    static int Main()
    {
        int failures = 0;
        PropertyChangeNotificationsPatchEntry._exitCode = 0; PropertyChangeNotificationsPatchEntry.Method0(); failures += PropertyChangeNotificationsPatchEntry._exitCode << 0;
        PropertyChangeNotificationsPatchEntry._exitCode = 0; PropertyChangeNotificationsPatchEntry.Method1(); failures += PropertyChangeNotificationsPatchEntry._exitCode << 1;
        PropertyChangeNotificationsPatchEntry._exitCode = 0; PropertyChangeNotificationsPatchEntry.Method2(); failures += PropertyChangeNotificationsPatchEntry._exitCode << 2;
        PropertyChangeNotificationsPatchEntry._exitCode = 0; PropertyChangeNotificationsPatchEntry.Method3(); failures += PropertyChangeNotificationsPatchEntry._exitCode << 3;
        return failures;
    }
}
