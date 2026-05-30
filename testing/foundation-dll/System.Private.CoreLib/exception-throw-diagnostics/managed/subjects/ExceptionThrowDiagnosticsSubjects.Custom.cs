// Handwritten custom subject implementations for exception-throw-diagnostics family.
//
// Subjects 6 (Exception.ToString) and 7 (Exception.GetBaseException) are
// instance methods that the auto-generator could not resolve.

using System;
using Chaos.TestFramework;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // [6] System.Exception::ToString()
    public static void CustomEntrySubject_6()
    {
        try
        {
        Assert.IsNotNull(new Exception().ToString().Length);
        }
        catch { _exitCode = 1; }

    }

    // [7] System.Exception::GetBaseException()
    public static void CustomEntrySubject_7()
    {
        try
        {
        Assert.IsNotNull(new Exception().GetBaseException().GetHashCode());
        }
        catch { _exitCode = 1; }

    }
}
