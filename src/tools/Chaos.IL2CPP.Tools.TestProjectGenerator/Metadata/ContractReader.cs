using System.Text.Json;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

public sealed class ContractReader
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    };

    /// <summary>
    /// Read a contract.json file and produce SubjectModel list from methodSubjectIds.
    /// Each methodSubjectId has format: Assembly/FullTypeName::MethodName:ReturnType(Params)
    /// </summary>
    public List<SubjectModel> ReadContract(string contractPath, SubjectKind kind = SubjectKind.Fact)
    {
        var json = File.ReadAllText(contractPath);
        using var doc = JsonDocument.Parse(json);
        var root = doc.RootElement;

        var assemblyName = root.TryGetProperty("assemblyName", out var asmEl)
            ? asmEl.GetString() ?? ""
            : "";

        var methodSubjectIds = new List<string>();
        if (root.TryGetProperty("methodSubjectIds", out var midsEl))
        {
            foreach (var item in midsEl.EnumerateArray())
            {
                var s = item.GetString();
                if (s is not null)
                    methodSubjectIds.Add(s);
            }
        }

        var subjects = new List<SubjectModel>();
        foreach (var subjectId in methodSubjectIds)
        {
            var parsed = ParseMethodSubjectId(subjectId);
            subjects.Add(new SubjectModel
            {
                Kind = kind,
                AssemblyName = parsed.assemblyName ?? assemblyName,
                FullTypeName = parsed.fullTypeName ?? "",
                MethodName = parsed.methodName ?? "",
                IsStatic = true,
                ReturnTypeFullName = parsed.returnType ?? "",
                Parameters = parsed.parameters,
                SubjectId = subjectId,
            });
        }

        return subjects;
    }

    private static (string? assemblyName, string? fullTypeName, string? methodName, string? returnType, List<ParameterModel> parameters)
        ParseMethodSubjectId(string subjectId)
    {
        // Format: Assembly/FullTypeName::MethodName:ReturnType(Param1,Param2,...)
        var assemblyName = (string?)null;
        var fullTypeName = (string?)null;
        var methodName = (string?)null;
        var returnType = (string?)null;
        var parameters = new List<ParameterModel>();

        // Split on :: to separate declaring type from method signature
        var doubleColonIdx = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (doubleColonIdx < 0)
            return (null, null, null, null, parameters);

        var typePart = subjectId[..doubleColonIdx];
        var methodPart = subjectId[(doubleColonIdx + 2)..];

        // typePart is Assembly/FullTypeName
        var slashIdx = typePart.IndexOf('/');
        if (slashIdx >= 0)
        {
            assemblyName = typePart[..slashIdx];
            fullTypeName = typePart[(slashIdx + 1)..];
        }
        else
        {
            fullTypeName = typePart;
        }

        // methodPart is MethodName:ReturnType(Param1,Param2,...)
        var colonIdx = methodPart.IndexOf(':');
        if (colonIdx >= 0)
        {
            methodName = methodPart[..colonIdx];
            var sigPart = methodPart[(colonIdx + 1)..];

            // sigPart is ReturnType(Param1,Param2,...)
            var parenIdx = sigPart.IndexOf('(');
            if (parenIdx >= 0)
            {
                returnType = sigPart[..parenIdx];
                var paramsPart = sigPart[(parenIdx + 1)..];
                if (paramsPart.EndsWith(')'))
                    paramsPart = paramsPart[..^1];

                if (!string.IsNullOrEmpty(paramsPart))
                {
                    var paramTypes = SplitParams(paramsPart);
                    for (int i = 0; i < paramTypes.Count; i++)
                    {
                        parameters.Add(new ParameterModel
                        {
                            TypeFullName = paramTypes[i],
                            Name = $"p{i}",
                        });
                    }
                }
            }
        }
        else
        {
            methodName = methodPart;
        }

        return (assemblyName, fullTypeName, methodName, returnType, parameters);
    }

    private static List<string> SplitParams(string paramsPart)
    {
        // Handle generic types with nested commas: e.g., "System.Collections.Generic.List<System.Int32>,System.String"
        var result = new List<string>();
        var depth = 0;
        var start = 0;
        for (int i = 0; i < paramsPart.Length; i++)
        {
            switch (paramsPart[i])
            {
                case '<':
                case '[':
                    depth++;
                    break;
                case '>':
                case ']':
                    depth--;
                    break;
                case ',' when depth == 0:
                    result.Add(paramsPart[start..i].Trim());
                    start = i + 1;
                    break;
            }
        }
        if (start < paramsPart.Length)
            result.Add(paramsPart[start..].Trim());

        return result;
    }
}
