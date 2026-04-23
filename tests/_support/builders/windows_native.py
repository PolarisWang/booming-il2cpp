from __future__ import annotations

import uuid
from pathlib import Path


def write_windows_subject_native_project_stubs(
    configure_root: Path,
    *,
    generated_text: str = "<Project />\n",
    proof_text: str = "<Project />\n",
    benchmark_text: str | None = None,
) -> None:
    generated_project = configure_root / "generated" / "chaos_subject_generated_native.vcxproj"
    proof_project = configure_root / "proof" / "chaos_subject_reference_proof.vcxproj"
    generated_project.parent.mkdir(parents=True, exist_ok=True)
    proof_project.parent.mkdir(parents=True, exist_ok=True)
    generated_project.write_text(generated_text, encoding="utf-8")
    proof_project.write_text(proof_text, encoding="utf-8")
    if benchmark_text is not None:
        benchmark_project = configure_root / "benchmark" / "chaos_subject_native_aot.vcxproj"
        benchmark_project.parent.mkdir(parents=True, exist_ok=True)
        benchmark_project.write_text(benchmark_text, encoding="utf-8")


def write_windows_subject_native_aot_project_stub(
    configure_root: Path,
    *,
    host_subdir: str,
    project_text: str = "<Project />\n",
) -> None:
    native_aot_project = configure_root / host_subdir / "chaos_subject_native_aot.vcxproj"
    native_aot_project.parent.mkdir(parents=True, exist_ok=True)
    native_aot_project.write_text(project_text, encoding="utf-8")


def write_windows_subject_native_solution_graph_stubs(configure_root: Path) -> None:
    project_specs = [
        ("ALL_BUILD.vcxproj", "ALL_BUILD"),
        ("ZERO_CHECK.vcxproj", "ZERO_CHECK"),
        ("bootstrap/chaos_bootstrap.vcxproj", "chaos_bootstrap"),
        ("runtime-core/chaos_runtime_core.vcxproj", "chaos_runtime_core"),
        ("generated/chaos_subject_generated_native.vcxproj", "chaos_subject_generated_native"),
        ("proof/chaos_subject_reference_proof.vcxproj", "chaos_subject_reference_proof"),
        ("proof/chaos_subject_reference_proof_run.vcxproj", "chaos_subject_reference_proof_run"),
        ("support/chaos_support.vcxproj", "chaos_support"),
    ]
    reference_specs = {
        "ALL_BUILD.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "bootstrap/chaos_bootstrap.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
            "support/chaos_support.vcxproj",
        ],
        "bootstrap/chaos_bootstrap.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
            "support/chaos_support.vcxproj",
        ],
        "generated/chaos_subject_generated_native.vcxproj": [
            "ZERO_CHECK.vcxproj",
        ],
        "proof/chaos_subject_reference_proof.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "bootstrap/chaos_bootstrap.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
            "generated/chaos_subject_generated_native.vcxproj",
            "support/chaos_support.vcxproj",
        ],
        "proof/chaos_subject_reference_proof_run.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "proof/chaos_subject_reference_proof.vcxproj",
        ],
        "runtime-core/chaos_runtime_core.vcxproj": [
            "ZERO_CHECK.vcxproj",
        ],
        "support/chaos_support.vcxproj": [
            "ZERO_CHECK.vcxproj",
            "runtime-core/chaos_runtime_core.vcxproj",
        ],
    }

    materialized_specs: list[tuple[str, str, str]] = []
    for relative_path, project_name in project_specs:
        project_guid = "{" + str(uuid.uuid5(uuid.NAMESPACE_URL, f"{configure_root.as_posix()}::{relative_path}")).upper() + "}"
        materialized_specs.append((relative_path, project_name, project_guid))
    guid_by_relative_path = {relative_path: project_guid for relative_path, _, project_guid in materialized_specs}
    name_by_relative_path = {relative_path: project_name for relative_path, project_name, _ in materialized_specs}

    for relative_path, project_name, project_guid in materialized_specs:
        project_path = configure_root / Path(relative_path)
        project_path.parent.mkdir(parents=True, exist_ok=True)
        reference_lines = []
        for reference_relative_path in reference_specs.get(relative_path, []):
            reference_path = configure_root / Path(reference_relative_path)
            reference_lines.extend(
                [
                    "  <ItemGroup>",
                    f'    <ProjectReference Include="{reference_path.as_posix()}">',
                    f"      <Project>{guid_by_relative_path[reference_relative_path]}</Project>",
                    f"      <Name>{name_by_relative_path[reference_relative_path]}</Name>",
                    "    </ProjectReference>",
                    "  </ItemGroup>",
                ]
            )
        project_path.write_text(
            "\n".join(
                [
                    "<Project>",
                    '  <PropertyGroup Label="Globals">',
                    f"    <ProjectGuid>{project_guid}</ProjectGuid>",
                    f"    <ProjectName>{project_name}</ProjectName>",
                    "  </PropertyGroup>",
                    *reference_lines,
                    "</Project>",
                    "",
                ]
            ),
            encoding="utf-8",
        )

    solution_lines = [
        "Microsoft Visual Studio Solution File, Format Version 12.00",
        "# Visual Studio Version 17",
        "VisualStudioVersion = 17.0.31903.59",
        "MinimumVisualStudioVersion = 10.0.40219.1",
    ]
    for relative_path, project_name, project_guid in materialized_specs:
        windows_relative_path = relative_path.replace("/", chr(92))
        solution_lines.append(
            f'Project("{{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}}") = "{project_name}", "{windows_relative_path}", "{project_guid}"'
        )
        solution_lines.append("EndProject")
    solution_lines.extend(
        [
            "Global",
            "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution",
            "\t\tDebug|x64 = Debug|x64",
            "\t\tRelease|x64 = Release|x64",
            "\tEndGlobalSection",
            "EndGlobal",
        ]
    )
    (configure_root / "chaos_subject_native_workspace.sln").write_text(
        "\n".join(solution_lines) + "\n",
        encoding="utf-8",
    )
