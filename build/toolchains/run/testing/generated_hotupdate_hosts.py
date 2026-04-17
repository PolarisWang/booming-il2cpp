from __future__ import annotations


_FRAMEWORK_PROJECT_REFERENCE = "../../../../src/reference/Chaos.TestFramework.Sdk/Chaos.TestFramework.Sdk.csproj"
_RUNTIME_PROJECT_REFERENCE = "../../../../src/reference/Chaos.TestFramework.Runtime/Chaos.TestFramework.Runtime.csproj"


def render_declared_hotupdate_host_source(*, subject_id: str, host_kind: str) -> str:
    class_name = _host_class_name(subject_id=subject_id, host_kind=host_kind)
    namespace_name = f"Chaos.Generated.HotUpdateTests.{_sanitize_identifier(subject_id)}"
    if host_kind == "proof-host":
        return "\n".join(
            [
                "using System;",
                "using Chaos.TestFramework;",
                "using Chaos.TestFramework.Runtime;",
                "",
                f"namespace {namespace_name};",
                "",
                f"public static class {class_name}",
                "{",
                "    public static int Main(string[] args)",
                "    {",
                "        var request = ChaosHotUpdateHostArguments.Parse(args);",
                "        try",
                "        {",
                "            ChaosAssertState.Reset();",
                "            ChaosHotUpdateExecutor.ExecuteProof(",
                "                request.CollectionPath,",
                "                request.BindingManifestPath,",
                "                request.EntryIndex);",
                "            return ChaosAssertState.Complete();",
                "        }",
                "        catch (ChaosAssertionException exception)",
                "        {",
                "            Console.Error.WriteLine(exception.Message);",
                "            ChaosAssertState.RecordFailure();",
                "            return ChaosAssertState.Complete();",
                "        }",
                "        catch (Exception exception)",
                "        {",
                "            Console.Error.WriteLine(exception);",
                "            ChaosAssertState.RecordFailure();",
                "            return ChaosAssertState.Complete();",
                "        }",
                "    }",
                "}",
                "",
            ]
        )

    if host_kind == "benchmark-host":
        return "\n".join(
            [
                "using System;",
                "using System.IO;",
                "using Chaos.TestFramework.Runtime;",
                "",
                f"namespace {namespace_name};",
                "",
                f"public static class {class_name}",
                "{",
                "    public static object? InvokeWorkload(string collectionPath, string bindingManifestPath, int entryIndex)",
                "    {",
                "        return ChaosHotUpdateExecutor.ExecuteBenchmarkWorkload(",
                "            collectionPath,",
                "            bindingManifestPath,",
                "            entryIndex,",
                "            ResolveHostBaseDirectory());",
                "    }",
                "",
                "    public static int Main(string[] args)",
                "    {",
                "        var request = ChaosHotUpdateHostArguments.Parse(args);",
                "        _ = InvokeWorkload(",
                "            request.CollectionPath,",
                "            request.BindingManifestPath,",
                "            request.EntryIndex);",
                "        return 0;",
                "    }",
                "",
                "    private static string ResolveHostBaseDirectory()",
                "    {",
                f"        return Path.GetDirectoryName(typeof({class_name}).Assembly.Location) ?? AppContext.BaseDirectory;",
                "    }",
                "}",
                "",
            ]
        )

    raise ValueError(f"unsupported hotupdate host kind: {host_kind}")


def render_declared_hotupdate_host_project(*, assembly_name: str, generated_source_path: str) -> str:
    return "\n".join(
        [
            '<Project Sdk="Microsoft.NET.Sdk">',
            "  <PropertyGroup>",
            "    <TargetFramework>net8.0</TargetFramework>",
            "    <OutputType>Exe</OutputType>",
            "    <ImplicitUsings>enable</ImplicitUsings>",
            "    <Nullable>enable</Nullable>",
            "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>",
            f"    <AssemblyName>{assembly_name}</AssemblyName>",
            f"    <RootNamespace>{assembly_name}</RootNamespace>",
            "  </PropertyGroup>",
            "  <ItemGroup>",
            f'    <ProjectReference Include="{_FRAMEWORK_PROJECT_REFERENCE}" />',
            f'    <ProjectReference Include="{_RUNTIME_PROJECT_REFERENCE}" />',
            "  </ItemGroup>",
            "  <ItemGroup>",
            f'    <Compile Include="{generated_source_path}" />',
            "  </ItemGroup>",
            "</Project>",
            "",
        ]
    )


def _host_class_name(*, subject_id: str, host_kind: str) -> str:
    host_suffix_by_kind = {
        "proof-host": "HotUpdateProofHost",
        "benchmark-host": "HotUpdateBenchmarkHost",
    }
    try:
        host_suffix = host_suffix_by_kind[host_kind]
    except KeyError as error:
        raise ValueError(f"unsupported hotupdate host kind: {host_kind}") from error
    return f"{_sanitize_identifier(subject_id)}{host_suffix}"


def _sanitize_identifier(value: str) -> str:
    sanitized = "".join(character if character.isalnum() else "_" for character in value.strip())
    return sanitized or "GeneratedHotUpdateHost"
