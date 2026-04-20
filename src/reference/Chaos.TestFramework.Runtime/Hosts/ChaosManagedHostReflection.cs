using System;
using System.IO;
using System.Linq;
using System.Reflection;

namespace Chaos.TestFramework.Runtime;

internal static class ChaosManagedHostReflection
{
    public static ChaosTestCollectionEntry ResolveEntry(ChaosManagedHostKind hostKind, ChaosManagedHostRequest request)
    {
        return ChaosTestCollectionLoader.ResolveEntry(request.CollectionPath, hostKind, request.EntryIndex);
    }

    public static void InvokeStaticEntry(ChaosTestCollectionEntry entry)
    {
        ArgumentNullException.ThrowIfNull(entry);

        var assembly = ResolveAssembly(entry.AssemblyName);
        var type = assembly.GetType(entry.DeclaringType, throwOnError: true, ignoreCase: false)
            ?? throw new InvalidOperationException($"unable to resolve declaring type: {entry.DeclaringType}");
        var method = ResolveMethod(type, entry.MethodName, entry.MethodSignature);
        if (method.GetParameters().Length != 0)
        {
            throw new InvalidOperationException($"managed host entry must be parameterless: {entry.DeclaringType}::{entry.MethodSignature}");
        }

        method.Invoke(null, null);
    }

    private static Assembly ResolveAssembly(string assemblyName)
    {
        if (string.IsNullOrWhiteSpace(assemblyName))
        {
            throw new InvalidOperationException("managed host entry is missing assemblyName.");
        }

        foreach (var loadedAssembly in AppDomain.CurrentDomain.GetAssemblies())
        {
            if (string.Equals(loadedAssembly.GetName().Name, assemblyName, StringComparison.Ordinal))
            {
                return loadedAssembly;
            }
        }

        var candidatePath = Path.Combine(AppContext.BaseDirectory, assemblyName + ".dll");
        if (File.Exists(candidatePath))
        {
            return Assembly.LoadFrom(candidatePath);
        }

        return Assembly.Load(new AssemblyName(assemblyName));
    }

    private static MethodInfo ResolveMethod(Type type, string methodName, string methodSignature)
    {
        if (string.IsNullOrWhiteSpace(methodName))
        {
            throw new InvalidOperationException($"managed host entry is missing methodName for type '{type.FullName}'.");
        }

        var hasSignature = !string.IsNullOrWhiteSpace(methodSignature);
        MethodInfo? fallback = null;
        foreach (var candidate in type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic | BindingFlags.Static))
        {
            if (!string.Equals(candidate.Name, methodName, StringComparison.Ordinal))
            {
                continue;
            }

            if (hasSignature && string.Equals(BuildMethodSignature(candidate), methodSignature, StringComparison.Ordinal))
            {
                return candidate;
            }

            fallback ??= candidate;
        }

        return fallback
            ?? throw new InvalidOperationException($"unable to resolve managed host method: {type.FullName}::{methodName}");
    }

    private static string BuildMethodSignature(MethodInfo method)
    {
        var parameters = string.Join(", ", method.GetParameters().Select(parameter => FormatParameterType(parameter.ParameterType)));
        return $"{method.Name}({parameters})";
    }

    private static string FormatParameterType(Type type)
    {
        if (!type.IsGenericType)
        {
            return type.FullName ?? type.Name;
        }

        var genericName = type.GetGenericTypeDefinition().FullName ?? type.Name;
        var tickIndex = genericName.IndexOf('`');
        if (tickIndex >= 0)
        {
            genericName = genericName[..tickIndex];
        }

        var genericArguments = string.Join(", ", type.GetGenericArguments().Select(FormatParameterType));
        return $"{genericName}<{genericArguments}>";
    }
}
