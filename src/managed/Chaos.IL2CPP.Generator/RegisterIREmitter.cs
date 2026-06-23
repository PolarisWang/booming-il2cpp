// RegisterIREmitter.cs — Managed bridge to pre-compile register IR into .patchdata reg_ir section
//
// The register IR section in .patchdata (v2+) carries pre-allocated
// RegisterInstruction[] data so the runtime can skip JSON deserialization
// and register allocation at load time.
//
// This emitter works in two modes:
//   1. Native tool mode: invokes the native `ir_precompile` tool to perform the
//      actual register allocation (C++ AllocateRegisters algorithm) and converts
//      the output to the .patchdata reg_ir section format.
//   2. Fallback mode: returns empty — the runtime falls back to the v1 JSON path.
//
// The .patchdata reg_ir format (from patch_data.h):
//   [offsets: uint32_t[count]]  — byte offset of each method's block from section start
//   [method blocks, one per method in MethodDef order:]
//       uint32_t max_regs
//       uint32_t instr_count
//       uint32_t seh_count
//       RegisterInstruction[instr_count]   (16 bytes each)
//       SEHClauseCompact[seh_count]        (24 bytes each: 6 x uint32_t)

using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public static class RegisterIREmitter
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
        WriteIndented = false,
    };

    /// <summary>
    /// Produce the reg_ir section binary from AotCoreIr artifacts.
    /// When <paramref name="irPrecompilePath"/> is null or the tool is unavailable,
    /// returns empty (runtime falls back to JSON-based register allocation).
    /// </summary>
    /// <param name="aotCoreIrJson">Path to the serialized AotCoreIr JSON file.</param>
    /// <param name="methodCount">Number of methods in the MethodDef table (for format header).</param>
    /// <param name="irPrecompilePath">Optional path to the native ir_precompile executable.</param>
    /// <returns>Binary reg_ir section data, or empty array if unavailable.</returns>
    public static byte[] EmitRegIrSection(string aotCoreIrJson, uint methodCount, string? irPrecompilePath = null)
    {
        if (string.IsNullOrEmpty(irPrecompilePath) || !File.Exists(irPrecompilePath))
            return [];

        if (!File.Exists(aotCoreIrJson))
            return [];

        // ── Step 1: Run native ir_precompile ─────────────────────────────
        string tempOutput = Path.GetTempFileName();
        try
        {
            var psi = new System.Diagnostics.ProcessStartInfo
            {
                FileName = irPrecompilePath,
                ArgumentList = { "--input", aotCoreIrJson, "--output", tempOutput },
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                UseShellExecute = false,
            };

            using var process = System.Diagnostics.Process.Start(psi);
            if (process == null)
                return [];

            string stderr = process.StandardError.ReadToEnd();
            process.WaitForExit();

            if (process.ExitCode != 0 || !File.Exists(tempOutput))
                return [];

            byte[] regirData = File.ReadAllBytes(tempOutput);

            // ── Step 2: Convert from ir_precompile format to .patchdata format ──
            return ConvertToPatchDataFormat(regirData, methodCount);
        }
        catch
        {
            return [];
        }
        finally
        {
            if (File.Exists(tempOutput))
                File.Delete(tempOutput);
        }
    }

    /// <summary>
    /// Produce the reg_ir section binary from the in-memory AotCoreIrArtifact.
    /// </summary>
    public static byte[] EmitRegIrSectionFromArtifact(
        AotCoreIrArtifact aotCoreIr,
        uint methodCount,
        string? irPrecompilePath = null)
    {
        if (string.IsNullOrEmpty(irPrecompilePath) || !File.Exists(irPrecompilePath))
            return [];

        // Serialize to temp JSON for the native tool
        string tempJson = Path.GetTempFileName();
        try
        {
            string json = JsonSerializer.Serialize(aotCoreIr, JsonOptions);
            File.WriteAllText(tempJson, json, Encoding.UTF8);
            return EmitRegIrSection(tempJson, methodCount, irPrecompilePath);
        }
        catch
        {
            return [];
        }
        finally
        {
            if (File.Exists(tempJson))
                File.Delete(tempJson);
        }
    }

    /// <summary>
    /// Convert ir_precompile binary format to .patchdata reg_ir section format.
    ///
    /// ir_precompile format:
    ///   uint32_t method_count
    ///   [for each method:]
    ///       RegirMethodHeader { subject_id_hash(4), max_regs(2), instr_count(2), seh_count(2), padding(2) } = 12 bytes
    ///       RegisterInstruction[instr_count]  (16 bytes each)
    ///       SEHClauseCompact[seh_count]       (24 bytes each)
    ///
    /// .patchdata reg_ir format:
    ///   uint32_t offsets[count]  — method index -> byte offset from section start
    ///   [for each method:]
    ///       uint32_t max_regs
    ///       uint32_t instr_count
    ///       uint32_t seh_count
    ///       RegisterInstruction[instr_count]  (16 bytes each)
    ///       SEHClauseCompact[seh_count]       (24 bytes each)
    /// </summary>
    private static byte[] ConvertToPatchDataFormat(byte[] regirData, uint expectedMethodCount)
    {
        if (regirData.Length < 4)
            return [];

        uint methodCount = BitConverter.ToUInt32(regirData, 0);
        if (methodCount == 0 || methodCount > 65536)
            return [];

        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);

        // ── Write offsets array (reserve space) ──────────────────────────
        // Each offset is uint32_t — relative to the start of the section
        long offsetsOffset = ms.Position;
        long offsetsEnd = offsetsOffset + methodCount * 4;
        ms.Seek((int)offsetsEnd, SeekOrigin.Begin);

        // ── Per-method blocks ──────────────────────────────────────────
        int pos = 4; // skip method_count
        uint[] offsets = new uint[methodCount];

        for (uint i = 0; i < methodCount; i++)
        {
            if (pos + 12 > regirData.Length)
                break;

            // Read ir_precompile header (skip subject_id_hash[4], use max_regs[2], instr_count[2], seh_count[2], padding[2])
            // RegirMethodHeader layout: subject_id_hash(4) + max_regs(2) + instr_count(2) + seh_count(2) + padding(2) = 12
            ushort maxRegs    = BitConverter.ToUInt16(regirData, pos + 4);
            ushort instrCount = BitConverter.ToUInt16(regirData, pos + 6);
            ushort sehCount   = BitConverter.ToUInt16(regirData, pos + 8);
            // padding at pos+10..pos+11 (2 bytes, ignored)

            uint instrBytes = (uint)(instrCount * 16);
            uint sehBytes   = (uint)(sehCount * 24);
            uint totalBlock  = 12 + instrBytes + sehBytes;

            if (pos + totalBlock > regirData.Length)
                break;

            // Record offset (relative to section start)
            offsets[i] = (uint)ms.Position;

            // Write .patchdata method block header
            bw.Write((uint)maxRegs);
            bw.Write((uint)instrCount);
            bw.Write((uint)sehCount);

            // Write RegisterInstruction array (16 bytes each)
            bw.Write(regirData, pos + 12, (int)instrBytes);

            // Write SEHClauseCompact array (24 bytes each)
            bw.Write(regirData, pos + 12 + (int)instrBytes, (int)sehBytes);

            pos += (int)totalBlock;
        }

        // ── Write offsets array at the beginning ─────────────────────────
        byte[] sectionData = ms.ToArray();
        for (uint i = 0; i < methodCount && i < offsets.Length; i++)
        {
            BitConverter.GetBytes(offsets[i]).CopyTo(sectionData, offsetsOffset + i * 4);
        }

        return sectionData;
    }
}
