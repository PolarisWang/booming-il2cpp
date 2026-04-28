"""Inject capabilityFamilyEnum into ledger entries to match the C# enum."""
import json
from pathlib import Path

REPO_ROOT = Path("D:/agent/booming-il2cpp")
LEDGER_PATH = REPO_ROOT / "verification" / "projections" / "foundation-dll-audit" / "capability-family-ledger.json"

# mapping: familyId -> enum name
ENUM_MAP = {
    "family/System.Private.CoreLib/convert/char": "SystemPrivateCoreLib_ConvertChar",
    "family/System.Private.CoreLib/buffer/memory": "SystemPrivateCoreLib_BufferMemory",
    "family/System.Private.CoreLib/enum/parsing": "SystemPrivateCoreLib_EnumParsing",
    "family/System.Private.CoreLib/primitive/numeric-conversions": "SystemPrivateCoreLib_PrimitiveNumericConversions",
    "family/System.Private.CoreLib/string/char-text-core": "SystemPrivateCoreLib_StringCharTextCore",
    "family/System.Private.CoreLib/array/indexing-copy": "SystemPrivateCoreLib_ArrayIndexingCopy",
    "family/System.Private.CoreLib/span/memory-buffers": "SystemPrivateCoreLib_SpanMemoryBuffers",
    "family/System.Private.CoreLib/object/equality-identity": "SystemPrivateCoreLib_ObjectEqualityIdentity",
    "family/System.Private.CoreLib/type/runtime-handles": "SystemPrivateCoreLib_TypeRuntimeHandles",
    "family/System.Private.CoreLib/exception/throw-diagnostics": "SystemPrivateCoreLib_ExceptionThrowDiagnostics",
    "family/System.Private.CoreLib/generic/nullable-value": "SystemPrivateCoreLib_GenericNullableValue",
    "family/System.Private.CoreLib/boxing/unboxing-casts": "SystemPrivateCoreLib_BoxingUnboxingCasts",
    "family/System.Private.CoreLib/delegate/core-invocation": "SystemPrivateCoreLib_DelegateCoreInvocation",
    "family/System.Private.CoreLib/reflection/member-basics": "SystemPrivateCoreLib_ReflectionMemberBasics",
    "family/System.Private.CoreLib/attributes/custom-metadata": "SystemPrivateCoreLib_AttributesCustomMetadata",
    "family/System.Private.CoreLib/threading/monitor-interlocked": "SystemPrivateCoreLib_ThreadingMonitorInterlocked",
    "family/System.Private.CoreLib/threading/tasks-primitives": "SystemPrivateCoreLib_ThreadingTasksPrimitives",
    "family/System.Private.CoreLib/time/date-time-timespan": "SystemPrivateCoreLib_TimeDateTimeTimespan",
    "family/System.Private.CoreLib/guid/random/hashcode": "SystemPrivateCoreLib_GuidRandomHashcode",
    "family/System.Private.CoreLib/math/numerics": "SystemPrivateCoreLib_MathNumerics",
    "family/System.Private.CoreLib/globalization/culture": "SystemPrivateCoreLib_GlobalizationCulture",
    "family/System.Private.CoreLib/io/streams-basics": "SystemPrivateCoreLib_IOStreamsBasics",
    "family/System.Private.CoreLib/collections/generic-core": "SystemPrivateCoreLib_CollectionsGenericCore",
    "family/System.Private.CoreLib/runtime/compiler-services": "SystemPrivateCoreLib_RuntimeCompilerServices",
}

ledger = json.loads(LEDGER_PATH.read_text(encoding="utf-8"))
updated = 0
for dll in ledger["dlls"]:
    for family in dll["families"]:
        fid = family["familyId"]
        if fid in ENUM_MAP:
            family["capabilityFamilyEnum"] = ENUM_MAP[fid]
            updated += 1

LEDGER_PATH.write_text(json.dumps(ledger, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
print(f"Updated {updated} families with capabilityFamilyEnum")
