const fs = require('fs');
let code = fs.readFileSync('src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/BridgeAotCompiler.cs', 'utf8');

// Replace the SubjectIdComponents record and ParseSubjectId method
const oldCode = `    private sealed record SubjectIdComponents(
        string AssemblyName, string DeclaringTypeSubjectId, string DeclaringTypeName,
        string MethodName, List<string> ParameterTypes);

    private static SubjectIdComponents? ParseSubjectId(string subjectId)
    {
        // Format: "Assembly/Type::MethodName:RetType(Param1,Param2)"
        var slash = subjectId.IndexOf('/', StringComparison.Ordinal);
        if (slash <= 0) return null;
        var asm = subjectId[..slash];
        var rest = subjectId[(slash + 1)..];
        var sep = rest.IndexOf("::", StringComparison.Ordinal);
        if (sep < 0) return null;
        var type = rest[..sep];
        var after = rest[(sep + 2)..];
        var paren = after.IndexOf('(', StringComparison.Ordinal);
        if (paren < 0) return null;
        var colon = after.IndexOf(':', StringComparison.Ordinal);
        var mEnd = colon >= 0 && colon < paren ? colon : paren;
        var mName = after[..mEnd];
        var pPart = after[(paren + 1)..^1];
        var pTypes = string.IsNullOrWhiteSpace(pPart) ? new List<string>() : pPart.Split(',').Select(p => p.Trim()).ToList();
        return new SubjectIdComponents(asm, $"{asm}/{type}", type, mName, pTypes);
    }`;

const newCode = `    private sealed record SubjectIdComponents(
        string AssemblyName, string DeclaringTypeSubjectId, string DeclaringTypeName,
        string MethodName, string ReturnType, List<string> ParameterTypes);

    private static SubjectIdComponents? ParseSubjectId(string subjectId)
    {
        // Format: "Assembly/Type::MethodName:ReturnType(Param1,Param2)"
        var slash = subjectId.IndexOf('/', StringComparison.Ordinal);
        if (slash <= 0) return null;
        var asm = subjectId[..slash];
        var rest = subjectId[(slash + 1)..];
        var sep = rest.IndexOf("::", StringComparison.Ordinal);
        if (sep < 0) return null;
        var type = rest[..sep];
        var after = rest[(sep + 2)..];
        var paren = after.IndexOf('(', StringComparison.Ordinal);
        if (paren < 0) return null;

        // MethodName:ReturnType(Params) - extract return type between ':' and '('
        var beforeParen = after[..paren];
        var returnColon = beforeParen.LastIndexOf(':');
        string methodName, returnType;
        if (returnColon >= 0)
        {
            methodName = after[..returnColon];
            returnType = after[(returnColon + 1)..paren];
        }
        else
        {
            methodName = beforeParen;
            returnType = "System.Void";
        }

        var pPart = after[(paren + 1)..^1];
        var pTypes = string.IsNullOrWhiteSpace(pPart) ? new List<string>() : pPart.Split(',').Select(p => p.Trim()).ToList();
        return new SubjectIdComponents(asm, $"{asm}/{type}", type, methodName, returnType, pTypes);
    }`;

if (!code.includes(oldCode)) { console.log('OLD CODE NOT FOUND'); process.exit(1); }
code = code.replace(oldCode, newCode);

// Also fix the ReturnType hardcode in CompileSingleMethod - use comp.ReturnType
const oldReturn = "ReturnType = \"System.Void\",";
const newReturn = "ReturnType = comp.ReturnType,";
if (!code.includes(oldReturn)) { console.log('OLD RETURN NOT FOUND'); process.exit(1); }
code = code.replace(oldReturn, newReturn);

fs.writeFileSync('src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/BridgeAotCompiler.cs', code);
console.log('FIXED');
