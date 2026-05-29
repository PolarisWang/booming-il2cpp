using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Runtime.Loader;
using System.Text;

namespace Chaos.IL2CPP.Driver;

internal static class JitAsmCapture
{
    // ── Win32 P/Invoke (platform-specific) ──────────────────────────────────
#if WINDOWS
    [StructLayout(LayoutKind.Sequential)]
    private struct MEMORY_BASIC_INFORMATION
    {
        public IntPtr BaseAddress;
        public IntPtr AllocationBase;
        public uint AllocationProtect;
        public ulong RegionSize;
        public uint State;
        public uint Protect;
        public uint Type;
    }

    [DllImport("kernel32.dll", SetLastError = true)]
    private static extern int VirtualQuery(
        IntPtr lpAddress,
        out MEMORY_BASIC_INFORMATION lpBuffer,
        int dwLength);

    [Flags]
    private enum MemState : uint
    {
        MemCommit = 0x1000,
        MemReserve = 0x2000,
        MemFree = 0x10000,
    }

    private const uint PageExecuteRead = 0x20;
    private const uint PageExecuteReadWrite = 0x40;
#endif

    // ── Platform-safe code buffer sizing ─────────────────────────────────────
    /// <summary>
    /// Determine safe capture buffer size. On Windows, uses VirtualQuery to
    /// validate the memory region is executable. On other platforms, defaults
    /// to 4096 and lets FindMethodBoundary detect the actual method end.
    /// </summary>
    private static int GetSafeCaptureSize(IntPtr fnPtr, out string? error)
    {
#if WINDOWS
        int mbiSize = Marshal.SizeOf<MEMORY_BASIC_INFORMATION>();
        if (VirtualQuery(fnPtr, out var mbi, mbiSize) == 0)
        {
            error = $"VirtualQuery failed (error {Marshal.GetLastPInvokeError()})";
            return 0;
        }
        if (mbi.State != (uint)MemState.MemCommit ||
            (mbi.Protect != PageExecuteRead && mbi.Protect != PageExecuteReadWrite))
        {
            error = $"Memory region not executable: state=0x{mbi.State:x} protect=0x{mbi.Protect:x}";
            return 0;
        }
        error = null;
        return Math.Min((int)mbi.RegionSize, 4096);
#else
        error = null;
        return 4096;
#endif
    }

    // ── Result types ────────────────────────────────────────────────────────

    public sealed record IlBodyInfo(
        int Offset,
        int Size,
        IReadOnlyList<IlInstruction> Instructions);

    public sealed record IlInstruction(
        int Offset,
        string OpCode,
        string Operand);

    public sealed record DecodedInstruction(
        IntPtr Address,
        byte[] RawBytes,
        string Mnemonic,
        string Operands)
    {
        public string HexBytes => string.Join(" ", RawBytes.Select(b => $"{b:x2}"));
    }

    public sealed record JitCaptureResult
    {
        public IntPtr Address { get; init; }
        public int Size { get; init; }
        public byte[]? Bytes { get; init; }
        public string? HexDump { get; init; }
        public IReadOnlyList<DecodedInstruction>? Instructions { get; init; }
        public IlBodyInfo? IlBody { get; init; }
        public string? Error { get; init; }
        public string? MethodFullName { get; init; }
        public bool Success => Error is null;
    }

    // ── Main entry point ────────────────────────────────────────────────────

    /// <summary>
    /// Disassemble raw code bytes (e.g. from .obj .text section) using the
    /// same in-house x86-64 decoder used for JIT capture.
    /// </summary>
    public static JitCaptureResult DisassembleRaw(byte[] code, ulong baseAddress, string? methodFullName = null)
    {
        try
        {
            var instructions = Disassemble(new IntPtr((long)baseAddress), code);
            var hexDump = FormatHexDump(new IntPtr((long)baseAddress), code, instructions);

            return new JitCaptureResult
            {
                Address = new IntPtr((long)baseAddress),
                Size = code.Length,
                Bytes = code,
                HexDump = hexDump,
                Instructions = instructions,
                MethodFullName = methodFullName,
            };
        }
        catch (Exception ex)
        {
            return new JitCaptureResult
            {
                Error = $"DisassembleRaw failed: {ex.Message}"
            };
        }
    }

    /// <summary>
    /// Load an assembly into a custom ALC that redirects v10+ framework
    /// assembly references to the versions already loaded in the default
    /// context.  This allows .NET 10 subjects DLLs to be loaded into a
    /// .NET 8 host for JIT capture.
    /// </summary>
    private static Assembly LoadAssemblyWithVersionRedirect(string assemblyPath)
    {
        var fullPath = Path.GetFullPath(assemblyPath);
        var searchDir = Path.GetDirectoryName(fullPath)!;

        // Use a deterministic name so the runtime can deduplicate
        var alc = new AssemblyLoadContext(
            "asm-cmp-" + Path.GetFileNameWithoutExtension(fullPath),
            isCollectible: false);

        alc.Resolving += (context, assemblyName) =>
        {
            var name = assemblyName.Name;
            if (name is null) return null;

            // Framework / runtime assemblies — redirect any version to
            // whatever the default ALC already provides.
            if (name.StartsWith("System.") ||
                name.StartsWith("Microsoft.") ||
                name is "mscorlib" or "netstandard" or "System.Private.CoreLib")
            {
                try
                {
                    return Assembly.Load(name);
                }
                catch
                {
                    return typeof(object).Assembly;
                }
            }

            // Probe the subjects DLL directory for dependency assemblies
            // (e.g. SnapshotTestFixtures.dll for snapshot-prover family).
            var depPath = Path.Combine(searchDir, name + ".dll");
            if (File.Exists(depPath))
            {
                try { return context.LoadFromAssemblyPath(depPath); }
                catch { /* fall through to default binder */ }
            }

            // Everything else: let the default binder try
            try
            {
                return Assembly.Load(name);
            }
            catch
            {
                return null;
            }
        };

        return alc.LoadFromAssemblyPath(fullPath);
    }

    /// <summary>
    /// Try to resolve a method from an assembly-prefixed methodSubjectId.
    /// Format: "AssemblyName/TypeName::MethodName:ReturnType(Params)"
    /// Returns null if the format doesn't match or resolution fails.
    /// </summary>
    private static MethodInfo? ResolveMethodFromSubjectId(string methodId)
    {
        // Must contain "/" indicating assembly prefix
        int slashIdx = methodId.IndexOf('/');
        if (slashIdx < 0)
            return null;

        var assemblyName = methodId[..slashIdx];
        var rest = methodId[(slashIdx + 1)..];

        // Must contain "::" separating type from method
        int doubleColonIdx = rest.IndexOf("::", StringComparison.Ordinal);
        if (doubleColonIdx < 0)
            return null;

        var typeName = rest[..doubleColonIdx];
        var methodSignature = rest[(doubleColonIdx + 2)..];

        // Extract bare method name (before first ':' or '(')
        string methodShortName;
        int colonIdx = methodSignature.IndexOf(':');
        int parenIdx = methodSignature.IndexOf('(');
        if (colonIdx >= 0 && (parenIdx < 0 || colonIdx < parenIdx))
            methodShortName = methodSignature[..colonIdx];
        else
            methodShortName = methodSignature;

        // Load the assembly from the default ALC (already loaded)
        Assembly? assembly;
        try
        {
            assembly = Assembly.Load(assemblyName);
        }
        catch
        {
            return null;
        }

        // Resolve the type
        Type? type;
        try
        {
            type = assembly.GetType(typeName);
        }
        catch
        {
            type = null;
        }

        if (type is null)
        {
            // Fallback: scan all types in assembly
            foreach (var t in assembly.GetTypes())
            {
                if (t.FullName != null &&
                    (t.FullName.Equals(typeName, StringComparison.OrdinalIgnoreCase) ||
                     t.FullName.EndsWith("." + typeName, StringComparison.OrdinalIgnoreCase)))
                {
                    type = t;
                    break;
                }
            }
        }

        if (type is null)
            return null;

        // Find method by name (exact match preferred)
        var exactByName = FindMethodInType(type, methodShortName);

        // If exact match by name found and param types available, verify it matches the
        // expected parameter types from the SubjectId to avoid picking the wrong overload.
        if (exactByName is not null)
        {
            var paramTypes = ParseParameterTypes(methodSignature);
            if (paramTypes is not null)
            {
                // Check if the name-only match has the right signature
                if (MethodMatchesParameters(exactByName, paramTypes))
                    return exactByName;

                // Name-only match was wrong overload — find by parameter types instead
                var byParams = FindMethodInType(type, methodShortName, paramTypes);
                if (byParams is not null)
                    return byParams;

                // Return the name-only match as best-effort fallback
                return exactByName;
            }
        }

        return exactByName;
    }

    public static JitCaptureResult Capture(string assemblyPath, string methodName)
    {
        try
        {
            // Phase 0: If methodName follows "AssemblyName/TypeName::Method..." format,
            // resolve the real target method directly from the framework assembly
            // (instead of capturing the Subjects DLL wrapper methods).
            MethodInfo? realMethod = null;
            if (methodName.Contains('/') && methodName.Contains("::"))
            {
                realMethod = ResolveMethodFromSubjectId(methodName);
            }

            if (realMethod is not null)
            {
                // JIT-compile the real target method (no Subjects DLL needed)
                RuntimeHelpers.PrepareMethod(realMethod.MethodHandle);
                var subjectFnPtr = realMethod.MethodHandle.GetFunctionPointer();

                int subjectMaxSize = GetSafeCaptureSize(subjectFnPtr, out var subjectError);
                if (subjectError is not null)
                {
                    return new JitCaptureResult
                    {
                        MethodFullName = realMethod.ToString(),
                        Error = subjectError
                    };
                }
                byte[] subjectCode = new byte[subjectMaxSize];
                Marshal.Copy(subjectFnPtr, subjectCode, 0, subjectMaxSize);

                int subjectActualSize = FindMethodBoundary(subjectCode, subjectMaxSize);
                byte[] subjectActualCode = new byte[subjectActualSize];
                Array.Copy(subjectCode, subjectActualCode, subjectActualSize);

                var subjectInstructions = Disassemble(subjectFnPtr, subjectActualCode);
                string subjectHexDump = FormatHexDump(subjectFnPtr, subjectActualCode, subjectInstructions);

                return new JitCaptureResult
                {
                    Address = subjectFnPtr,
                    Size = subjectActualSize,
                    Bytes = subjectActualCode,
                    HexDump = subjectHexDump,
                    Instructions = subjectInstructions,
                    MethodFullName = realMethod.ToString(),
                };
            }

            // Phase 1: Read IL body from assembly metadata (no execution needed)
            var ilBody = ReadIlFromAssembly(assemblyPath, methodName);

            // Phase 2: Load and JIT the method
            Assembly assembly;
            try
            {
                assembly = LoadAssemblyWithVersionRedirect(assemblyPath);
            }
            catch (Exception ex)
            {
                return new JitCaptureResult
                {
                    IlBody = ilBody,
                    Error = $"Failed to load assembly: {ex.Message}"
                };
            }

            var method = ResolveMethod(assembly, methodName);
            if (method is null)
            {
                return new JitCaptureResult
                {
                    IlBody = ilBody,
                    Error = $"Method '{methodName}' not found in assembly"
                };
            }

            // Force JIT compilation (Tier1)
            RuntimeHelpers.PrepareMethod(method.MethodHandle);

            var fnPtr = method.MethodHandle.GetFunctionPointer();

            // Determine actual code size by scanning for method boundaries
            int maxSize = GetSafeCaptureSize(fnPtr, out var mbiError);
            if (mbiError is not null)
            {
                return new JitCaptureResult
                {
                    IlBody = ilBody,
                    MethodFullName = method.ToString(),
                    Error = mbiError
                };
            }
            byte[] code = new byte[maxSize];
            Marshal.Copy(fnPtr, code, 0, maxSize);

            int actualSize = FindMethodBoundary(code, maxSize);
            byte[] actualCode = new byte[actualSize];
            Array.Copy(code, actualCode, actualSize);

            // Disassemble
            var instructions = Disassemble(fnPtr, actualCode);
            string hexDump = FormatHexDump(fnPtr, actualCode, instructions);

            return new JitCaptureResult
            {
                Address = fnPtr,
                Size = actualSize,
                Bytes = actualCode,
                HexDump = hexDump,
                Instructions = instructions,
                IlBody = ilBody,
                MethodFullName = method.ToString(),
            };
        }
        catch (Exception ex)
        {
            return new JitCaptureResult
            {
                Error = $"JIT capture failed: {ex.Message}"
            };
        }
    }

    // ── IL Body reading via MetadataLoadContext ──────────────────────────────

    private static IlBodyInfo? ReadIlFromAssembly(string assemblyPath, string methodName)
    {
        try
        {
            using var assemblyStream = File.OpenRead(assemblyPath);
            using var peReader = new PEReader(assemblyStream);
            var mdReader = peReader.GetMetadataReader();

            // Parse methodName: support "TypeName::MethodName" or "TypeName.MethodName"
            string typeName, methodShortName;
            if (methodName.Contains("::"))
            {
                var parts = methodName.Split("::", 2);
                typeName = parts[0].Trim();
                methodShortName = parts[1].Trim();
            }
            else if (methodName.Contains('.'))
            {
                int lastDot = methodName.LastIndexOf('.');
                typeName = methodName[..lastDot];
                methodShortName = methodName[(lastDot + 1)..];
            }
            else
            {
                // Search all methods
                typeName = "";
                methodShortName = methodName;
            }

            foreach (var typeHandle in mdReader.TypeDefinitions)
            {
                var typeDef = mdReader.GetTypeDefinition(typeHandle);
                var typeFullName = GetTypeFullName(mdReader, typeDef);
                if (!string.IsNullOrEmpty(typeName) && !MatchesTypeName(typeFullName, typeName))
                    continue;

                foreach (var methodHandle in typeDef.GetMethods())
                {
                    var methodDef = mdReader.GetMethodDefinition(methodHandle);
                    var mName = mdReader.GetString(methodDef.Name);
                    if (!string.Equals(mName, methodShortName, StringComparison.OrdinalIgnoreCase))
                        continue;

                    // Found the method — read IL body via PEReader.GetMethodBody
                    if (methodDef.RelativeVirtualAddress == 0)
                        continue;

                    var bodyBlock = peReader.GetMethodBody(methodDef.RelativeVirtualAddress);
                    if (bodyBlock is null)
                        continue;

                    var ilBytes = bodyBlock.GetILBytes();
                    if (ilBytes is null || ilBytes.Length == 0)
                        continue;

                    // Decode IL instructions
                    var ilInstructions = DecodeIlInstructions(ilBytes);

                    return new IlBodyInfo(methodDef.RelativeVirtualAddress, ilBytes.Length, ilInstructions);
                }
            }

            return null;
        }
        catch (Exception ex)
        {
            Debug.WriteLine($"IL read failed: {ex.Message}");
            return null;
        }
    }

    private static string GetTypeFullName(MetadataReader reader, TypeDefinition typeDef)
    {
        var name = reader.GetString(typeDef.Name);
        var ns = typeDef.Namespace.IsNil ? "" : reader.GetString(typeDef.Namespace);
        return string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
    }

    private static bool MatchesTypeName(string fullName, string searchName)
    {
        return fullName.Equals(searchName, StringComparison.OrdinalIgnoreCase) ||
               fullName.EndsWith("." + searchName, StringComparison.OrdinalIgnoreCase);
    }

    // ── IL instruction decoder ─────────────────────────────────────────────

    private static readonly Dictionary<byte, string> IlOpCodeNames = new()
    {
        { 0x00, "nop" }, { 0x01, "break" }, { 0x02, "ldarg.0" }, { 0x03, "ldarg.1" },
        { 0x04, "ldarg.2" }, { 0x05, "ldarg.3" }, { 0x06, "ldloc.0" }, { 0x07, "ldloc.1" },
        { 0x08, "ldloc.2" }, { 0x09, "ldloc.3" }, { 0x0a, "stloc.0" }, { 0x0b, "stloc.1" },
        { 0x0c, "stloc.2" }, { 0x0d, "stloc.3" }, { 0x0e, "ldarg.s" }, { 0x0f, "ldarga.s" },
        { 0x10, "starg.s" }, { 0x11, "ldloc.s" }, { 0x12, "ldloca.s" }, { 0x13, "stloc.s" },
        { 0x14, "ldnull" }, { 0x15, "ldc.i4.m1" }, { 0x16, "ldc.i4.0" }, { 0x17, "ldc.i4.1" },
        { 0x18, "ldc.i4.2" }, { 0x19, "ldc.i4.3" }, { 0x1a, "ldc.i4.4" }, { 0x1b, "ldc.i4.5" },
        { 0x1c, "ldc.i4.6" }, { 0x1d, "ldc.i4.7" }, { 0x1e, "ldc.i4.8" }, { 0x1f, "ldc.i4.s" },
        { 0x20, "ldc.i4" }, { 0x21, "ldc.i8" }, { 0x22, "ldc.r4" }, { 0x23, "ldc.r8" },
        { 0x25, "dup" }, { 0x26, "pop" }, { 0x27, "jmp" }, { 0x28, "call" },
        { 0x29, "calli" }, { 0x2a, "ret" }, { 0x2b, "br.s" }, { 0x2c, "brfalse.s" },
        { 0x2d, "brtrue.s" }, { 0x2e, "beq.s" }, { 0x2f, "bge.s" },
        { 0x30, "bgt.s" }, { 0x31, "ble.s" }, { 0x32, "blt.s" }, { 0x33, "bne.un.s" },
        { 0x34, "bge.un.s" }, { 0x35, "bgt.un.s" }, { 0x36, "ble.un.s" }, { 0x37, "blt.un.s" },
        { 0x38, "br" }, { 0x39, "brfalse" }, { 0x3a, "brtrue" }, { 0x3b, "beq" },
        { 0x3c, "bge" }, { 0x3d, "bgt" }, { 0x3e, "ble" }, { 0x3f, "blt" },
        { 0x40, "bne.un" }, { 0x41, "bge.un" }, { 0x42, "bgt.un" }, { 0x43, "ble.un" },
        { 0x44, "blt.un" }, { 0x45, "switch" }, { 0x46, "ldind.i1" }, { 0x47, "ldind.u1" },
        { 0x48, "ldind.i2" }, { 0x49, "ldind.u2" }, { 0x4a, "ldind.i4" }, { 0x4b, "ldind.u4" },
        { 0x4c, "ldind.i8" }, { 0x4d, "ldind.i" }, { 0x4e, "ldind.r4" }, { 0x4f, "ldind.r8" },
        { 0x50, "ldind.ref" }, { 0x51, "stind.ref" }, { 0x52, "stind.i1" }, { 0x53, "stind.i2" },
        { 0x54, "stind.i4" }, { 0x55, "stind.i8" }, { 0x56, "stind.r4" }, { 0x57, "stind.r8" },
        { 0x58, "add" }, { 0x59, "sub" }, { 0x5a, "mul" }, { 0x5b, "div" },
        { 0x5c, "div.un" }, { 0x5d, "rem" }, { 0x5e, "rem.un" }, { 0x5f, "and" },
        { 0x60, "or" }, { 0x61, "xor" }, { 0x62, "shl" }, { 0x63, "shr" },
        { 0x64, "shr.un" }, { 0x65, "neg" }, { 0x66, "not" }, { 0x67, "conv.i1" },
        { 0x68, "conv.i2" }, { 0x69, "conv.i4" }, { 0x6a, "conv.i8" }, { 0x6b, "conv.r4" },
        { 0x6c, "conv.r8" }, { 0x6d, "conv.u4" }, { 0x6e, "conv.u8" },
        { 0x70, "ckfinite" }, { 0x71, "mkrefany" }, { 0x72, "ldtoken" }, { 0x73, "conv.u2" },
        { 0x74, "conv.u1" }, { 0x75, "conv.i" }, { 0x76, "conv.ovf.i" }, { 0x77, "conv.ovf.u" },
        { 0x78, "add.ovf" }, { 0x79, "add.ovf.un" }, { 0x7a, "mul.ovf" }, { 0x7b, "mul.ovf.un" },
        { 0x7c, "sub.ovf" }, { 0x7d, "sub.ovf.un" }, { 0x7e, "endfinally" }, { 0x7f, "leave" },
        { 0x80, "leave.s" }, { 0x81, "stind.i" }, { 0x82, "conv.ovf.i.un" }, { 0x83, "conv.ovf.u.un" },
        { 0x84, "conv.ovf.i1" }, { 0x85, "conv.ovf.i1.un" }, { 0x86, "conv.ovf.u1" }, { 0x87, "conv.ovf.u1.un" },
        { 0x88, "conv.ovf.i2" }, { 0x89, "conv.ovf.i2.un" }, { 0x8a, "conv.ovf.u2" }, { 0x8b, "conv.ovf.u2.un" },
        { 0x8c, "conv.ovf.i4" }, { 0x8d, "conv.ovf.i4.un" }, { 0x8e, "conv.ovf.u4" }, { 0x8f, "conv.ovf.u4.un" },
        { 0x90, "conv.ovf.i8" }, { 0x91, "conv.ovf.i8.un" }, { 0x92, "conv.ovf.u8" }, { 0x93, "conv.ovf.u8.un" },
        { 0x94, "refanyval" }, { 0x95, "ckfinite" }, // placeholder dup
        { 0x9c, "throw" }, { 0x9d, "sizeof" }, { 0x9e, "refanytype" },
        // 0x100+ are 2-byte opcodes (prefix 0xfe)
    };

    private static readonly Dictionary<byte, string> IlTwoByteOpCodeNames = new()
    {
        { 0x01, "ceq" }, { 0x02, "cgt" }, { 0x03, "cgt.un" }, { 0x04, "clt" }, { 0x05, "clt.un" },
        { 0x06, "ldftn" }, { 0x07, "ldvirtftn" }, { 0x09, "ldarg" }, { 0x0a, "ldarga" },
        { 0x0b, "starg" }, { 0x0c, "ldloc" }, { 0x0d, "ldloca" }, { 0x0e, "stloc" },
        { 0x0f, "ldnull" }, // placeholder
        { 0x11, "volatile." }, { 0x12, "tail." }, { 0x13, "constrained." }, { 0x14, "readonly." },
        { 0x15, "unaligned." }, { 0x16, "arglist" },
        { 0x17, "ldfld" }, { 0x18, "ldflda" }, { 0x19, "stfld" }, { 0x1a, "ldsfld" },
        { 0x1b, "ldsflda" }, { 0x1c, "stsfld" }, { 0x1d, "stobj" },
        { 0x1e, "conv.ovf.i.un" }, { 0x1f, "conv.ovf.u.un" },
        { 0x20, "box" }, { 0x21, "newarr" }, { 0x22, "ldlen" }, { 0x23, "ldelema" },
        { 0x24, "ldelem.i1" }, { 0x25, "ldelem.u1" }, { 0x26, "ldelem.i2" }, { 0x27, "ldelem.u2" },
        { 0x28, "ldelem.i4" }, { 0x29, "ldelem.u4" }, { 0x2a, "ldelem.i8" }, { 0x2b, "ldelem.i" },
        { 0x2c, "ldelem.r4" }, { 0x2d, "ldelem.r8" }, { 0x2e, "ldelem.ref" },
        { 0x2f, "stelem.i" }, { 0x30, "stelem.i1" }, { 0x31, "stelem.i2" },
        { 0x32, "stelem.i4" }, { 0x33, "stelem.i8" }, { 0x34, "stelem.r4" }, { 0x35, "stelem.r8" },
        { 0x36, "stelem.ref" }, { 0x37, "ldelem" }, { 0x38, "stelem" },
        { 0x39, "unbox" }, { 0x3a, "unbox.any" },
        { 0x3b, "conv.r.un" }, { 0x3c, "conv.ovf.i" }, { 0x3d, "conv.ovf.u" },
        { 0x3e, "refanyval" }, { 0x3f, "ckfinite" }, // placeholder
        { 0x41, "mkrefany" }, // placeholder
        { 0x44, "ldtoken" },
        { 0x45, "conv.u2" }, { 0x46, "conv.u1" }, { 0x47, "conv.i" },
        { 0x48, "conv.ovf.i" }, { 0x49, "conv.ovf.u" },
        { 0x4a, "add.ovf" }, { 0x4b, "add.ovf.un" },
        { 0x4c, "mul.ovf" }, { 0x4d, "mul.ovf.un" },
        { 0x4e, "sub.ovf" }, { 0x4f, "sub.ovf.un" },
        { 0x50, "endfinally" }, { 0x51, "leave" }, { 0x52, "leave.s" },
        { 0x53, "stind.i" },
        { 0x54, "conv.ovf.i.un" }, { 0x55, "conv.ovf.u.un" },
        { 0x56, "newobj" },
        { 0x58, "initobj" },
        { 0x5b, "sizeof" },
        { 0x5c, "refanytype" },
        { 0x5d, "readonly." }, // careful: conflicts
        { 0x61, "ldelem" }, { 0x62, "stelem" }, { 0x63, "unbox" }, { 0x64, "unbox.any" },
        { 0x65, "conv.r.un" },
        { 0x66, "conv.ovf.i" }, { 0x67, "conv.ovf.u" },
        { 0x68, "refanyval" },
        { 0x69, "ckfinite" },
        { 0x6a, "mkrefany" },
        { 0x6c, "ldtoken" },
        { 0x6d, "conv.u2" }, { 0x6e, "conv.u1" }, { 0x6f, "conv.i" },
        { 0x70, "callvirt" }, { 0x71, "newobj" }, { 0x72, "ldstr" },
        { 0x73, "isinst" }, { 0x74, "castclass" },
        { 0x75, "ldnull" }, // unbox
        { 0x79, "unbox" },
        { 0x7a, "throw" },
        { 0x7b, "sizeof" },
        { 0x7c, "refanytype" },
    };

    private static IReadOnlyList<IlInstruction> DecodeIlInstructions(byte[] ilBytes)
    {
        var instructions = new List<IlInstruction>();
        int offset = 0;
        while (offset < ilBytes.Length)
        {
            int startOffset = offset;
            byte op = ilBytes[offset++];
            string opName;
            bool twoByte = false;

            if (op == 0xfe && offset < ilBytes.Length)
            {
                twoByte = true;
                byte op2 = ilBytes[offset++];
                opName = IlTwoByteOpCodeNames.TryGetValue(op2, out var name) ? name : $"0xfe_{op2:x2}";
            }
            else
            {
                opName = IlOpCodeNames.TryGetValue(op, out var name) ? name : $"{op:x2}";
            }

            // Parse operand based on opcode
            string operand = "";

            // Short branches (1 byte operand, signed)
            if ((op >= 0x2b && op <= 0x44) || op == 0x80) // br.s, brfalse.s, brtrue.s, beq.s, bge.s, bgt.s, ble.s, blt.s, bne.un.s, bge.un.s, bgt.un.s, ble.un.s, blt.un.s, leave.s
            {
                if (offset < ilBytes.Length)
                {
                    sbyte disp = (sbyte)ilBytes[offset++];
                    int target = startOffset + disp + (twoByte ? 2 : 1);
                    operand = $"IL_{target:x4}";
                }
            }
            // Long branches (4 byte operand)
            else if (op is 0x38 or 0x39 or 0x3a or 0x3b or 0x3c or 0x3d or 0x3e or 0x3f
                  or 0x40 or 0x41 or 0x42 or 0x43 or 0x44 || op == 0x7f) // br, brfalse, brtrue, beq, bge, bgt, ble, blt, bne.un, bge.un, bgt.un, ble.un, blt.un, leave
            {
                if (offset + 4 <= ilBytes.Length)
                {
                    int disp = ilBytes[offset] | (ilBytes[offset + 1] << 8) | (ilBytes[offset + 2] << 16) | (ilBytes[offset + 3] << 24);
                    offset += 4;
                    int target = startOffset + disp + (twoByte ? 2 : 1);
                    operand = $"IL_{target:x4}";
                }
            }
            // ldc.i4 (4 byte operand)
            else if (op == 0x20)
            {
                if (offset + 4 <= ilBytes.Length)
                {
                    int val = ilBytes[offset] | (ilBytes[offset + 1] << 8) | (ilBytes[offset + 2] << 16) | (ilBytes[offset + 3] << 24);
                    offset += 4;
                    operand = $"{val}";
                }
            }
            // ldc.i4.s (1 byte)
            else if (op == 0x1f)
            {
                if (offset < ilBytes.Length)
                    operand = $"{ilBytes[offset++]}";
            }
            // ldc.i8 (8 byte)
            else if (op == 0x21)
            {
                if (offset + 8 <= ilBytes.Length)
                {
                    long val = (long)ilBytes[offset] | ((long)ilBytes[offset + 1] << 8) | ((long)ilBytes[offset + 2] << 16) | ((long)ilBytes[offset + 3] << 24)
                             | ((long)ilBytes[offset + 4] << 32) | ((long)ilBytes[offset + 5] << 40) | ((long)ilBytes[offset + 6] << 48) | ((long)ilBytes[offset + 7] << 56);
                    offset += 8;
                    operand = $"0x{val:x}";
                }
            }
            // call, callvirt, newobj, ldfld, stfld, ldsfld, stsfld etc: 4 byte metadata token
            else if (op == 0x28 || op == 0x29 || (twoByte && (op == 0x70 || op == 0x71 || op == 0x72 || op == 0x73 || op == 0x74
                     || op == 0x17 || op == 0x18 || op == 0x19 || op == 0x1a || op == 0x1b || op == 0x1c || op == 0x56)))
            {
                if (offset + 4 <= ilBytes.Length)
                {
                    int token = ilBytes[offset] | (ilBytes[offset + 1] << 8) | (ilBytes[offset + 2] << 16) | (ilBytes[offset + 3] << 24);
                    offset += 4;
                    operand = $"0x{token:x8}";
                }
            }
            // ldloc.s, stloc.s, ldarg.s, starg.s: 1 byte index
            else if (op is 0x11 or 0x12 or 0x13 or 0x0e or 0x0f or 0x10)
            {
                if (offset < ilBytes.Length)
                    operand = $"{ilBytes[offset++]}";
            }
            // ldc.r4 (4 byte float)
            else if (op == 0x22)
            {
                if (offset + 4 <= ilBytes.Length)
                {
                    float val = BitConverter.ToSingle(ilBytes, offset);
                    offset += 4;
                    operand = $"{val}";
                }
            }
            // ldc.r8 (8 byte double)
            else if (op == 0x23)
            {
                if (offset + 8 <= ilBytes.Length)
                {
                    double val = BitConverter.ToDouble(ilBytes, offset);
                    offset += 8;
                    operand = $"{val}";
                }
            }
            // switch
            else if (op == 0x45)
            {
                if (offset + 4 <= ilBytes.Length)
                {
                    int n = ilBytes[offset] | (ilBytes[offset + 1] << 8) | (ilBytes[offset + 2] << 16) | (ilBytes[offset + 3] << 24);
                    offset += 4;
                    var targets = new List<string>();
                    for (int i = 0; i < n && offset + 4 <= ilBytes.Length; i++)
                    {
                        int disp = ilBytes[offset] | (ilBytes[offset + 1] << 8) | (ilBytes[offset + 2] << 16) | (ilBytes[offset + 3] << 24);
                        offset += 4;
                        targets.Add($"IL_{startOffset + disp + 1:x4}");
                    }
                    operand = $"({n}) " + string.Join(", ", targets);
                }
            }

            instructions.Add(new IlInstruction(startOffset, opName, operand));
        }
        return instructions;
    }

    // ── Method resolution via Reflection ────────────────────────────────────

    private static MethodInfo? ResolveMethod(Assembly assembly, string methodName)
    {
        // Support "TypeName::MethodName" or "TypeName.MethodName"
        string typeName, shortName;
        if (methodName.Contains("::"))
        {
            var parts = methodName.Split("::", 2);
            typeName = parts[0].Trim();
            shortName = parts[1].Trim();
        }
        else if (methodName.Contains('.'))
        {
            int lastDot = methodName.LastIndexOf('.');
            typeName = methodName[..lastDot];
            shortName = methodName[(lastDot + 1)..];
        }
        else
        {
            // Try to find a unique match across all types
            foreach (var type in assembly.GetTypes())
            {
                var method = FindMethodInType(type, methodName);
                if (method != null) return method;
            }
            return null;
        }

        var resolvedType = assembly.GetType(typeName);
        if (resolvedType is null)
        {
            foreach (var type in assembly.GetTypes())
            {
                if (type.FullName != null && MatchesTypeName(type.FullName, typeName))
                {
                    resolvedType = type;
                    break;
                }
            }
        }
        if (resolvedType is null) return null;

        return FindMethodInType(resolvedType, shortName);
    }

    private static MethodInfo? FindMethodInType(Type type, string shortName)
    {
        var methods = type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic |
                                       BindingFlags.Static | BindingFlags.Instance |
                                       BindingFlags.DeclaredOnly);
        // Exact match first
        var method = methods.FirstOrDefault(m =>
            m.Name.Equals(shortName, StringComparison.OrdinalIgnoreCase));
        if (method != null) return method;

        // Match by name + signature
        var candidate = methods.FirstOrDefault(m =>
            m.Name.Contains(shortName, StringComparison.OrdinalIgnoreCase));
        return candidate;
    }

    /// Try to resolve a method by matching both name and parameter types.
    /// This avoids picking the wrong overload when multiple methods share a name.
    private static MethodInfo? FindMethodInType(Type type, string shortName, string[] paramTypes)
    {
        var methods = type.GetMethods(BindingFlags.Public | BindingFlags.NonPublic |
                                       BindingFlags.Static | BindingFlags.Instance |
                                       BindingFlags.DeclaredOnly);

        // Match by name and parameter types
        foreach (var m in methods)
        {
            if (!m.Name.Equals(shortName, StringComparison.OrdinalIgnoreCase))
                continue;
            if (MethodMatchesParameters(m, paramTypes))
                return m;
        }

        return null;
    }

    /// Check if a MethodInfo's parameter types match the expected type names.
    private static bool MethodMatchesParameters(MethodInfo method, string[] paramTypes)
    {
        var ps = method.GetParameters();
        if (ps.Length != paramTypes.Length)
            return false;

        for (int i = 0; i < ps.Length; i++)
        {
            var expected = paramTypes[i];
            var actual = ps[i].ParameterType.FullName ?? ps[i].ParameterType.Name;

            // Support both full name (System.Boolean) and short name (Boolean) matching
            if (!actual.EndsWith(expected, StringComparison.OrdinalIgnoreCase) &&
                !expected.EndsWith(actual, StringComparison.OrdinalIgnoreCase))
                return false;
        }

        return true;
    }

    /// Parse parameter type names from a methodSignature like "ToChar:System.Char(System.Boolean)".
    /// Returns null if no parameters, or the array of parameter type name strings.
    private static string[]? ParseParameterTypes(string methodSignature)
    {
        int openParen = methodSignature.IndexOf('(');
        int closeParen = methodSignature.LastIndexOf(')');
        if (openParen < 0 || closeParen <= openParen)
            return null;

        var paramsStr = methodSignature[(openParen + 1)..closeParen];
        if (string.IsNullOrWhiteSpace(paramsStr))
            return null;

        return paramsStr.Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries);
    }

    // ── Native code boundary detection ──────────────────────────────────────

    private static int FindMethodBoundary(byte[] code, int maxSize)
    {
        int pos = 0;
        int lastRet = -1;

        while (pos < maxSize)
        {
            byte op = code[pos];

            // ret, retf
            if (op == 0xC3 || op == 0xCB)
            {
                lastRet = pos + 1;
                // If next byte looks like padding (0xCC int3 or 0x00 or 0x90 nop), stop here
                if (pos + 1 < maxSize)
                {
                    byte next = code[pos + 1];
                    if (next == 0xCC || next == 0x00 || next == 0x90)
                        return pos + 1;
                }
                pos++;
                continue;
            }

            int len = GetInstructionLength(code, pos, maxSize);
            if (len <= 0) break;
            pos += len;
        }

        if (lastRet > 0) return lastRet;
        return Math.Min(maxSize, 256); // fallback
    }

    // ── x86-64 Instruction Decoder ─────────────────────────────────────────

    private static readonly byte[] OpcodeLengths = BuildOpcodeLengthTable();

    private static byte[] BuildOpcodeLengthTable()
    {
        // Primary opcode table (0x00-0xFF): 0=variable/unknown, 1=fixed length
        byte[] table = new byte[256];

        // Most 1-byte opcodes with no ModRM
        for (int i = 0; i < 256; i++)
            table[i] = 1;

        // Opcodes that include ModRM byte (and thus may be longer)
        // These are approximate - correct for the common case
        byte[] hasModRM = {
            0x00, 0x01, 0x02, 0x03, 0x08, 0x09, 0x0A, 0x0B,
            0x0C, 0x0D, 0x10, 0x11, 0x12, 0x13, 0x18, 0x19,
            0x1A, 0x1B, 0x20, 0x21, 0x22, 0x23, 0x28, 0x29,
            0x2A, 0x2B, 0x30, 0x31, 0x32, 0x33, 0x38, 0x39,
            0x3A, 0x3B,
            0x60, 0x61, 0x62, 0x63,
            0x88, 0x89, 0x8A, 0x8B, 0x8C, 0x8D, 0x8E, 0x8F,
            0xC0, 0xC1, 0xC4, 0xC5, 0xC6, 0xC7,
            0xD0, 0xD1, 0xD2, 0xD3,
            0xD8, 0xD9, 0xDA, 0xDB, 0xDC, 0xDD, 0xDE, 0xDF,
            0xE0, 0xE1, 0xE2, 0xE3,
            0xF6, 0xF7, 0xFE, 0xFF
        };
        foreach (byte b in hasModRM) table[b] = 2; // opcode + ModRM (minimum)

        // Exceptions
        table[0x0F] = 0; // two-byte opcode prefix
        table[0x66] = 0; // operand size prefix
        table[0x67] = 0; // address size prefix
        table[0x40] = 0; // REX prefix range 0x40-0x4F
        for (int i = 0x41; i <= 0x4F; i++) table[i] = 0;
        table[0x64] = 0; // FS segment override
        table[0x65] = 0; // GS segment override
        table[0x26] = 0; // ES segment override
        table[0x2E] = 0; // CS segment override
        table[0x36] = 0; // SS segment override
        table[0x3E] = 0; // DS segment override
        table[0x9C] = 1; // pushfq
        table[0x9D] = 1; // popfq
        table[0x9E] = 1; // sahf
        table[0x9F] = 1; // lahf
        table[0xE8] = 0; // call rel32 - variable
        table[0xE9] = 0; // jmp rel32 - variable
        table[0xEB] = 0; // jmp rel8
        table[0xCC] = 1; // int3
        table[0xC3] = 1; // ret
        table[0xC9] = 1; // leave
        table[0x90] = 1; // nop
        table[0x50] = 0; // push reg (0x50-0x57)
        for (int i = 0x51; i <= 0x57; i++) table[i] = 0;
        table[0x58] = 0; // pop reg (0x58-0x5F)
        for (int i = 0x59; i <= 0x5F; i++) table[i] = 0;
        table[0x70] = 0; // jcc rel8 range (0x70-0x7F)
        for (int i = 0x71; i <= 0x7F; i++) table[i] = 0;
        table[0xA0] = 0; // mov al, moffs
        table[0xA1] = 0; // mov eax/rax, moffs
        table[0xA2] = 0; // mov moffs, al
        table[0xA3] = 0; // mov moffs, eax/rax
        table[0xA8] = 2; // test al, imm8
        table[0xA9] = 0; // test eax/rax, imm32
        table[0xB0] = 0; // mov r8, imm8 (0xB0-0xB7)
        for (int i = 0xB1; i <= 0xB7; i++) table[i] = 0;
        table[0xB8] = 0; // mov r32/r64, imm32 (0xB8-0xBF)
        for (int i = 0xB9; i <= 0xBF; i++) table[i] = 0;
        table[0xC2] = 0; // ret imm16
        table[0xCD] = 0; // int imm8
        table[0xE4] = 0; // in al, imm8
        table[0xE5] = 0; // in eax, imm8
        table[0xE6] = 0; // out imm8, al
        table[0xE7] = 0; // out imm8, eax
        table[0xEE] = 0; // out dx, al
        table[0xEF] = 0; // out dx, eax
        table[0xEC] = 1; // in al, dx
        table[0xED] = 1; // in eax, dx
        table[0xF5] = 1; // cmc
        table[0xF8] = 1; // clc
        table[0xF9] = 1; // stc
        table[0xFC] = 1; // cld
        table[0xFD] = 1; // std
        table[0xFA] = 1; // cli
        table[0xFB] = 1; // sti
        table[0xCF] = 1; // iret

        return table;
    }

    private static int GetInstructionLength(byte[] code, int pos, int maxSize)
    {
        if (pos >= maxSize) return 0;
        byte first = code[pos];

        int prefixCount = 0;
        int offset = pos;

        // Count prefixes (REX, segment overrides, 0x66, 0x67, lock, rep)
        while (offset < maxSize)
        {
            byte b = code[offset];
            bool isPrefix = false;

            if ((b & 0xF0) == 0x40) // REX prefix (0x40-0x4F)
                isPrefix = true;
            else if (b is 0x64 or 0x65 or 0x26 or 0x2E or 0x36 or 0x3E or 0x66 or 0x67 or 0xF0 or 0xF2 or 0xF3)
                isPrefix = true;

            if (!isPrefix) break;
            prefixCount++;
            offset++;
            if (prefixCount > 5) break; // sanity
        }

        if (offset >= maxSize) return 0;
        byte opcode = code[offset];

        // Two-byte opcode (0x0F)
        if (opcode == 0x0F)
        {
            offset++;
            if (offset >= maxSize) return 0;
            byte op2 = code[offset];

            // VEX 3-byte prefix starting with 0x0F
            // 0x0F 0x38 or 0x0F 0x3A = 3-byte opcode
            // Common two-byte opcodes with ModRM
            return EstimateLength(code, pos, offset + 1, maxSize);
        }

        // Check if opcode has ModRM
        int baseLen = offset - pos; // prefix count
        if (HasModRM(opcode))
        {
            baseLen += 1; // opcode
            int modrmOffset = offset + 1;
            if (modrmOffset >= maxSize) return 0;

            byte modrm = code[modrmOffset];
            int mod = (modrm >> 6) & 3;
            int rm = modrm & 7;

            baseLen += 1; // ModRM

            // SIB byte (when ModRM.Mod != 3 and ModRM.RM == 4)
            if (mod != 3 && rm == 4)
            {
                baseLen += 1; // SIB
                int sibOffset = offset + 2;
                if (sibOffset < maxSize)
                {
                    byte sib = code[sibOffset];
                    int sibBase = sib & 7;
                    // Displacement after SIB
                    if (mod == 0 && sibBase == 5)
                        baseLen += 4; // [register*scale + disp32]
                    else if (mod == 1)
                        baseLen += 1; // disp8
                    else if (mod == 2)
                        baseLen += 4; // disp32
                }
            }
            else if (mod == 0 && rm == 5)
            {
                baseLen += 4; // RIP-relative disp32
            }
            else if (mod == 1)
            {
                baseLen += 1; // disp8
            }
            else if (mod == 2)
            {
                baseLen += 4; // disp32
            }
            // mod == 3: register direct, no displacement

            // Immediate after ModRM/SIB/displacement
            baseLen += GetImmediateSize(code, offset, modrm);
        }
        else
        {
            // Opcode without ModRM
            switch (opcode)
            {
                case 0xE8: // call rel32
                case 0xE9: // jmp rel32
                    baseLen += 5; // 1(op) + 4(disp)
                    break;
                case 0xEB: // jmp rel8
                case 0x70: case 0x71: case 0x72: case 0x73: case 0x74:
                case 0x75: case 0x76: case 0x77: case 0x78: case 0x79:
                case 0x7A: case 0x7B: case 0x7C: case 0x7D: case 0x7E:
                case 0x7F: // jcc rel8
                    baseLen += 2; // 1(op) + 1(disp)
                    break;
                case 0xA9: // test eax/rax, imm32
                    baseLen += 5; // 1(op) + 4(imm)
                    break;
                case 0xA8: // test al, imm8
                    baseLen += 2;
                    break;
                case 0xC2: // ret imm16
                    baseLen += 3;
                    break;
                case 0xCD: // int imm8
                    baseLen += 2;
                    break;
                case 0xE4: case 0xE6: // in/out imm8
                    baseLen += 2;
                    break;
                case 0xE5: case 0xE7: // in/out imm32 (actually in eax, imm8)
                    baseLen += 2;
                    break;
                default:
                    baseLen += 1; // just opcode
                    break;
            }

            // For push/pop reg: 0x50-0x5F are single byte
            if (opcode >= 0x50 && opcode <= 0x5F)
                baseLen = offset - pos + 1;

            // For mov reg, imm: 0xB0-0xB7 (mov r8, imm8) and 0xB8-0xBF (mov r64, imm32/64)
            if (opcode >= 0xB0 && opcode <= 0xBF)
            {
                bool rexW = false;
                for (int i = pos; i < offset; i++)
                    if ((code[i] & 0x48) == 0x48) // REX.W
                        rexW = true;

                if (opcode <= 0xB7)
                    baseLen = offset - pos + 2; // 1 op + 1 imm8
                else
                    baseLen = offset - pos + (rexW ? 9 : 5); // 1 op + 4/8 imm
            }

            // For mov al/ax/eax/rax, moffs: 0xA0-0xA3
            if (opcode >= 0xA0 && opcode <= 0xA3)
            {
                if ((code[pos] & 0x48) == 0x48) // with REX.W
                    baseLen = offset - pos + 9; // 1 op + 8 addr
                else
                    baseLen = offset - pos + 5; // 1 op + 4 addr
            }
        }

        return Math.Max(baseLen, 1); // at minimum 1 byte
    }

    private static int EstimateLength(byte[] code, int startPos, int afterOpPos, int maxSize)
    {
        // Simplified length estimation for two-byte opcodes
        int len = afterOpPos - startPos + 1; // prefixes + 2 opcode bytes

        byte modrm = code[afterOpPos];
        int mod = (modrm >> 6) & 3;
        int rm = modrm & 7;

        len++; // ModRM

        if (mod != 3 && rm == 4)
        {
            len++; // SIB
        }

        if (mod == 0 && rm == 5)
            len += 4; // RIP-relative
        else if (mod == 1)
            len += 1;
        else if (mod == 2)
            len += 4;

        return len;
    }

    private static bool HasModRM(byte opcode)
    {
        // Opcodes that use ModRM byte
        return opcode is
            0x00 or 0x01 or 0x02 or 0x03 or
            0x08 or 0x09 or 0x0A or 0x0B or
            0x0C or 0x0D or
            0x10 or 0x11 or 0x12 or 0x13 or
            0x18 or 0x19 or 0x1A or 0x1B or
            0x20 or 0x21 or 0x22 or 0x23 or
            0x28 or 0x29 or 0x2A or 0x2B or
            0x30 or 0x31 or 0x32 or 0x33 or
            0x38 or 0x39 or 0x3A or 0x3B or
            0x60 or 0x61 or 0x62 or 0x63 or
            0x69 or 0x6B or
            0x80 or 0x81 or 0x82 or 0x83 or
            0x84 or 0x85 or
            0x88 or 0x89 or 0x8A or 0x8B or
            0x8C or 0x8D or 0x8E or 0x8F or
            0xC0 or 0xC1 or 0xC4 or 0xC5 or
            0xC6 or 0xC7 or
            0xD0 or 0xD1 or 0xD2 or 0xD3 or
            0xD8 or 0xD9 or 0xDA or 0xDB or
            0xDC or 0xDD or 0xDE or 0xDF or
            0xE0 or 0xE1 or 0xE2 or 0xE3 or
            0xF6 or 0xF7 or 0xFE or 0xFF;
    }

    private static int GetImmediateSize(byte[] code, int opcodeOffset, byte modrm)
    {
        byte opcode = code[opcodeOffset];
        int mod = (modrm >> 6) & 3;
        int reg = (modrm >> 3) & 7;

        // grp instructions (opcode determines group)
        bool hasImm = false;
        int immSize = 0;

        switch (opcode)
        {
            case 0xC6: // mov r/m8, imm8 (grp11, reg=0)
                if (reg == 0) { hasImm = true; immSize = 1; }
                break;
            case 0xC7: // mov r/m32/64, imm32 (grp11, reg=0)
                if (reg == 0) { hasImm = true; immSize = 4; }
                break;
            case 0xC0: // grp2: r/m8, imm8 (reg=0-7, shift/rotate)
                hasImm = true; immSize = 1;
                break;
            case 0xC1: // grp2: r/m32/64, imm8
                hasImm = true; immSize = 1;
                break;
            case 0xD0: case 0xD2: // grp2: r/m8, 1 (no immediate)
                break;
            case 0xD1: case 0xD3: // grp2: r/m32/64, 1 (no immediate)
                break;
            case 0xF6: // grp3: test/mul/div/not/neg
                if (reg == 0 || reg == 1) // test r/m8, imm8
                { hasImm = true; immSize = 1; }
                break;
            case 0xF7: // grp3: test/mul/div/not/neg
                if (reg == 0 || reg == 1) // test r/m32/64, imm32
                { hasImm = true; immSize = 4; }
                break;
            case 0x80: // grp1: r/m8, imm8 (add/or/adc/sbb/and/sub/xor/cmp)
                hasImm = true; immSize = 1;
                break;
            case 0x81: // grp1: r/m32/64, imm32
                hasImm = true; immSize = 4;
                break;
            case 0x83: // grp1: r/m32/64, imm8 (sign-extended)
                hasImm = true; immSize = 1;
                break;
            case 0x8D: // lea — no immediate
                break;
            default:
                // For standard opcodes with ModRM, immediate is uncommon
                break;
        }

        return hasImm ? immSize : 0;
    }

    // ── Disassembly ─────────────────────────────────────────────────────────

    private static readonly string[] Reg64 = { "rax", "rcx", "rdx", "rbx", "rsp", "rbp", "rsi", "rdi", "r8", "r9", "r10", "r11", "r12", "r13", "r14", "r15" };
    private static readonly string[] Reg32 = { "eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi", "r8d", "r9d", "r10d", "r11d", "r12d", "r13d", "r14d", "r15d" };
    private static readonly string[] Reg8 = { "al", "cl", "dl", "bl", "ah", "ch", "dh", "bh", "r8b", "r9b", "r10b", "r11b", "r12b", "r13b", "r14b", "r15b" };

    private static readonly string[] OpcodeNames = BuildOpcodeNames();

    private static string[] BuildOpcodeNames()
    {
        var names = new string[256];
        names[0x00] = "add"; names[0x01] = "add"; names[0x02] = "add"; names[0x03] = "add";
        names[0x04] = "add"; names[0x05] = "add";
        names[0x06] = "push"; names[0x07] = "pop";
        names[0x08] = "or"; names[0x09] = "or"; names[0x0A] = "or"; names[0x0B] = "or";
        names[0x0C] = "or"; names[0x0D] = "or";
        names[0x0E] = "push"; names[0x0F] = "two-byte";
        names[0x10] = "adc"; names[0x11] = "adc"; names[0x12] = "adc"; names[0x13] = "adc";
        names[0x14] = "adc"; names[0x15] = "adc";
        names[0x16] = "push"; names[0x17] = "pop";
        names[0x18] = "sbb"; names[0x19] = "sbb";
        names[0x1A] = "sbb"; names[0x1B] = "sbb";
        names[0x1C] = "sbb"; names[0x1D] = "sbb";
        names[0x1E] = "push"; names[0x1F] = "pop";
        names[0x20] = "and"; names[0x21] = "and"; names[0x22] = "and"; names[0x23] = "and";
        names[0x24] = "and"; names[0x25] = "and";
        names[0x26] = "es:"; names[0x27] = "daa";
        names[0x28] = "sub"; names[0x29] = "sub"; names[0x2A] = "sub"; names[0x2B] = "sub";
        names[0x2C] = "sub"; names[0x2D] = "sub";
        names[0x2E] = "cs:"; names[0x2F] = "das";
        names[0x30] = "xor"; names[0x31] = "xor"; names[0x32] = "xor"; names[0x33] = "xor";
        names[0x34] = "xor"; names[0x35] = "xor";
        names[0x36] = "ss:"; names[0x37] = "aaa";
        names[0x38] = "cmp"; names[0x39] = "cmp"; names[0x3A] = "cmp"; names[0x3B] = "cmp";
        names[0x3C] = "cmp"; names[0x3D] = "cmp";
        names[0x3E] = "ds:"; names[0x3F] = "aas";
        names[0x40] = "rex"; names[0x41] = "rex.b"; names[0x42] = "rex.x"; names[0x43] = "rex.xb";
        names[0x44] = "rex.r"; names[0x45] = "rex.rb"; names[0x46] = "rex.rx"; names[0x47] = "rex.rxb";
        names[0x48] = "rex.w"; names[0x49] = "rex.wb"; names[0x4A] = "rex.wx"; names[0x4B] = "rex.wxb";
        names[0x4C] = "rex.wr"; names[0x4D] = "rex.wrb"; names[0x4E] = "rex.wrx"; names[0x4F] = "rex.wrxb";
        names[0x50] = "push"; names[0x51] = "push"; names[0x52] = "push"; names[0x53] = "push";
        names[0x54] = "push"; names[0x55] = "push"; names[0x56] = "push"; names[0x57] = "push";
        names[0x58] = "pop"; names[0x59] = "pop"; names[0x5A] = "pop"; names[0x5B] = "pop";
        names[0x5C] = "pop"; names[0x5D] = "pop"; names[0x5E] = "pop"; names[0x5F] = "pop";
        names[0x60] = "pusha"; names[0x61] = "popa";
        names[0x62] = "bound"; names[0x63] = "movsxd";
        names[0x64] = "fs:"; names[0x65] = "gs:";
        names[0x66] = "0x66"; names[0x67] = "0x67";
        names[0x68] = "push"; names[0x69] = "imul";
        names[0x6A] = "push"; names[0x6B] = "imul";
        names[0x6C] = "insb"; names[0x6D] = "insd";
        names[0x6E] = "outsb"; names[0x6F] = "outsd";
        names[0x70] = "jo"; names[0x71] = "jno"; names[0x72] = "jb"; names[0x73] = "jnb";
        names[0x74] = "jz"; names[0x75] = "jnz"; names[0x76] = "jbe"; names[0x77] = "ja";
        names[0x78] = "js"; names[0x79] = "jns"; names[0x7A] = "jp"; names[0x7B] = "jnp";
        names[0x7C] = "jl"; names[0x7D] = "jge"; names[0x7E] = "jle"; names[0x7F] = "jg";
        names[0x80] = "grp1"; names[0x81] = "grp1"; names[0x82] = "grp1"; names[0x83] = "grp1";
        names[0x84] = "test"; names[0x85] = "test";
        names[0x86] = "xchg"; names[0x87] = "xchg";
        names[0x88] = "mov"; names[0x89] = "mov"; names[0x8A] = "mov"; names[0x8B] = "mov";
        names[0x8C] = "mov"; names[0x8D] = "lea"; names[0x8E] = "mov";
        names[0x8F] = "pop";
        names[0x90] = "nop"; names[0x91] = "xchg"; names[0x92] = "xchg"; names[0x93] = "xchg";
        names[0x94] = "xchg"; names[0x95] = "xchg"; names[0x96] = "xchg"; names[0x97] = "xchg";
        names[0x98] = "cwde"; names[0x99] = "cdq"; names[0x9A] = "callf"; names[0x9B] = "wait";
        names[0x9C] = "pushfq"; names[0x9D] = "popfq"; names[0x9E] = "sahf"; names[0x9F] = "lahf";
        names[0xA0] = "mov"; names[0xA1] = "mov"; names[0xA2] = "mov"; names[0xA3] = "mov";
        names[0xA4] = "movsb"; names[0xA5] = "movsd";
        names[0xA6] = "cmpsb"; names[0xA7] = "cmpsd";
        names[0xA8] = "test"; names[0xA9] = "test";
        names[0xAA] = "stosb"; names[0xAB] = "stosd";
        names[0xAC] = "lodsb"; names[0xAD] = "lodsd";
        names[0xAE] = "scasb"; names[0xAF] = "scasd";
        names[0xB0] = "mov"; names[0xB1] = "mov"; names[0xB2] = "mov"; names[0xB3] = "mov";
        names[0xB4] = "mov"; names[0xB5] = "mov"; names[0xB6] = "mov"; names[0xB7] = "mov";
        names[0xB8] = "mov"; names[0xB9] = "mov"; names[0xBA] = "mov"; names[0xBB] = "mov";
        names[0xBC] = "mov"; names[0xBD] = "mov"; names[0xBE] = "mov"; names[0xBF] = "mov";
        names[0xC0] = "grp2"; names[0xC1] = "grp2";
        names[0xC2] = "retn"; names[0xC3] = "ret";
        names[0xC4] = "les"; names[0xC5] = "lds";
        names[0xC6] = "mov"; names[0xC7] = "mov";
        names[0xC8] = "enter"; names[0xC9] = "leave";
        names[0xCA] = "retf"; names[0xCB] = "retf";
        names[0xCC] = "int3"; names[0xCD] = "int";
        names[0xCE] = "into"; names[0xCF] = "iret";
        names[0xD0] = "grp2"; names[0xD1] = "grp2"; names[0xD2] = "grp2"; names[0xD3] = "grp2";
        names[0xD4] = "aam"; names[0xD5] = "aad";
        names[0xD6] = "0xd6"; names[0xD7] = "xlat";
        names[0xD8] = "esc"; names[0xD9] = "esc";
        names[0xDA] = "esc"; names[0xDB] = "esc";
        names[0xDC] = "esc"; names[0xDD] = "esc";
        names[0xDE] = "esc"; names[0xDF] = "esc";
        names[0xE0] = "loopne"; names[0xE1] = "loope"; names[0xE2] = "loop"; names[0xE3] = "jecxz";
        names[0xE4] = "in"; names[0xE5] = "in"; names[0xE6] = "out"; names[0xE7] = "out";
        names[0xE8] = "call"; names[0xE9] = "jmp"; names[0xEA] = "jmpf"; names[0xEB] = "jmp";
        names[0xEC] = "in"; names[0xED] = "in"; names[0xEE] = "out"; names[0xEF] = "out";
        names[0xF0] = "lock"; names[0xF1] = "int1";
        names[0xF2] = "repne"; names[0xF3] = "rep";
        names[0xF4] = "hlt";
        names[0xF5] = "cmc";
        names[0xF6] = "grp3"; names[0xF7] = "grp3";
        names[0xF8] = "clc"; names[0xF9] = "stc";
        names[0xFA] = "cli"; names[0xFB] = "sti";
        names[0xFC] = "cld"; names[0xFD] = "std";
        names[0xFE] = "grp4"; names[0xFF] = "grp5";
        return names;
    }

    private static readonly Dictionary<byte, string> TwoByteOpcodeNames = new()
    {
        { 0x01, "lgdt" }, { 0x06, "clts" }, { 0x08, "invd" }, { 0x09, "wbinvd" },
        { 0x0B, "ud2" }, { 0x10, "movsd" }, { 0x11, "movsd" },
        { 0x12, "movhlps" }, { 0x13, "movlps" },
        { 0x16, "movhps" }, { 0x17, "movhps" },
        { 0x1F, "nop" }, // multi-byte nop
        { 0x28, "movaps" }, { 0x29, "movaps" },
        { 0x2A, "cvtpi2ps" }, { 0x2B, "movntps" },
        { 0x2C, "cvttps2pi" }, { 0x2D, "cvtps2pi" },
        { 0x2E, "ucomiss" }, { 0x2F, "comiss" },
        { 0x30, "wrmsr" }, { 0x31, "rdtsc" }, { 0x32, "rdmsr" }, { 0x33, "rdpmc" },
        { 0x34, "sysenter" }, { 0x35, "sysexit" },
        { 0x37, "getsec" },
        { 0x38, "" }, // three-byte opcode prefix
        { 0x3A, "" }, // three-byte opcode prefix
        { 0x40, "cmovo" }, { 0x41, "cmovno" }, { 0x42, "cmovb" }, { 0x43, "cmovae" },
        { 0x44, "cmove" }, { 0x45, "cmovne" }, { 0x46, "cmovbe" }, { 0x47, "cmova" },
        { 0x48, "cmovs" }, { 0x49, "cmovns" }, { 0x4A, "cmovp" }, { 0x4B, "cmovnp" },
        { 0x4C, "cmovl" }, { 0x4D, "cmovge" }, { 0x4E, "cmovle" }, { 0x4F, "cmovg" },
        { 0x50, "movmskps" }, { 0x51, "sqrtps" }, { 0x52, "rsqrtps" }, { 0x53, "rcpps" },
        { 0x54, "andps" }, { 0x55, "andnps" }, { 0x56, "orps" }, { 0x57, "xorps" },
        { 0x58, "addps" }, { 0x59, "mulps" }, { 0x5A, "cvtps2pd" }, { 0x5B, "cvtdq2ps" },
        { 0x5C, "subps" }, { 0x5D, "minps" }, { 0x5E, "divps" }, { 0x5F, "maxps" },
        { 0x60, "punpcklbw" }, { 0x61, "punpcklwd" }, { 0x62, "punpckldq" }, { 0x63, "packsswb" },
        { 0x64, "pcmpgtb" }, { 0x65, "pcmpgtw" }, { 0x66, "pcmpgtd" }, { 0x67, "packuswb" },
        { 0x68, "punpckhbw" }, { 0x69, "punpckhwd" }, { 0x6A, "punpckhdq" }, { 0x6B, "packssdw" },
        { 0x6C, "punpcklqdq" }, { 0x6D, "punpckhqdq" },
        { 0x6E, "movd" }, { 0x6F, "movdqa" },
        { 0x70, "pshufw" }, { 0x71, "psrlw" }, { 0x72, "psrld" }, { 0x73, "psrlq" },
        { 0x74, "pcmpeqb" }, { 0x75, "pcmpeqw" }, { 0x76, "pcmpeqd" },
        { 0x77, "emms" }, { 0x78, "vmread" }, { 0x79, "vmwrite" },
        { 0x7C, "haddpd" }, { 0x7D, "haddps" },
        { 0x7E, "movd" }, { 0x7F, "movdqa" },
        { 0x80, "jo" }, { 0x81, "jno" }, { 0x82, "jb" }, { 0x83, "jnb" },
        { 0x84, "jz" }, { 0x85, "jnz" }, { 0x86, "jbe" }, { 0x87, "ja" },
        { 0x88, "js" }, { 0x89, "jns" }, { 0x8A, "jp" }, { 0x8B, "jnp" },
        { 0x8C, "jl" }, { 0x8D, "jge" }, { 0x8E, "jle" }, { 0x8F, "jg" },
        { 0x90, "seto" }, { 0x91, "setno" }, { 0x92, "setb" }, { 0x93, "setae" },
        { 0x94, "sete" }, { 0x95, "setne" }, { 0x96, "setbe" }, { 0x97, "seta" },
        { 0x98, "sets" }, { 0x99, "setns" }, { 0x9A, "setp" }, { 0x9B, "setnp" },
        { 0x9C, "setl" }, { 0x9D, "setge" }, { 0x9E, "setle" }, { 0x9F, "setg" },
        { 0xA0, "push fs" }, { 0xA1, "pop fs" },
        { 0xA2, "cpuid" },
        { 0xA3, "bt" }, { 0xA4, "shld" }, { 0xA5, "shld" },
        { 0xA6, "xbts" }, { 0xA7, "ibts" },
        { 0xA8, "push gs" }, { 0xA9, "pop gs" },
        { 0xAA, "rsm" },
        { 0xAB, "bts" }, { 0xAC, "shrd" }, { 0xAD, "shrd" },
        { 0xAE, "fxsave" }, { 0xAF, "imul" },
        { 0xB0, "cmpxchg" }, { 0xB1, "cmpxchg" },
        { 0xB2, "lss" }, { 0xB3, "btr" },
        { 0xB4, "lfs" }, { 0xB5, "lgs" },
        { 0xB6, "movzx" }, { 0xB7, "movzx" },
        { 0xB8, "" }, // jmpe or popcnt (with F3 prefix)
        { 0xB9, "ud1" },
        { 0xBA, "bt/btc/btr/bts" }, { 0xBB, "btc" },
        { 0xBC, "bsf" }, { 0xBD, "bsr" },
        { 0xBE, "movsx" }, { 0xBF, "movsx" },
        { 0xC0, "xadd" }, { 0xC1, "xadd" },
        { 0xC3, "movnti" },
        { 0xC4, "pinsrw" }, { 0xC5, "pextrw" },
        { 0xC6, "shufps" },
        { 0xC7, "cmpxchg8b/16b" },
        { 0xC8, "bswap" }, { 0xC9, "bswap" }, { 0xCA, "bswap" }, { 0xCB, "bswap" },
        { 0xCC, "bswap" }, { 0xCD, "bswap" }, { 0xCE, "bswap" }, { 0xCF, "bswap" },
        { 0xD0, "addsubpd" },
        { 0xD1, "psrlw" }, { 0xD2, "psrld" }, { 0xD3, "psrlq" },
        { 0xD4, "paddq" },
        { 0xD5, "pmullw" },
        { 0xD6, "movq" },
        { 0xD7, "pmovmskb" },
        { 0xD8, "psubusb" }, { 0xD9, "psubusw" },
        { 0xDA, "pminub" }, { 0xDB, "pand" },
        { 0xDC, "paddusb" }, { 0xDD, "paddusw" },
        { 0xDE, "pmaxub" }, { 0xDF, "pandn" },
        { 0xE0, "pavgb" },
        { 0xE1, "psraw" }, { 0xE2, "psrad" }, { 0xE3, "pavgw" },
        { 0xE4, "pmulhuw" }, { 0xE5, "pmulhw" },
        { 0xE6, "cvttpd2dq" },
        { 0xE7, "movntq" },
        { 0xE8, "psubsb" }, { 0xE9, "psubsw" },
        { 0xEA, "pminsw" }, { 0xEB, "por" },
        { 0xEC, "paddsb" }, { 0xED, "paddsw" },
        { 0xEE, "pmaxsw" }, { 0xEF, "pxor" },
        { 0xF0, "lddqu" },
        { 0xF1, "psllw" }, { 0xF2, "pslld" }, { 0xF3, "psllq" },
        { 0xF4, "pmuludq" },
        { 0xF5, "pmaddwd" },
        { 0xF6, "psadbw" },
        { 0xF7, "maskmovq" },
        { 0xF8, "psubb" }, { 0xF9, "psubw" },
        { 0xFA, "psubd" },
        { 0xFB, "psubq" },
        { 0xFC, "paddb" }, { 0xFD, "paddw" },
        { 0xFE, "paddd" },
    };

    private static IReadOnlyList<DecodedInstruction> Disassemble(IntPtr baseAddress, byte[] code)
    {
        var instructions = new List<DecodedInstruction>();
        int pos = 0;
        var startBytes = new List<byte>();

        while (pos < code.Length)
        {
            int startPos = pos;

            // Parse prefixes
            bool rexW = false;
            bool has66 = false;
            int rexB = 0;
            int rexX = 0;
            int rexR = 0;

            while (pos < code.Length)
            {
                byte b = code[pos];
                if ((b & 0xF0) == 0x40) // REX
                {
                    rexW = (b & 0x08) != 0;
                    rexB = (b & 0x01);
                    rexX = (b & 0x02) >> 1;
                    rexR = (b & 0x04) >> 2;
                    pos++;
                }
                else if (b == 0x66) { has66 = true; pos++; }
                else if (b == 0x67) { pos++; }
                else if (b is 0xF2 or 0xF3) { pos++; }
                else if (b is 0x64 or 0x65 or 0x26 or 0x2E or 0x36 or 0x3E) { pos++; }
                else break;
            }

            if (pos >= code.Length) break;

            int opcodeOffset = pos;
            byte opcode = code[pos];
            byte op2 = 0;
            string mnemonic;

            bool isTwoByte = false;
            if (opcode == 0x0F && pos + 1 < code.Length)
            {
                isTwoByte = true;
                pos++;
                op2 = code[pos];
                if (!TwoByteOpcodeNames.TryGetValue(op2, out mnemonic))
                    mnemonic = $"?0f_{op2:x2}";
            }
            else
            {
                mnemonic = OpcodeNames[opcode];
                if (string.IsNullOrEmpty(mnemonic))
                    mnemonic = $"?_{opcode:x2}";
            }

            pos++;

            // Parse ModRM if needed
            int mod = 0, reg = 0, rm = 0;
            bool hasModRM = false;
            string modrmStr = "";

            if (NeedsModRM(opcode, isTwoByte, op2))
            {
                if (pos < code.Length)
                {
                    hasModRM = true;
                    byte modrm = code[pos];
                    mod = (modrm >> 6) & 3;
                    reg = ((modrm >> 3) & 7) | (rexR << 3);
                    rm = (modrm & 7) | (rexB << 3);
                    pos++;
                    modrmStr = $"r{mod}/r{reg}/r{rm}";
                }
            }

            // Parse SIB
            string sibStr = "";
            bool hasSib = false;
            if (hasModRM && mod != 3 && rm == 4 && pos < code.Length)
            {
                hasSib = true;
                byte sib = code[pos++];
                int scale = (sib >> 6) & 3;
                int index = ((sib >> 3) & 7) | (rexX << 3);
                int sibBase = (sib & 7) | (rexB << 3);
                sibStr = $"s={scale} i={index} b={sibBase}";
            }

            // Parse displacement
            string dispStr = "";
            int dispSize = 0;
            long disp = 0;
            bool hasDisp = false;

            if (hasModRM)
            {
                if (mod == 0 && rm == 5)
                {
                    dispSize = 4; // RIP-relative disp32
                    hasDisp = true;
                }
                else if (mod == 1)
                {
                    dispSize = 1;
                    hasDisp = true;
                }
                else if (mod == 2)
                {
                    dispSize = 4;
                    hasDisp = true;
                }
                else if (mod == 0 && hasSib)
                {
                    // SIB with base==5 means disp32 — check if SIB base field is 5
                    // SIB byte is at opcodeOffset + 2 (opcode + modrm)
                    int sibOffset = opcodeOffset + 2;
                    if (sibOffset < code.Length && (code[sibOffset] & 7) == 5)
                    {
                        dispSize = 4;
                        hasDisp = true;
                    }
                }
            }

            if (hasDisp && pos + dispSize <= code.Length)
            {
                if (dispSize == 1)
                    disp = (sbyte)code[pos];
                else if (dispSize == 4)
                    disp = code[pos] | (code[pos + 1] << 8) | (code[pos + 2] << 16) | (code[pos + 3] << 24);
                pos += dispSize;
            }

            // Parse immediate
            long imm = 0;
            int immSize = 0;
            bool hasImm = false;

            // Determine immediate size based on opcode
            if (isTwoByte)
            {
                // Two-byte opcodes with immediate
                if ((op2 >= 0x80 && op2 <= 0x8F) || op2 == 0xA3)
                {
                    immSize = 4; // jcc rel32
                }
                else if (op2 == 0xC4 || op2 == 0xC5)
                {
                    immSize = 1; // pinsrw/pextrw imm8
                }
            }
            else
            {
                if (opcode == 0xE8 || opcode == 0xE9 || (opcode == 0xC7 && reg == 0))
                {
                    immSize = 4;
                }
                else if (opcode == 0xEB || (opcode >= 0x70 && opcode <= 0x7F))
                {
                    immSize = 1;
                }
                else if (opcode is 0x69 or 0x68 or 0xA9 or 0x05 or 0x0D or 0x15 or 0x1D or 0x25 or 0x2D or 0x35 or 0x3D)
                {
                    immSize = 4;
                }
                else if (opcode is 0x6B or 0x6A or 0xA8 or 0x04 or 0x0C or 0x14 or 0x1C or 0x24 or 0x2C or 0x34 or 0x3C)
                {
                    immSize = 1;
                }
                else if (opcode is 0xC0 or 0xC1 or 0xD0 or 0xD1 or 0xD2 or 0xD3)
                {
                    immSize = (opcode is 0xC0 or 0xC1 or 0xD1 or 0xD3) ? 1 : 0;
                }
                else if (opcode is 0x80 or 0x82 or 0x83)
                {
                    immSize = 1;
                }
                else if (opcode == 0x81)
                {
                    immSize = 4;
                }
                else if ((opcode == 0x84 || opcode == 0x85) && hasModRM)
                {
                    immSize = 0;
                }
                else if (opcode == 0xF6 && reg <= 1)
                {
                    immSize = 1;
                }
                else if (opcode == 0xF7 && reg <= 1)
                {
                    immSize = 4;
                }
                else if (opcode >= 0xB0 && opcode <= 0xB7)
                {
                    immSize = 1;
                }
                else if (opcode >= 0xB8 && opcode <= 0xBF)
                {
                    immSize = rexW ? 8 : 4;
                }
            }

            if (immSize > 0 && pos + immSize <= code.Length)
            {
                if (immSize == 1)
                    imm = (sbyte)code[pos];
                else if (immSize == 2)
                    imm = (short)(code[pos] | (code[pos + 1] << 8));
                else if (immSize == 4)
                    imm = code[pos] | (code[pos + 1] << 8) | (code[pos + 2] << 16) | (code[pos + 3] << 24);
                else if (immSize == 8)
                    imm = code[pos] | ((long)code[pos + 1] << 8) | ((long)code[pos + 2] << 16) | ((long)code[pos + 3] << 24)
                        | ((long)code[pos + 4] << 32) | ((long)code[pos + 5] << 40) | ((long)code[pos + 6] << 48) | ((long)code[pos + 7] << 56);
                pos += immSize;
                hasImm = true;
            }

            // Build operand text
            // For group opcodes, adjust mnemonic based on ModRM.reg
            string effectiveMnemonic = mnemonic;
            if (opcode == 0xFF && hasModRM)
            {
                effectiveMnemonic = reg switch
                {
                    0 => "inc",  // grp5
                    1 => "dec",
                    2 => "call",
                    3 => "callf",
                    4 => "jmp",
                    5 => "jmpf",
                    6 => "push",
                    _ => "grp5"
                };
            }
            else if ((opcode == 0xF6 || opcode == 0xF7) && hasModRM)
            {
                effectiveMnemonic = reg switch
                {
                    0 => "test",
                    1 => "test",
                    2 => "not",
                    3 => "neg",
                    4 => "mul",
                    5 => "imul",
                    6 => "div",
                    7 => "idiv",
                    _ => "grp3"
                };
            }
            else if ((opcode is 0x80 or 0x81 or 0x82 or 0x83) && hasModRM)
            {
                effectiveMnemonic = reg switch
                {
                    0 => "add",
                    1 => "or",
                    2 => "adc",
                    3 => "sbb",
                    4 => "and",
                    5 => "sub",
                    6 => "xor",
                    7 => "cmp",
                    _ => "grp1"
                };
            }
            else if ((opcode is 0xC0 or 0xC1 or 0xD0 or 0xD1 or 0xD2 or 0xD3) && hasModRM)
            {
                effectiveMnemonic = reg switch
                {
                    0 => "rol",
                    1 => "ror",
                    2 => "rcl",
                    3 => "rcr",
                    4 => "shl",
                    5 => "shr",
                    6 => "sal",
                    7 => "sar",
                    _ => "grp2"
                };
            }
            else if (opcode == 0xFE && hasModRM)
            {
                effectiveMnemonic = reg switch
                {
                    0 => "inc",
                    1 => "dec",
                    _ => "grp4"
                };
            }

            string operands = BuildOperandText(effectiveMnemonic, opcode, isTwoByte, op2, mod, reg, rm, disp, imm, has66, rexW, rexB, immSize, hasDisp, hasImm, baseAddress, startPos, code);

            int totalLen = pos - startPos;
            byte[] raw = new byte[totalLen];
            Array.Copy(code, startPos, raw, 0, totalLen);

            instructions.Add(new DecodedInstruction(
                baseAddress + startPos, raw, effectiveMnemonic, operands));

            if (opcode == 0xC3 || opcode == 0xCB || (opcode == 0xC2))
                break; // ret
        }

        return instructions;
    }

    private static bool NeedsModRM(byte opcode, bool twoByte, byte op2)
    {
        if (twoByte)
        {
            // Most 2-byte opcodes use ModRM except control instructions
            return op2 is not (0x01 or 0x06 or 0x08 or 0x09 or 0x0B or
                0x30 or 0x31 or 0x32 or 0x33 or 0x34 or 0x35 or 0x37 or
                0x77 or 0xA0 or 0xA1 or 0xA2 or 0xA6 or 0xA7 or 0xA8 or 0xA9 or 0xAA or
                0xB8 or 0xB9 or 0xC8 or 0xC9 or 0xCA or 0xCB or 0xCC or 0xCD or 0xCE or 0xCF);
        }

        return HasModRM(opcode);
    }

    private static string BuildOperandText(
        string mnemonic, byte opcode, bool twoByte, byte op2,
        int mod, int reg, int rm, long disp, long imm,
        bool has66, bool rexW, int rexB, int immSize, bool hasDisp, bool hasImm,
        IntPtr baseAddress, int startPos, byte[] code)
    {
        var parts = new List<string>();

        // Compute the effective operand size (32-bit vs 64-bit)
        bool is64Bit = rexW || mnemonic is "push" or "pop" or "call" or "ret" or "retn" or "retf" or "pushfq" or "popfq";
        bool is32Bit = !is64Bit && !has66;
        bool is16Bit = has66 && !rexW;

        if (twoByte)
        {
            // Handle conditional jumps (0x80-0x8F)
            if (op2 >= 0x80 && op2 <= 0x8F && immSize == 4)
            {
                long target = imm + baseAddress.ToInt64() + (startPos + 6);
                parts.Add($"0x{target:x}");
                return string.Join(", ", parts);
            }
        }

        // Handle call/jmp with rel32
        if (opcode == 0xE8 || opcode == 0xE9)
        {
            long target = imm + baseAddress.ToInt64() + startPos + 5;
            parts.Add($"0x{target:x}");
            return string.Join(", ", parts);
        }

        // Handle jmp/jcc rel8
        if (opcode == 0xEB || (opcode >= 0x70 && opcode <= 0x7F))
        {
            long target = imm + baseAddress.ToInt64() + startPos + 2;
            parts.Add($"0x{target:x}");
            return string.Join(", ", parts);
        }

        // Handle push/pop reg
        if (opcode >= 0x50 && opcode <= 0x5F)
        {
            int regIdx = (opcode - 0x50) | (opcode >= 0x58 ? 0 : 0);
            parts.Add(Reg64[opcode - 0x50]);
            return string.Join(", ", parts);
        }

        // Non-ModRM instructions
        if (opcode == 0xC3 || opcode == 0xCB || opcode == 0xC2 || opcode == 0xCC)
            return "";

        if (opcode == 0x90) return ""; // nop

        // ret imm16
        if (opcode == 0xC2 && immSize == 2)
        {
            parts.Add($"0x{imm:x}");
            return string.Join(", ", parts);
        }

        // mov r8/16/32/64, imm
        if (opcode >= 0xB0 && opcode <= 0xBF)
        {
            int regIdx = (opcode - 0xB0) | (rexB << 3);
            if (opcode <= 0xB7)
            {
                parts.Add(Reg8[regIdx]);
                parts.Add($"0x{imm:x}");
            }
            else
            {
                parts.Add(is64Bit ? Reg64[regIdx] : Reg32[regIdx]);
                parts.Add(immSize == 8 ? $"0x{imm:x}" : $"0x{imm:x}");
            }
            return string.Join(", ", parts);
        }

        if (!HasModRM(opcode) && !twoByte)
            return hasImm ? $"0x{imm:x}" : "";

        // ModRM-based operand construction
        string dst = "";
        string src = "";

        // Determine direction from opcode
        bool isMove = mnemonic is "mov" or "movzx" or "movsx" or "movsxd" or "xchg";
        bool isTest = mnemonic is "test";
        bool isCmp = mnemonic is "cmp";
        bool isLea = mnemonic is "lea";

        var rmStr = FormatRM(mod, rm, disp, is64Bit, has66, rexW, baseAddress, startPos, code);
        var regStr = FormatReg(reg, is64Bit, has66, rexW);

        if (isLea)
        {
            parts.Add(regStr);
            parts.Add(rmStr);
        }
        else if (isMove)
        {
            // For mov, mod=3 means reg-reg; direction depends on opcode
            // Opcodes 0x88, 0x8A, 0x8C: dst is r/m, src is reg
            // Opcodes 0x89, 0x8B, 0x8E: dst is reg, src is r/m
            if (opcode == 0x8B || opcode == 0x8D || opcode == 0x8E)
            {
                parts.Add(regStr);
                parts.Add(rmStr);
            }
            else if (opcode == 0x89 || opcode == 0x88 || opcode == 0x8C)
            {
                parts.Add(rmStr);
                parts.Add(regStr);
            }
            else
            {
                parts.Add(rmStr);
                parts.Add(regStr);
            }
        }
        else if (isTest || isCmp)
        {
            // cmp/test: dst is r/m, src is reg/imm
            if (hasImm)
            {
                parts.Add(rmStr);
                parts.Add($"0x{imm:x}");
            }
            else if (mod == 3)
            {
                parts.Add(rmStr);
                parts.Add(regStr);
            }
            else
            {
                parts.Add(rmStr);
                parts.Add(regStr);
            }
        }
        else if (mnemonic is "push" or "pop" or "call" or "jmp" or "inc" or "dec" or "not" or "neg")
        {
            // Instructions with only 1 operand (r/m), or no register operand
            if (mod == 3)
                parts.Add(regStr);
            else
                parts.Add(rmStr);
        }
        else if (mnemonic is "imul" or "and" or "or" or "xor" or "add" or "sub" or "adc" or "sbb")
        {
            // Arithmetic: dst is r/m, src is reg/imm
            if (hasImm)
            {
                parts.Add(rmStr);
                parts.Add($"0x{imm:x}");
            }
            else if (mod == 3)
            {
                parts.Add(rmStr);
                parts.Add(regStr);
            }
            else
            {
                parts.Add(rmStr);
                parts.Add(regStr);
            }
        }
        else
        {
            // Default: show what we have
            if (mod == 3)
            {
                parts.Add(rmStr);
                if (reg != 0 || hasImm)
                    parts.Add(hasImm ? $"0x{imm:x}" : regStr);
            }
            else
            {
                parts.Add(rmStr);
                if (hasImm)
                    parts.Add($"0x{imm:x}");
                else if (reg != 0)
                    parts.Add(regStr);
            }
        }

        return string.Join(", ", parts);
    }

    private static string FormatRM(int mod, int rm, long disp, bool is64Bit, bool has66, bool rexW, IntPtr baseAddress, int startPos, byte[] code)
    {
        if (mod == 3)
            return FormatReg(rm, is64Bit, has66, rexW);

        string baseReg = FormatReg(rm, true, false, true);
        string result;

        if (disp == 0 && mod == 0 && rm != 5)
            result = $"[{baseReg}]";
        else if (disp == 0 && rm == 5)
            result = $"[rip+0x0]";
        else if (mod == 1)
            result = $"[{baseReg}+0x{disp:x}]";
        else if (mod == 2 || (mod == 0 && rm == 5))
        {
            if (rm == 5)
            {
                long target = disp + baseAddress.ToInt64() + startPos;
                result = $"[rip+0x{disp:x}]";
            }
            else
                result = $"[{baseReg}+0x{disp:x}]";
        }
        else
            result = $"[{baseReg}+{disp}]";

        return result;
    }

    private static string FormatReg(int reg, bool is64Bit, bool has66, bool rexW)
    {
        if (is64Bit || rexW)
            return Reg64[reg % Reg64.Length];
        if (has66)
        {
            // 16-bit register
            string[] reg16 = { "ax", "cx", "dx", "bx", "sp", "bp", "si", "di",
                               "r8w", "r9w", "r10w", "r11w", "r12w", "r13w", "r14w", "r15w" };
            return reg16[reg % reg16.Length];
        }
        return Reg32[reg % Reg32.Length];
    }

    // ── Hex dump formatter ──────────────────────────────────────────────────

    private static string FormatHexDump(IntPtr baseAddress, byte[] code, IReadOnlyList<DecodedInstruction> instructions)
    {
        var sb = new StringBuilder();
        foreach (var inst in instructions)
        {
            bool hasInstData = inst.RawBytes.Length > 0;
            if (hasInstData)
            {
                string hex = string.Join(" ", inst.RawBytes.Select(b => $"{b:x2}"));
                string addr = $"0x{inst.Address.ToInt64():x}";
                sb.AppendLine($"  {addr,-20}{hex,-24}{inst.Mnemonic,-10}{inst.Operands}");
            }
        }
        return sb.ToString();
    }
}