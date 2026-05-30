// Handwritten custom subject implementations for exception-throw-diagnostics family.
//
// Subjects 6 (Exception.ToString) and 7 (Exception.GetBaseException) are
// instance methods that the auto-generator could not resolve.

using System;
using Chaos.TestFramework;

public static partial class ExceptionThrowDiagnosticsSubjects
{
    // [6] System.Exception::ToString()
    [Fact]
    public static void CustomEntrySubject_6()
    {
        Assert.IsNotNull(new Exception().ToString().Length);
    }

    // [7] System.Exception::GetBaseException()
    [Fact]
    public static void CustomEntrySubject_7()
    {
        Assert.IsNotNull(new Exception().GetBaseException().GetHashCode());
    }
}
