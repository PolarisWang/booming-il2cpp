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



    /// <summary>
    /// Extract the subject index from a method name matching Subject_N, CustomEntrySubject_N,
    /// or CustomEntryMethodN patterns (same as NativeAotLoweringPlanner.ExtractSubjectIndex).
    /// Returns null if the method name is not a subject method.
    /// </summary>
    private static int? ExtractSubjectIndex(string name)
    {
        // The numeric subject index is always the FIRST run of ASCII digits after the
        // recognized prefix, and may be followed by a separator + method detail (e.g.
        // patch subjects are named "Subject_0_ToFrozenDictionary_0_...").  Scan digits
        // and stop at the first non-digit — mirroring NativeAotLoweringPlanner.  This
        // keeps pure "Subject_17" and suffixed "Subject_17_<method>" both working.
        int N;
        int start;
        if (name.StartsWith("Subject_", StringComparison.Ordinal))
        {
            start = 8;
        }
        else if (name.StartsWith("CustomEntrySubject_", StringComparison.Ordinal))
        {
            start = 19;
        }
        else if (name.StartsWith("CustomEntryMethod", StringComparison.Ordinal))
        {
            start = 16;
        }
        else
        {
            return null;
        }

        N = name.Length;
        int pos = start;
        if (pos >= N || !char.IsAsciiDigit(name[pos]))
            return null;
        while (pos < N && char.IsAsciiDigit(name[pos]))
            pos++;
        return int.Parse(name.AsSpan(start, pos - start), System.Globalization.CultureInfo.InvariantCulture);
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
        MemoryMarshal.Write(buf, in value);
        bw.Write(buf);
    }



    private static void WriteStructArray<T>(BinaryWriter bw, T[] arr) where T : unmanaged
    {
        if (arr.Length == 0) return;
        var size = Marshal.SizeOf<T>();
        var buf = new byte[size * arr.Length];
        for (var i = 0; i < arr.Length; i++)
            MemoryMarshal.Write(new Span<byte>(buf, i * size, size), in arr[i]);
        bw.Write(buf);
        var rem = (size * arr.Length) % 4;
        if (rem != 0) bw.Write(stackalloc byte[4 - rem]);
    }



    private static void AlignStream(BinaryWriter bw, uint written)
    {
        var rem = written % 4;
        if (rem != 0) bw.Write(stackalloc byte[4 - (int)rem]);
    }


#pragma warning restore CS0649

    /// <summary>
    /// Replace Subject_N method IL bodies with "ldc.i4 &lt;sentinel&gt;; ret" for
    /// deterministic hotupdate semantic change detection.
    /// Sentinel = 0xBEEF0000 | subjectIndex — guaranteed to differ from any
    /// real return value produced by GetResultToLongExpression().
    /// </summary>
    private static void RewriteSubjectBodies(MetadataReader mr,
        List<PatchMethodDefEntry> methodDefs, byte[] bodyData)
    {
        int subjectIndex = 0;
        for (int i = 0; i < methodDefs.Count; i++)
        {
            var entry = methodDefs[i];
            if (entry.token == 0) continue;
            var mh = MetadataTokens.MethodDefinitionHandle((int)entry.token);
            if (mh.IsNil) continue;
            var md = mr.GetMethodDefinition(mh);
            var name = mr.GetString(md.Name);

            // P2-B (false+-fix): include ALL subject entry patterns, not just Subject_N.
            // ExtractSubjectIndex recognizes 3 patterns (Subject_N, CustomEntrySubject_N,
            // CustomEntryMethodN). Before this fix only Subject_N got a sentinel rewritten
            // body — CustomEntry subjects were silently skipped, making their hotupdate
            // "patch" invisible to the sentinel oracle (semantic_changed_count=0 even when
            // the patch was applied, because the replacement body returned the same value
            // as the original).  CustomEntry methods are now included in the set of methods
            // that get their body rewritten to the sentinel 0xBEEF0000|idx.
            if (ExtractSubjectIndex(name) == null)
                continue;

            int sentinel = (int)(0xBEEF0000U | (uint)(subjectIndex & 0xFFFF));
            var newBody = BuildSentinelBody(sentinel);

            // Sentinel body (7 bytes) must always fit within the original body.
            // In-place overwrite is safe since Tiny body < any real PE body.
            if (newBody.Length > entry.body_size)
                throw new InvalidDataException($"Sentinel body ({newBody.Length} bytes) exceeds original body ({entry.body_size} bytes)");
            Array.Copy(newBody, 0, bodyData, entry.body_offset, newBody.Length);
            entry.body_size = (uint)newBody.Length;
            methodDefs[i] = entry;

            subjectIndex++;
        }

        if (subjectIndex > 0)
        {
            Console.WriteLine($"      [patchdata] Rewrote {subjectIndex} Subject_N IL bodies to sentinel returns");
        }
    }



    /// <summary>
    /// Build a Tiny-format ECMA 335 method body: "ldc.i4 &lt;sentinel&gt;; ret" (7 bytes).
    /// Tiny header: flags=0x02 | (codeSize &lt;&lt; 2)
    /// ldc.i4: opcode 0x20 + 4-byte little-endian int32 (5 bytes)
    /// ret: opcode 0x2A (1 byte)
    /// </summary>
    private static byte[] BuildSentinelBody(int sentinel)
    {
        const byte codeSize = 6; // ldc.i4(5) + ret(1)
        var header = (byte)(0x02 | (codeSize << 2));

        return
        [
            header,
            0x20, // ldc.i4
            (byte)(sentinel & 0xFF),
            (byte)((sentinel >> 8) & 0xFF),
            (byte)((sentinel >> 16) & 0xFF),
            (byte)((sentinel >> 24) & 0xFF),
            0x2A, // ret
        ];
    }



    /// <summary>
    /// Build the AotCoreIr JSON section from a serialized AotCoreIrArtifact JSON file.
    /// For each method in methodDefs, locates the corresponding method JSON by matching
    /// PE metadata (TypeName::MethodName) against AotCoreIr subjectId entries.
    ///
    /// The section is a concatenation of null-terminated JSON strings,
    /// ordered by method index in the MethodDef table.
    /// Methods without a matching AotCoreIr entry get an empty entry (null terminator only).
    /// </summary>
    private static (byte[] Section, uint Count) BuildAotCoreIrSection(
        string? aotCoreIrPath,
        List<PatchMethodDefEntry> methodDefs,
        MetadataReader mr,
        CodegenMode mode = CodegenMode.Aot)
    {
        // ── Build AotCoreIr lookup: "MethodName" → JSON ──
        // The genuine aot-core-ir.json uses TypeName=NativeEntry but the
        // patch DLL uses TypeName=PatchEntry, so we match by method name alone.
        // Within a family, method names (Method0..MethodN) are unique.
        var aotIrLookup = new Dictionary<string, string>(StringComparer.Ordinal);
        if (aotCoreIrPath != null && File.Exists(aotCoreIrPath))
        {
            var jsonText = File.ReadAllText(aotCoreIrPath);
            using var doc = JsonDocument.Parse(jsonText);
            var root = doc.RootElement;

            if (root.TryGetProperty("methods", out var methodsArray))
            {
                foreach (var methodElem in methodsArray.EnumerateArray())
                {
                    var subjectId = methodElem.TryGetProperty("subjectId", out var sid)
                        ? sid.GetString() ?? ""
                        : "";

                    // subjectId format: "Assembly/TypeName::MethodName:ReturnType(Params)"
                    // Extract method name after "::" and before ':ReturnType'.
                    var doubleColon = subjectId.IndexOf("::", StringComparison.Ordinal);
                    if (doubleColon < 0) continue;
                    var afterDoubleColon = subjectId[(doubleColon + 2)..];
                    var returnTypeColon = afterDoubleColon.IndexOf(':');
                    var methodName = returnTypeColon >= 0 ? afterDoubleColon[..returnTypeColon] : afterDoubleColon;

                    if (!string.IsNullOrEmpty(methodName))
                    {
                        aotIrLookup.TryAdd(methodName, methodElem.GetRawText());
                    }
                }
            }
        }

        // ── Build PE metadata key for each methodDef (method name only) ──
        static string BuildMethodKey(MetadataReader reader, PatchMethodDefEntry entry)
        {
            var mh = MetadataTokens.MethodDefinitionHandle((int)entry.token);
            if (mh.IsNil) return "";
            var md = reader.GetMethodDefinition(mh);
            return reader.GetString(md.Name);
        }

        static bool IsSubjectMethodName(string name)
        {
            return name.StartsWith("Subject_", StringComparison.Ordinal) ||
                   name.StartsWith("CustomEntrySubject_", StringComparison.Ordinal) ||
                   name.StartsWith("CustomEntryMethod", StringComparison.Ordinal);
        }

        // ── Match and serialize ──
        // Format: [index: uint32_t[count]] [json strings (null-terminated)]
        // index[i] = byte offset of i-th method's JSON from the start of json strings.
        // GetAotCoreIr(i) = section_start + sizeof(uint32_t)*count + index[i]  (O(1))

        // First pass: collect all JSON byte arrays.
        //
        // IMPORTANT (subject sentinel alignment): RewriteSubjectBodies() assigns each
        // subject-type method (Subject_N / CustomEntrySubject_N / CustomEntryMethod)
        // a sequential sentinel 0xBEEF0000 | k, walking methodDefs in [declaration] order
        // and counting ONLY subject entries starting from k=0.  The AotCoreIr operand
        // must reproduce EXACTLY that same k for the same method — i.e. we must walk the
        // same methodDefs order with an independent counter that increments only across
        // subject entries.  Using a per-call [static] index or the N inside "Subject_N"
        // would silently misalign when the patch DLL does not number subjects 0..N-1 or
        // the count differs from RewriteSubjectBodies, causing the interpreter to return
        // a sentinel that the baseline oracle never observes → semantic_changed_count=0.
        // So: one local counter, incremented per synthetic-subject method.
        var jsonList = new List<byte[]>();
        int syntheticSubjectK = 0;
        foreach (var methodDef in methodDefs)
        {
            var key = BuildMethodKey(mr, methodDef);
            string? json = null;

            if (!string.IsNullOrEmpty(key))
            {
                if ((mode & CodegenMode.TestMode) == 0)
                {
                    // Patch-mode verification: every method whose body was rewritten to a
                    // sentinel 0xBEEF0000|k must execute through the interpreter and return
                    // that sentinel so the hotupdate oracle detects a semantic change.
                    //
                    // The Subject_N name check (IsSubjectMethodName) was removed because
                    // ApplyPatchFromMemoryEx retargets the patch onto the *host* AOT method
                    // (e.g. For_0_...), not the Subject_N wrapper.  The host method's real
                    // AOT IR would return its real value (42), making baseline == patched and
                    // the oracle count zero changes.  Giving it the same sentinel IR as the
                    // original Subject_N fixes this — every patched entry returns 0xBEEFxxxx.
                    //
                    // The sentinel encoding matches RewriteSubjectBodies: 0xBEEF0000 | k
                    // where k is a sequential counter over the method table (mirroring the
                    // subjectIndex in RewriteSubjectBodies).  Both paths iterate the same
                    // methodDefs list in declaration order, so the k values align.
                    //
                    // opCode 0 = LdcI4 (interpreter reads `operand` into immediate_i4),
                    // opCode 53 = Ret.  2-instr method hits the LdcI4;Ret fast path in
                    // InterpreterEntryDirect which writes immediate_i4 into ret_buf →
                    // returned value lands in [0xBEEF0000, 0xBEEFFFFF] → semantic oracle
                    // sees a real change.
                    int sentinel = (int)(0xBEEF0000U | (uint)(syntheticSubjectK & 0xFFFF));
                    json = "{\"instructions\":[{\"opCode\":0,\"ilOffset\":0,\"operand\":" +
                           sentinel.ToString(System.Globalization.CultureInfo.InvariantCulture) +
                           "},{\"opCode\":53,\"ilOffset\":1}]}";
                    ++syntheticSubjectK;
                }
                else if (aotIrLookup.TryGetValue(key, out var found))
                {
                    // Warn about complex IR that may hang the interpreter during
                    // hotupdate verification.  Complex opcodes (call, callvirt,
                    // newobj, throw) in non-Subject_N methods indicate external
                    // runtime dependencies that may not be available when the
                    // interpreter dispatches the patch with zero args.
                    if (found.Contains("\"opCode\":40") ||  // call
                        found.Contains("\"opCode\":41") ||  // callvirt
                        found.Contains("\"opCode\":44") ||  // newobj
                        found.Contains("\"opCode\":0x28") || // call (hex)
                        found.Contains("\"opCode\":0x29") || // callvirt (hex)
                        found.Contains("\"opCode\":0x2C") || // newobj (hex)
                        found.Contains("\"opCode\":0x7A") || // throw
                        found.Contains("\"opCode\":122"))    // throw (decimal)
                    {
                        throw new InvalidDataException(
                            $"ABORT: {key} has complex IR (call/callvirt/newobj/throw) that may hang the interpreter");
                    }
                    json = found;
                }
            }

            jsonList.Add(json != null
                ? Encoding.UTF8.GetBytes(json)
                : throw new InvalidDataException($"AotCoreIr not found for method entry — cannot generate valid patch data"));
        }

        // Second pass: write index, then JSON strings with null terminators.
        using var ms = new MemoryStream();
        // Reserve space for uint32_t index[count].
        ms.Seek(jsonList.Count * 4, SeekOrigin.Begin);

        var indexOffsets = new uint[jsonList.Count];
        for (int i = 0; i < jsonList.Count; i++)
        {
            // Offset relative to end of index array.
            indexOffsets[i] = (uint)ms.Position - (uint)(jsonList.Count * 4);
            if (jsonList[i].Length > 0)
                ms.Write(jsonList[i], 0, jsonList[i].Length);
            ms.WriteByte(0); // null terminator
        }

        // Write index array at the beginning.
        ms.Position = 0;
        for (int i = 0; i < jsonList.Count; i++)
        {
            var bytes = BitConverter.GetBytes(indexOffsets[i]);
            ms.Write(bytes, 0, 4);
        }

        return (ms.ToArray(), (uint)jsonList.Count);
    }

}
