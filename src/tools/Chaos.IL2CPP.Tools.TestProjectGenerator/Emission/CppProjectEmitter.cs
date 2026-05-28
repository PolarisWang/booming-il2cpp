using System.Text.Json;
using System.Text.Json.Serialization;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Codegen;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Templating;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Emission;

public sealed class CppProjectEmitter
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
        DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull,
    };

    private static readonly string[] AllTemplateNames =
    [
        "TestProject.Entry.cpp.scriban",
        "TestProject.Entry.h.scriban",
        "TestProject.Dispatch.cpp.scriban",
        "TestProject.CMakeLists.txt.scriban",
        "TestProject.CMakePresets.json.scriban",
        "TestProject.RuntimePatchdata.cpp.scriban",
        "TestProject.chaos-config.cmake.scriban",
        "TestProject.chaos-targets.cmake.scriban",
        "TestProject.metadata.json.scriban",
    ];

    /// <summary>
    /// Emit only dispatch.cpp and metadata (no codegen required).
    /// Used by the verification pipeline to generate verification_dispatch.generated.cpp
    /// after entry.exe has been built with sentinel dispatch.
    /// </summary>
    public void EmitDispatchOnly(
        string outputDir,
        IReadOnlyList<SubjectModel> subjects,
        bool isJit = false,
        string configTier = "check",
        bool isWindows = true)
    {
        Directory.CreateDirectory(outputDir);

        var model = new ProjectModelBuilder().Build(
            subjects: subjects,
            isJit: isJit,
            configTier: configTier,
            isWindows: isWindows,
            hasPatchData: false,
            patchDataSize: 0,
            patchDataHostClass: "",
            projectName: "entry");

        // verification_dispatch.generated.cpp
        var dispatchCode = TemplateCatalog.Render("TestProject.Dispatch.cpp.scriban", model);
        File.WriteAllText(Path.Combine(outputDir, "verification_dispatch.generated.cpp"), dispatchCode);

        // metadata/subjects.json
        GenerateMetadataJson(outputDir, subjects);
    }

    /// <summary>
    /// Full project emit: copies SDK + codegen outputs, generates all source files
    /// via Scriban templates, writes CMake build system and metadata.
    /// </summary>
    public void Emit(
        string outputDir,
        CodegenResult codegen,
        IReadOnlyList<SubjectModel> subjects,
        bool isJit = false,
        string configTier = "check",
        bool isWindows = true)
    {
        // ── 1. Copy SDK files (everything except generated/) to chaos-sdk/ ──
        var sdkDst = Path.Combine(outputDir, "chaos-sdk");
        Directory.CreateDirectory(sdkDst);
        foreach (var entry in Directory.GetFileSystemEntries(codegen.OutputDir))
        {
            var name = Path.GetFileName(entry);
            if (name == "generated") continue;
            var dest = Path.Combine(sdkDst, name);
            if (Directory.Exists(entry))
                CopyDirectory(entry, dest);
            else
                File.Copy(entry, dest, overwrite: true);
        }

        // ── 2. Copy codegen generated files to subjects/ ──
        var subjectsDir = Path.Combine(outputDir, "subjects");
        Directory.CreateDirectory(subjectsDir);
        foreach (var genDir in codegen.GeneratedDirs)
        {
            foreach (var file in Directory.GetFiles(genDir, "*.cpp"))
                File.Copy(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
            foreach (var file in Directory.GetFiles(genDir, "*.h"))
                File.Copy(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
        }

        // ── 3. Build Scriban model ──
        var model = new ProjectModelBuilder().Build(
            subjects: subjects,
            isJit: isJit,
            configTier: configTier,
            isWindows: isWindows,
            hasPatchData: false,
            patchDataSize: 0,
            patchDataHostClass: "",
            projectName: "entry");

        // ── 4. Render templates → output files ──
        RenderToFile("TestProject.Entry.cpp.scriban", model, outputDir, "entry.cpp");
        RenderToFile("TestProject.Entry.h.scriban", model, outputDir, "entry.h");
        RenderToFile("TestProject.Dispatch.cpp.scriban", model, outputDir, "dispatch.cpp");
        RenderToFile("TestProject.CMakeLists.txt.scriban", model, outputDir, "CMakeLists.txt");
        RenderToFile("TestProject.CMakePresets.json.scriban", model, outputDir, "CMakePresets.json");
        RenderToFile("TestProject.RuntimePatchdata.cpp.scriban", model, outputDir, "runtime-patchdata.cpp");

        // chaos-sdk cmake files
        var cmakeDir = Path.Combine(sdkDst, "cmake");
        Directory.CreateDirectory(cmakeDir);
        RenderToFile("TestProject.chaos-config.cmake.scriban", model, cmakeDir, "chaos-config.cmake");
        RenderToFile("TestProject.chaos-targets.cmake.scriban", model, cmakeDir, "chaos-targets.cmake");

        // metadata/subjects.json
        GenerateMetadataJson(outputDir, subjects);
    }

    private static void RenderToFile(string templateName, Scriban.Runtime.ScriptObject model, string outputDir, string outputFileName)
    {
        var rendered = TemplateCatalog.Render(templateName, model);
        var path = Path.Combine(outputDir, outputFileName);
        Directory.CreateDirectory(Path.GetDirectoryName(path)!);
        File.WriteAllText(path, rendered);
    }

    private static void GenerateMetadataJson(string outputDir, IReadOnlyList<SubjectModel> subjects)
    {
        var metadataDir = Path.Combine(outputDir, "metadata");
        Directory.CreateDirectory(metadataDir);

        var entries = subjects.Select((s, i) => new
        {
            index = i,
            kind = s.Kind.ToString().ToLowerInvariant(),
            subjectId = s.SubjectId,
            assemblyName = s.AssemblyName,
            typeName = s.FullTypeName,
            methodName = s.MethodName,
            isStatic = s.IsStatic,
        }).ToList();

        var obj = new
        {
            schemaVersion = 1,
            generatedAt = DateTime.UtcNow.ToString("O"),
            generator = "Chaos.IL2CPP.Tools.TestProjectGenerator",
            entryExecutable = "entry",
            subjects = entries,
            summary = new
            {
                totalSubjects = subjects.Count,
                factCount = subjects.Count(s => s.Kind == SubjectKind.Fact),
                benchmarkCount = subjects.Count(s => s.Kind == SubjectKind.Benchmark),
                hotUpdateCount = subjects.Count(s => s.Kind == SubjectKind.HotUpdate),
            },
        };

        var json = JsonSerializer.Serialize(obj, JsonOptions);
        File.WriteAllText(Path.Combine(metadataDir, "subjects.json"), json);
    }

    private static void CopyDirectory(string sourceDir, string destDir)
    {
        Directory.CreateDirectory(destDir);
        foreach (var file in Directory.GetFiles(sourceDir, "*", SearchOption.AllDirectories))
        {
            var relative = Path.GetRelativePath(sourceDir, file);
            var dest = Path.Combine(destDir, relative);
            Directory.CreateDirectory(Path.GetDirectoryName(dest)!);
            File.Copy(file, dest, overwrite: true);
        }
    }
}
