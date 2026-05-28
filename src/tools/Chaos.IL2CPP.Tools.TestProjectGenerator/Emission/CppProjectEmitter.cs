using System.Text.Json;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Codegen;
using Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Emission;

public sealed class CppProjectEmitter
{
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        WriteIndented = true,
        PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    };

    public void EmitDispatchOnly(string outputDir, IReadOnlyList<SubjectModel> subjects)
    {
        Directory.CreateDirectory(outputDir);

        // 1. Generate dispatch.cpp
        var dispatchEmitter = new DispatchEmitter();
        var dispatchCode = dispatchEmitter.GenerateDispatch(subjects);
        File.WriteAllText(Path.Combine(outputDir, "verification_dispatch.generated.cpp"), dispatchCode);

        // 2. Generate metadata/subjects.json
        var metadataDir = Path.Combine(outputDir, "metadata");
        Directory.CreateDirectory(metadataDir);
        var metadataJson = GenerateSubjectsJson(subjects);
        File.WriteAllText(Path.Combine(metadataDir, "subjects.json"), metadataJson);
    }

    public void Emit(string outputDir, CodegenResult codegen, IReadOnlyList<SubjectModel> subjects)
    {
        var emitter = new EntryPointEmitter();
        var dispatchEmitter = new DispatchEmitter();

        // 1. Copy SDK files from codegen output (everything except generated/) to chaos-sdk/
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

        // 2. Copy codegen generated files to subjects/
        var subjectsDir = Path.Combine(outputDir, "subjects");
        Directory.CreateDirectory(subjectsDir);
        foreach (var genDir in codegen.GeneratedDirs)
        {
            foreach (var file in Directory.GetFiles(genDir, "*.cpp"))
                File.Copy(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
            foreach (var file in Directory.GetFiles(genDir, "*.h"))
                File.Copy(file, Path.Combine(subjectsDir, Path.GetFileName(file)), overwrite: true);
        }

        // 3. Generate entry.cpp
        var entryCode = emitter.GenerateEntryPoint(subjects);
        File.WriteAllText(Path.Combine(outputDir, "entry.cpp"), entryCode);

        // 4. Generate entry.h
        File.WriteAllText(Path.Combine(outputDir, "entry.h"), emitter.GenerateEntryHeader());

        // 5. Generate dispatch.cpp
        var dispatchCode = dispatchEmitter.GenerateDispatch(subjects);
        File.WriteAllText(Path.Combine(outputDir, "dispatch.cpp"), dispatchCode);

        // 6. Generate CMakeLists.txt
        var cmakeCode = GenerateCmake(outputDir);
        File.WriteAllText(Path.Combine(outputDir, "CMakeLists.txt"), cmakeCode);

        // 7. Generate metadata/subjects.json
        var metadataDir = Path.Combine(outputDir, "metadata");
        Directory.CreateDirectory(metadataDir);
        var metadataJson = GenerateSubjectsJson(subjects);
        File.WriteAllText(Path.Combine(metadataDir, "subjects.json"), metadataJson);

    }

    private static string GenerateCmake(string outputDir)
    {
        // Find repo root by looking for CLAUDE.md
        var repoRoot = FindRepoRoot().Replace('\\', '/');
        var repoRootEscaped = repoRoot.Replace("\\", "/");

        return $$"""
            cmake_minimum_required(VERSION 3.20)
            project(entry CXX)
            set(CMAKE_CXX_STANDARD 20)

            add_compile_options(/utf-8 /GS- /FS)
            if(NOT DEFINED CHAOS_IL2CPP_CONFIG_TIER)
              set(CHAOS_IL2CPP_CONFIG_TIER "debug")
            endif()

            # chaos-sdk (provides chaos::runtime target)
            set(CHAOS_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/chaos-sdk")
            find_package(chaos REQUIRED PATHS "${CHAOS_SDK_DIR}")

            set(CHAOS_PROJECT_ROOT "{{repoRootEscaped}}")

            # Codegen sources
            file(GLOB CHAOS_CODEGEN_CPP "subjects/*.cpp")

            # Native stubs in this directory
            file(GLOB CHAOS_NATIVE_STUBS "*.cpp")
            list(REMOVE_ITEM CHAOS_NATIVE_STUBS
                "${CMAKE_CURRENT_SOURCE_DIR}/entry.cpp"
            )

            # Runtime stub sources from the source tree
            file(GLOB CHAOS_RUNTIME_STUB_SOURCES
                "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/*.cpp"
            )
            list(REMOVE_ITEM CHAOS_RUNTIME_STUB_SOURCES
                "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/guid_stubs.cpp"
                "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs/threading_stubs.cpp"
            )

            set(CHAOS_PROFILE_GLOBALS
                "${CHAOS_PROJECT_ROOT}/src/native/common/chaos/profile_globals.cpp"
            )

            add_executable(entry
                entry.cpp
                ${CHAOS_CODEGEN_CPP}
                ${CHAOS_NATIVE_STUBS}
                ${CHAOS_RUNTIME_STUB_SOURCES}
                ${CHAOS_PROFILE_GLOBALS}
            )

            target_include_directories(entry PRIVATE
                "${CHAOS_PROJECT_ROOT}/src/native/runtime-core"
                "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/gc"
                "${CHAOS_PROJECT_ROOT}/src/native/runtime-core/runtime_stubs"
                "${CHAOS_PROJECT_ROOT}/src/native/common"
                "${CHAOS_PROJECT_ROOT}/src/native/bootstrap"
                "${CHAOS_PROJECT_ROOT}/src/native"
                "${CHAOS_PROJECT_ROOT}/src/native/interpreter"
                "${CHAOS_PROJECT_ROOT}/src/native/interpreter/generated"
                "${CHAOS_PROJECT_ROOT}/src/native/support"
                "${CHAOS_PROJECT_ROOT}/src/native/hot-update"
                "${CHAOS_PROJECT_ROOT}/third_party/unordered_dense/include"
                "${CMAKE_CURRENT_SOURCE_DIR}"
                "${CMAKE_CURRENT_SOURCE_DIR}/subjects"
            )

            target_compile_options(entry PRIVATE /EHa)
            target_link_libraries(entry PRIVATE chaos::runtime)
            target_link_options(entry PRIVATE /FORCE:MULTIPLE)
            """;
    }

    private static string FindRepoRoot()
    {
        var dir = AppContext.BaseDirectory;
        while (dir is not null)
        {
            if (File.Exists(Path.Combine(dir, "CLAUDE.md")))
                return dir;
            dir = Path.GetDirectoryName(dir);
        }
        return "";
    }

    private static string GenerateSubjectsJson(IReadOnlyList<SubjectModel> subjects)
    {
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

        return JsonSerializer.Serialize(obj, JsonOptions);
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
