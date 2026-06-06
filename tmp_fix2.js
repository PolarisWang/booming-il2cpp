const fs = require('fs');
let code = fs.readFileSync('src/managed/Chaos.IL2CPP.Generator/CodeGenStage.cs', 'utf8');

const insert = "\n" +
"        // Phase L2: Bridge method AOT compilation (post-processing).\n" +
"        try\n" +
"        {\n" +
"            var bridgeCompiler = new BridgeAotCompiler(linkedWorld, codeRegistration);\n" +
"            var bridgedMethods = bridgeCompiler.CompileBridgedMethods(aotCoreIr);\n" +
"            if (bridgedMethods.Count > 0)\n" +
"            {\n" +
"                var existing = aotCoreIr.Methods.ToList();\n" +
"                existing.AddRange(bridgedMethods);\n" +
"                aotCoreIr = aotCoreIr with { Methods = existing };\n" +
'                Console.Error.WriteLine($"[BRIDGE-AOT] Compiled {bridgedMethods.Count} bridged method(s)");\n' +
"            }\n" +
"        }\n" +
"        catch (Exception ex)\n" +
"        {\n" +
'            Console.Error.WriteLine($"[BRIDGE-AOT] Compilation failed: {ex.Message}");\n' +
"        }\n";

const marker = "var aotCoreIr = new AotCoreIrLowering().Create(linkedWorld, typedIl, codeRegistration);\n        var genericInstantiationDemandGraph";
const replacement = "var aotCoreIr = new AotCoreIrLowering().Create(linkedWorld, typedIl, codeRegistration);" + insert + "        var genericInstantiationDemandGraph";

if (!code.includes(marker)) {
    console.log("Marker NOT found!");
    process.exit(1);
}

code = code.replace(marker, replacement);
fs.writeFileSync('src/managed/Chaos.IL2CPP.Generator/CodeGenStage.cs', code);
console.log("Insertion successful");
