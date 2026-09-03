using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

/// <summary>
/// Build-time tool: reads a patch assembly's PE metadata and produces a
/// self-contained .patchdata binary that the runtime PatchLoader can apply.
/// </summary>
public sealed partial class PatchDataExtractor
{
    private const uint Magic = 0x50415854; // "PADT"

    // Track string insertion order for correct offset computation.
    private readonly List<(string Key, uint Offset)> _insertOrder = new();

    public void Extract(string dllPath, string outputPath, string? aotCoreIrPath = null,
        string? genuineIrPath = null, string? direction = null,
        bool subjectOnly = false, string? subjectIndices = null,
        CodegenMode mode = CodegenMode.Aot)
    {
        using var stream = File.OpenRead(dllPath);
        using var peReader = new PEReader(stream);
        var mr = peReader.GetMetadataReader();

        // Pass 1: Collect all strings and blobs
        var strings = new Dictionary<string, uint>(StringComparer.Ordinal);
        var blobHandles = new List<BlobHandle>();

        // ── String offset computation ──────────────────────────────
        // We use _insertOrder to track insertion order so that each new
        // string is placed immediately after the most recently added one.
        // Dictionary<>.Last() is NOT reliable for this — Dictionary does
        // not guarantee ordering.
        _insertOrder.Clear();
        uint AllocString(string s)
        {
            if (string.IsNullOrEmpty(s)) return 0;
            if (strings.TryGetValue(s, out var o)) return o;
            uint baseOff = 1;
            if (_insertOrder.Count > 0)
            {
                var (lastKey, lastOff) = _insertOrder[^1];
                baseOff = lastOff + (uint)Encoding.UTF8.GetByteCount(lastKey) + 1;
            }
            o = baseOff;
            strings[s] = o;
            _insertOrder.Add((s, o));
            return o;
        }

        uint AllocBlob(BlobHandle h)
        {
            if (h.IsNil) return 0;
            var idx = blobHandles.IndexOf(h);
            if (idx >= 0) return (uint)idx;
            blobHandles.Add(h);
            return (uint)(blobHandles.Count - 1);
        }

        // Scan all tables to register strings and blobs
        foreach (var h in mr.AssemblyReferences)
            AllocString(mr.GetString(mr.GetAssemblyReference(h).Name));
        foreach (var h in mr.TypeReferences)
        {
            var r = mr.GetTypeReference(h);
            AllocString(mr.GetString(r.Name));
            AllocString(mr.GetString(r.Namespace));
        }
        foreach (var h in mr.TypeDefinitions)
        {
            var td = mr.GetTypeDefinition(h);
            var name = mr.GetString(td.Name);
            if (name == "<Module>") continue;
            AllocString(name);
            AllocString(mr.GetString(td.Namespace));
        }
        foreach (var h in mr.FieldDefinitions)
        {
            AllocString(mr.GetString(mr.GetFieldDefinition(h).Name));
            AllocBlob(mr.GetFieldDefinition(h).Signature);
        }
        foreach (var h in mr.MethodDefinitions)
        {
            AllocString(mr.GetString(mr.GetMethodDefinition(h).Name));
            AllocBlob(mr.GetMethodDefinition(h).Signature);
        }
        foreach (var h in mr.MemberReferences)
        {
            AllocString(mr.GetString(mr.GetMemberReference(h).Name));
            AllocBlob(mr.GetMemberReference(h).Signature);
        }
        AllocString(mr.GetString(mr.GetModuleDefinition().Name));

        // Recalculate correct string offsets
        var sortedStrings = strings.OrderBy(kv => kv.Value).ToList();
        var stringOffsets = new Dictionary<string, uint>(StringComparer.Ordinal);
        uint off = 1;
        foreach (var (s, _) in sortedStrings)
        {
            stringOffsets[s] = off;
            off += (uint)Encoding.UTF8.GetByteCount(s) + 1;
        }
        uint StrOff(string s) => stringOffsets.GetValueOrDefault(s);

        // Build table entries with final offsets
        var asmRefs = BuildAssemblyRefs(mr, StrOff);
        var typeRefs = BuildTypeRefs(mr, StrOff);
        var typeDefs = BuildTypeDefs(mr, StrOff);
        var fieldDefs = BuildFieldDefs(mr, StrOff, AllocBlob);
        var methodDefs = BuildMethodDefs(mr, StrOff, AllocBlob);
        var memberRefs = BuildMemberRefs(mr, StrOff, AllocBlob);
        var userStringBytes = BuildUserStringHeap(mr);

        // ── Subject-only filtering ──
        if (subjectOnly && !string.IsNullOrEmpty(subjectIndices))
        {
            var allowed = subjectIndices
                .Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries)
                .Select(int.Parse)
                .ToHashSet();
            methodDefs = methodDefs
                .Where(entry =>
                {
                    var mh = MetadataTokens.MethodDefinitionHandle((int)entry.token);
                    if (mh.IsNil) return false;
                    var md = mr.GetMethodDefinition(mh);
                    var name = mr.GetString(md.Name);
                    var idx = ExtractSubjectIndex(name);
                    return idx.HasValue && allowed.Contains(idx.Value);
                })
                .ToList();
        }

        // ── Optional AotCoreIr JSON section ──
        byte[]? aotCoreIrSection = null;
        uint aotCoreIrCount = 0;
        if (aotCoreIrPath != null && File.Exists(aotCoreIrPath))
        {
            (aotCoreIrSection, aotCoreIrCount) = BuildAotCoreIrSection(aotCoreIrPath, methodDefs, mr, mode);
        }

        // Build binary heaps
        var stringHeap = BuildStringHeap(stringOffsets);
        var blobHeap = BuildBlobHeap(blobHandles, mr);

        // Read method bodies
        var bodyData = ReadBodyData(peReader, mr, methodDefs);

        // ── IL Rewriting: Replace Subject_N method bodies with sentinel return ──
        // The patch DLL's Subject_N wrappers are AOT-replaced by ApplyPatchFromMemoryEx
        // during hotupdate.  To make semantic change detection deterministic, rewrite
        // their IL to just "ldc.i4 <sentinel>; ret" — the patched runtime will return
        // this sentinel, which differs from the baseline's real return value.
        // Sentinel = 0xBEEF0000 | subjectIndex (guaranteed != any real encoding).
        RewriteSubjectBodies(mr, methodDefs, bodyData);

        // Serialize
        Serialize(outputPath, stringHeap, blobHeap, userStringBytes,
            [.. asmRefs], [.. typeRefs], [.. typeDefs],
            [.. fieldDefs], [.. methodDefs], [.. memberRefs], bodyData,
            aotCoreIrSection, aotCoreIrCount);

        // Write sidecar metadata (direction, etc.)
        var metaPath = outputPath + ".meta.json";
        var meta = new Dictionary<string, object?>
        {
            ["direction"] = direction ?? "forward",
            ["sourceDll"] = Path.GetFileName(dllPath),
            ["patchdataSize"] = new FileInfo(outputPath).Length,
        };
        File.WriteAllText(metaPath, JsonSerializer.Serialize(meta));
        Console.WriteLine($"      [patchdata] meta: direction={direction ?? "forward"}");
    }

    private static List<PatchAssemblyRefEntry> BuildAssemblyRefs(
        MetadataReader mr, Func<string, uint> strOff)
    {
        var list = new List<PatchAssemblyRefEntry>();
        foreach (var h in mr.AssemblyReferences)
        {
            var r = mr.GetAssemblyReference(h);
            list.Add(new PatchAssemblyRefEntry
            {
                name_offset = strOff(mr.GetString(r.Name)),
                token = (uint)MetadataTokens.GetToken(h),
            });
        }
        return list;
    }

    private static List<PatchTypeRefEntry> BuildTypeRefs(
        MetadataReader mr, Func<string, uint> strOff)
    {
        var list = new List<PatchTypeRefEntry>();
        foreach (var h in mr.TypeReferences)
        {
            var r = mr.GetTypeReference(h);
            list.Add(new PatchTypeRefEntry
            {
                type_name_offset = strOff(mr.GetString(r.Name)),
                namespace_offset = strOff(mr.GetString(r.Namespace)),
                resolution_scope_token = (uint)MetadataTokens.GetToken(r.ResolutionScope),
                token = (uint)MetadataTokens.GetToken(h),
            });
        }
        return list;
    }

    private static List<PatchTypeDefEntry> BuildTypeDefs(
        MetadataReader mr, Func<string, uint> strOff)
    {
        var list = new List<PatchTypeDefEntry>();
        foreach (var h in mr.TypeDefinitions)
        {
            var td = mr.GetTypeDefinition(h);
            var name = mr.GetString(td.Name);
            if (name == "<Module>") continue;
            list.Add(new PatchTypeDefEntry
            {
                type_name_offset = strOff(name),
                namespace_offset = strOff(mr.GetString(td.Namespace)),
                enclosing_type_token = (uint)MetadataTokens.GetToken(td.GetDeclaringType()),
                extends_token = (uint)MetadataTokens.GetToken(td.BaseType),
                token = (uint)MetadataTokens.GetToken(h),
                flags = (ushort)td.Attributes,
            });
        }
        return list;
    }

    private static List<PatchFieldDefEntry> BuildFieldDefs(
        MetadataReader mr, Func<string, uint> strOff,
        Func<BlobHandle, uint> blobOff)
    {
        var owners = BuildFieldOwnerMap(mr);
        var list = new List<PatchFieldDefEntry>();
        foreach (var h in mr.FieldDefinitions)
        {
            var fd = mr.GetFieldDefinition(h);
            list.Add(new PatchFieldDefEntry
            {
                name_offset = strOff(mr.GetString(fd.Name)),
                signature_offset = blobOff(fd.Signature),
                declaring_type_token = owners.TryGetValue(h, out var th)
                    ? (uint)MetadataTokens.GetToken(th) : 0u,
                token = (uint)MetadataTokens.GetToken(h),
                flags = (ushort)fd.Attributes,
            });
        }
        return list;
    }

    private static List<PatchMethodDefEntry> BuildMethodDefs(
        MetadataReader mr, Func<string, uint> strOff,
        Func<BlobHandle, uint> blobOff)
    {
        var owners = BuildMethodOwnerMap(mr);
        var list = new List<PatchMethodDefEntry>();
        foreach (var h in mr.MethodDefinitions)
        {
            var md = mr.GetMethodDefinition(h);
            list.Add(new PatchMethodDefEntry
            {
                name_offset = strOff(mr.GetString(md.Name)),
                signature_offset = blobOff(md.Signature),
                body_offset = 0,
                body_size = 0,
                declaring_type_token = owners.TryGetValue(h, out var th)
                    ? (uint)MetadataTokens.GetToken(th) : 0u,
                token = (uint)MetadataTokens.GetToken(h),
                impl_flags = 0,
                flags = (ushort)md.Attributes,
            });
        }
        return list;
    }

    private static List<PatchMemberRefEntry> BuildMemberRefs(
        MetadataReader mr, Func<string, uint> strOff,
        Func<BlobHandle, uint> blobOff)
    {
        var list = new List<PatchMemberRefEntry>();
        foreach (var h in mr.MemberReferences)
        {
            var mr2 = mr.GetMemberReference(h);
            list.Add(new PatchMemberRefEntry
            {
                name_offset = strOff(mr.GetString(mr2.Name)),
                signature_offset = blobOff(mr2.Signature),
                parent_token = (uint)MetadataTokens.GetToken(mr2.Parent),
                token = (uint)MetadataTokens.GetToken(h),
            });
        }
        return list;
    }

    private static Dictionary<FieldDefinitionHandle, TypeDefinitionHandle> BuildFieldOwnerMap(MetadataReader mr)
    {
        var m = new Dictionary<FieldDefinitionHandle, TypeDefinitionHandle>();
        foreach (var th in mr.TypeDefinitions)
            foreach (var fh in mr.GetTypeDefinition(th).GetFields())
                m[fh] = th;
        return m;
    }

    private static Dictionary<MethodDefinitionHandle, TypeDefinitionHandle> BuildMethodOwnerMap(MetadataReader mr)
    {
        var m = new Dictionary<MethodDefinitionHandle, TypeDefinitionHandle>();
        foreach (var th in mr.TypeDefinitions)
            foreach (var mh in mr.GetTypeDefinition(th).GetMethods())
                m[mh] = th;
        return m;
    }

    private static byte[] ReadBodyData(PEReader peReader, MetadataReader mr,
        List<PatchMethodDefEntry> methodDefs)
    {
        using var ms = new MemoryStream();
        for (var i = 0; i < methodDefs.Count; i++)
        {
            var entry = methodDefs[i];
            if (entry.token == 0) continue;
            var md = mr.GetMethodDefinition(
                MetadataTokens.MethodDefinitionHandle((int)entry.token));
            var rva = md.RelativeVirtualAddress;
            if (rva == 0) continue;
            var body = peReader.GetMethodBody(rva);
            var ilBytes = body?.GetILBytes();
            if (ilBytes is null || ilBytes.Length == 0) continue;

            entry.body_offset = (uint)ms.Position;

            // Reconstruct the method body header (ECMA 335 II.25.4).
            // body.GetILBytes() returns ONLY the IL code bytes, stripping the header.
            // ParseMethodBodyHeader on the C++ side expects the full body with header.
            //
            // Tiny format (code_size < 64, max_stack <= 8):
            //   1 byte: (code_size << 2) | 0x02
            // Fat format:
            //   12 bytes: flags(2) + max_stack(2) + code_size(4) + local_sig_tok(4)
            if (ilBytes.Length < 64 && body!.MaxStack <= 8)
            {
                var hdr = (byte)((ilBytes.Length << 2) | 0x02);
                ms.WriteByte(hdr);
                ms.Write(ilBytes, 0, ilBytes.Length);
                entry.body_size = (uint)ilBytes.Length + 1;
            }
            else
            {
                var flags = (ushort)0x0003;  // CorILMethod_FatFormat
                var maxStack = (ushort)body!.MaxStack;
                var codeSize = (uint)ilBytes.Length;
                var localSig = (uint)MetadataTokens.GetToken(body.LocalSignature);

                ms.Write(BitConverter.GetBytes(flags));
                ms.Write(BitConverter.GetBytes(maxStack));
                ms.Write(BitConverter.GetBytes(codeSize));
                ms.Write(BitConverter.GetBytes(localSig));
                ms.Write(ilBytes, 0, ilBytes.Length);
                entry.body_size = (uint)ilBytes.Length + 12;
            }

            methodDefs[i] = entry;
        }
        return ms.ToArray();
    }

    private static byte[] BuildStringHeap(Dictionary<string, uint> offsets)
    {
        if (offsets.Count == 0) return [0];
        var sorted = offsets.OrderBy(kv => kv.Value).ToArray();
        var last = sorted[^1];
        var heap = new byte[last.Value + (uint)Encoding.UTF8.GetByteCount(last.Key) + 1];
        foreach (var (s, o) in sorted)
        {
            var bytes = Encoding.UTF8.GetBytes(s);
            Array.Copy(bytes, 0, heap, (int)o, bytes.Length);
            heap[o + bytes.Length] = 0;
        }
        return heap;
    }

    private static byte[] BuildBlobHeap(List<BlobHandle> handles, MetadataReader mr)
    {
        if (handles.Count == 0) return [];
        using var ms = new MemoryStream();
        foreach (var h in handles)
        {
            var bytes = mr.GetBlobBytes(h);
            if (bytes is null) continue;
            WriteCompressedUInt(ms, (uint)bytes.Length);
            ms.Write(bytes, 0, bytes.Length);
        }
        return ms.ToArray();
    }

    private static byte[] BuildUserStringHeap(MetadataReader mr)
    {
        var usHeapSize = mr.GetHeapSize(HeapIndex.UserString);
        if (usHeapSize == 0) return [];
        using var ms = new MemoryStream();
        var offset = 0;
        try
        {
            while (offset < usHeapSize)
            {
                var handle = MetadataTokens.UserStringHandle(offset);
                var str = mr.GetUserString(handle);
                if (str is null) break;
                var utf16Bytes = Encoding.Unicode.GetBytes(str);
                if (utf16Bytes.Length == 0) { offset += 2; continue; }
                var entrySize = ComputeUserStringEntrySize((uint)utf16Bytes.Length);
                if (offset + entrySize > usHeapSize) break;
                WriteCompressedUInt(ms, (uint)utf16Bytes.Length);
                ms.Write(utf16Bytes, 0, utf16Bytes.Length);
                offset += entrySize;
            }
        }
        catch { }
        return ms.ToArray();
    }

    private static int ComputeUserStringEntrySize(uint utf16ByteLen)
    {
        var cs = (int)CompressedUIntSize(utf16ByteLen);
        var total = cs + (int)utf16ByteLen;
        if (total % 2 != 0) total++;
        return total;
    }

    private static uint CompressedUIntSize(uint value)
    {
        if (value <= 0x7F) return 1;
        if (value <= 0x3FFF) return 2;
        return 4;
    }

    private static void WriteCompressedUInt(Stream s, uint value)
    {
        if (value <= 0x7F) { s.WriteByte((byte)value); }
        else if (value <= 0x3FFF)
        {
            s.WriteByte((byte)(0x80 | (value >> 8)));
            s.WriteByte((byte)(value & 0xFF));
        }
        else
        {
            s.WriteByte((byte)(0xC0 | (value >> 24)));
            s.WriteByte((byte)((value >> 16) & 0xFF));
            s.WriteByte((byte)((value >> 8) & 0xFF));
            s.WriteByte((byte)(value & 0xFF));
        }
    }

    private void Serialize(string outputPath,
        byte[] stringHeap, byte[] blobHeap, byte[] userStrings,
        PatchAssemblyRefEntry[] asmRefs,
        PatchTypeRefEntry[] typeRefs,
        PatchTypeDefEntry[] typeDefs,
        PatchFieldDefEntry[] fieldDefs,
        PatchMethodDefEntry[] methodDefs,
        PatchMemberRefEntry[] memberRefs,
        byte[] bodyData,
        byte[]? aotCoreIrSection = null,
        uint aotCoreIrCount = 0)
    {
        using var fs = File.Create(outputPath);
        using var bw = new BinaryWriter(fs);

        var hdrSize = (uint)Marshal.SizeOf<FileHeader>();
        var off = hdrSize;

        var strOff = off; off += Pad4((uint)stringHeap.Length);
        var blbOff = off; off += Pad4((uint)blobHeap.Length);
        var usOff = off; off += Pad4((uint)userStrings.Length);
        var arrOff = Align4(off); off = arrOff + Pad4(SizeOf<PatchAssemblyRefEntry>() * (uint)asmRefs.Length);
        var trOff = Align4(off); off = trOff + Pad4(SizeOf<PatchTypeRefEntry>() * (uint)typeRefs.Length);
        var tdOff = Align4(off); off = tdOff + Pad4(SizeOf<PatchTypeDefEntry>() * (uint)typeDefs.Length);
        var fdOff = Align4(off); off = fdOff + Pad4(SizeOf<PatchFieldDefEntry>() * (uint)fieldDefs.Length);
        var mdOff = Align4(off); off = mdOff + Pad4(SizeOf<PatchMethodDefEntry>() * (uint)methodDefs.Length);
        var mrOff = Align4(off); off = mrOff + Pad4(SizeOf<PatchMemberRefEntry>() * (uint)memberRefs.Length);
        var bodyOff = Align4(off); off = bodyOff + Pad4((uint)bodyData.Length);
        var irOff = off; off = irOff + Pad4((uint)(aotCoreIrSection?.Length ?? 0));
        var regIrOff = off;        // reg_ir section always comes after IR (empty for now, runtime falls back to JSON)
        var depOff = Align4(off);  // dependency section after reg_ir (empty for now)

        // Build dependency entries from AssemblyRef entries.
        // Each dependency uses the same assembly name string already in the string heap.
        // min_version = 0 means optional/informational — PatchLoader skips missing deps.
        var deps = new PatchDataDependency[asmRefs.Length];
        for (int i = 0; i < asmRefs.Length; i++)
            deps[i] = new PatchDataDependency { assembly_name_offset = asmRefs[i].name_offset, min_version = 0 };

        var blobOffsets = ComputeBlobOffsets(blobHeap);
        RemapBlobOffsets(fieldDefs, blobOffsets);
        RemapBlobOffsets(methodDefs, blobOffsets);
        RemapBlobOffsets(memberRefs, blobOffsets);

        var hdr = new FileHeader
        {
            magic = Magic,
            version = 4,            // v4: trailing min_host_revision + patch_revision
            header_size = hdrSize,
            string_heap_offset = strOff,
            string_heap_size = (uint)stringHeap.Length,
            blob_heap_offset = blbOff,
            blob_heap_size = (uint)blobHeap.Length,
            user_string_heap_offset = usOff,
            user_string_heap_size = (uint)userStrings.Length,
            assembly_ref_offset = arrOff,
            assembly_ref_count = (uint)asmRefs.Length,
            type_ref_offset = trOff,
            type_ref_count = (uint)typeRefs.Length,
            type_def_offset = tdOff,
            type_def_count = (uint)typeDefs.Length,
            field_def_offset = fdOff,
            field_def_count = (uint)fieldDefs.Length,
            method_def_offset = mdOff,
            method_def_count = (uint)methodDefs.Length,
            member_ref_offset = mrOff,
            member_ref_count = (uint)memberRefs.Length,
            standalone_sig_offset = 0,
            standalone_sig_count = 0,
            body_data_offset = bodyOff,
            body_data_size = (uint)bodyData.Length,
            aot_core_ir_offset = irOff,
            aot_core_ir_size = (uint)(aotCoreIrSection?.Length ?? 0),
            aot_core_ir_count = aotCoreIrCount,
            reg_ir_offset = 0,
            reg_ir_size = 0,
            reg_ir_count = 0,
            dependency_offset = depOff,
            dependency_count = (uint)deps.Length,
            min_host_revision = 0,           // v4+: 0 = compatible with any host (default)
            patch_revision = 0,              // v4+: 0 = unversioned (default)
        };
        WriteStruct(bw, hdr);

        bw.Write(stringHeap); AlignStream(bw, (uint)stringHeap.Length);
        bw.Write(blobHeap); AlignStream(bw, (uint)blobHeap.Length);
        bw.Write(userStrings); AlignStream(bw, (uint)userStrings.Length);
        WriteStructArray(bw, asmRefs);
        WriteStructArray(bw, typeRefs);
        WriteStructArray(bw, typeDefs);
        WriteStructArray(bw, fieldDefs);
        WriteStructArray(bw, methodDefs);
        WriteStructArray(bw, memberRefs);
        bw.Write(bodyData);
        AlignStream(bw, (uint)bodyData.Length);
        if (aotCoreIrSection != null)
            bw.Write(aotCoreIrSection);
        if (deps.Length > 0)
            WriteStructArray(bw, deps);
    }

    private static uint[] ComputeBlobOffsets(byte[] blobHeap)
    {
        if (blobHeap.Length == 0) return [];
        var offsets = new List<uint>();
        var i = 0;
        while (i < blobHeap.Length)
        {
            offsets.Add((uint)i);
            if (!TryReadCompressedUInt(blobHeap, i, out var len, out var lenSz))
                break;
            i += lenSz + (int)len;
        }
        return [.. offsets];
    }

    private static bool TryReadCompressedUInt(byte[] data, int offset, out uint value, out int size)
    {
        value = 0; size = 0;
        if (offset >= data.Length) return false;
        var b = data[offset];
        if ((b & 0x80) == 0) { value = (uint)b; size = 1; return true; }
        if ((b & 0xC0) == 0x80 && offset + 1 < data.Length) { value = (uint)(((b & 0x3F) << 8) | data[offset + 1]); size = 2; return true; }
        if ((b & 0xE0) == 0xC0 && offset + 3 < data.Length) { value = (uint)(((b & 0x1F) << 24) | (data[offset + 1] << 16) | (data[offset + 2] << 8) | data[offset + 3]); size = 4; return true; }
        return false;
    }

    /// <summary>
    /// Extract the subject index from a method name matching Subject_N, CustomEntrySubject_N,
    /// or CustomEntryMethodN patterns (same as NativeAotLoweringPlanner.ExtractSubjectIndex).
    /// Returns null if the method name is not a subject method.
    /// </summary>

#pragma warning disable CS0649
    [StructLayout(LayoutKind.Sequential)]
    private struct FileHeader
    {
        public uint magic, version, header_size;
        public uint string_heap_offset, string_heap_size;
        public uint blob_heap_offset, blob_heap_size;
        public uint user_string_heap_offset, user_string_heap_size;
        public uint assembly_ref_offset, assembly_ref_count;
        public uint type_ref_offset, type_ref_count;
        public uint type_def_offset, type_def_count;
        public uint field_def_offset, field_def_count;
        public uint method_def_offset, method_def_count;
        public uint member_ref_offset, member_ref_count;
        public uint standalone_sig_offset, standalone_sig_count;
        public uint body_data_offset, body_data_size;
        public uint aot_core_ir_offset, aot_core_ir_size, aot_core_ir_count;
        public uint reg_ir_offset, reg_ir_size, reg_ir_count;
        public uint dependency_offset, dependency_count;
        public uint min_host_revision, patch_revision;   // v4+
    }

    [StructLayout(LayoutKind.Sequential)]
    private struct PatchDataDependency { public uint assembly_name_offset, min_version; }

    [StructLayout(LayoutKind.Sequential)]
    private struct PatchAssemblyRefEntry { public uint name_offset, token; }
    [StructLayout(LayoutKind.Sequential)]
    private struct PatchTypeRefEntry { public uint type_name_offset, namespace_offset, resolution_scope_token, token; }
    [StructLayout(LayoutKind.Sequential)]
    private struct PatchTypeDefEntry { public uint type_name_offset, namespace_offset, enclosing_type_token, extends_token, token; public ushort flags; }
    [StructLayout(LayoutKind.Sequential)]
    private struct PatchFieldDefEntry { public uint name_offset, signature_offset, declaring_type_token, token; public ushort flags; }
    [StructLayout(LayoutKind.Sequential)]
    private struct PatchMethodDefEntry { public uint name_offset, signature_offset, body_offset, body_size, declaring_type_token, token; public ushort impl_flags, flags; }
    [StructLayout(LayoutKind.Sequential)]
    private struct PatchMemberRefEntry { public uint name_offset, signature_offset, parent_token, token; }
#pragma warning restore CS0649

    /// <summary>
    /// Replace Subject_N method IL bodies with "ldc.i4 &lt;sentinel&gt;; ret" for
    /// deterministic hotupdate semantic change detection.
    /// Sentinel = 0xBEEF0000 | subjectIndex — guaranteed to differ from any
    /// real return value produced by GetResultToLongExpression().
    /// </summary>

    /// <summary>
    /// Build a Tiny-format ECMA 335 method body: "ldc.i4 &lt;sentinel&gt;; ret" (7 bytes).
    /// Tiny header: flags=0x02 | (codeSize &lt;&lt; 2)
    /// ldc.i4: opcode 0x20 + 4-byte little-endian int32 (5 bytes)
    /// ret: opcode 0x2A (1 byte)
    /// </summary>

    /// <summary>
    /// Build the AotCoreIr JSON section from a serialized AotCoreIrArtifact JSON file.
    /// For each method in methodDefs, locates the corresponding method JSON by matching
    /// PE metadata (TypeName::MethodName) against AotCoreIr subjectId entries.
    ///
    /// The section is a concatenation of null-terminated JSON strings,
    /// ordered by method index in the MethodDef table.
    /// Methods without a matching AotCoreIr entry get an empty entry (null terminator only).
    /// </summary>
}
