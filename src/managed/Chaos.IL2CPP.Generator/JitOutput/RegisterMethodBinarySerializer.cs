// RegisterMethodBinarySerializer.cs — Standalone Binary IR serializer for single-method RegisterMethod
//
// Produces the v1 Binary IR format consumed by the JIT runtime deserializer
// (jit_binary_reader.h/cpp).  This format is used by JIT/Hybrid codegen modes
// to embed pre-allocated register IR alongside AOT C++ output.
//
// The serializer accepts:
//   1. AotCoreIr JSON + ir_precompile output → standalone .bir binary
//   2. Pre-serialized RegisterInstruction data (from native tool)
//
// Binary IR v1 format (all integers little-endian):
//   [BinaryIrHeader]          (32 bytes)
//   [RegisterInstruction[]]   (16 bytes each)
//   [RegisterSehClause[]]     (24 bytes each)
//   [BinaryCatchHandlerEntry] (12 bytes each)
//   [uint32_t[]]              (IL offsets)
//   [BinaryStackMapEntry]     (25 bytes each)

using System.Runtime.InteropServices;
using System.Text;

namespace Chaos.IL2CPP.Generator.JitOutput;

/// <summary>
/// Standalone Binary IR serializer for single-method RegisterMethod transport.
/// </summary>
public static class RegisterMethodBinarySerializer
{
    // ── Magic number ────────────────────────────────────────────────────
    // "BIR\0" = 0x00524942 (little-endian)
    private const uint BinaryIrMagic = 0x00524942u;
    private const uint BinaryIrVersion = 1u;

    // ── Header layout (32 bytes, matches BinaryIrHeader in jit_binary_reader.h) ──
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private struct BinaryIrHeader
    {
        public uint Magic;              // kBinaryIrMagic
        public uint Version;            // kBinaryIrVersion
        public uint MaxRegs;            // highest register used
        public uint InstrCount;         // number of RegisterInstruction
        public uint SehCount;           // number of SEH clauses
        public uint CatchHandlerCount;  // number of catch handler entries
        public uint IlOffsetCount;      // number of IL offsets
        public uint StackMapCount;      // number of stack map entries
        public uint TotalSize;          // total bytes of all data after header
        public uint Reserved0;          // must be 0
        public uint Reserved1;          // must be 0
    }

    // ── RegisterInstruction (16 bytes, matches register_ir.h) ────────────
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct RegisterInstruction
    {
        public ulong Header;      // packed fields
        public ulong ImmPayload;  // immediate value
    }

    // ── RegisterSehClause (24 bytes, matches register_ir.h) ──────────────
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct RegisterSehClause
    {
        public uint Flags;
        public uint TryStartIdx;
        public uint TryEndIdx;
        public uint HandlerStartIdx;
        public uint HandlerEndIdx;
        public uint ClassToken;
    }

    // ── BinaryCatchHandlerEntry (12 bytes) ───────────────────────────────
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct BinaryCatchHandlerEntry
    {
        public uint HandlerStartIdx;
        public byte ExceptionReg;
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 3)]
        public byte[] Pad;          // padding
        public uint ClassToken;
    }

    // ── BinaryStackMapEntry (25 bytes) ───────────────────────────────────
    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    public struct BinaryStackMapEntry
    {
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 16)]
        public sbyte[] SlotRegs;    // slot_regs[16]
        [MarshalAs(UnmanagedType.ByValArray, SizeConst = 8)]
        public sbyte[] LocalRegs;   // local_regs[8]
        public byte StackDepth;
    }

    // ── Serialization data input ─────────────────────────────────────────
    /// <summary>
    /// Input data for a single method's register IR.
    /// </summary>
    public sealed class MethodRegisterIr
    {
        public uint MaxRegs { get; set; }
        public RegisterInstruction[] Instructions { get; set; } = [];
        public RegisterSehClause[] SehClauses { get; set; } = [];
        public BinaryCatchHandlerEntry[] CatchHandlerEntries { get; set; } = [];
        public uint[] IlOffsets { get; set; } = [];
        public BinaryStackMapEntry[] StackMapEntries { get; set; } = [];
    }

    /// <summary>
    /// Serialize a single method's register IR to standalone Binary IR format.
    /// </summary>
    public static byte[] Serialize(MethodRegisterIr method)
    {
        if (method == null || method.Instructions.Length == 0)
            return [];

        int instrCount = method.Instructions.Length;
        int sehCount = method.SehClauses.Length;
        int catchCount = method.CatchHandlerEntries.Length;
        int ilCount = method.IlOffsets.Length;
        int smCount = method.StackMapEntries.Length;

        int instrBytes = instrCount * Marshal.SizeOf<RegisterInstruction>();
        int sehBytes = sehCount * Marshal.SizeOf<RegisterSehClause>();
        int catchBytes = catchCount * Marshal.SizeOf<BinaryCatchHandlerEntry>();
        int ilBytes = ilCount * sizeof(uint);
        int smBytes = smCount * Marshal.SizeOf<BinaryStackMapEntry>();
        uint totalDataSize = (uint)(instrBytes + sehBytes + catchBytes + ilBytes + smBytes);

        using var ms = new MemoryStream();
        using var bw = new BinaryWriter(ms);

        // Write header
        var hdr = new BinaryIrHeader
        {
            Magic = BinaryIrMagic,
            Version = BinaryIrVersion,
            MaxRegs = method.MaxRegs,
            InstrCount = (uint)instrCount,
            SehCount = (uint)sehCount,
            CatchHandlerCount = (uint)catchCount,
            IlOffsetCount = (uint)ilCount,
            StackMapCount = (uint)smCount,
            TotalSize = totalDataSize,
            Reserved0 = 0,
            Reserved1 = 0,
        };

        WriteStruct(bw, hdr);

        // Write RegisterInstruction array
        foreach (var instr in method.Instructions)
            WriteStruct(bw, instr);

        // Write RegisterSehClause array
        foreach (var sc in method.SehClauses)
            WriteStruct(bw, sc);

        // Write BinaryCatchHandlerEntry array
        foreach (var ce in method.CatchHandlerEntries)
            WriteStruct(bw, ce);

        // Write IL offsets
        foreach (var off in method.IlOffsets)
            bw.Write(off);

        // Write BinaryStackMapEntry array
        foreach (var sm in method.StackMapEntries)
            WriteStruct(bw, sm);

        return ms.ToArray();
    }

    /// <summary>
    /// Convert raw ir_precompile binary (multi-method format) to per-method
    /// standalone Binary IR format.
    /// </summary>
    /// <param name="irPrecompileData">Raw output from ir_precompile tool.</param>
    /// <param name="methodIndex">Index of the method to extract (0-based).</param>
    /// <returns>Standalone Binary IR bytes, or empty array on failure.</returns>
    public static byte[] ConvertFromPrecompile(byte[] irPrecompileData, uint methodIndex)
    {
        if (irPrecompileData == null || irPrecompileData.Length < 4)
            return [];

        uint methodCount = BitConverter.ToUInt32(irPrecompileData, 0);
        if (methodIndex >= methodCount)
            return [];

        // Parse ir_precompile format to find the method header
        int pos = 4;
        for (uint i = 0; i <= methodIndex && pos + 12 <= irPrecompileData.Length; i++)
        {
            if (pos + 12 > irPrecompileData.Length)
                break;

            ushort maxRegs = BitConverter.ToUInt16(irPrecompileData, pos + 4);
            ushort instrCount = BitConverter.ToUInt16(irPrecompileData, pos + 6);
            ushort sehCount = BitConverter.ToUInt16(irPrecompileData, pos + 8);
            // skip subject_id_hash(4) + padding(2) = 6 bytes

            uint instrBytes = (uint)(instrCount * 16);
            uint sehBytes = (uint)(sehCount * 24);
            uint totalBlock = 12 + instrBytes + sehBytes;

            if (pos + totalBlock > irPrecompileData.Length)
                break;

            if (i == methodIndex)
            {
                // Extract: header(12) + instrs(16*count) + seh(24*count)
                uint methodBlockSize = 12 + instrBytes + sehBytes;
                var methodBlock = new byte[methodBlockSize];
                Buffer.BlockCopy(irPrecompileData, pos, methodBlock, 0, (int)methodBlockSize);

                // Convert to standalone format
                return ConvertToStandalone(methodBlock);
            }

            pos += (int)totalBlock;
        }

        return [];
    }

    /// <summary>
    /// Convert a single-method raw block (ir_precompile format header) to standalone Binary IR.
    /// </summary>
    private static byte[] ConvertToStandalone(byte[] methodBlock)
    {
        if (methodBlock.Length < 12)
            return [];

        ushort maxRegs = BitConverter.ToUInt16(methodBlock, 4);
        ushort instrCount = BitConverter.ToUInt16(methodBlock, 6);
        ushort sehCount = BitConverter.ToUInt16(methodBlock, 8);

        uint instrBytes = (uint)(instrCount * 16);
        uint sehBytes = (uint)(sehCount * 24);

        // Build MethodRegisterIr from the raw block
        var method = new MethodRegisterIr
        {
            MaxRegs = maxRegs,
        };

        // Read instructions
        method.Instructions = new RegisterInstruction[instrCount];
        for (int i = 0; i < instrCount; i++)
        {
            int off = 12 + i * 16;
            method.Instructions[i] = new RegisterInstruction
            {
                Header = BitConverter.ToUInt64(methodBlock, off),
                ImmPayload = BitConverter.ToUInt64(methodBlock, off + 8),
            };
        }

        // Read SEH clauses
        method.SehClauses = new RegisterSehClause[sehCount];
        for (int i = 0; i < sehCount; i++)
        {
            int off = 12 + (int)instrBytes + i * 24;
            method.SehClauses[i] = new RegisterSehClause
            {
                Flags = BitConverter.ToUInt32(methodBlock, off),
                TryStartIdx = BitConverter.ToUInt32(methodBlock, off + 4),
                TryEndIdx = BitConverter.ToUInt32(methodBlock, off + 8),
                HandlerStartIdx = BitConverter.ToUInt32(methodBlock, off + 12),
                HandlerEndIdx = BitConverter.ToUInt32(methodBlock, off + 16),
                ClassToken = BitConverter.ToUInt32(methodBlock, off + 20),
            };
        }

        // No catch handler entries, IL offsets, or stack maps in ir_precompile format
        method.CatchHandlerEntries = [];
        method.IlOffsets = [];
        method.StackMapEntries = [];

        return Serialize(method);
    }

    private static void WriteStruct<T>(BinaryWriter bw, T value)
    {
        int size = Marshal.SizeOf<T>();
        IntPtr ptr = Marshal.AllocHGlobal(size);
        try
        {
            Marshal.StructureToPtr(value!, ptr, false);
            byte[] buffer = new byte[size];
            Marshal.Copy(ptr, buffer, 0, size);
            bw.Write(buffer);
        }
        finally
        {
            Marshal.FreeHGlobal(ptr);
        }
    }
}
