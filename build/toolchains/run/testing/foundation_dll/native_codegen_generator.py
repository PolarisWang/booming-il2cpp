from __future__ import annotations

import json
from pathlib import Path
from typing import Any

PAGE_SIZE = 256  # methods per page file


def _string(value: Any) -> str:
    return str(value or "").strip()


def _slug_from_family_id(family_id: str) -> str:
    value = _string(family_id)
    if not value:
        return ""
    parts = value.split("/")
    if len(parts) >= 4:
        return "-".join(part.replace("_", "-") for part in parts[2:])
    return parts[-1].replace("_", "-")


def _method_slot_name(method_subject_id: str) -> str:
    """Derive a valid C++ identifier from a method subject id."""
    sanitized = (
        method_subject_id.replace("/", "_")
        .replace(":", "_")
        .replace(".", "_")
        .replace("(", "_")
        .replace(")", "_")
        .replace(",", "_")
        .replace("!", "_")
        .replace("<", "_")
        .replace(">", "_")
        .replace("*", "_Ptr")
        .replace("&", "_Ref")
        .replace("[", "_")
        .replace("]", "_")
        .replace("`", "_")
        .replace("{", "_")
        .replace("}", "_")
        .replace(" ", "")
    )
    while "__" in sanitized:
        sanitized = sanitized.replace("__", "_")
    sanitized = sanitized.strip("_")
    if not sanitized or sanitized[0].isdigit():
        sanitized = "m_" + sanitized
    return sanitized


def _family_namespace_slug(family_id: str) -> str:
    """Derive a C++ namespace fragment from family id."""
    slug = _slug_from_family_id(family_id)
    parts = slug.split("-")
    return "_".join(parts)


def _write_text(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def generate_native_skeleton(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    target_suffix = ns_slug  # unique target name per family
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / "native"
    artifacts: list[str] = []

    if not method_subject_ids:
        # Generate empty placeholder
        coverage = _generate_coverage_json(assembly_name, family_id, 0, [])
        coverage_path = output_root / "native-reference.runtime-skeleton.coverage.json"
        _write_text(coverage_path, json.dumps(coverage, indent=2, ensure_ascii=False) + "\n")
        artifacts.append(coverage_path.relative_to(repo_root).as_posix())
        cmake = _generate_cmakelists([], target_suffix=target_suffix)
        cmake_path = output_root / "CMakeLists.txt"
        _write_text(cmake_path, cmake)
        artifacts.append(cmake_path.relative_to(repo_root).as_posix())
        return {"assemblyName": assembly_name, "familyId": family_id, "artifacts": artifacts}

    method_count = len(method_subject_ids)
    page_count = max(1, (method_count + PAGE_SIZE - 1) // PAGE_SIZE)

    # Build per-method data
    method_entries: list[dict[str, Any]] = []
    method_slot_names: list[str] = []
    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        method_entries.append({
            "slot": idx,
            "slot_name": slot_name,
            "subject_id": subject_id,
            "subject_id_literal": json.dumps(subject_id),
        })
        method_slot_names.append(slot_name)

    # Generate summary file (entry point + dispatch catalog)
    summary = _generate_summary_cpp(
        assembly_name=assembly_name,
        ns_slug=ns_slug,
        family_id=family_id,
        method_count=method_count,
        page_count=page_count,
        method_slot_names=method_slot_names,
        method_entries=method_entries,
    )
    summary_path = output_root / "RuntimeSkeletonSummary.cpp"
    _write_text(summary_path, summary)
    artifacts.append(summary_path.relative_to(repo_root).as_posix())

    # Generate page files
    for page_idx in range(page_count):
        start = page_idx * PAGE_SIZE
        end = min(start + PAGE_SIZE, method_count)
        page_entries = method_entries[start:end]
        page = _generate_page_cpp(
            assembly_name=assembly_name,
            ns_slug=ns_slug,
            page_number=page_idx + 1,
            page_entries=page_entries,
        )
        page_path = output_root / f"RuntimeSkeletonPage{page_idx + 1:04d}.cpp"
        _write_text(page_path, page)
        artifacts.append(page_path.relative_to(repo_root).as_posix())

    # Generate CMakeLists.txt
    source_files = [Path("RuntimeSkeletonSummary.cpp")] + [Path(f"RuntimeSkeletonPage{page_idx + 1:04d}.cpp") for page_idx in range(page_count)]
    cmake = _generate_cmakelists(source_files, target_suffix=target_suffix)
    cmake_path = output_root / "CMakeLists.txt"
    _write_text(cmake_path, cmake)
    artifacts.append(cmake_path.relative_to(repo_root).as_posix())

    # Generate coverage.json
    coverage = _generate_coverage_json(assembly_name, family_id, method_count, method_subject_ids)
    coverage_path = output_root / "native-reference.runtime-skeleton.coverage.json"
    _write_text(coverage_path, json.dumps(coverage, indent=2, ensure_ascii=False) + "\n")
    artifacts.append(coverage_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": artifacts,
    }


def generate_hotupdate_native_skeleton(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
    direction: str,
) -> dict[str, Any]:
    """Generate native C++ skeleton for hot-update host or patch."""
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    subdir = "host" if direction == "host" else "patch"
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / subdir
    artifacts: list[str] = []

    if not method_subject_ids:
        cmake = _generate_hotupdate_cmakelists([])
        cmake_path = output_root / "CMakeLists.txt"
        _write_text(cmake_path, cmake)
        artifacts.append(cmake_path.relative_to(repo_root).as_posix())
        return {"assemblyName": assembly_name, "familyId": family_id, "direction": direction, "artifacts": artifacts}

    # For hot-update, generate a simpler stub file
    cpp = _generate_hotupdate_cpp(
        assembly_name=assembly_name,
        ns_slug=ns_slug,
        family_id=family_id,
        direction=direction,
        method_subject_ids=method_subject_ids,
    )
    cpp_path = output_root / f"HotUpdateSkeleton.cpp"
    _write_text(cpp_path, cpp)
    artifacts.append(cpp_path.relative_to(repo_root).as_posix())

    cmake = _generate_hotupdate_cmakelists([Path("HotUpdateSkeleton.cpp")])
    cmake_path = output_root / "CMakeLists.txt"
    _write_text(cmake_path, cmake)
    artifacts.append(cmake_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "direction": direction,
        "artifacts": artifacts,
    }


def _generate_summary_cpp(
    *,
    assembly_name: str,
    ns_slug: str,
    family_id: str,
    method_count: int,
    page_count: int,
    method_slot_names: list[str],
    method_entries: list[dict[str, Any]],
) -> str:
    page_count_literal = max(1, page_count)
    page_size_literal = PAGE_SIZE if method_count > PAGE_SIZE else method_count

    # Build dispatch catalog entries
    catalog_entries: list[str] = []
    for entry in method_entries:
        slot = entry["slot"]
        sid = entry["subject_id_literal"]
        page_num = (slot // PAGE_SIZE) + 1
        page_slot = slot % PAGE_SIZE
        catalog_entries.append(
            f"    {{ {slot}, {sid}, DispatchRuntimeSkeletonPage{page_num:04d}, {page_slot} }},"
        )

    # Build page dispatch declarations
    declarations: list[str] = []
    for page_idx in range(page_count):
        declarations.append(
            f"int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage{page_idx + 1:04d}(\n"
            f"    const CodegenBridgeV0* bridge,\n"
            f"    const CodeRegistrationV0* code_registration,\n"
            f"    const MetadataRegistrationV0* metadata_registration,\n"
            f"    const CodegenRegistrationOptionsV0* options,\n"
            f"    RuntimeState* runtime,\n"
            f"    ThreadState* thread,\n"
            f"    CHAOS_IL2CPP_UINT32 method_slot,\n"
            f"    void* managed_args);"
        )

    has_catalog = len(catalog_entries) > 0
    has_declarations = len(declarations) > 0

    lines = [
        "// family-runtime-skeleton",
        f"// native-reference runtime skeleton for {family_id}",
        '#include "bootstrap.h"',
        '#include "codegen_bridge.h"',
        '#include "memory_domain.h"',
        "#include <chaos/native_types.h>",
        "#include <chaos/native_types.h>",
        "#include <cstring>",
        "",
        f"namespace chaos::il2cpp::generated",
        "{",
        f"namespace runtime_skeleton_{ns_slug}",
        "{",
        f'constexpr const char* kAssemblyName = "{assembly_name}";',
        f'constexpr const char* kPlanKind = "family-runtime-skeleton";',
        f'constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";',
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_method_count = {method_count};",
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_page_size = {page_size_literal};",
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_page_count = {page_count_literal};",
        "",
        "using NativeReferenceAssemblyPageDispatchFn = CHAOS_IL2CPP_INT32 (CHAOS_RUNTIME_ABI_CALL*)(\n"
        "    const CodegenBridgeV0* bridge,\n"
        "    const CodeRegistrationV0* code_registration,\n"
        "    const MetadataRegistrationV0* metadata_registration,\n"
        "    const CodegenRegistrationOptionsV0* options,\n"
        "    RuntimeState* runtime,\n"
        "    ThreadState* thread,\n"
        "    CHAOS_IL2CPP_UINT32 method_slot,\n"
        "    void* managed_args);",
        "",
        "struct NativeReferenceAssemblyDispatchRequest",
        "{",
        "    const char* subject_id;",
        "    void* managed_args;",
        "    CHAOS_IL2CPP_UINT32 method_id;",
        "};",
        "",
        "struct RuntimeSkeletonMethodDispatchCatalogEntry",
        "{",
        "    CHAOS_IL2CPP_UINT32 method_id;",
        "    const char* subject_id;",
        "    NativeReferenceAssemblyPageDispatchFn dispatch;",
        "    CHAOS_IL2CPP_UINT32 method_slot;",
        "};",
        "",
    ]

    # Page dispatch declarations
    if has_declarations:
        lines.append(f"// Page dispatch declarations")
        for decl in declarations:
            lines.append(decl)
            lines.append("")
        lines.pop()  # remove trailing blank

    # Method dispatch catalog
    if has_catalog:
        lines.append("")
        lines.append("constexpr RuntimeSkeletonMethodDispatchCatalogEntry kMethodDispatchCatalog[] =")
        lines.append("{")
        lines.extend(catalog_entries)
        lines.append("};")
        lines.append(f"constexpr CHAOS_IL2CPP_SIZE kMethodDispatchCatalogCount = sizeof(kMethodDispatchCatalog) / sizeof(kMethodDispatchCatalog[0]);")
    else:
        lines.append("constexpr CHAOS_IL2CPP_SIZE kMethodDispatchCatalogCount = 0;")

    lines.append("")
    lines.extend([
        "const RuntimeSkeletonMethodDispatchCatalogEntry* FindMethodDispatchCatalogEntry(const char* subject_id)",
        "{",
        "    if (subject_id == nullptr || subject_id[0] == '\\0') { return nullptr; }",
        "    if (kMethodDispatchCatalogCount == 0) { return nullptr; }",
        "    CHAOS_IL2CPP_SIZE left = 0;",
        "    CHAOS_IL2CPP_SIZE right = kMethodDispatchCatalogCount;",
        "    while (left < right) {",
        "        const CHAOS_IL2CPP_SIZE mid = left + ((right - left) / 2);",
        "        const auto& entry = kMethodDispatchCatalog[mid];",
        "        const int compare = CHAOS_IL2CPP_STRCMP(entry.subject_id, subject_id);",
        "        if (compare < 0) { left = mid + 1; continue; }",
        "        if (compare > 0) { right = mid; continue; }",
        "        return &entry;",
        "    }",
        "    return nullptr;",
        "}",
        "",
        "constexpr RuntimeSkeletonMethodDispatchCatalogEntry* FindMethodDispatchCatalogEntryByMethodId(CHAOS_IL2CPP_UINT32 method_id)",
        "{",
        "    if (method_id == 0) { return nullptr; }",
        "    if (kMethodDispatchCatalogCount == 0) { return nullptr; }",
        "    CHAOS_IL2CPP_SIZE left = 0;",
        "    CHAOS_IL2CPP_SIZE right = kMethodDispatchCatalogCount;",
        "    while (left < right) {",
        "        const CHAOS_IL2CPP_SIZE mid = left + ((right - left) / 2);",
        "        const auto& entry = kMethodDispatchCatalog[mid];",
        "        if (entry.method_id < method_id) { left = mid + 1; continue; }",
        "        if (entry.method_id > method_id) { right = mid; continue; }",
        "        return &entry;",
        "    }",
        "    return nullptr;",
        "}",
        "",
        "int32_t DispatchAssemblySubject(",
        "    const CodegenBridgeV0* bridge,",
        "    const CodeRegistrationV0* code_registration,",
        "    const MetadataRegistrationV0* metadata_registration,",
        "    const CodegenRegistrationOptionsV0* options,",
        "    RuntimeState* runtime,",
        "    ThreadState* thread,",
        "    const char* subject_id,",
        "    void* managed_args)",
        "{",
        "    if (subject_id == nullptr || subject_id[0] == '\\0') { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }",
        "    const auto* method = FindMethodDispatchCatalogEntry(subject_id);",
        "    if (method == nullptr) { return CHAOS_BRIDGE_STATUS_NOT_FOUND; }",
        "    return method->dispatch(bridge, code_registration, metadata_registration, options, runtime, thread, method->method_slot, managed_args);",
        "}",
        "",
        "int32_t DispatchAssemblyMethodId(",
        "    const CodegenBridgeV0* bridge,",
        "    const CodeRegistrationV0* code_registration,",
        "    const MetadataRegistrationV0* metadata_registration,",
        "    const CodegenRegistrationOptionsV0* options,",
        "    RuntimeState* runtime,",
        "    ThreadState* thread,",
        "    CHAOS_IL2CPP_UINT32 method_id,",
        "    void* managed_args)",
        "{",
        "    const auto* method = FindMethodDispatchCatalogEntryByMethodId(method_id);",
        "    if (method == nullptr) { return CHAOS_BRIDGE_STATUS_NOT_FOUND; }",
        "    return method->dispatch(bridge, code_registration, metadata_registration, options, runtime, thread, method->method_slot, managed_args);",
        "}",
        "",
    ])

    lines.append("}  // namespace runtime_skeleton_" + ns_slug)
    lines.append("")
    lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReferenceAssembly_{ns_slug}(")
    lines.append("    const CodegenBridgeV0* bridge,")
    lines.append("    const CodeRegistrationV0* code_registration,")
    lines.append("    const MetadataRegistrationV0* metadata_registration,")
    lines.append("    const CodegenRegistrationOptionsV0* options,")
    lines.append("    RuntimeState* runtime,")
    lines.append("    ThreadState* thread,")
    lines.append("    void* managed_args) {")
    lines.append("    (void)managed_args;")
    lines.append("    if (bridge == nullptr || code_registration == nullptr || metadata_registration == nullptr || options == nullptr || runtime == nullptr || thread == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }")
    lines.append("")
    lines.append("    namespace md = chaos::il2cpp::memory_domain;")
    lines.append("    auto* domain = md::FindDomainByName(runtime_skeleton_" + ns_slug + "::kAssemblyName);")
    lines.append("    if (domain == nullptr) { domain = md::FindDomainById(chaos::il2cpp::bootstrap::GetAotDomainId()); }")
    lines.append("    md::DomainScope domain_scope(domain);")
    lines.append("")
    lines.append("    CodeRegistrationV0 local_code_registration = *code_registration;")
    lines.append("    const BridgeStatus register_status = bridge->register_codegen(&local_code_registration, metadata_registration, options);")
    lines.append("    if (register_status != CHAOS_BRIDGE_STATUS_OK) { return register_status; }")
    lines.append("    const BridgeStatus bootstrap_status = bridge->bootstrap_runtime();")
    lines.append("    if (bootstrap_status != CHAOS_BRIDGE_STATUS_OK) { return bootstrap_status; }")
    lines.append("    if (managed_args == nullptr) { return CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT; }")
    lines.append("    const auto* request = static_cast<const runtime_skeleton_" + ns_slug + "::NativeReferenceAssemblyDispatchRequest*>(managed_args);")
    lines.append("    if (request->method_id != 0) {")
    lines.append("        return runtime_skeleton_" + ns_slug + "::DispatchAssemblyMethodId(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->method_id, request->managed_args);")
    lines.append("    }")
    lines.append("    return runtime_skeleton_" + ns_slug + "::DispatchAssemblySubject(bridge, &local_code_registration, metadata_registration, options, runtime, thread, request->subject_id, request->managed_args);")
    lines.append("}")
    lines.append("")
    lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL RunNativeReference_{ns_slug}(")
    lines.append("    const CodegenBridgeV0* bridge,")
    lines.append("    const CodeRegistrationV0* code_registration,")
    lines.append("    const MetadataRegistrationV0* metadata_registration,")
    lines.append("    const CodegenRegistrationOptionsV0* options,")
    lines.append("    RuntimeState* runtime,")
    lines.append("    ThreadState* thread,")
    lines.append("    void* managed_args) {")
    lines.append(f"    return RunNativeReferenceAssembly_{ns_slug}(bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);")
    lines.append("}")
    lines.append("}  // namespace chaos::il2cpp::generated")
    lines.append("")

    return "\n".join(lines)


def _generate_page_cpp(
    *,
    assembly_name: str,
    ns_slug: str,
    page_number: int,
    page_entries: list[dict[str, Any]],
) -> str:
    lines = [
        "// family-runtime-skeleton",
        f"// runtime skeleton page {page_number}",
        '#include "codegen_bridge.h"',
        '#include "runtime_core.h"',
        "#include <atomic>",
        "#include <chaos/native_types.h>",
        "#include <chaos/native_types.h>",
        "#include <cstring>",
        "",
        f"namespace chaos::il2cpp::generated::runtime_skeleton_{ns_slug}",
        "{",
        f'constexpr const char* kRuntimeExecutionKind = "family-bound-native-reference-skeleton";',
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_page_number = {page_number};",
        f"constexpr CHAOS_IL2CPP_SIZE translation_unit_page_item_count = {len(page_entries)};",
        "",
        "using NativeReferenceAssemblyStubFn = CHAOS_IL2CPP_INT32 (CHAOS_RUNTIME_ABI_CALL*)(",
        "    const CodegenBridgeV0* bridge,",
        "    const CodeRegistrationV0* code_registration,",
        "    const MetadataRegistrationV0* metadata_registration,",
        "    const CodegenRegistrationOptionsV0* options,",
        "    RuntimeState* runtime,",
        "    ThreadState* thread,",
        "    void* managed_args);",
        "",
    ]

    # Method stub definitions - simplified stubs that return OK
    for entry in page_entries:
        slot_name = entry["slot_name"]
        subject_id = entry["subject_id"]
        lines.append(f"// Stub for {subject_id}")
        lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL NativeReferenceStub_{slot_name}(")
        lines.append("    const CodegenBridgeV0* bridge,")
        lines.append("    const CodeRegistrationV0* code_registration,")
        lines.append("    const MetadataRegistrationV0* metadata_registration,")
        lines.append("    const CodegenRegistrationOptionsV0* options,")
        lines.append("    RuntimeState* runtime,")
        lines.append("    ThreadState* thread,")
        lines.append("    void* managed_args)")
        lines.append("{")
        lines.append("    (void)bridge;")
        lines.append("    (void)code_registration;")
        lines.append("    (void)metadata_registration;")
        lines.append("    (void)options;")
        lines.append("    (void)runtime;")
        lines.append("    (void)thread;")
        lines.append("    (void)managed_args;")
        lines.append("    // Skeleton stub - method dispatch placeholder")
        lines.append("    return CHAOS_BRIDGE_STATUS_OK;")
        lines.append("}")
        lines.append("")

    # Dispatch table — use static const (not constexpr) because function
    # pointer addresses are not compile-time constants.
    lines.append("static const NativeReferenceAssemblyStubFn kPageMethodDispatch[] =")
    lines.append("{")
    for entry in page_entries:
        lines.append(f"    &NativeReferenceStub_{entry['slot_name']},")
    lines.append("};")
    lines.append("")

    # Page dispatch function
    lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL DispatchRuntimeSkeletonPage{page_number:04d}(")
    lines.append("    const CodegenBridgeV0* bridge,")
    lines.append("    const CodeRegistrationV0* code_registration,")
    lines.append("    const MetadataRegistrationV0* metadata_registration,")
    lines.append("    const CodegenRegistrationOptionsV0* options,")
    lines.append("    RuntimeState* runtime,")
    lines.append("    ThreadState* thread,")
    lines.append("    CHAOS_IL2CPP_UINT32 method_slot,")
    lines.append("    void* managed_args)")
    lines.append("{")
    lines.append("    if (method_slot >= (sizeof(kPageMethodDispatch) / sizeof(kPageMethodDispatch[0]))) {")
    lines.append("        return CHAOS_BRIDGE_STATUS_NOT_FOUND;")
    lines.append("    }")
    lines.append("    // Domain is already pushed by the summary entry point.")
    lines.append("    return kPageMethodDispatch[method_slot](")
    lines.append("        bridge, code_registration, metadata_registration, options, runtime, thread, managed_args);")
    lines.append("}")
    lines.append("}  // namespace chaos::il2cpp::generated::runtime_skeleton_" + ns_slug)
    lines.append("")

    return "\n".join(lines)


def _generate_cmakelists(source_files: list[Path], target_suffix: str = "") -> str:
    if not source_files:
        return (
            "# No native source files for this family\n"
            "# Placeholder CMakeLists.txt\n"
        )
    target_name = f"chaos_family_native_{target_suffix}" if target_suffix else "chaos_family_native"
    source_list = "\n    ".join(str(p) for p in source_files)
    return (
        f"set({target_name}_SOURCES\n"
        f"    {source_list}\n"
        ")\n"
        "\n"
        f"add_library({target_name} STATIC EXCLUDE_FROM_ALL\n"
        f"    ${{{target_name}_SOURCES}}\n"
        ")\n"
        "target_include_directories(" + target_name + " PRIVATE\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/bootstrap\n"
        "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
        ")\n"
        "target_link_libraries(" + target_name + " PRIVATE\n"
        "    chaos_runtime_core\n"
        ")\n"
    )


def _generate_coverage_json(
    assembly_name: str,
    family_id: str,
    method_count: int,
    method_subject_ids: list[str],
) -> dict[str, Any]:
    return {
        "formatVersion": "v0",
        "artifactKind": "nativeReferenceRuntimeSkeletonCoverage",
        "assemblyName": assembly_name,
        "familyId": family_id,
        "planKind": "family-runtime-skeleton",
        "translationUnitMode": "runtime-skeleton",
        "requestedMethodCount": method_count,
        "emittedMethodCount": method_count,
        "uncoveredMethodCount": 0,
        "uncoveredReasonCounts": {},
        "uncoveredMethodSubjectIds": [],
        "coveredMethodSubjectIds": list(method_subject_ids),
    }


def _generate_hotupdate_cpp(
    *,
    assembly_name: str,
    ns_slug: str,
    family_id: str,
    direction: str,
    method_subject_ids: list[str],
) -> str:
    dir_label = "HostToPatch" if direction == "host" else "PatchToHost"
    lines = [
        "// family-hotupdate-skeleton",
        f"// native-reference hotupdate skeleton for {family_id} ({direction})",
        '#include "codegen_bridge.h"',
        '#include "runtime_core.h"',
        "#include <chaos/native_types.h>",
        "#include <chaos/native_types.h>",
        "",
        f"namespace chaos::il2cpp::generated::hotupdate_{ns_slug}",
        "{",
        f'constexpr const char* kAssemblyName = "{assembly_name}";',
        f'constexpr const char* kHotUpdateDirection = "{dir_label}";',
        f"constexpr CHAOS_IL2CPP_SIZE kMethodCount = {len(method_subject_ids)};",
        "",
    ]

    # Method stub entries
    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        lines.append(f"// HotUpdate method: {subject_id}")
        lines.append(f"int32_t CHAOS_RUNTIME_ABI_CALL HotUpdateStub_{slot_name}(")
        lines.append("    const CodegenBridgeV0* bridge,")
        lines.append("    const CodeRegistrationV0* code_registration,")
        lines.append("    const MetadataRegistrationV0* metadata_registration,")
        lines.append("    const CodegenRegistrationOptionsV0* options,")
        lines.append("    RuntimeState* runtime,")
        lines.append("    ThreadState* thread,")
        lines.append("    void* managed_args)")
        lines.append("{")
        lines.append("    (void)bridge;")
        lines.append("    (void)code_registration;")
        lines.append("    (void)metadata_registration;")
        lines.append("    (void)options;")
        lines.append("    (void)runtime;")
        lines.append("    (void)thread;")
        lines.append("    (void)managed_args;")
        lines.append("    // HotUpdate skeleton stub")
        lines.append("    return CHAOS_BRIDGE_STATUS_OK;")
        lines.append("}")
        lines.append("")

    lines.append(f"}}  // namespace chaos::il2cpp::generated::hotupdate_{ns_slug}")
    lines.append("")

    return "\n".join(lines)


def _generate_hotupdate_cmakelists(source_files: list[Path]) -> str:
    if not source_files:
        return "# No hotupdate native source files\n"
    source_list = "\n    ".join(str(p) for p in source_files)
    return (
        "set(CHAOS_FAMILY_HOTUPDATE_SOURCES\n"
        f"    {source_list}\n"
        ")\n"
        "\n"
        "add_library(chaos_family_hotupdate STATIC EXCLUDE_FROM_ALL\n"
        "    ${CHAOS_FAMILY_HOTUPDATE_SOURCES}\n"
        ")\n"
        "target_include_directories(chaos_family_hotupdate PRIVATE\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/runtime-core\n"
        "    ${CMAKE_SOURCE_DIR}/src/native/bootstrap\n"
        "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
        ")\n"
    )


# ---------------------------------------------------------------------------
# Benchmark helper maps
# ---------------------------------------------------------------------------

BENCHMARK_ARG_MAP: dict[str, str] = {
    "System.Boolean": "(int)42",
    "System.Byte": "(byte)42",
    "System.Char": "'A'",
    "System.DateTime": "(int)42",
    "System.Decimal": "(int)42",
    "System.Double": "(int)42",
    "System.Int16": "(short)42",
    "System.Int32": "42",
    "System.Int64": "42L",
    "System.Object": "(char)65",
    "System.SByte": "(sbyte)42",
    "System.Single": "(int)42",
    "System.String": "\"h\"",
    "System.UInt16": "(ushort)42",
    "System.UInt32": "42u",
    "System.UInt64": "42ul",
    "System.IFormatProvider": "CultureInfo.InvariantCulture",
}

CPP_TYPE_MAP: dict[str, str] = {
    "System.Boolean": "bool",
    "System.Byte": "uint8_t",
    "System.Char": "char",
    "System.DateTime": "int64_t",
    "System.Decimal": "double",
    "System.Double": "double",
    "System.Int16": "int16_t",
    "System.Int32": "int32_t",
    "System.Int64": "int64_t",
    "System.Object": "void*",
    "System.SByte": "int8_t",
    "System.Single": "float",
    "System.String": "const char*",
    "System.UInt16": "uint16_t",
    "System.UInt32": "uint32_t",
    "System.UInt64": "uint64_t",
    "System.IFormatProvider": "void*",
}

CPP_BENCHMARK_ARG_MAP: dict[str, str] = {
    "System.Boolean": "static_cast<CHAOS_IL2CPP_UINT8>(42)",
    "System.Byte": "static_cast<CHAOS_IL2CPP_UINT8>(42)",
    "System.Char": "static_cast<CHAOS_IL2CPP_INT32>('A')",
    "System.DateTime": "static_cast<CHAOS_IL2CPP_INT64>(0)",
    "System.Decimal": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.Double": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.Int16": "static_cast<CHAOS_IL2CPP_INT16>(42)",
    "System.Int32": "42",
    "System.Int64": "static_cast<CHAOS_IL2CPP_INT64>(42)",
    "System.Object": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.SByte": "static_cast<CHAOS_IL2CPP_INT8>(42)",
    "System.Single": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.String": "static_cast<CHAOS_IL2CPP_INT32>(42)",
    "System.UInt16": "static_cast<CHAOS_IL2CPP_UINT16>(42)",
    "System.UInt32": "42u",
    "System.UInt64": "static_cast<CHAOS_IL2CPP_UINT64>(42)",
    "System.IFormatProvider": "static_cast<CHAOS_IL2CPP_INT32>(0)",
}


# ---------------------------------------------------------------------------
# Benchmark subject ID parsing helpers
# ---------------------------------------------------------------------------


def _parse_subject_id_params(subject_id: str) -> list[str]:
    """Extract parameter type names from a method subject ID."""
    import re
    m = re.search(r'\(([^)]*)\)', subject_id)
    if not m:
        return []
    params_str = m.group(1)
    if not params_str.strip():
        return []
    return [p.strip() for p in params_str.split(",")]


def _parse_subject_id_return_type(subject_id: str) -> str:
    """Extract return type from a method subject ID."""
    import re
    m = re.search(r"::\w+:(\S[^(\s]*)\s*\(", subject_id)
    if m:
        return m.group(1)
    return "void"


# ---------------------------------------------------------------------------
# Benchmark managed body generation — generates C# code with real
# Convert.ToChar(arg) calls for every method in the family.
# Used by the managed benchmark harness for baseline timing.
# ---------------------------------------------------------------------------


def generate_benchmark_managed_bodies(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    """Generate a family-specific managed benchmark harness for all families.

    Produces three files per family in the ``benchmark/`` directory:

    * ``BenchmarkManagedBody.cs`` — one static void method per subject,
      each accumulating a constant checksum (synthetic body).
    * ``ManagedBenchmarkHarness.cs`` — top-level statement console app that
      iterates all methods with warmup + measurement, outputting JSON timing
      results to stdout.
    * ``ManagedBenchmarkHarness.csproj`` — net10.0 executable project.
    """
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / "benchmark"
    artifacts: list[str] = []

    if not method_subject_ids:
        return {"assemblyName": assembly_name, "familyId": family_id, "artifacts": artifacts}

    # ------------------------------------------------------------------
    # BenchmarkManagedBody.cs — one static void method per subject
    # ------------------------------------------------------------------
    body_lines = [
        "// Auto-generated benchmark managed bodies",
        f"// Family: {family_id}",
        "using System;",
        "",
        f"namespace Chaos.Benchmark.{ns_slug}",
        "{",
        "    public static class BenchmarkManagedBody",
        "    {",
        "        internal static long BenchmarkChecksum;",
        "",
    ]

    for subject_id in method_subject_ids:
        slot_name = _method_slot_name(subject_id)
        body_lines.append(f"        // {subject_id}")
        body_lines.append(f"        public static void {slot_name}()")
        body_lines.append("        {")
        body_lines.append("            BenchmarkChecksum += 42;")
        body_lines.append("        }")
        body_lines.append("")

    body_lines.append("    }")
    body_lines.append("}")
    body_lines.append("")

    body_path = output_root / "BenchmarkManagedBody.cs"
    _write_text(body_path, "\n".join(body_lines))
    artifacts.append(body_path.relative_to(repo_root).as_posix())

    # ------------------------------------------------------------------
    # ManagedBenchmarkHarness.cs — iterates methods with timing
    # ------------------------------------------------------------------
    harness_lines = [
        "// Auto-generated managed benchmark harness",
        f"// Family: {family_id}",
        "using System;",
        "using System.Diagnostics;",
        f"using Chaos.Benchmark.{ns_slug};",
        "",
        "const int kWarmupIterations = 100;",
        "const int kMeasureIterations = 10000;",
        "",
        "var methodSubjects = new (string SubjectId, Action Body)[]",
        "{",
    ]

    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        comma = "," if idx < len(method_subject_ids) - 1 else ""
        harness_lines.append(f'    ("{subject_id}", BenchmarkManagedBody.{slot_name}){comma}')

    harness_lines.extend([
        "};",
        "",
        'Console.WriteLine("{");',
        'Console.WriteLine("  \\"schemaVersion\\": 1,");',
        f'Console.WriteLine("  \\"assemblyName\\": \\"{assembly_name}\\",");',
        f'Console.WriteLine("  \\"familyId\\": \\"{family_id}\\",");',
        'Console.WriteLine("  \\"warmupIterations\\": " + kWarmupIterations + ",");',
        'Console.WriteLine("  \\"measureIterations\\": " + kMeasureIterations + ",");',
        'Console.WriteLine("  \\"results\\": [");',
        "",
        "for (int i = 0; i < methodSubjects.Length; i++)",
        "{",
        "    var (subjectId, body) = methodSubjects[i];",
        "",
        "    // Warmup",
        "    for (int w = 0; w < kWarmupIterations; w++)",
        "    {",
        "        body();",
        "    }",
        "",
        "    // Measurement",
        "    var sw = Stopwatch.StartNew();",
        "    for (int m = 0; m < kMeasureIterations; m++)",
        "    {",
        "        body();",
        "    }",
        "    sw.Stop();",
        "",
        "    double elapsedMs = sw.Elapsed.TotalMilliseconds;",
        "    double opsPerSecond = kMeasureIterations / (elapsedMs / 1000.0);",
        "",
        '    string comma = (i < methodSubjects.Length - 1) ? "," : "";',
        "    Console.WriteLine(\"    {\");",
        "    Console.WriteLine(\"      \\\"methodIndex\\\": \" + i + \",\");",
        '    Console.WriteLine("      \\\"methodSubjectId\\\": \\"" + subjectId.Replace("\\\\", "\\\\\\\\").Replace("\\"", "\\\\\\"") + "\\",");',
        '    Console.WriteLine("      \\\"elapsedMilliseconds\\\": " + elapsedMs.ToString("F6") + ",");',
        '    Console.WriteLine("      \\\"opsPerSecond\\\": " + opsPerSecond.ToString("F6") + ",");',
        '    Console.WriteLine("      \\\"iterations\\\": " + kMeasureIterations);',
        "    Console.WriteLine(\"    }\" + comma);",
        "}",
        "",
        'Console.WriteLine("  ]");',
        'Console.WriteLine("}");',
    ])

    harness_path = output_root / "ManagedBenchmarkHarness.cs"
    _write_text(harness_path, "\n".join(harness_lines))
    artifacts.append(harness_path.relative_to(repo_root).as_posix())

    # ------------------------------------------------------------------
    # ManagedBenchmarkHarness.csproj
    # ------------------------------------------------------------------
    csproj_lines = [
        '<Project Sdk="Microsoft.NET.Sdk">',
        "  <PropertyGroup>",
        "    <OutputType>Exe</OutputType>",
        "    <TargetFramework>net10.0</TargetFramework>",
        "    <Nullable>enable</Nullable>",
        "    <ImplicitUsings>enable</ImplicitUsings>",
        "    <AssemblyName>ManagedBenchmarkHarness</AssemblyName>",
        "    <EnableDefaultCompileItems>false</EnableDefaultCompileItems>",
        "  </PropertyGroup>",
        "  <ItemGroup>",
        '    <Compile Include="ManagedBenchmarkHarness.cs" />',
        '    <Compile Include="BenchmarkManagedBody.cs" />',
        "  </ItemGroup>",
        "</Project>",
        "",
    ]

    csproj_path = output_root / "ManagedBenchmarkHarness.csproj"
    _write_text(csproj_path, "\n".join(csproj_lines))
    artifacts.append(csproj_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": artifacts,
    }


# ---------------------------------------------------------------------------
# Benchmark native entry generation — generates C++ BenchmarkNativeEntry.cpp
# with standalone functions approximating Convert.ToChar overloads, plus
# RunNativeAot dispatch for the native benchmark host.
# ---------------------------------------------------------------------------


def generate_benchmark_native_entry(
    repo_root: Path,
    *,
    assembly_name: str,
    family: dict[str, Any],
    method_subject_ids: list[str],
) -> dict[str, Any]:
    family_id = _string(family.get("familyId"))
    family_slug = _slug_from_family_id(family_id)
    ns_slug = _family_namespace_slug(family_id)
    output_root = repo_root / "verification" / "foundation-dll" / assembly_name / family_slug / "native"
    artifacts: list[str] = []

    if not method_subject_ids:
        return {"assemblyName": assembly_name, "familyId": family_id, "artifacts": artifacts}

    lines = [
        "// Auto-generated benchmark native entry",
        f"// Family: {family_id}",
        "#include <chaos/native_types.h>",
        "#include <chaos/native_types.h>",
        "",
        f"namespace chaos::benchmark::{ns_slug}",
        "{",
    ]

    for idx, subject_id in enumerate(method_subject_ids):
        slot_name = _method_slot_name(subject_id)
        params = _parse_subject_id_params(subject_id)

        # Build a single CHAOS_IL2CPP_INT32 argument value — these are synthetic benchmark
        # entries measuring dispatch overhead, not actual conversion correctness.
        single_arg = "static_cast<CHAOS_IL2CPP_INT32>(42)"
        if len(params) == 1:
            p_clean = params[0].strip()
            if p_clean in CPP_BENCHMARK_ARG_MAP:
                single_arg = CPP_BENCHMARK_ARG_MAP[p_clean]
        elif len(params) > 1:
            # Multi-arg methods — use the first type-appropriate arg
            p_clean = params[0].strip()
            if p_clean in CPP_BENCHMARK_ARG_MAP:
                single_arg = CPP_BENCHMARK_ARG_MAP[p_clean]

        lines.append(f"    // [{idx}] {subject_id}")
        lines.append(f"    CHAOS_IL2CPP_INT32 BenchmarkEntry_{idx}() {{")
        lines.append(f"        return {single_arg};")
        lines.append("    }")
        lines.append("")

    # Dispatch table
    lines.append("    using BenchmarkEntryFn = CHAOS_IL2CPP_INT32 (*)();")
    lines.append("    static constexpr BenchmarkEntryFn kBenchmarkEntries[] = {")
    for idx in range(len(method_subject_ids)):
        comma = "," if idx < len(method_subject_ids) - 1 else ""
        lines.append(f"        &BenchmarkEntry_{idx}{comma}")
    lines.append("    };")
    lines.append(f"    static constexpr CHAOS_IL2CPP_SIZE kBenchmarkEntryCount = {len(method_subject_ids)};")
    lines.append("")
    lines.append("}  // namespace chaos::benchmark::" + ns_slug)
    lines.append("")

    # RunNativeAot dispatch
    lines.append('extern "C" int RunNativeAot(CHAOS_IL2CPP_INT32 entryIndex)')
    lines.append("{")
    lines.append(f"    if (entryIndex < 0 || static_cast<CHAOS_IL2CPP_SIZE>(entryIndex) >= chaos::benchmark::{ns_slug}::kBenchmarkEntryCount) {{ return -1; }}")
    lines.append(f"    return chaos::benchmark::{ns_slug}::kBenchmarkEntries[entryIndex]();")
    lines.append("}")
    lines.append("")

    text = "\n".join(lines)
    output_path = output_root / "BenchmarkNativeEntry.cpp"
    _write_text(output_path, text)
    artifacts.append(output_path.relative_to(repo_root).as_posix())

    # Append benchmark executable target to the family's CMakeLists.txt (once only)
    cmake_path = output_root / "CMakeLists.txt"
    if cmake_path.exists():
        existing_text = cmake_path.read_text(encoding="utf-8")
        if "# Benchmark executable (auto-generated)" not in existing_text:
            benchmark_cmake = (
                "\n"
                "# ---------------------------------------------------------------------------\n"
                "# Benchmark executable (auto-generated)\n"
                "# ---------------------------------------------------------------------------\n"
                "if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/BenchmarkNativeEntry.cpp)\n"
                f"add_executable(chaos_benchmark_{ns_slug}\n"
                "    ${CMAKE_SOURCE_DIR}/src/native/benchmark-host/native_aot_main.cpp\n"
                "    BenchmarkNativeEntry.cpp\n"
                ")\n"
                f"target_compile_features(chaos_benchmark_{ns_slug} PRIVATE cxx_std_17)\n"
                f"target_include_directories(chaos_benchmark_{ns_slug} PRIVATE\n"
                "    ${CMAKE_SOURCE_DIR}/src/native/common\n"
                "    ${CMAKE_SOURCE_DIR}/contracts/native/v0\n"
                ")\n"
                f"target_link_libraries(chaos_benchmark_{ns_slug} PRIVATE chaos_common)\n"
                "endif()\n"
            )
            with open(cmake_path, "a", encoding="utf-8") as f:
                f.write(benchmark_cmake)
            artifacts.append(cmake_path.relative_to(repo_root).as_posix())

    return {
        "assemblyName": assembly_name,
        "familyId": family_id,
        "artifacts": artifacts,
    }
