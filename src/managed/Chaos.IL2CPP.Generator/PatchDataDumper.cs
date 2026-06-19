using System.Runtime.InteropServices;
using System.Text;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Reads a .patchdata binary file and prints a human-readable dump for
/// debugging and audit purposes.
/// </summary>
public static class PatchDataDumper
{
    public static void Dump(string patchdataPath, TextWriter writer)
    {
        var fileSize = new FileInfo(patchdataPath).Length;
        var bytes = File.ReadAllBytes(patchdataPath);
        if (bytes.Length < Marshal.SizeOf<FileHeader>())
        {
            writer.WriteLine("Error: file too small for header");
            return;
        }

        var hdr = MemoryMarshal.Read<FileHeader>(bytes);
        if (hdr.magic != 0x50415854)
        {
            writer.WriteLine($"Error: bad magic 0x{hdr.magic:X8} (expected 0x50415854 \"PADT\")");
            return;
        }

        writer.WriteLine($"{"Magic:",-30} PADT (0x{hdr.magic:X8})");
        writer.WriteLine($"{"Version:",-30} {hdr.version}");
        writer.WriteLine($"{"Header size:",-30} {hdr.header_size} bytes");
        writer.WriteLine($"{"Total file size:",-30} {fileSize} bytes");
        writer.WriteLine();

        // Layout check
        if (hdr.version >= 1 && hdr.header_size < 112)
        {
            writer.WriteLine("Warning: header_size smaller than v1 minimum (112)");
        }
        if (hdr.version >= 2 && hdr.header_size < 124)
        {
            writer.WriteLine("Warning: v2 header_size smaller than 124");
        }
        if (bytes.Length < PatchData_TotalSize(in hdr))
        {
            writer.WriteLine("Warning: file truncated — declared total exceeds file size");
        }

        // Heaps
        writer.WriteLine("── Heaps ──");
        writer.WriteLine($"{"String heap:",-42} offset={hdr.string_heap_offset,8}  size={hdr.string_heap_size,8} bytes");
        writer.WriteLine($"{"Blob heap:",-42} offset={hdr.blob_heap_offset,8}  size={hdr.blob_heap_size,8} bytes");
        writer.WriteLine($"{"User-string heap:",-42} offset={hdr.user_string_heap_offset,8}  size={hdr.user_string_heap_size,8} bytes");
        writer.WriteLine();

        // Tables
        writer.WriteLine("── Metadata tables ──");
        DumpTable(writer, "AssemblyRef", hdr.assembly_ref_offset, hdr.assembly_ref_count, 8);
        DumpTable(writer, "TypeRef", hdr.type_ref_offset, hdr.type_ref_count, 16);
        DumpTable(writer, "TypeDef", hdr.type_def_offset, hdr.type_def_count, 22);
        DumpTable(writer, "FieldDef", hdr.field_def_offset, hdr.field_def_count, 18);
        DumpTable(writer, "MethodDef", hdr.method_def_offset, hdr.method_def_count, 28);
        DumpTable(writer, "MemberRef", hdr.member_ref_offset, hdr.member_ref_count, 16);
        DumpTable(writer, "StandaloneSig", hdr.standalone_sig_offset, hdr.standalone_sig_count, 8);

        // Method bodies
        int bodyCount = 0;
        if (hdr.method_def_count > 0)
        {
            var methodDefs = MemoryMarshal.Cast<byte, PatchMethodDefEntry>(
                bytes.AsSpan((int)hdr.method_def_offset, (int)hdr.method_def_count * 28));
            foreach (ref readonly var m in methodDefs)
                if (m.body_size > 0) bodyCount++;
        }
        writer.WriteLine();
        writer.WriteLine($"{"Method bodies:",-42} offset={hdr.body_data_offset,8}  size={hdr.body_data_size,8} bytes  ({bodyCount}/{hdr.method_def_count} methods have bodies)");

        writer.WriteLine();
        writer.WriteLine($"├── {"offset",8} {"count",6} {"content",-20}");
        writer.WriteLine($"{"BodyData",-9} {hdr.body_data_offset,8} {hdr.method_def_count,6} methods  ({bodyCount} non-empty bodies, {hdr.body_data_size} bytes total)");

        // AotCoreIr section
        writer.WriteLine();
        writer.WriteLine("── AotCoreIr section ──");
        if (hdr.aot_core_ir_count > 0 && hdr.aot_core_ir_size > 0)
        {
            writer.WriteLine($"{"Method count:",-30} {hdr.aot_core_ir_count}");
            writer.WriteLine($"{"Section size:",-30} {hdr.aot_core_ir_size} bytes");
            writer.WriteLine($"{"Offset:",-30} {hdr.aot_core_ir_offset}");

            // Read the JSON index and print method names
            DumpAotCoreIrMethods(bytes, in hdr, writer);
        }
        else
        {
            writer.WriteLine("(empty — no IR embedded)");
        }

        // RegIr section
        writer.WriteLine();
        writer.WriteLine("── Register IR section (v2+) ──");
        if (hdr.reg_ir_count > 0 && hdr.reg_ir_size > 0)
        {
            writer.WriteLine($"{"Method count:",-30} {hdr.reg_ir_count}");
            writer.WriteLine($"{"Section size:",-30} {hdr.reg_ir_size} bytes");
            writer.WriteLine($"{"Offset:",-30} {hdr.reg_ir_offset}");
        }
        else
        {
            writer.WriteLine("(empty — runtime will fall back to AotCoreIr JSON)");
        }

        // String heap sample
        writer.WriteLine();
        writer.WriteLine("── String heap sample (first 20) ──");
        DumpStringHeap(bytes, in hdr, writer);

        // TypeDef sample
        writer.WriteLine();
        writer.WriteLine("── TypeDef entries ──");
        DumpTypeDefs(bytes, in hdr, writer);

        // MethodDef sample
        writer.WriteLine();
        writer.WriteLine("── MethodDef entries ──");
        DumpMethodDefs(bytes, in hdr, writer);
    }

    private static void DumpTable(TextWriter w, string name, uint offset, uint count, int entrySize)
    {
        if (count == 0)
            w.WriteLine($"  {name,-15} (empty)");
        else
            w.WriteLine($"  {name,-15} offset={offset,8}  count={count,6}  entry={entrySize}B  total={count * (uint)entrySize,8}B");
    }

    private static void DumpAotCoreIrMethods(byte[] bytes, ref readonly FileHeader hdr, TextWriter w)
    {
        var idxStart = (int)hdr.aot_core_ir_offset;
        var jsonStart = idxStart + (int)hdr.aot_core_ir_count * 4;
        var maxCount = Math.Min((int)hdr.aot_core_ir_count, 50); // cap output

        for (int i = 0; i < maxCount; i++)
        {
            if (idxStart + (i + 1) * 4 > bytes.Length) break;
            var strOff = BitConverter.ToUInt32(bytes, idxStart + i * 4);
            var strPos = jsonStart + (int)strOff;
            if (strPos >= bytes.Length) continue;

            // Read null-terminated JSON
            var end = strPos;
            while (end < bytes.Length && bytes[end] != 0) end++;
            if (end == strPos) continue;

            var snippet = Encoding.UTF8.GetString(bytes, strPos, Math.Min(end - strPos, 200));
            // Extract method name from JSON snippet
            var name = ExtractMethodName(snippet);
            w.WriteLine($"  [{i,2}] \"{name}\"  (JSON: {end - strPos} bytes)");
        }

        if (hdr.aot_core_ir_count > 50)
            w.WriteLine($"  ... and {hdr.aot_core_ir_count - 50} more");
    }

    private static string ExtractMethodName(string jsonSnippet)
    {
        // Try to extract subjectId from the JSON
        var search = "\"subjectId\":\"";
        var idx = jsonSnippet.IndexOf(search, StringComparison.Ordinal);
        if (idx >= 0)
        {
            idx += search.Length;
            var close = jsonSnippet.IndexOf('"', idx);
            if (close > idx)
                return jsonSnippet[idx..close];
        }
        // Fallback: truncate the raw snippet
        return jsonSnippet.Length > 80 ? jsonSnippet[..80] + "..." : jsonSnippet;
    }

    private static void DumpStringHeap(byte[] bytes, ref readonly FileHeader hdr, TextWriter w)
    {
        if (hdr.string_heap_size == 0) { w.WriteLine("  (empty)"); return; }
        var heapStart = (int)hdr.string_heap_offset;
        var heapEnd = heapStart + (int)hdr.string_heap_size;
        var pos = heapStart;
        var count = 0;

        while (pos < heapEnd && count < 20)
        {
            var end = pos;
            while (end < heapEnd && bytes[end] != 0) end++;
            if (end == pos) { pos++; continue; }
            var s = Encoding.UTF8.GetString(bytes, pos, end - pos);
            if (s.Length > 64) s = s[..61] + "...";
            w.WriteLine($"  [{count}] \"{s}\"");
            count++;
            pos = end + 1;
        }
        if (pos < heapEnd)
            w.WriteLine($"  ... (more strings beyond first 20)");
    }

    private static void DumpTypeDefs(byte[] bytes, ref readonly FileHeader hdr, TextWriter w)
    {
        if (hdr.type_def_count == 0) { w.WriteLine("  (none)"); return; }

        var defs = MemoryMarshal.Cast<byte, PatchTypeDefEntry>(
            bytes.AsSpan((int)hdr.type_def_offset, (int)hdr.type_def_count * 22));

        var max = Math.Min(defs.Length, 15);
        for (int i = 0; i < max; i++)
        {
            ref readonly var d = ref defs[i];
            var name = ReadString(bytes, hdr.string_heap_offset, d.type_name_offset);
            var ns = ReadString(bytes, hdr.string_heap_offset, d.namespace_offset);
            var fullName = string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
            w.WriteLine($"  [{i,2}] token=0x{d.token:X8}  {fullName}");
        }
        if (defs.Length > 15)
            w.WriteLine($"  ... and {defs.Length - 15} more");
    }

    private static void DumpMethodDefs(byte[] bytes, ref readonly FileHeader hdr, TextWriter w)
    {
        if (hdr.method_def_count == 0) { w.WriteLine("  (none)"); return; }

        var defs = MemoryMarshal.Cast<byte, PatchMethodDefEntry>(
            bytes.AsSpan((int)hdr.method_def_offset, (int)hdr.method_def_count * 28));

        var max = Math.Min(defs.Length, 20);
        for (int i = 0; i < max; i++)
        {
            ref readonly var d = ref defs[i];
            var name = ReadString(bytes, hdr.string_heap_offset, d.name_offset);
            var hasBody = d.body_size > 0 ? "body" : "no-body";
            w.WriteLine($"  [{i,2}] token=0x{d.token:X8}  \"{name}\"  body={d.body_size,5}B  ({hasBody})");
        }
        if (defs.Length > 20)
            w.WriteLine($"  ... and {defs.Length - 20} more");
    }

    private static string ReadString(byte[] bytes, uint heapOffset, uint strOffset)
    {
        if (strOffset == 0) return "";
        var pos = (int)(heapOffset + strOffset);
        if (pos >= bytes.Length) return "";
        var end = pos;
        while (end < bytes.Length && bytes[end] != 0) end++;
        return Encoding.UTF8.GetString(bytes, pos, end - pos);
    }

    private static uint PatchData_TotalSize(ref readonly FileHeader hdr)
    {
        uint sectionEnd = hdr.body_data_offset + hdr.body_data_size;
        uint irEnd = hdr.aot_core_ir_offset + hdr.aot_core_ir_size;
        uint regIrEnd = hdr.reg_ir_offset + hdr.reg_ir_size;
        uint maxEnd = irEnd > sectionEnd ? irEnd : sectionEnd;
        return regIrEnd > maxEnd ? regIrEnd : maxEnd;
    }

    // ── Matching layout in patch_data.h ──────────────────────────────

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private readonly struct FileHeader
    {
        public readonly uint magic;
        public readonly uint version;
        public readonly uint header_size;
        public readonly uint string_heap_offset;
        public readonly uint string_heap_size;
        public readonly uint blob_heap_offset;
        public readonly uint blob_heap_size;
        public readonly uint user_string_heap_offset;
        public readonly uint user_string_heap_size;
        public readonly uint assembly_ref_offset;
        public readonly uint assembly_ref_count;
        public readonly uint type_ref_offset;
        public readonly uint type_ref_count;
        public readonly uint type_def_offset;
        public readonly uint type_def_count;
        public readonly uint field_def_offset;
        public readonly uint field_def_count;
        public readonly uint method_def_offset;
        public readonly uint method_def_count;
        public readonly uint member_ref_offset;
        public readonly uint member_ref_count;
        public readonly uint standalone_sig_offset;
        public readonly uint standalone_sig_count;
        public readonly uint body_data_offset;
        public readonly uint body_data_size;
        public readonly uint aot_core_ir_offset;
        public readonly uint aot_core_ir_size;
        public readonly uint aot_core_ir_count;
        public readonly uint reg_ir_offset;
        public readonly uint reg_ir_size;
        public readonly uint reg_ir_count;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private readonly struct PatchMethodDefEntry
    {
        public readonly uint name_offset;
        public readonly uint signature_offset;
        public readonly uint body_offset;
        public readonly uint body_size;
        public readonly uint declaring_type_token;
        public readonly uint token;
        public readonly ushort impl_flags;
        public readonly ushort flags;
    }

    [StructLayout(LayoutKind.Sequential, Pack = 1)]
    private readonly struct PatchTypeDefEntry
    {
        public readonly uint type_name_offset;
        public readonly uint namespace_offset;
        public readonly uint enclosing_type_token;
        public readonly uint extends_token;
        public readonly uint token;
        public readonly ushort flags;
    }
}
