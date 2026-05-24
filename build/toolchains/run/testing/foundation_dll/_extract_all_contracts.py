"""Extract method subject IDs for 11 unstarted foundation DLLs and generate contract files.

For each target assembly:
1. Runs a C# helper that uses Reflection to enumerate public method signatures
2. Categorizes methods by type into families based on rule_registry marker rules
3. Generates capability-family-contract.json files for each family
4. Updates the capability-family-ledger.json with populated methodSubjectIds
"""
from __future__ import annotations

import json
import subprocess
import sys
from pathlib import Path
from typing import Any

_HERE = Path(__file__).resolve().parent
_REPO_ROOT = _HERE.parents[4]
_LEDGER_PATH = _REPO_ROOT / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"

# Reference assemblies path (NET 10)
NET10_REF_DIR = Path(r"C:\Program Files\dotnet\shared\Microsoft.NETCore.App\10.0.6")

# Target assemblies and their rule_registry marker rules
# Imported from rule_registry.py to ensure consistency
TARGET_ASSEMBLIES = [
    "System.Formats.Asn1",
    "System.IO.Compression.Brotli",
    "System.IO.Compression.ZipFile",
    "System.IO.Pipelines",
    "System.Net.ServerSentEvents",
    "System.ObjectModel",
    "System.Runtime.InteropServices",
    "System.Runtime.Serialization.Formatters",
    "System.Security.Principal.Windows",
    "System.Text.Json",
    "System.Threading.Tasks.Parallel",
]

# Marker rules copied from rule_registry.py for self-contained execution
MARKER_RULES: dict[str, list[tuple[str, str, tuple[str, ...]]]] = {
    "System.Formats.Asn1": [
        ("tag-model", "ASN.1 Tag Model", ("System.Formats.Asn1.Asn1Tag", "System.Formats.Asn1.TagClass", "System.Formats.Asn1.UniversalTagNumber")),
        ("encoding-rules", "ASN.1 Encoding Rules", ("System.Formats.Asn1.AsnEncodingRules",)),
        ("reader", "ASN.1 Reader", ("System.Formats.Asn1.AsnReader",)),
        ("writer", "ASN.1 Writer", ("System.Formats.Asn1.AsnWriter",)),
        ("decoder", "ASN.1 Decoder", ("System.Formats.Asn1.AsnDecoder",)),
        ("options-errors", "ASN.1 Options And Errors", ("System.Formats.Asn1.AsnReaderOptions", "System.Formats.Asn1.AsnContentException")),
    ],
    "System.IO.Compression.Brotli": [
        ("stream", "Brotli Stream", ("System.IO.Compression.BrotliStream",)),
        ("encoder", "Brotli Encoder", ("System.IO.Compression.BrotliEncoder",)),
        ("decoder", "Brotli Decoder", ("System.IO.Compression.BrotliDecoder",)),
        ("options", "Brotli Options", ("System.IO.Compression.BrotliCompressionOptions",)),
    ],
    "System.IO.Compression.ZipFile": [
        ("zip-file-apis", "ZipFile APIs", ("System.IO.Compression.ZipFile",)),
        ("zip-file-extensions", "ZipFile Extensions", ("System.IO.Compression.ZipFileExtensions",)),
    ],
    "System.IO.Pipelines": [
        ("pipe-core", "Pipe Core", ("System.IO.Pipelines.Pipe", "System.IO.Pipelines.IDuplexPipe")),
        ("reader-writer", "Pipe Reader Writer", ("System.IO.Pipelines.PipeReader", "System.IO.Pipelines.PipeWriter")),
        ("read-flush-results", "Read And Flush Results", ("System.IO.Pipelines.ReadResult", "System.IO.Pipelines.FlushResult")),
        ("options-scheduler", "Options And Scheduler", ("System.IO.Pipelines.PipeOptions", "System.IO.Pipelines.PipeScheduler", "System.IO.Pipelines.StreamPipeReaderOptions", "System.IO.Pipelines.StreamPipeWriterOptions")),
        ("stream-adapters", "Stream Adapters", ("System.IO.Pipelines.StreamPipeExtensions",)),
    ],
    "System.Net.ServerSentEvents": [
        ("sse-item", "SSE Item Model", ("System.Net.ServerSentEvents.SseItem",)),
        ("parser", "SSE Parser", ("System.Net.ServerSentEvents.SseParser",)),
        ("item-parser", "SSE Item Parser", ("System.Net.ServerSentEvents.SseItemParser",)),
        ("formatter", "SSE Formatter", ("System.Net.ServerSentEvents.SseFormatter",)),
    ],
    "System.ObjectModel": [
        ("object-model-collections", "ObjectModel Collections", ("System.Collections.ObjectModel.Collection", "System.Collections.ObjectModel.ReadOnlyCollection", "System.Collections.ObjectModel.KeyedCollection")),
        ("observable-collections", "Observable Collections", ("System.Collections.ObjectModel.ObservableCollection", "System.Collections.ObjectModel.ReadOnlyObservableCollection")),
        ("collection-change-notifications", "Collection Change Notifications", ("System.Collections.Specialized.INotifyCollectionChanged", "System.Collections.Specialized.NotifyCollectionChanged")),
        ("property-change-notifications", "Property Change Notifications", ("System.ComponentModel.INotifyPropertyChanged", "System.ComponentModel.INotifyPropertyChanging", "System.ComponentModel.PropertyChanged", "System.ComponentModel.PropertyChanging")),
        ("data-errors", "Data Errors", ("System.ComponentModel.INotifyDataErrorInfo", "System.ComponentModel.DataErrorsChangedEventArgs")),
        ("command-type-metadata", "Command And Type Metadata", ("System.Windows.Input.ICommand", "System.Reflection.ICustomTypeProvider", "System.ComponentModel.TypeConverterAttribute", "System.Windows.Markup.ValueSerializerAttribute")),
    ],
    "System.Runtime.InteropServices": [
        ("marshalling-attributes", "Marshalling Attributes", ("System.Runtime.InteropServices.StructLayoutAttribute", "System.Runtime.InteropServices.MarshalAsAttribute", "System.Runtime.InteropServices.InAttribute", "System.Runtime.InteropServices.OutAttribute", "System.Runtime.InteropServices.OptionalAttribute", "System.Runtime.InteropServices.PreserveSigAttribute")),
        ("pinvoke-dllimport", "PInvoke DllImport", ("System.Runtime.InteropServices.DllImportAttribute", "System.Runtime.InteropServices.LibraryImportAttribute")),
        ("native-memory-pointers", "Native Memory And Pointers", ("System.Runtime.InteropServices.Marshal", "System.IO.UnmanagedMemoryAccessor")),
        ("handles-safehandle-gchandle", "Handles SafeHandle GCHandle", ("System.Runtime.InteropServices.SafeHandle", "System.Runtime.InteropServices.CriticalHandle", "System.Runtime.InteropServices.GCHandle")),
        ("com-types", "COM Types", ("System.Runtime.InteropServices.ComTypes.",)),
        ("com-wrappers", "COM Wrappers", ("System.Runtime.InteropServices.ComWrappers",)),
        ("generated-marshalling", "Generated Marshalling", ("System.Runtime.InteropServices.Marshalling.",)),
        ("custom-marshaller-contracts", "Custom Marshaller Contracts", ("System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute",)),
        ("function-pointers", "Function Pointers", ("System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute",)),
        ("unmanaged-calling-conventions", "Unmanaged Calling Conventions", ("System.Runtime.InteropServices.CallingConvention", "System.Runtime.InteropServices.SuppressGCTransitionAttribute")),
        ("objective-c-interop", "Objective-C Interop", ("System.Runtime.InteropServices.ObjectiveC.",)),
        ("exception-errors", "Interop Exceptions And Errors", ("System.Runtime.InteropServices.COMException", "System.Runtime.InteropServices.ExternalException", "System.Runtime.InteropServices.SEHException", "System.DllNotFoundException")),
        ("secure-string-marshal", "SecureString Marshal", ("System.Security.SecureStringMarshal",)),
        ("runtime-interop-services", "Runtime Interop Services", ("System.Runtime.InteropServices.RuntimeInformation", "System.Runtime.InteropServices.OSPlatform", "System.Runtime.InteropServices.Architecture")),
    ],
    "System.Runtime.Serialization.Formatters": [
        ("formatter-base", "Formatter Base", ("System.Runtime.Serialization.Formatter", "System.Runtime.Serialization.IFormatter")),
        ("binary-formatter", "Binary Formatter", ("System.Runtime.Serialization.Formatters.Binary.BinaryFormatter",)),
        ("binder-type-style", "Binder And Type Style", ("System.Runtime.Serialization.SerializationBinder", "System.Runtime.Serialization.Formatters.FormatterAssemblyStyle", "System.Runtime.Serialization.Formatters.FormatterTypeStyle", "System.Runtime.Serialization.Formatters.TypeFilterLevel")),
        ("surrogate-selection", "Surrogate Selection", ("System.Runtime.Serialization.ISerializationSurrogate", "System.Runtime.Serialization.ISurrogateSelector", "System.Runtime.Serialization.SurrogateSelector")),
        ("object-manager-id-generator", "Object Manager And ID Generator", ("System.Runtime.Serialization.ObjectManager", "System.Runtime.Serialization.ObjectIDGenerator")),
        ("formatter-services-converter", "Formatter Services And Converter", ("System.Runtime.Serialization.FormatterServices", "System.Runtime.Serialization.FormatterConverter", "System.Runtime.Serialization.Formatters.IFieldInfo", "System.Runtime.Serialization.SerializationObjectManager")),
    ],
    "System.Security.Principal.Windows": [
        ("identity-reference", "Identity Reference", ("System.Security.Principal.IdentityReference", "System.Security.Principal.IdentityReferenceCollection")),
        ("nt-account", "NT Account", ("System.Security.Principal.NTAccount",)),
        ("security-identifier", "Security Identifier", ("System.Security.Principal.SecurityIdentifier",)),
        ("token-access", "Token Access", ("System.Security.Principal.TokenAccessLevels", "Microsoft.Win32.SafeHandles.SafeAccessTokenHandle")),
        ("windows-identity", "Windows Identity", ("System.Security.Principal.WindowsIdentity",)),
        ("windows-principal-roles", "Windows Principal Roles", ("System.Security.Principal.WindowsPrincipal", "System.Security.Principal.WindowsBuiltInRole", "System.Security.Principal.WindowsAccountType")),
    ],
    "System.Text.Json": [
        ("document-element", "JSON Document Element", ("System.Text.Json.JsonDocument", "System.Text.Json.JsonElement")),
        ("serializer", "JSON Serializer", ("System.Text.Json.JsonSerializer",)),
        ("serializer-options", "JSON Serializer Options", ("System.Text.Json.JsonSerializerOptions", "System.Text.Json.JsonNamingPolicy")),
        ("reader", "JSON Reader", ("System.Text.Json.Utf8JsonReader",)),
        ("writer", "JSON Writer", ("System.Text.Json.Utf8JsonWriter",)),
        ("converters", "JSON Converters", ("System.Text.Json.Serialization.JsonConverter",)),
        ("converter-factory", "JSON Converter Factory", ("System.Text.Json.Serialization.JsonConverterFactory",)),
        ("attributes", "JSON Attributes", ("System.Text.Json.Serialization.JsonAttribute",)),
        ("source-generation-context", "JSON Source Generation Context", ("System.Text.Json.Serialization.JsonSerializerContext",)),
        ("metadata-typeinfo", "JSON Metadata TypeInfo", ("System.Text.Json.Serialization.Metadata.JsonTypeInfo",)),
        ("nodes", "JSON Nodes", ("System.Text.Json.Nodes.JsonNode", "System.Text.Json.Nodes.JsonObject", "System.Text.Json.Nodes.JsonArray", "System.Text.Json.Nodes.JsonValue")),
        ("schema", "JSON Schema", ("System.Text.Json.Schema.JsonSchemaExporter",)),
        ("polymorphism-reference", "JSON Polymorphism And Reference", ("System.Text.Json.Serialization.ReferenceHandler", "System.Text.Json.Serialization.ReferenceResolver")),
    ],
    "System.Threading.Tasks.Parallel": [
        ("parallel-apis", "Parallel APIs", ("System.Threading.Tasks.Parallel",)),
        ("parallel-options", "Parallel Options", ("System.Threading.Tasks.ParallelOptions",)),
        ("loop-state", "Parallel Loop State", ("System.Threading.Tasks.ParallelLoopState",)),
        ("loop-result", "Parallel Loop Result", ("System.Threading.Tasks.ParallelLoopResult",)),
    ],
}

# LINQ-style anchor-bundle DLLs (System.Linq already done, this is for reference)
ANCHOR_BUNDLE_RULES: dict[str, dict[str, Any]] = {}


def _clr_type_to_subject_id(type_full_name: str) -> str:
    """Convert CLR type name with brackets to curly braces for subject ID format."""
    return type_full_name.replace("[", "{").replace("]", "}")


def _build_method_subject_id(
    asm_name: str,
    declaring_type: str,
    method_name: str,
    generic_args: list[str],
    param_types: list[str],
    return_type: str,
) -> str:
    """Build CLR method subject ID in format:
    AssemblyName/FullTypeName::Method{GenArgs}:ReturnType(Params)
    """
    gen_part = ""
    if generic_args:
        args = ",".join(_clr_type_to_subject_id(a) for a in generic_args)
        gen_part = "{" + args + "}"

    params_part = ",".join(_clr_type_to_subject_id(p) for p in param_types)
    ret_part = _clr_type_to_subject_id(return_type)

    return f"{asm_name}/{declaring_type}::{method_name}{gen_part}:{ret_part}({params_part})"


def _type_matches_marker(type_full_name: str, marker: str) -> bool:
    """Check if a type matches a marker prefix rule."""
    if marker.endswith("."):
        # Namespace prefix match (e.g., "System.Runtime.InteropServices.ComTypes.")
        return type_full_name.startswith(marker)
    # Exact type name match (with possible generic backtick)
    base_name = type_full_name.split("`")[0]
    return base_name == marker or type_full_name.startswith(marker + "`")


def generate_csharp_extractor() -> str:
    """Generate C# code that extracts all public methods from target assemblies."""
    return r'''
using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;

string[] targetAssemblies = new string[] {
    "System.Formats.Asn1",
    "System.IO.Compression.Brotli",
    "System.IO.Compression.ZipFile",
    "System.IO.Pipelines",
    "System.Net.ServerSentEvents",
    "System.ObjectModel",
    "System.Runtime.InteropServices",
    "System.Runtime.Serialization.Formatters",
    "System.Security.Principal.Windows",
    "System.Text.Json",
    "System.Threading.Tasks.Parallel",
};

string refDir = @"C:\Program Files\dotnet\shared\Microsoft.NETCore.App\10.0.6";

// Scan target assemblies
foreach (var asmName in targetAssemblies)
{
    var path = Path.Combine(refDir, asmName + ".dll");
    if (!File.Exists(path)) continue;
    ScanAssembly(asmName, path);
}

// Cross-assembly: extract interop types from System.Private.CoreLib
// but tag them as belonging to System.Runtime.InteropServices.
// These types (Marshal, SafeHandle, GCHandle, etc.) live in CoreLib
// but logically belong to the Interop verification domain.
// Scan CoreLib for Interop types (tagged as System.Runtime.InteropServices)
// CoreLib is already loaded by the runtime — use typeof(int).Assembly
{
    Assembly corelib = typeof(int).Assembly;
    string[] interopTypes = new string[] {
        "System.Runtime.InteropServices.Marshal",
        "System.Runtime.InteropServices.SafeHandle",
        "System.Runtime.InteropServices.CriticalHandle",
        "System.Runtime.InteropServices.GCHandle",
        "System.Runtime.InteropServices.ComWrappers",
        "System.Runtime.InteropServices.DllImportAttribute",
        "System.Runtime.InteropServices.LibraryImportAttribute",
        "System.Runtime.InteropServices.StructLayoutAttribute",
        "System.Runtime.InteropServices.MarshalAsAttribute",
        "System.Runtime.InteropServices.InAttribute",
        "System.Runtime.InteropServices.OutAttribute",
        "System.Runtime.InteropServices.OptionalAttribute",
        "System.Runtime.InteropServices.PreserveSigAttribute",
        "System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute",
        "System.Runtime.InteropServices.CallingConvention",
        "System.Runtime.InteropServices.SuppressGCTransitionAttribute",
        "System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal",
        "System.Runtime.InteropServices.COMException",
        "System.Runtime.InteropServices.ExternalException",
        "System.Runtime.InteropServices.SEHException",
        "System.Runtime.InteropServices.RuntimeInformation",
        "System.Runtime.InteropServices.OSPlatform",
        "System.Runtime.InteropServices.Architecture",
        "System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute",
        "System.DllNotFoundException",
        "System.IO.UnmanagedMemoryAccessor",
    };

    try
    {
        foreach (var tn in interopTypes)
        {
            Type? t = corelib.GetType(tn);
            if (t == null) continue;
            EmitTypeMethods("System.Runtime.InteropServices", t);
        }
    }
    catch (Exception ex) { Console.Error.WriteLine($"ERROR reflecting CoreLib interop types: {ex.Message}"); }
}
static void ScanAssembly(string asmName, string path)
{
    Assembly asm;
    try { asm = Assembly.LoadFrom(path); }
    catch (Exception ex) { Console.Error.WriteLine($"ERROR loading {asmName}: {ex.Message}"); return; }

    try
    {
        var types = asm.GetExportedTypes();
        foreach (var t in types.OrderBy(tt => tt.FullName))
            EmitTypeMethods(asmName, t);
    }
    catch (Exception ex) { Console.Error.WriteLine($"ERROR reflecting {asmName}: {ex.Message}"); }
}

static void EmitTypeMethods(string outputAsmName, Type t)
{
    string fullName = t.FullName ?? t.Name;

    var methods = t.GetMethods(BindingFlags.Public | BindingFlags.Static | BindingFlags.Instance | BindingFlags.DeclaredOnly);
    foreach (var m in methods.OrderBy(mm => mm.Name))
    {
        var parms = string.Join(",", m.GetParameters()
            .Select(p => FormatType(p.ParameterType)));
        var ret = FormatType(m.ReturnType);
        var genArgs = m.IsGenericMethodDefinition
            ? string.Join(",", m.GetGenericArguments().Select(FormatType))
            : "";
        Console.WriteLine(outputAsmName + "|" + fullName + "|" + m.Name + "|" + genArgs + "|" + parms + "|" + ret);
    }

    // Also include constructors
    var ctors = t.GetConstructors(BindingFlags.Public | BindingFlags.Instance | BindingFlags.DeclaredOnly);
    foreach (var c in ctors.OrderBy(cc => cc.Name))
    {
        var parms = string.Join(",", c.GetParameters()
            .Select(p => FormatType(p.ParameterType)));
        Console.WriteLine(outputAsmName + "|" + fullName + "|" + ".ctor" + "||" + parms + "|System.Void");
    }

    // Include properties (getter/setter as methods)
    var props = t.GetProperties(BindingFlags.Public | BindingFlags.Static | BindingFlags.Instance | BindingFlags.DeclaredOnly);
    foreach (var p in props)
    {
        if (p.CanRead)
            Console.WriteLine(outputAsmName + "|" + fullName + "|get_" + p.Name + "|||" + FormatType(p.PropertyType));
        if (p.CanWrite)
            Console.WriteLine(outputAsmName + "|" + fullName + "|set_" + p.Name + "||" + FormatType(p.PropertyType) + "|System.Void");
    }
}

static string FormatType(Type t)
{
    if (t == null) return "";
    if (t.IsGenericType)
    {
        var def = t.GetGenericTypeDefinition();
        var name = def.FullName ?? def.Name;
        var idx = name.IndexOf('`');
        if (idx >= 0) name = name.Substring(0, idx);
        var args = string.Join(",", t.GetGenericArguments().Select(FormatType));
        return name + "[" + args + "]";
    }
    if (t.IsByRef)
        return FormatType(t.GetElementType()!) + "&";
    if (t.IsArray)
        return FormatType(t.GetElementType()!) + "[]";
    if (t.IsPointer)
        return FormatType(t.GetElementType()!) + "*";
    return t.FullName ?? t.Name;
}
'''


def extract_methods(assembly_names: list[str] | None = None) -> list[dict[str, Any]]:
    """Run C# extractor and return list of method dicts."""
    code = generate_csharp_extractor()
    tmp_dir = Path.home() / "AppData" / "Local" / "Temp" / "fdll_extract"
    tmp_dir.mkdir(parents=True, exist_ok=True)

    (tmp_dir / "Program.cs").write_text(code, encoding="utf-8")
    csproj = tmp_dir / "fdll_extract.csproj"
    if not csproj.exists():
        csproj.write_text(
            '<Project Sdk="Microsoft.NET.Sdk">\n'
            "  <PropertyGroup>\n"
            "    <OutputType>Exe</OutputType>\n"
            "    <TargetFramework>net10.0</TargetFramework>\n"
            "  </PropertyGroup>\n"
            "</Project>\n",
            encoding="utf-8",
        )

    print("  Running C# reflection extractor...")
    result = subprocess.run(
        ["dotnet", "run", "--project", str(tmp_dir)],
        capture_output=True, text=True, timeout=300,
    )

    if result.stderr and result.stderr.strip():
        for line in result.stderr.strip().split("\n"):
            print(f"  C# stderr: {line}")

    if result.returncode != 0:
        print(f"  FAILED (exit {result.returncode}): {result.stderr[:500]}")
        return []

    methods = []
    for line in result.stdout.strip().split("\n"):
        line = line.strip()
        if not line:
            continue
        parts = line.split("|", 5)
        if len(parts) != 6:
            continue
        asm_name, decl_type, method_name, gen_args_str, params_str, ret_type = parts
        gen_args = [g.strip() for g in gen_args_str.split(",") if g.strip()]
        param_types = [p.strip() for p in params_str.split(",") if p.strip()]
        if assembly_names is None or asm_name in assembly_names:
            methods.append({
                "assembly": asm_name,
                "declaring_type": decl_type,
                "name": method_name,
                "gen_args": gen_args,
                "param_types": param_types,
                "return_type": ret_type,
            })

    return methods


def categorize_methods_by_family(
    asm_name: str,
    methods: list[dict[str, Any]],
) -> dict[str, list[dict[str, Any]]]:
    """Categorize methods by family based on type-name marker rules."""
    if asm_name not in MARKER_RULES:
        print(f"  WARNING: No marker rules for {asm_name}")
        return {}

    rules = MARKER_RULES[asm_name]
    families: dict[str, list[dict[str, Any]]] = {}

    for slug, display_name, markers in rules:
        families[slug] = []

    for m in methods:
        decl_type = m["declaring_type"]
        for slug, display_name, markers in rules:
            for marker in markers:
                if _type_matches_marker(decl_type, marker):
                    families[slug].append(m)
                    break
            else:
                continue
            break  # Method assigned to first matching family

    return families


def generate_contract(
    asm_name: str,
    family_slug: str,
    display_name: str,
    methods: list[dict[str, Any]],
) -> dict[str, Any]:
    """Generate capability-family-contract.json for a family."""
    subject_ids = []
    for m in methods:
        sid = _build_method_subject_id(
            asm_name=asm_name,
            declaring_type=m["declaring_type"],
            method_name=m["name"],
            generic_args=m["gen_args"],
            param_types=m["param_types"],
            return_type=m["return_type"],
        )
        subject_ids.append(sid)

    # Mark methods with generic args or Func/Action params as custom entries
    custom_indices = []
    for i, m in enumerate(methods):
        if m["gen_args"]:
            custom_indices.append(i)
        elif any("System.Func" in p or "System.Action" in p for p in m["param_types"]):
            custom_indices.append(i)
        elif any("System.Func" in p or "System.Action" in p for p in m["return_type"]):
            custom_indices.append(i)

    # Deduplicate subject IDs (overloaded methods may produce duplicates)
    seen = set()
    unique_ids = []
    for sid in subject_ids:
        if sid not in seen:
            seen.add(sid)
            unique_ids.append(sid)

    return {
        "schemaVersion": 1,
        "assemblyName": asm_name,
        "ownerSubjectId": "SolutionCorePack",
        "familyId": f"family/{asm_name}/{family_slug}",
        "displayName": display_name,
        "methodSubjectIds": unique_ids,
        "methodSubjectCount": len(unique_ids),
        "customEntryIndices": custom_indices,
        "functionalObligation": {"required": True},
        "performanceObligation": {"required": False},
        "hotupdateFunctionalObligation": {"required": False},
        "hotupdatePerformanceObligation": {"required": False},
        "reviewBundleRequired": True,
    }


def update_ledger_mids(
    asm_name: str,
    family_data: dict[str, dict[str, Any]],
) -> int:
    """Update the capability-family-ledger.json with methodSubjectIds for a DLL."""
    ledger = json.loads(_LEDGER_PATH.read_text(encoding="utf-8"))

    total_mids = 0
    for dll in ledger.get("dlls", []):
        if dll["assemblyName"] != asm_name:
            continue
        for family in dll.get("families", []):
            fid = family["familyId"]
            slug = fid.split("/")[-1]
            if slug in family_data:
                contract = family_data[slug]
                mids = contract["methodSubjectIds"]
                family["methodSubjectIds"] = mids
                family["methodCount"] = len(mids)
                total_mids += len(mids)
                print(f"    {slug:40s} {len(mids):3d} MIDs")
            else:
                print(f"    {slug:40s} SKIPPED (no extraction data)")

    _LEDGER_PATH.write_text(json.dumps(ledger, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return total_mids


def write_contracts(
    asm_name: str,
    family_data: dict[str, dict[str, Any]],
) -> None:
    """Write capability-family-contract.json files for each family."""
    asm_dir = _REPO_ROOT / "testing" / "foundation-dll" / asm_name
    for slug, contract in family_data.items():
        fam_dir = asm_dir / slug
        fam_dir.mkdir(parents=True, exist_ok=True)
        contract_path = fam_dir / "capability-family-contract.json"
        contract_path.write_text(json.dumps(contract, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")


def process_assembly(asm_name: str, all_methods: list[dict[str, Any]]) -> int:
    """Process one assembly: categorize, generate contracts, update ledger."""
    print(f"\n{'='*60}")
    print(f"Processing {asm_name}")
    print(f"{'='*60}")

    asm_methods = [m for m in all_methods if m["assembly"] == asm_name]
    print(f"  Total methods extracted: {len(asm_methods)}")

    if asm_name not in MARKER_RULES:
        print(f"  SKIPPED: no marker rules for {asm_name}")
        return 0

    families = categorize_methods_by_family(asm_name, asm_methods)

    # Show categorization stats
    total_categorized = 0
    for slug, display_name, markers in MARKER_RULES[asm_name]:
        count = len(families.get(slug, []))
        total_categorized += count
        print(f"    {slug:40s} {count:3d} methods")

    unmapped = len(asm_methods) - total_categorized
    if unmapped > 0:
        print(f"    {'(unmapped)':40s} {unmapped:3d} methods")

    # Generate contracts
    family_data: dict[str, dict[str, Any]] = {}
    for slug, display_name, markers in MARKER_RULES[asm_name]:
        fam_methods = families.get(slug, [])
        if not fam_methods:
            print(f"    -> {slug}: EMPTY (0 methods), skipping contract")
            continue
        contract = generate_contract(asm_name, slug, display_name, fam_methods)
        family_data[slug] = contract

    # Write contracts and update ledger
    if family_data:
        write_contracts(asm_name, family_data)
        total = update_ledger_mids(asm_name, family_data)
        print(f"  -> {len(family_data)} families, {total} total MIDs written")
    else:
        print(f"  -> No families with methods, nothing written")

    return len(family_data)


def main() -> int:
    """Main entry point."""
    print("=" * 60)
    print("Foundation DLL Method Extraction & Contract Generation")
    print("=" * 60)
    print(f"Target assemblies: {', '.join(TARGET_ASSEMBLIES)}")

    # Step 1: Extract all methods via C# reflection
    print("\n[Step 1] Extracting methods from .NET 10 reference assemblies...")
    all_methods = extract_methods()
    print(f"  Total: {len(all_methods)} methods across all assemblies")

    # Step 2: Process each assembly
    print("\n[Step 2] Categorizing methods and generating contracts...")
    total_families = 0
    for asm_name in TARGET_ASSEMBLIES:
        families = process_assembly(asm_name, all_methods)
        total_families += families

    print(f"\n{'='*60}")
    print(f"Done! Processed {total_families} families across {len(TARGET_ASSEMBLIES)} DLLs")
    print(f"{'='*60}")
    return 0


if __name__ == "__main__":
    sys.exit(main())
