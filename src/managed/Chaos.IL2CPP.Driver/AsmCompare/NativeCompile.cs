using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;

namespace Chaos.IL2CPP.Driver;

internal static class NativeCompile
{
    public sealed record NativeCompileResult
    {
        public bool CompileSuccess { get; init; }
        public bool FoundMsvc { get; init; }
        public string? ObjectPath { get; init; }
        public string? ObjectSize { get; init; }
        public string? Error { get; init; }
        public string? CompileOutput { get; init; }
        public string? Disassembly { get; init; }
        public string? MsvcVersion { get; init; }
        public byte[]? TextSectionBytes { get; init; }
        public string? TextSectionName { get; init; }
        public JitAsmCapture.JitCaptureResult? AotDisasm { get; init; }
    }

    public static NativeCompileResult Compile(string tempDir, string nativeSymbol)
    {
        try
        {
            var repoRoot = FindRepoRoot();
            if (repoRoot is null)
                return Fail("Cannot determine repository root");

            var (clExe, env) = FindMsvc();
            if (clExe is null || env is null)
                return new NativeCompileResult { FoundMsvc = false, Error = "MSVC not found." };

            var clDir = Path.GetDirectoryName(clExe)!;
            var msvcVersion = ExtractMsvcVersion(clDir);

            // Find the generated file in the subdirectory
            var genDir = Path.Combine(tempDir, "generated");
            var genCppFiles = Directory.Exists(genDir)
                ? Directory.GetFiles(genDir, "*.cpp", SearchOption.TopDirectoryOnly)
                : [];
            if (genCppFiles.Length == 0)
                return Fail("No generated .cpp files in tempDir/generated/");

            var cppPath = genCppFiles.First();
            var objDir = Path.Combine(tempDir, "obj");
            Directory.CreateDirectory(objDir);
            var objPath = Path.Combine(objDir, Path.GetFileNameWithoutExtension(cppPath) + ".obj");

            // Find include dirs
            var includeDirs = GetIncludeDirs(repoRoot);
            var existingIncludes = includeDirs.Where(Directory.Exists).ToList();
            var includeFlags = string.Join(" ", existingIncludes.Select(d => $"-I\"{d}\""));

            // Add the generated headers dir and tempDir for any extra headers
            if (Directory.Exists(genDir))
                includeFlags += $" -I\"{genDir}\"";

            // Also add any subdirectories that contain headers
            var headerDirs = Directory.GetDirectories(tempDir, "*", SearchOption.AllDirectories)
                .Where(d => Directory.GetFiles(d, "*.h").Length > 0)
                .ToList();
            foreach (var d in headerDirs)
                includeFlags += $" -I\"{d}\"";

            // Compile
            var compileCmd = $"\"{clExe}\" /nologo /std:c++20 /c /EHsc /W3 /utf-8 " +
                             $"{includeFlags} " +
                             "-DCHAOS_IL2CPP_CONFIG_CHECK -DCHAOS_IL2CPP_TRACE_ENABLED " +
                             "-D_CRT_SECURE_NO_WARNINGS " +
                             $"-Fo\"{objPath}\" \"{cppPath}\"";

            var (compileExit, compileOut, compileErr) = RunProcess(compileCmd, env, tempDir);
            var compileOutput = string.Join(Environment.NewLine,
                new[] { compileOut, compileErr }.Where(s => !string.IsNullOrWhiteSpace(s)));

            if (compileExit != 0)
            {
                return new NativeCompileResult
                {
                    FoundMsvc = true,
                    CompileSuccess = false,
                    MsvcVersion = msvcVersion,
                    CompileOutput = Truncate(compileOutput, 4000),
                    Error = "Compilation failed.",
                };
            }

            var objSize = new FileInfo(objPath).Length;

            // Extract function bytes from .obj (COFF symbol table)
            byte[]? textSectionBytes = null;
            string? textSectionName = null;
            JitAsmCapture.JitCaptureResult? aotDisasm = null;
            try
            {
                // Prefer function-level extraction via COFF symbol table
                (textSectionBytes, textSectionName) = ExtractFunctionBytes(objPath, nativeSymbol);
                if (textSectionBytes is null)
                {
                    // Fallback to whole-section extraction
                    (textSectionBytes, textSectionName) = ExtractTextSection(objPath);
                }

                if (textSectionBytes is not null)
                {
                    aotDisasm = JitAsmCapture.DisassembleRaw(
                        textSectionBytes,
                        0x10000000,
                        nativeSymbol);
                }
            }
            catch { /* non-critical */ }

            // Run dumpbin for disassembly
            string? disasm = null;
            var dumpbinExe = Path.Combine(clDir, "dumpbin.exe");
            if (File.Exists(dumpbinExe))
            {
                var dumpbinCmd = $"\"{dumpbinExe}\" /DISASM /SYMBOLS \"{objPath}\"";
                var (_, dumpOut, dumpErr) = RunProcess(dumpbinCmd, env, tempDir);
                disasm = ExtractFunctionDisassembly(dumpOut + dumpErr, nativeSymbol);
            }

            return new NativeCompileResult
            {
                FoundMsvc = true,
                CompileSuccess = true,
                MsvcVersion = msvcVersion,
                ObjectPath = objPath,
                ObjectSize = FormatSize(objSize),
                CompileOutput = Truncate(compileOutput, 2000),
                Disassembly = disasm,
                TextSectionBytes = textSectionBytes,
                TextSectionName = textSectionName,
                AotDisasm = aotDisasm,
            };
        }
        catch (Exception ex)
        {
            return Fail($"Native compile failed: {ex.Message}");
        }
    }

    private static string? FindRepoRoot()
    {
        var dir = new DirectoryInfo(Directory.GetCurrentDirectory());
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir.FullName, "CLAUDE.md")) &&
                Directory.Exists(Path.Combine(dir.FullName, "src", "native")))
                return dir.FullName;
            dir = dir.Parent;
        }
        return null;
    }

    private static (string? clExe, Dictionary<string, string>? env) FindMsvc()
    {
        var candidates = new[]
        {
            @"C:\Program Files\Microsoft Visual Studio\2022\Professional",
            @"C:\Program Files\Microsoft Visual Studio\2022\Community",
            @"C:\Program Files\Microsoft Visual Studio\2022\Enterprise",
            @"C:\Program Files (x86)\Microsoft Visual Studio\2022\Professional",
        };

        string? vcvars = null;
        foreach (var baseDir in candidates)
        {
            var c = Path.Combine(baseDir, @"VC\Auxiliary\Build\vcvarsall.bat");
            if (File.Exists(c)) { vcvars = c; break; }
        }

        if (vcvars is null) return (null, null);

        try
        {
            var psi = new ProcessStartInfo
            {
                FileName = "cmd.exe",
                Arguments = $"/c \"\"{vcvars}\" x64 >nul 2>&1 && set\"",
                UseShellExecute = false,
                RedirectStandardOutput = true,
                CreateNoWindow = true,
            };
            using var proc = Process.Start(psi);
            if (proc is null) return (null, null);
            var stdout = proc.StandardOutput.ReadToEnd();
            proc.WaitForExit(30000);

            var env = new Dictionary<string, string>(StringComparer.OrdinalIgnoreCase);
            foreach (var line in stdout.Split('\n', StringSplitOptions.RemoveEmptyEntries))
            {
                var eq = line.IndexOf('=');
                if (eq > 0) env[line[..eq].Trim()] = line[(eq + 1)..].Trim();
            }

            if (env.TryGetValue("PATH", out var path))
                foreach (var dir in path.Split(';', StringSplitOptions.RemoveEmptyEntries))
                {
                    var cl = Path.Combine(dir.Trim(), "cl.exe");
                    if (File.Exists(cl)) return (cl, env);
                }

            return (null, env);
        }
        catch { return (null, null); }
    }

    private static string? ExtractMsvcVersion(string clDir)
    {
        var parts = clDir.Split(Path.DirectorySeparatorChar, Path.AltDirectorySeparatorChar);
        for (int i = 0; i < parts.Length - 1; i++)
            if (parts[i].Equals("MSVC", StringComparison.OrdinalIgnoreCase) && i + 1 < parts.Length)
                return parts[i + 1];
        return null;
    }

    private static List<string> GetIncludeDirs(string repoRoot) => new()
    {
        Path.Combine(repoRoot, "src", "native", "common"),
        Path.Combine(repoRoot, "src", "native", "runtime-core"),
        Path.Combine(repoRoot, "src", "native", "runtime-core", "gc"),
        Path.Combine(repoRoot, "src", "native", "bootstrap"),
        Path.Combine(repoRoot, "src", "native", "interpreter"),
        Path.Combine(repoRoot, "src", "native", "interpreter", "generated"),
        Path.Combine(repoRoot, "src", "native", "support"),
        Path.Combine(repoRoot, "src", "native", "hot-update"),
        Path.Combine(repoRoot, "src", "native"),
        Path.Combine(repoRoot, "contracts", "native", "v0"),
        Path.Combine(repoRoot, "third_party", "fmt", "include"),
    };

    private static (int exitCode, string stdout, string stderr) RunProcess(
        string command, Dictionary<string, string> env, string workDir)
    {
        string filename, arguments;
        if (command.StartsWith('"'))
        {
            var end = command.IndexOf('"', 1);
            if (end > 0) { filename = command[1..end]; arguments = command[(end + 1)..].TrimStart(); }
            else { filename = command.Trim('"'); arguments = ""; }
        }
        else
        {
            var space = command.IndexOf(' ');
            if (space > 0) { filename = command[..space]; arguments = command[(space + 1)..]; }
            else { filename = command; arguments = ""; }
        }

        var psi = new ProcessStartInfo
        {
            FileName = filename, Arguments = arguments,
            UseShellExecute = false, RedirectStandardOutput = true,
            RedirectStandardError = true, CreateNoWindow = true,
            WorkingDirectory = workDir,
        };
        foreach (var kv in env) psi.EnvironmentVariables[kv.Key] = kv.Value;

        using var proc = Process.Start(psi);
        if (proc is null) return (-1, "", "Failed to start process");
        var stdout = proc.StandardOutput.ReadToEnd();
        var stderr = proc.StandardError.ReadToEnd();
        proc.WaitForExit(120000);
        return (proc.ExitCode, stdout, stderr);
    }

    private static string? ExtractFunctionDisassembly(string dumpOutput, string nativeSymbol)
    {
        if (string.IsNullOrEmpty(dumpOutput)) return null;

        var lines = dumpOutput.Split('\n', StringSplitOptions.None);
        var result = new StringBuilder();
        bool inCodeSection = false;
        bool inSymbolTable = false;

        foreach (var line in lines)
        {
            var trimmed = line.TrimEnd();

            // Determine what section we're in
            if (trimmed.StartsWith("SECTION HEADER"))
            {
                inCodeSection = trimmed.Contains("TEXT") || trimmed.Contains("code");
                inSymbolTable = false;
                continue;
            }

            if (trimmed.StartsWith("COFF SYMBOL TABLE") || trimmed.Contains("Summary"))
            {
                inSymbolTable = true;
                inCodeSection = false;
                continue;
            }

            if (inSymbolTable && !string.IsNullOrEmpty(nativeSymbol))
            {
                // Capture function symbol references from symbol table
                if (trimmed.IndexOf(nativeSymbol, StringComparison.OrdinalIgnoreCase) >= 0)
                {
                    result.AppendLine($"  ; --- SYMBOL: {trimmed} ---");
                }
                continue;
            }

            if (inCodeSection)
            {
                // COFF disassembly lines: "  0000000000000000: XX XX XX  mnemonic  operands"
                if (trimmed.Length > 0 && (char.IsDigit(trimmed[0]) || trimmed[0] == ' '))
                {
                    result.AppendLine($"  {trimmed}");
                }
                else if (trimmed.Length == 0 && result.Length > 0)
                {
                    // Empty line within code section
                }
                else if (result.Length > 0 && trimmed.StartsWith("  "))
                {
                    result.AppendLine($"  {trimmed}");
                }
                else if (result.Length > 0 && trimmed.Length > 0)
                {
                    // Non-matching line after we've collected data — stop
                    break;
                }

                if (result.Length > 15000) break;
            }
        }

        var output = result.ToString().TrimEnd();
        return output.Length > 0 ? output : null;
    }

    private static (byte[]? bytes, string? name) ExtractTextSection(string objPath)
    {
        using var fs = new FileStream(objPath, FileMode.Open, FileAccess.Read);
        using var br = new BinaryReader(fs);

        var header = br.ReadBytes(20);
        if (header.Length < 20) return (null, null);
        int numSections = BitConverter.ToUInt16(header, 2);
        int optsSize = BitConverter.ToUInt16(header, 16);
        int symTableOffset = BitConverter.ToInt32(header, 8);
        int numSymbols = BitConverter.ToInt32(header, 12);

        br.BaseStream.Seek(20 + optsSize, SeekOrigin.Begin);

        // Read section headers
        var sections = new List<(string name, int rawPtr, int rawSize)>();
        for (int i = 0; i < numSections; i++)
        {
            var sec = br.ReadBytes(40);
            if (sec.Length < 40) break;

            var nameBytes = sec[..8];
            var name = Encoding.ASCII.GetString(nameBytes).TrimEnd('\0');

            int rawSize = BitConverter.ToInt32(sec, 16);
            int rawPtr = BitConverter.ToInt32(sec, 20);

            sections.Add((name, rawPtr, rawSize));
        }

        // Collect all .text* subsections (MSVC uses function-level linking,
        // so code is in .text$mn, exception in .text$x, unwind in .text$unwind, etc.)
        var codeSections = sections
            .Where(s => s.name.StartsWith(".text", StringComparison.OrdinalIgnoreCase) &&
                        s.rawSize > 0 && s.rawPtr > 0 &&
                        (!s.name.Contains('$') || s.name.Contains("$mn") || s.name.Contains("$di")))
            .OrderBy(s => s.rawPtr)
            .ToList();

        if (codeSections.Count == 0) return (null, null);

        // Concatenate all code sections into one blob
        var allBytes = new List<byte>();
        var usedNames = new List<string>();
        foreach (var (name, rawPtr, rawSize) in codeSections)
        {
            br.BaseStream.Seek(rawPtr, SeekOrigin.Begin);
            allBytes.AddRange(br.ReadBytes(rawSize));
            usedNames.Add(name);
        }

        var combinedName = string.Join("+", usedNames);
        return (allBytes.ToArray(), combinedName);
    }

    /// <summary>
    /// Extract only the bytes for a specific function from the .obj file,
    /// using the COFF symbol table to locate the function's offset and size.
    /// Returns (bytes, sectionName) or (null, null) if not found.
    /// </summary>
    private static (byte[]? bytes, string? sectionName) ExtractFunctionBytes(
        string objPath, string nativeSymbol)
    {
        using var fs = new FileStream(objPath, FileMode.Open, FileAccess.Read);
        using var br = new BinaryReader(fs);

        var header = br.ReadBytes(20);
        if (header.Length < 20) return (null, null);
        int numSections = BitConverter.ToUInt16(header, 2);
        int optsSize = BitConverter.ToUInt16(header, 16);
        int symTableOffset = BitConverter.ToInt32(header, 8);
        int numSymbols = BitConverter.ToInt32(header, 12);

        // Read section headers to map section index → raw pointer
        br.BaseStream.Seek(20 + optsSize, SeekOrigin.Begin);
        var sectionHeaders = new List<(string name, int rawPtr, int rawSize, int virtSize)>();
        for (int i = 0; i < numSections; i++)
        {
            var sec = br.ReadBytes(40);
            if (sec.Length < 40) break;
            var nameBytes = sec[..8];
            var name = Encoding.ASCII.GetString(nameBytes).TrimEnd('\0');
            int virtSize = BitConverter.ToInt32(sec, 8);
            int rawSize = BitConverter.ToInt32(sec, 16);
            int rawPtr = BitConverter.ToInt32(sec, 20);
            sectionHeaders.Add((name, rawPtr, rawSize, virtSize));
        }

        if (symTableOffset == 0 || numSymbols == 0) return (null, null);

        // Read COFF symbol table
        br.BaseStream.Seek(symTableOffset, SeekOrigin.Begin);

        // COFF symbol: 18 bytes each
        // [0..7] Name (8 bytes, or first 4 bytes = offset into string table if 0x00000000)
        // [8..11] Value (4 bytes) = offset within section
        // [12..13] SectionNumber (2 bytes) = 1-based section index
        // [14..15] Type (2 bytes)
        // [16] StorageClass (1 byte)
        // [17] NumberOfAuxSymbols (1 byte)

        int? targetSection = null;
        int targetOffset = 0;
        int stringTableOffset = symTableOffset + numSymbols * 18;

        // First pass: find the target function symbol
        long currentOffset = symTableOffset;
        int symIndex = 0;
        while (symIndex < numSymbols)
        {
            br.BaseStream.Seek(currentOffset, SeekOrigin.Begin);
            var symBytes = br.ReadBytes(18);
            if (symBytes.Length < 18) break;

            var nameField = symBytes[..8];
            int value = BitConverter.ToInt32(symBytes, 8);
            short sectionNumber = BitConverter.ToInt16(symBytes, 12);
            byte auxCount = symBytes[17];

            string symName;
            // Check if name is in string table (first 4 bytes are zero)
            if (nameField[0] == 0 && nameField[1] == 0 && nameField[2] == 0 && nameField[3] == 0)
            {
                int strOffset = BitConverter.ToInt32(nameField, 4);
                var savedPos = br.BaseStream.Position;
                br.BaseStream.Seek(stringTableOffset + strOffset, SeekOrigin.Begin);
                var nameBytes = new List<byte>();
                int b;
                while ((b = br.ReadByte()) != 0) nameBytes.Add((byte)b);
                symName = Encoding.UTF8.GetString(nameBytes.ToArray());
                br.BaseStream.Seek(savedPos, SeekOrigin.Begin);
            }
            else
            {
                symName = Encoding.ASCII.GetString(nameField).TrimEnd('\0');
            }

            if (sectionNumber > 0 && value > 0 &&
                symName.Equals(nativeSymbol, StringComparison.OrdinalIgnoreCase))
            {
                targetSection = sectionNumber;
                targetOffset = value;
                // Scan forward to find the next symbol in the same section to determine size
                int nextOffset = value;
                long scanPos = currentOffset + 18 + auxCount * 18;
                var scanSaved = br.BaseStream.Position;
                while (scanPos < symTableOffset + numSymbols * 18)
                {
                    br.BaseStream.Seek(scanPos, SeekOrigin.Begin);
                    var scanSym = br.ReadBytes(18);
                    if (scanSym.Length < 18) break;
                    short scanSection = BitConverter.ToInt16(scanSym, 12);
                    int scanValue = BitConverter.ToInt32(scanSym, 8);
                    byte scanAux = scanSym[17];

                    if (scanSection == targetSection && scanValue > nextOffset)
                    {
                        nextOffset = scanValue;
                        break;
                    }
                    scanPos += 18 + scanAux * 18;
                }
                br.BaseStream.Seek(scanSaved, SeekOrigin.Begin);

                // Map section index (1-based) to section header
                int secIdx = targetSection.Value - 1;
                if (secIdx >= 0 && secIdx < sectionHeaders.Count)
                {
                    var (secName, rawPtr, rawSize, virtSize) = sectionHeaders[secIdx];
                    int funcSize = nextOffset - targetOffset;
                    if (funcSize <= 0 || funcSize > 4096) funcSize = Math.Min(virtSize - targetOffset, 512);

                    // Read function bytes from the raw section data
                    br.BaseStream.Seek(rawPtr + targetOffset, SeekOrigin.Begin);
                    var funcBytes = br.ReadBytes(Math.Min(funcSize, 4096));
                    return (funcBytes, secName);
                }
                break;
            }

            currentOffset += 18 + auxCount * 18;
            symIndex += 1 + auxCount;
        }

        return (null, null);
    }

    private static string FormatSize(long bytes)
    {
        if (bytes < 1024) return $"{bytes} B";
        if (bytes < 1024 * 1024) return $"{bytes / 1024.0:F1} KB";
        return $"{bytes / (1024.0 * 1024.0):F1} MB";
    }

    private static string? Truncate(string? value, int maxLen)
    {
        if (value is null || value.Length <= maxLen) return value;
        return value[..maxLen] + $"\n... (truncated)";
    }

    private static NativeCompileResult Fail(string message) => new() { Error = message };
}