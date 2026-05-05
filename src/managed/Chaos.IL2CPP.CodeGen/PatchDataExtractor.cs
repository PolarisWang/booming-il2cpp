using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.InteropServices;
using System.Text;

namespace Chaos.IL2CPP.CodeGen;

/// <summary>
/// Build-time tool: reads a patch assembly's PE metadata and produces a
/// self-contained .patchdata binary that the runtime PatchLoader can apply.
/// </summary>
public sealed class PatchDataExtractor
{
    private const uint Magic = 0x50415854; // "PADT"

    public void Extract(string dllPath, string outputPath)
    {
        using var stream = File.OpenRead(dllPath);
        using var peReader = new PEReader(stream);
        var mr = peReader.GetMetadataReader();

        // Pass 1: Collect all strings and blobs
        var strings = new Dictionary<string, uint>(StringComparer.Ordinal);
        var blobHandles = new List<BlobHandle>();

        uint AllocString(string s)
        {
            if (string.IsNullOrEmpty(s)) return 0;
            if (strings.TryGetValue(s, out var o)) return o;
            o = (uint)(strings.Count > 0
                ? strings.Values.Max() + (uint)Encoding.UTF8.GetByteCount(strings.Last().Key) + 1
                : 1);
            strings[s] = o;
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

        // Build binary heaps
        var stringHeap = BuildStringHeap(stringOffsets);
        var blobHeap = BuildBlobHeap(blobHandles, mr);

        // Read method bodies
        var bodyData = ReadBodyData(peReader, mr, methodDefs);

        // Serialize
        Serialize(outputPath, stringHeap, blobHeap, userStringBytes,
            [.. asmRefs], [.. typeRefs], [.. typeDefs],
            [.. fieldDefs], [.. methodDefs], [.. memberRefs], bodyData);
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
            entry.body_size = (uint)ilBytes.Length;
            ms.Write(ilBytes, 0, ilBytes.Length);
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
        byte[] bodyData)
    {
        using var fs = File.Create(outputPath);
        using var bw = new BinaryWriter(fs);

        var hdrSize = (uint)Marshal.SizeOf<FileHeader>();
        var off = hdrSize;

        var strOff = off;        off += Pad4((uint)stringHeap.Length);
        var blbOff = off;        off += Pad4((uint)blobHeap.Length);
        var usOff = off;         off += Pad4((uint)userStrings.Length);
        var arrOff = Align4(off); off = arrOff + Pad4(SizeOf<PatchAssemblyRefEntry>() * (uint)asmRefs.Length);
        var trOff = Align4(off);  off = trOff + Pad4(SizeOf<PatchTypeRefEntry>() * (uint)typeRefs.Length);
        var tdOff = Align4(off);  off = tdOff + Pad4(SizeOf<PatchTypeDefEntry>() * (uint)typeDefs.Length);
        var fdOff = Align4(off);  off = fdOff + Pad4(SizeOf<PatchFieldDefEntry>() * (uint)fieldDefs.Length);
        var mdOff = Align4(off);  off = mdOff + Pad4(SizeOf<PatchMethodDefEntry>() * (uint)methodDefs.Length);
        var mrOff = Align4(off);  off = mrOff + Pad4(SizeOf<PatchMemberRefEntry>() * (uint)memberRefs.Length);
        var bodyOff = Align4(off);

        var blobOffsets = ComputeBlobOffsets(blobHeap);
        RemapBlobOffsets(fieldDefs, blobOffsets);
        RemapBlobOffsets(methodDefs, blobOffsets);
        RemapBlobOffsets(memberRefs, blobOffsets);

        var hdr = new FileHeader
        {
            magic = Magic, version = 1, header_size = hdrSize,
            string_heap_offset = strOff, string_heap_size = (uint)stringHeap.Length,
            blob_heap_offset = blbOff, blob_heap_size = (uint)blobHeap.Length,
            user_string_heap_offset = usOff, user_string_heap_size = (uint)userStrings.Length,
            assembly_ref_offset = arrOff, assembly_ref_count = (uint)asmRefs.Length,
            type_ref_offset = trOff, type_ref_count = (uint)typeRefs.Length,
            type_def_offset = tdOff, type_def_count = (uint)typeDefs.Length,
            field_def_offset = fdOff, field_def_count = (uint)fieldDefs.Length,
            method_def_offset = mdOff, method_def_count = (uint)methodDefs.Length,
            member_ref_offset = mrOff, member_ref_count = (uint)memberRefs.Length,
            standalone_sig_offset = 0, standalone_sig_count = 0,
            body_data_offset = bodyOff, body_data_size = (uint)bodyData.Length,
        };
        WriteStruct(bw, hdr);

        bw.Write(stringHeap);   AlignStream(bw, (uint)stringHeap.Length);
        bw.Write(blobHeap);     AlignStream(bw, (uint)blobHeap.Length);
        bw.Write(userStrings);  AlignStream(bw, (uint)userStrings.Length);
        WriteStructArray(bw, asmRefs);
        WriteStructArray(bw, typeRefs);
        WriteStructArray(bw, typeDefs);
        WriteStructArray(bw, fieldDefs);
        WriteStructArray(bw, methodDefs);
        WriteStructArray(bw, memberRefs);
        bw.Write(bodyData);
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

    private static void RemapBlobOffsets(PatchFieldDefEntry[] entries, uint[] blobOffsets)
    {
        for (var i = 0; i < entries.Length; i++)
        {
            var idx = (int)entries[i].signature_offset;
            entries[i].signature_offset = idx < blobOffsets.Length ? blobOffsets[idx] : 0;
        }
    }

    private static void RemapBlobOffsets(PatchMethodDefEntry[] entries, uint[] blobOffsets)
    {
        for (var i = 0; i < entries.Length; i++)
        {
            var idx = (int)entries[i].signature_offset;
            entries[i].signature_offset = idx < blobOffsets.Length ? blobOffsets[idx] : 0;
        }
    }

    private static void RemapBlobOffsets(PatchMemberRefEntry[] entries, uint[] blobOffsets)
    {
        for (var i = 0; i < entries.Length; i++)
        {
            var idx = (int)entries[i].signature_offset;
            entries[i].signature_offset = idx < blobOffsets.Length ? blobOffsets[idx] : 0;
        }
    }

    private static uint SizeOf<T>() => (uint)Marshal.SizeOf<T>();
    private static uint Align4(uint v) => (v + 3) & ~3u;

    private static uint Pad4(uint v)
    {
        var rem = v % 4;
        return rem == 0 ? v : v + (4 - rem);
    }

    private static void WriteStruct<T>(BinaryWriter bw, T value) where T : unmanaged
    {
        Span<byte> buf = stackalloc byte[Marshal.SizeOf<T>()];
        MemoryMarshal.Write(buf, ref value);
        bw.Write(buf);
    }

    private static void WriteStructArray<T>(BinaryWriter bw, T[] arr) where T : unmanaged
    {
        if (arr.Length == 0) return;
        var size = Marshal.SizeOf<T>();
        var buf = new byte[size * arr.Length];
        for (var i = 0; i < arr.Length; i++)
            MemoryMarshal.Write(new Span<byte>(buf, i * size, size), ref arr[i]);
        bw.Write(buf);
        var rem = (size * arr.Length) % 4;
        if (rem != 0) bw.Write(stackalloc byte[4 - rem]);
    }

    private static void AlignStream(BinaryWriter bw, uint written)
    {
        var rem = written % 4;
        if (rem != 0) bw.Write(stackalloc byte[4 - (int)rem]);
    }

#pragma warning disable CS0649
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
    }

    private struct PatchAssemblyRefEntry { public uint name_offset, token; }
    private struct PatchTypeRefEntry { public uint type_name_offset, namespace_offset, resolution_scope_token, token; }
    private struct PatchTypeDefEntry { public uint type_name_offset, namespace_offset, enclosing_type_token, extends_token, token; public ushort flags; }
    private struct PatchFieldDefEntry { public uint name_offset, signature_offset, declaring_type_token, token; public ushort flags; }
    private struct PatchMethodDefEntry { public uint name_offset, signature_offset, body_offset, body_size, declaring_type_token, token; public ushort impl_flags, flags; }
    private struct PatchMemberRefEntry { public uint name_offset, signature_offset, parent_token, token; }
#pragma warning restore CS0649
}
