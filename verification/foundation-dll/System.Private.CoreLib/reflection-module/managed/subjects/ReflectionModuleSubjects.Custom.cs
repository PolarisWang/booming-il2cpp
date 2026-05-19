// Handwritten custom subject implementation for Module.GetCustomAttributes.
//
// Method placed here by customEntryIndices in capability-family-contract.json:
// Subject_5 (Module.GetCustomAttributes(Type)) — the auto-generated expression
// can't handle the metadata-vs-runtime type mismatch: metadata says
// System.Object[] but .NET 8+ returns IEnumerable<Attribute> (no .Length).
//
// This file is a partial class of the auto-generated ReflectionModuleSubjects.
// Pipeline includes it automatically when present.

using System;
using System.Collections.Generic;
using System.Reflection;

public static partial class ReflectionModuleSubjects
{
    // [5] System.Private.CoreLib/System.Reflection.Module::GetCustomAttributes:System.Object[](System.Type)
    public static void CustomEntrySubject_5()
    {
        var attrs = typeof(byte).Module.GetCustomAttributes(typeof(AssemblyDescriptionAttribute));
        // Convert IEnumerable<Attribute> to a list so we can check Count
        var list = new List<Attribute>(attrs);
        // At minimum there should be an AssemblyDescriptionAttribute on mscorlib
        if (list.Count == 0) _exitCode = 1;
    }
}
