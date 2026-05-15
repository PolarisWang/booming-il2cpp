using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Driver;

internal static class AsmCompareReport
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
        NumberHandling = System.Text.Json.Serialization.JsonNumberHandling.AllowNamedFloatingPointLiterals,
    };

    public static string Generate(
        string assemblyPath,
        string methodName,
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact? aotMethod,
        string cppSource,
        NativeCompile.NativeCompileResult? nativeResult,
        AsmCompareConfig config)
    {
        var sb = new StringBuilder();

        if (config.HasSection("header"))
            WriteHeader(sb, assemblyPath, methodName, jitResult, aotMethod);

        if (config.HasSection("il-dump"))
            WriteIlDump(sb, jitResult);

        if (config.HasSection("raw-jit") && jitResult.Success)
            WriteRawJit(sb, jitResult);

        if (config.HasSection("raw-cpp"))
            WriteRawCpp(sb, cppSource);

        if (config.HasSection("raw-aot") && nativeResult is not null)
            WriteRawAot(sb, nativeResult);

        if (config.HasSection("side-by-side"))
            WriteSideBySideV2(sb, jitResult, aotMethod, nativeResult, cppSource);

        if (config.HasSection("ir-trace") && aotMethod is not null)
            WriteIrTrace(sb, jitResult, aotMethod);

        if (config.HasSection("metrics"))
            WriteMetrics(sb, jitResult, aotMethod);

        if (config.HasSection("inline-map") && aotMethod is not null)
            WriteInlineMap(sb, aotMethod);

        if (config.HasSection("analysis") && aotMethod is not null)
            WriteAnalysis(sb, jitResult, aotMethod);

        return sb.ToString();
    }

    // ── JSON output ──────────────────────────────────────────────────────────

    public static string GenerateJson(
        string assemblyPath,
        string methodName,
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact? aotMethod,
        string cppSource,
        NativeCompile.NativeCompileResult? nativeResult,
        AsmCompareConfig config)
    {
        var data = new Dictionary<string, object?>();

        data["tool"] = "asm-compare";
        data["method"] = methodName;
        data["assembly"] = assemblyPath;
        data["capturedAt"] = DateTime.UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ");

        // HEADER
        data["jit"] = new Dictionary<string, object?>
        {
            ["status"] = jitResult.Success ? "ok" : "failed",
            ["error"] = jitResult.Error,
            ["address"] = jitResult.Success ? $"0x{jitResult.Address.ToInt64():x}" : null,
            ["size"] = jitResult.Success ? jitResult.Size : null,
            ["instructions"] = jitResult.Success ? jitResult.Instructions?.Count : null,
        };

        data["aot"] = aotMethod is null ? null : new Dictionary<string, object?>
        {
            ["status"] = "ok",
            ["symbol"] = aotMethod.NativeSymbol,
            ["methodId"] = aotMethod.MethodId,
            ["subjectId"] = aotMethod.SubjectId,
            ["signature"] = aotMethod.Signature,
            ["instructions"] = aotMethod.Instructions.Count,
            ["locals"] = aotMethod.LocalCount,
            ["params"] = aotMethod.ParameterCount,
            ["returnType"] = aotMethod.ReturnType,
            ["isStatic"] = aotMethod.IsStatic,
        };

        // IL-DUMP
        if (config.HasSection("il-dump") && jitResult.IlBody?.Instructions is not null)
        {
            data["ilDump"] = jitResult.IlBody.Instructions.Select(il => new
            {
                offset = $"IL_{il.Offset:x4}",
                opCode = il.OpCode,
                operand = string.IsNullOrEmpty(il.Operand) ? null : il.Operand,
            }).ToList();
        }

        // RAW-JIT
        if (config.HasSection("raw-jit") && jitResult.Success && jitResult.Instructions is not null)
        {
            data["rawJit"] = jitResult.Instructions.Select(i => new
            {
                address = $"0x{i.Address.ToInt64():x}",
                bytes = i.HexBytes,
                mnemonic = i.Mnemonic,
                operands = i.Operands,
            }).ToList();
        }

        // RAW-CPP
        if (config.HasSection("raw-cpp") && !string.IsNullOrEmpty(cppSource))
        {
            var lines = cppSource.Split('\n');
            data["rawCpp"] = new Dictionary<string, object?>
            {
                ["lineCount"] = lines.Length,
                ["content"] = cppSource,
            };
        }

        // RAW-AOT
        if (config.HasSection("raw-aot") && nativeResult is not null)
        {
            var aot = new Dictionary<string, object?>
            {
                ["msvcFound"] = nativeResult.FoundMsvc,
                ["compileSuccess"] = nativeResult.CompileSuccess,
                ["msvcVersion"] = nativeResult.MsvcVersion,
                ["objectPath"] = nativeResult.ObjectPath,
                ["objectSize"] = nativeResult.ObjectSize,
                ["error"] = nativeResult.Error,
            };

            if (nativeResult.AotDisasm?.Success == true && nativeResult.AotDisasm.Instructions is not null)
            {
                aot["disassembly"] = nativeResult.AotDisasm.Instructions.Select(i => new
                {
                    address = $"0x{i.Address.ToInt64():x}",
                    bytes = i.HexBytes,
                    mnemonic = i.Mnemonic,
                    operands = i.Operands,
                }).ToList();
            }

            if (nativeResult.Disassembly is not null)
                aot["dumpbinDisassembly"] = nativeResult.Disassembly;

            data["rawAot"] = aot;
        }

        // METRICS
        if (config.HasSection("metrics"))
        {
            long jitInstrCount = jitResult.Success ? (jitResult.Instructions?.Count ?? 0) : 0;
            long aotInstrCount = aotMethod?.Instructions.Count ?? 0;
            long jitBytes = jitResult.Success ? jitResult.Size : 0;

            data["metrics"] = new Dictionary<string, object>
            {
                ["jitInstructionCount"] = jitInstrCount,
                ["aotInstructionCount"] = aotInstrCount,
                ["jitCodeSize"] = jitBytes,
                ["aotApproxCodeSize"] = aotInstrCount * 8L,
                ["ratio"] = jitInstrCount > 0 ? Math.Round(aotInstrCount / (double)jitInstrCount, 2) : null,
            };
        }

        // ANALYSIS
        if (config.HasSection("analysis") && aotMethod is not null)
        {
            var findings = new List<Dictionary<string, string?>>();
            foreach (var call in aotMethod.Instructions.Where(i => i.DispatchKindCode == HybridDispatchKind.ExternalRuntime))
            {
                findings.Add(new()
                {
                    ["severity"] = "info",
                    ["category"] = "external_runtime_call",
                    ["message"] = $"Call to '{call.Callee ?? call.TargetSymbol ?? "unknown"}' uses ExternalRuntime dispatch",
                });
            }
            if (findings.Count > 0)
                data["analysis"] = findings;
        }

        return JsonSerializer.Serialize(data, JsonOptions);
    }

    // ── Section: HEADER ─────────────────────────────────────────────────────

    private static void WriteHeader(StringBuilder sb, string assemblyPath, string methodName,
        JitAsmCapture.JitCaptureResult jitResult, AotCoreIrMethodArtifact? aotMethod)
    {
        sb.AppendLine("========================================================================");
        sb.AppendLine("ASM-COMPARE  |  " + methodName);
        sb.AppendLine("========================================================================");
        sb.AppendLine();
        sb.AppendLine("[HEADER]");
        sb.AppendLine($"  binary        = {assemblyPath}");
        sb.AppendLine($"  method        = {methodName}");
        sb.AppendLine($"  jit_status    = {(jitResult.Success ? "ok" : $"failed: {jitResult.Error}")}");
        sb.AppendLine($"  aot_status    = {(aotMethod is not null ? "ok" : "not found in pipeline output")}");
        sb.AppendLine($"  jit_source    = CoreCLR JIT x64");
        sb.AppendLine($"  aot_source    = Chaos IL2CPP AOT (NativeAotEmitter)");
        sb.AppendLine($"  captured_at   = {DateTime.UtcNow:yyyy-MM-ddTHH:mm:ssZ}");
        sb.AppendLine();

        if (jitResult.Success)
        {
            sb.AppendLine($"  jit.address   = 0x{jitResult.Address.ToInt64():x}");
            sb.AppendLine($"  jit.size      = {jitResult.Size} bytes");
            sb.AppendLine($"  jit.instructions = {jitResult.Instructions?.Count ?? 0}");
        }

        if (aotMethod is not null)
        {
            sb.AppendLine($"  aot.symbol    = {aotMethod.NativeSymbol}");
            sb.AppendLine($"  aot.method_id = {aotMethod.MethodId}");
            sb.AppendLine($"  aot.subject_id = {aotMethod.SubjectId}");
            sb.AppendLine($"  aot.signature = {aotMethod.Signature}");
            sb.AppendLine($"  aot.instructions = {aotMethod.Instructions.Count}");
            sb.AppendLine($"  aot.locals    = {aotMethod.LocalCount}");
            sb.AppendLine($"  aot.params    = {aotMethod.ParameterCount}");
            sb.AppendLine($"  aot.return_type = {aotMethod.ReturnType}");
            sb.AppendLine($"  aot.is_static = {aotMethod.IsStatic}");
        }

        sb.AppendLine();
    }

    // ── Section: IL-DUMP ────────────────────────────────────────────────────

    private static void WriteIlDump(StringBuilder sb, JitAsmCapture.JitCaptureResult jitResult)
    {
        sb.AppendLine("[IL-DUMP]");
        sb.AppendLine();

        if (jitResult.IlBody?.Instructions is null)
        {
            sb.AppendLine("  (IL body not available)");
            sb.AppendLine();
            return;
        }

        foreach (var il in jitResult.IlBody.Instructions)
        {
            var offset = $"IL_{il.Offset:x4}";
            var operand = string.IsNullOrEmpty(il.Operand) ? "" : $" {il.Operand}";
            sb.AppendLine($"  {offset,-12}{il.OpCode,-16}{operand}");
        }

        sb.AppendLine();
    }

    // ── Section: RAW-JIT ────────────────────────────────────────────────────

    private static void WriteRawJit(StringBuilder sb, JitAsmCapture.JitCaptureResult jitResult)
    {
        sb.AppendLine("[RAW-JIT]");
        sb.AppendLine();

        if (jitResult.HexDump is not null)
        {
            sb.Append(jitResult.HexDump);
        }
        else if (jitResult.Bytes is not null)
        {
            // Fallback: raw hex dump
            for (int i = 0; i < jitResult.Bytes.Length; i += 16)
            {
                var addr = $"0x{(jitResult.Address.ToInt64() + i):x}";
                var hex = string.Join(" ", jitResult.Bytes.Skip(i).Take(16).Select(b => $"{b:x2}"));
                sb.AppendLine($"  {addr,-20}{hex}");
            }
        }
        else
        {
            sb.AppendLine("  (JIT code not available)");
        }

        sb.AppendLine();
    }

    // ── Section: RAW-CPP ────────────────────────────────────────────────────

    private static void WriteRawCpp(StringBuilder sb, string cppSource)
    {
        sb.AppendLine("[RAW-CPP]");
        sb.AppendLine();

        if (string.IsNullOrEmpty(cppSource))
        {
            sb.AppendLine("  (generated C++ not available)");
        }
        else
        {
            // Show line count and first/last lines for context
            var lines = cppSource.Split('\n');
            sb.AppendLine($"  // Generated C++ ({lines.Length} lines)");
            sb.AppendLine();
            sb.Append(cppSource);
        }

        sb.AppendLine();
    }

    // ── Section: RAW-AOT (Phase 3 — native AOT disassembly) ─────────────────

    private static void WriteRawAot(StringBuilder sb, NativeCompile.NativeCompileResult nativeResult)
    {
        sb.AppendLine("[RAW-AOT]  (native AOT compilation & disassembly)");
        sb.AppendLine();

        if (!nativeResult.FoundMsvc)
        {
            sb.AppendLine("  MSVC not found — native AOT compilation unavailable.");
            sb.AppendLine("  Install Visual Studio 2022 with C++ workload.");
            sb.AppendLine();
            return;
        }

        if (!nativeResult.CompileSuccess)
        {
            sb.AppendLine("  (native compilation failed)");
            sb.AppendLine();

            if (nativeResult.MsvcVersion is not null)
                sb.AppendLine($"  MSVC version  = {nativeResult.MsvcVersion}");

            if (nativeResult.CompileOutput is not null)
            {
                sb.AppendLine();
                sb.AppendLine("  Compiler output:");
                sb.AppendLine(nativeResult.CompileOutput);
            }

            if (nativeResult.Error is not null)
                sb.AppendLine($"  Error: {nativeResult.Error}");

            sb.AppendLine();
            return;
        }

        sb.AppendLine($"  MSVC version  = {nativeResult.MsvcVersion}");
        sb.AppendLine($"  Object file   = {nativeResult.ObjectPath}");
        sb.AppendLine($"  Object size   = {nativeResult.ObjectSize}");
        sb.AppendLine();

        if (nativeResult.Disassembly is not null)
        {
            sb.AppendLine("  -- dumpbin /DISASM --");
            sb.AppendLine(nativeResult.Disassembly);
        }

        // In-house decoder disassembly (unified format with JIT)
        if (nativeResult.AotDisasm?.Success == true && nativeResult.AotDisasm.HexDump is not null)
        {
            sb.AppendLine();
            sb.AppendLine("  -- In-house decoder (unified format) --");
            sb.AppendLine($"  .text sections: {nativeResult.TextSectionName}");
            sb.AppendLine($"  .text size: {nativeResult.AotDisasm.Size} bytes");
            sb.AppendLine($"  Instructions: {nativeResult.AotDisasm.Instructions?.Count ?? 0}");
            sb.AppendLine();
            sb.Append(nativeResult.AotDisasm.HexDump);
        }
        else if (nativeResult.Disassembly is null)
        {
            sb.AppendLine("  (disassembly not available — dumpbin.exe not found or no text section)");
        }

        if (nativeResult.CompileOutput is not null && nativeResult.CompileOutput.Length > 0)
        {
            sb.AppendLine();
            sb.AppendLine("  Compiler output:");
            sb.AppendLine(nativeResult.CompileOutput);
        }

        sb.AppendLine();
    }

    // ── Section: SIDE-BY-SIDE (Phase 2 — semantic alignment) ─────────────────

    private static void WriteSideBySideV2(StringBuilder sb,
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact? aotMethod,
        NativeCompile.NativeCompileResult? nativeResult = null,
        string? cppSource = null)
    {
        sb.AppendLine("[SIDE-BY-SIDE]  (native-vs-native aligned comparison)");
        sb.AppendLine();

        // Phase 3: native-vs-native when AOT disassembly is available
        if (nativeResult?.AotDisasm?.Success == true && jitResult.Success)
        {
            var blocks = AsmSemanticAligner.AlignNativeVsNative(jitResult, nativeResult.AotDisasm);
            sb.Append(AsmSemanticAligner.RenderNativeBlocks(blocks));
            return;
        }

        // Fallback: IR-vs-JIT alignment
        if (aotMethod is null)
        {
            sb.AppendLine("  (AOT data required for side-by-side comparison)");
            sb.AppendLine();
            return;
        }

        var irBlocks = AsmSemanticAligner.Align(jitResult, aotMethod);
        sb.Append(AsmSemanticAligner.RenderBlocks(irBlocks));
    }

    private static string FormatAotOp(AotCoreIrInstructionArtifact instr)
    {
        var op = instr.Op;

        if (!string.IsNullOrEmpty(instr.Callee))
            return $"{op,-20} {instr.Callee}";

        if (instr.Operand is not null)
        {
            var operandStr = instr.Operand?.ToString() ?? "";
            if (operandStr.Length > 60)
                operandStr = operandStr[..57] + "...";
            return $"{op,-20} {operandStr}";
        }

        if (instr.RuntimeServiceKind is not null)
            return $"{op,-20} [runtime_service: {instr.RuntimeServiceKind}]";

        if (!string.IsNullOrEmpty(instr.TargetSymbol))
            return $"{op,-20} symbol: {instr.TargetSymbol}";

        if (instr.DispatchKindCode is not null)
            return $"{op,-20} dispatch: {instr.DispatchKindCode}";

        return op;
    }

    // ── Section: IR-TRACE ────────────────────────────────────────────────────

    private static void WriteIrTrace(StringBuilder sb,
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact aotMethod)
    {
        sb.AppendLine("[IR-TRACE]  (IL → AotCoreIR instruction trace)");
        sb.AppendLine();

        // Group AOT IR by IL offset
        var aotByOffset = new SortedDictionary<int, List<AotCoreIrInstructionArtifact>>();
        foreach (var instr in aotMethod.Instructions)
        {
            if (!aotByOffset.ContainsKey(instr.IlOffset))
                aotByOffset[instr.IlOffset] = new List<AotCoreIrInstructionArtifact>();
            aotByOffset[instr.IlOffset].Add(instr);
        }

        // Build IL offset → opcode mapping
        var ilByOffset = new Dictionary<int, JitAsmCapture.IlInstruction>();
        if (jitResult.IlBody?.Instructions is not null)
        {
            foreach (var il in jitResult.IlBody.Instructions)
                ilByOffset[il.Offset] = il;
        }

        sb.AppendLine($"  Total AotCoreIR instructions: {aotMethod.Instructions.Count}");
        sb.AppendLine($"  Unique IL offsets referenced: {aotByOffset.Count}");
        sb.AppendLine();

        foreach (var kvp in aotByOffset)
        {
            int ilOffset = kvp.Key;
            var aotInstrs = kvp.Value;

            var ilLabel = $"IL_{ilOffset:x4}";

            // Show source IL if available
            if (ilByOffset.TryGetValue(ilOffset, out var ilInstr))
            {
                sb.AppendLine($"  {ilLabel}:    {ilInstr.OpCode,-16}{ilInstr.Operand}");
            }
            else
            {
                sb.AppendLine($"  {ilLabel}:    (no IL instruction — synthetic/AOT-generated)");
            }

            // Show each AOT IR instruction for this offset
            foreach (var aot in aotInstrs)
            {
                var detail = FormatAotIrDetail(aot);
                sb.AppendLine($"    {detail}");
            }

            sb.AppendLine();
        }
    }

    private static string FormatAotIrDetail(AotCoreIrInstructionArtifact instr)
    {
        var parts = new List<string>();
        parts.Add(instr.Op);

        if (!string.IsNullOrEmpty(instr.ResultType) && instr.ResultType != "System.Void")
            parts.Add($":{instr.ResultType}");

        if (!string.IsNullOrEmpty(instr.Callee))
            parts.Add($"call={instr.Callee}");

        if (instr.Operand is not null)
        {
            var val = instr.Operand.ToString() ?? "";
            if (val.Length > 40) val = val[..37] + "...";
            parts.Add($"val={val}");
        }

        if (instr.RuntimeServiceKind is not null)
            parts.Add($"svc={instr.RuntimeServiceKind}");

        if (!string.IsNullOrEmpty(instr.TargetSymbol))
            parts.Add($"sym={instr.TargetSymbol}");

        if (instr.DispatchKindCode is not null)
            parts.Add($"dispatch={instr.DispatchKindCode}");

        if (instr.TargetParameterCount is not null)
            parts.Add($"nparams={instr.TargetParameterCount}");

        if (!string.IsNullOrEmpty(instr.TargetReturnType))
            parts.Add($"ret={instr.TargetReturnType}");

        if (!string.IsNullOrEmpty(instr.ConstrainedTypeSubjectId))
            parts.Add($"constrained={instr.ConstrainedTypeSubjectId}");

        return string.Join(" ", parts);
    }

    // ── Section: METRICS ─────────────────────────────────────────────────────

    private static void WriteMetrics(StringBuilder sb,
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact? aotMethod)
    {
        sb.AppendLine("[METRICS]");
        sb.AppendLine();

        // Column headers
        sb.AppendLine($"  {"Metric",-35} {"JIT",-15} {"AOT (IR)",-15} {"Ratio",-10}");
        sb.AppendLine($"  {"------",-35} {"---",-15} {"---------",-15} {"-----",-10}");
        sb.AppendLine();

        // 1. Instruction count
        long jitInstrCount = jitResult.Success ? (jitResult.Instructions?.Count ?? 0) : 0;
        long aotInstrCount = aotMethod?.Instructions.Count ?? 0;
        WriteMetricRow(sb, "Instruction count (IR)", jitInstrCount, aotInstrCount);

        // 2. Code size
        long jitBytes = jitResult.Success ? jitResult.Size : 0;
        long aotBytes = aotMethod is not null ? aotMethod.Instructions.Count * 8L : 0; // approx IR size
        WriteMetricRow(sb, "Code size (bytes)", jitBytes, aotBytes);

        // 3. Call instructions
        long jitCalls = jitResult.Success
            ? (jitResult.Instructions?.Count(i => i.Mnemonic is "call" or "jmp" or "grp5") ?? 0)
            : 0;
        long aotCalls = aotMethod?.Instructions.Count(i =>
            i.Op == "call" || i.Op == "callvirt" || i.Op == "newobj" || i.Op == "jmp") ?? 0;
        WriteMetricRow(sb, "Call instructions", jitCalls, aotCalls);

        // 4. Branch/jump instructions
        long jitBranches = jitResult.Success
            ? (jitResult.Instructions?.Count(i =>
                i.Mnemonic is "jmp" or "call" or "ret" or
                "jo" or "jno" or "jb" or "jnb" or "jz" or "jnz" or
                "jbe" or "ja" or "js" or "jns" or "jp" or "jnp" or
                "jl" or "jge" or "jle" or "jg" or "loop" or "loope" or "loopne" or "jecxz") ?? 0)
            : 0;
        long aotBranches = aotMethod?.Instructions.Count(i =>
            i.Op == "br" || i.Op == "brtrue" || i.Op == "brfalse" ||
            i.Op.StartsWith("b.") || i.Op == "switch" || i.Op == "ret" ||
            i.Op == "leave" || i.Op == "endfinally") ?? 0;
        WriteMetricRow(sb, "Branch/jump/ret instructions", jitBranches, aotBranches);

        // 5. Memory access (load/store)
        long jitMemAccess = jitResult.Success
            ? (jitResult.Instructions?.Count(i =>
                (i.Mnemonic == "mov" && i.Operands.Contains('[')) ||
                i.Mnemonic == "push" || i.Mnemonic == "pop" ||
                i.Mnemonic is "movzx" or "movsx") ?? 0)
            : 0;
        long aotMemAccess = aotMethod?.Instructions.Count(i =>
            i.Op == "ldfld" || i.Op == "stfld" || i.Op == "ldsfld" ||
            i.Op == "stsfld" || i.Op == "ldind" || i.Op == "stind" ||
            i.Op == "ldobj" || i.Op == "stobj" || i.Op == "ldlen" ||
            i.Op == "ldelem" || i.Op == "stelem") ?? 0;
        WriteMetricRow(sb, "Memory access instructions", jitMemAccess, aotMemAccess);

        // 6. Estimated basic blocks
        long jitBlocks = jitResult.Success
            ? (jitResult.Instructions?.Count(i =>
                i.Mnemonic is "ret" ||
                (i.Mnemonic.Length <= 3 && i.Mnemonic.StartsWith("j") &&
                 i.Mnemonic is not "jmp" and not "jo" and not "jn" and not "jp" and not "js")) + 1 ?? 0)
            : 0;
        long aotBlocks = aotMethod?.Instructions.Count(i =>
            i.Op == "br" || i.Op == "ret" || i.Op == "leave" ||
            i.Op == "endfinally") + 1 ?? 0;
        WriteMetricRow(sb, "Estimated basic blocks", jitBlocks, aotBlocks);

        // 7. Unique IL offsets in AOT
        if (aotMethod is not null)
        {
            var uniqueIlOffsets = aotMethod.Instructions.Select(i => i.IlOffset).Distinct().Count();
            var ilCount = jitResult.IlBody?.Instructions?.Count ?? 0;
            sb.AppendLine();
            sb.AppendLine($"  AOT IR covers {uniqueIlOffsets}/{ilCount} IL offsets ({100.0 * uniqueIlOffsets / Math.Max(ilCount, 1):F1}%)");
        }

        // 8. Opcode category distribution (AOT)
        if (aotMethod is not null)
        {
            sb.AppendLine();
            sb.AppendLine("  AOT IR opcode distribution:");
            var categories = aotMethod.Instructions
                .GroupBy(i => i.Op)
                .OrderByDescending(g => g.Count())
                .Select(g => $"    {g.Key,-25} {g.Count(),4}");

            foreach (var cat in categories)
                sb.AppendLine(cat);
        }

        sb.AppendLine();
    }

    private static void WriteMetricRow(StringBuilder sb, string label, long jit, long aot)
    {
        string ratio = (jit > 0 && aot > 0)
            ? $"{aot / (double)jit:F2}x"
            : (jit == aot ? "1.00x" : "N/A");
        sb.AppendLine($"  {label,-35} {jit,-15} {aot,-15} {ratio,-10}");
    }

    // ── Section: INLINE-MAP ──────────────────────────────────────────────────

    private static void WriteInlineMap(StringBuilder sb, AotCoreIrMethodArtifact aotMethod)
    {
        sb.AppendLine("[INLINE-MAP]  (AOT IR inline/call decisions)");
        sb.AppendLine();

        var callInstructions = aotMethod.Instructions
            .Where(i => i.Op == "call" || i.Op == "callvirt" || i.Op == "newobj" || i.Op == "jmp")
            .ToList();

        if (callInstructions.Count == 0)
        {
            sb.AppendLine("  (no call instructions in AOT IR)");
            sb.AppendLine();
            return;
        }

        foreach (var call in callInstructions)
        {
            var ilOffset = $"IL_{call.IlOffset:x4}";
            var op = call.Op;

            sb.AppendLine($"  {ilOffset} | {op,-10} |");

            if (!string.IsNullOrEmpty(call.Callee))
                sb.AppendLine($"         | callee      = {call.Callee}");

            if (!string.IsNullOrEmpty(call.TargetSymbol))
                sb.AppendLine($"         | symbol      = {call.TargetSymbol}");

            if (call.DispatchKindCode is not null)
                sb.AppendLine($"         | dispatch    = {call.DispatchKindCode}");

            if (!string.IsNullOrEmpty(call.ConstrainedTypeSubjectId))
                sb.AppendLine($"         | constrained = {call.ConstrainedTypeSubjectId}");

            if (call.TargetParameterCount is not null)
                sb.AppendLine($"         | nparams     = {call.TargetParameterCount}");

            if (!string.IsNullOrEmpty(call.TargetReturnType))
                sb.AppendLine($"         | return_type = {call.TargetReturnType}");

            if (call.RuntimeServiceKind is not null)
                sb.AppendLine($"         | runtime_svc = {call.RuntimeServiceKind}");

            sb.AppendLine();
        }
    }

    // ── Section: ANALYSIS ────────────────────────────────────────────────────

    private static void WriteAnalysis(StringBuilder sb,
        JitAsmCapture.JitCaptureResult jitResult,
        AotCoreIrMethodArtifact aotMethod)
    {
        sb.AppendLine("[ANALYSIS]  (optimization opportunity detection)");
        sb.AppendLine();

        var findings = new List<(string Severity, string Category, string Message, string? Suggestion)>();

        // 1. ExternalRuntime calls — potential inline candidates
        var externalCalls = aotMethod.Instructions
            .Where(i => i.DispatchKindCode == HybridDispatchKind.ExternalRuntime)
            .ToList();
        if (externalCalls.Count > 0)
        {
            foreach (var call in externalCalls)
            {
                var callee = call.Callee ?? call.TargetSymbol ?? "unknown";
                findings.Add(("INFO", "EXTERNAL_RUNTIME_CALL",
                    $"Call to '{callee}' uses ExternalRuntime dispatch (IL_{call.IlOffset:x4})",
                    "Consider implementing as Direct dispatch for better AOT performance"));
            }
        }

        // 2. Virtual dispatch calls — potential vtable optimization
        var virtualCalls = aotMethod.Instructions
            .Where(i => i.DispatchKindCode == HybridDispatchKind.Virtual)
            .ToList();
        if (virtualCalls.Count > 0)
        {
            var count = virtualCalls.Count;
            findings.Add(("INFO", "VIRTUAL_DISPATCH",
                $"{count} virtual/interface dispatch call(s) — cannot inline, resolved at runtime",
                "If callee is sealed or final, convert to Direct dispatch"));
        }

        // 3. Direct calls that could be inlined (small callees)
        var directCalls = aotMethod.Instructions
            .Where(i => i.DispatchKindCode == HybridDispatchKind.Direct && !string.IsNullOrEmpty(i.Callee))
            .ToList();
        if (directCalls.Count > 0)
        {
            foreach (var call in directCalls)
            {
                // Check if this is a call to our own subjects (potential inline)
                if (call.Callee?.Contains("/") == true)
                {
                    findings.Add(("INFO", "DIRECT_CALL_INLINE_CANDIDATE",
                        $"'{call.Callee}' is a direct call within the same assembly (IL_{call.IlOffset:x4})",
                        "Consider implementing method inlining for small callees"));
                }
            }
        }

        // 4. Runtime service calls (stsfld, etc) — potential overhead
        var runtimeSvcCalls = aotMethod.Instructions
            .Where(i => i.RuntimeServiceKind is not null)
            .ToList();
        if (runtimeSvcCalls.Count > 0)
        {
            var svcGroups = runtimeSvcCalls
                .GroupBy(i => i.RuntimeServiceKind!.Value)
                .Select(g => $"{g.Key}: {g.Count()}x");
            findings.Add(("INFO", "RUNTIME_SERVICE_CALLS",
                $"Runtime service calls: {string.Join(", ", svcGroups)}",
                "Each RuntimeService call bypasses AOT codegen — batch or inline where possible"));
        }

        // 5. Box/unbox operations — allocation overhead
        var boxOps = aotMethod.Instructions.Where(i => i.Op == "box").Count();
        var unboxOps = aotMethod.Instructions.Where(i => i.Op is "unbox" or "unbox.any").Count();
        if (boxOps > 0 || unboxOps > 0)
        {
            findings.Add(("WARN", "BOXING_OVERHEAD",
                $"{boxOps} box + {unboxOps} unbox operations — GC allocation pressure",
                "Eliminate boxing by using generic overloads or ref returns"));
        }

        // 6. Null checks pattern in SIDE-BY-SIDE (guard-null pattern)
        var nullGuardPatterns = DetectNullGuardPatterns(aotMethod);
        if (nullGuardPatterns > 0)
        {
            findings.Add(("INFO", "NULL_GUARD_OVERHEAD",
                $"{nullGuardPatterns} null guard pattern(s) detected — runtime overhead per call",
                "Consider static analysis to prove non-null at call sites"));
        }

        // 7. Large basic block count relative to IL size
        int ilCount = jitResult.IlBody?.Instructions?.Count ?? 0;
        int irCount = aotMethod.Instructions.Count;
        if (ilCount > 0 && irCount > ilCount * 2)
        {
            findings.Add(("WARN", "IR_EXPANSION",
                $"AotCoreIR has {irCount} instructions vs {ilCount} IL instructions ({irCount / (double)ilCount:F1}x expansion)",
                "Check for redundant IR instructions; optimize lowering passes"));
        }

        // 8. Branch-heavy methods
        int branchCount = aotMethod.Instructions.Count(i =>
            i.Op is "br" or "brtrue" or "brfalse" or "switch");
        if (branchCount > 10)
        {
            findings.Add(("INFO", "BRANCH_HEAVY",
                $"{branchCount} branches detected — complex control flow",
                "AOT codegen may produce many basic blocks; consider loop optimizations"));
        }

        // Report
        if (findings.Count == 0)
        {
            sb.AppendLine("  (no optimization opportunities detected)");
        }
        else
        {
            // Group by severity
            var grouped = findings
                .GroupBy(f => f.Severity)
                .OrderByDescending(g => g.Key == "WARN" ? 0 : g.Key == "INFO" ? 1 : 2);

            int count = 0;
            foreach (var group in grouped)
            {
                foreach (var finding in group)
                {
                    count++;
                    var severity = finding.Severity switch
                    {
                        "WARN" => "[!]",
                        "INFO" => "[i]",
                        _ => "[?]"
                    };
                    sb.AppendLine($"  #{count,-3} {severity,-5} [{finding.Category}]");
                    sb.AppendLine($"       {finding.Message}");
                    if (!string.IsNullOrEmpty(finding.Suggestion))
                        sb.AppendLine($"       -> {finding.Suggestion}");
                    sb.AppendLine();
                }
            }
        }

        sb.AppendLine();
    }

    private static int DetectNullGuardPatterns(AotCoreIrMethodArtifact aotMethod)
    {
        int count = 0;
        for (int i = 0; i < aotMethod.Instructions.Count - 1; i++)
        {
            var curr = aotMethod.Instructions[i];
            var next = aotMethod.Instructions[i + 1];

            // Pattern: ldarg + brfalse → null guard
            if (curr.Op == "ldarg" && next.Op == "brfalse")
                count++;

            // Pattern: ldloc (after ldfld) + brfalse → indirect null guard
            if (curr.Op == "ldfld" && next.Op == "brfalse")
                count++;
        }
        return count;
    }
}