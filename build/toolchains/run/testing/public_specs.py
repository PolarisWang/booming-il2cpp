from __future__ import annotations


PUBLIC_TEST_SPECS = [
    {
        "id": "smoke/HelloWorld",
        "family": "smoke",
        "suite": "HelloWorld",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {
            "build": "build-smoke-helloworld",
            "run": "test-smoke-helloworld",
            "all": "test-smoke-helloworld",
        },
        "level": "code",
        "primaryModuleId": "managed-smoke",
        "moduleIds": ["managed-smoke"],
        "subsystemIds": ["managed-execution"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "smoke/GenericEcho",
        "family": "smoke",
        "suite": "GenericEcho",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {
            "build": "build-smoke-genericecho",
            "run": "test-smoke-genericecho",
            "all": "test-smoke-genericecho",
        },
        "level": "code",
        "primaryModuleId": "managed-smoke",
        "moduleIds": ["managed-smoke"],
        "subsystemIds": ["managed-execution"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "smoke/ReflectionLite",
        "family": "smoke",
        "suite": "ReflectionLite",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {
            "build": "build-smoke-reflectionlite",
            "run": "test-smoke-reflectionlite",
            "all": "test-smoke-reflectionlite",
        },
        "level": "code",
        "primaryModuleId": "reflection",
        "moduleIds": ["reflection"],
        "subsystemIds": ["managed-execution"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "smoke/PInvokeLite",
        "family": "smoke",
        "suite": "PInvokeLite",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {
            "build": "build-smoke-pinvokelite",
            "run": "test-smoke-pinvokelite",
            "all": "test-smoke-pinvokelite",
        },
        "level": "code",
        "primaryModuleId": "interop",
        "moduleIds": ["interop"],
        "subsystemIds": ["native-bridge"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "smoke/HostEmbeddingLite",
        "family": "smoke",
        "suite": "HostEmbeddingLite",
        "stages": ["all", "build", "run"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {
            "build": "build-smoke-hostembeddinglite",
            "run": "test-smoke-hostembeddinglite",
            "all": "test-smoke-hostembeddinglite",
        },
        "level": "code",
        "primaryModuleId": "hosted-runtime",
        "moduleIds": ["hosted-runtime", "managed-smoke"],
        "subsystemIds": ["hosting", "managed-execution"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "contract/analysis-schema",
        "family": "contract",
        "suite": "analysis-schema",
        "stages": ["all"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"all": "test-contract-analysis-schema"},
        "level": "code",
        "primaryModuleId": "analysis",
        "moduleIds": ["analysis"],
        "subsystemIds": ["analysis-contracts"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "contract/managed-closure-bundle",
        "family": "contract",
        "suite": "managed-closure-bundle",
        "stages": ["all"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"all": "test-contract-managed-closure-bundle"},
        "level": "code",
        "primaryModuleId": "managed-closure",
        "moduleIds": ["managed-closure"],
        "subsystemIds": ["managed-pipeline"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "contract/trace-schema",
        "family": "contract",
        "suite": "trace-schema",
        "stages": ["all"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"all": "test-contract-trace-schema"},
        "level": "code",
        "primaryModuleId": "trace-export",
        "moduleIds": ["trace-export"],
        "subsystemIds": ["hosting"],
        "docRefs": ["wiki/06-测试验证/模块/trace-export.md"],
    },
    {
        "id": "contract/native-abi",
        "family": "contract",
        "suite": "native-abi",
        "stages": ["all"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"all": "build-native-contract-abi"},
        "level": "code",
        "primaryModuleId": "interop",
        "moduleIds": ["interop"],
        "subsystemIds": ["native-bridge"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "contract/native-bridge",
        "family": "contract",
        "suite": "native-bridge",
        "stages": ["all"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"all": "build-native-contract-bridge"},
        "level": "code",
        "primaryModuleId": "interop",
        "moduleIds": ["interop"],
        "subsystemIds": ["native-bridge"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "contract/trace-compare-windows",
        "family": "contract",
        "suite": "trace-compare-windows",
        "stages": ["all", "run"],
        "supported_hosts": ["windows"],
        "legacy_commands": {"run": "test-trace-compare-windows", "all": "test-trace-compare-windows"},
        "level": "code",
        "primaryModuleId": "trace-export",
        "moduleIds": ["trace-export"],
        "subsystemIds": ["hosting"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "contract/trace-compare-macos",
        "family": "contract",
        "suite": "trace-compare-macos",
        "stages": ["all", "run"],
        "supported_hosts": ["macos"],
        "legacy_commands": {"run": "test-trace-compare-macos", "all": "test-trace-compare-macos"},
        "level": "code",
        "primaryModuleId": "trace-export",
        "moduleIds": ["trace-export"],
        "subsystemIds": ["hosting"],
        "docRefs": ["wiki/06-测试验证/模块映射.md"],
    },
    {
        "id": "gate/windows-reference-desktop",
        "family": "gate",
        "suite": "windows-reference-desktop",
        "stages": ["all"],
        "supported_hosts": ["windows"],
        "legacy_commands": {"all": "build-platform-windows-reference-desktop"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/整体验证场景/platform-gates.md"],
    },
    {
        "id": "gate/macos-reference-desktop",
        "family": "gate",
        "suite": "macos-reference-desktop",
        "stages": ["all"],
        "supported_hosts": ["macos"],
        "legacy_commands": {"all": "build-platform-macos-reference-desktop"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/整体验证场景/platform-gates.md"],
    },
    {
        "id": "gate/android-arm64-smoke",
        "family": "gate",
        "suite": "android-arm64-smoke",
        "stages": ["all"],
        "supported_hosts": ["windows"],
        "legacy_commands": {"all": "build-platform-android-arm64-smoke"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/整体验证场景/platform-gates.md"],
    },
    {
        "id": "gate/ios-arm64-packaging",
        "family": "gate",
        "suite": "ios-arm64-packaging",
        "stages": ["all"],
        "supported_hosts": ["macos"],
        "legacy_commands": {"all": "build-platform-ios-arm64-packaging"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/整体验证场景/platform-gates.md"],
    },
    {
        "id": "gate/linux-x64-packaging",
        "family": "gate",
        "suite": "linux-x64-packaging",
        "stages": ["all"],
        "supported_hosts": ["windows", "macos", "linux"],
        "legacy_commands": {"all": "build-platform-linux-x64-packaging"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/整体验证场景/platform-gates.md"],
    },
    {
        "id": "workflow/runtime-baseline-windows",
        "family": "workflow",
        "suite": "runtime-baseline-windows",
        "stages": ["all"],
        "supported_hosts": ["windows"],
        "legacy_commands": {"all": "test-workflow-runtime-baseline-windows"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/测试分层.md"],
    },
    {
        "id": "workflow/runtime-baseline-macos",
        "family": "workflow",
        "suite": "runtime-baseline-macos",
        "stages": ["all"],
        "supported_hosts": ["macos"],
        "legacy_commands": {"all": "test-workflow-runtime-baseline-macos"},
        "level": "system",
        "primaryModuleId": "runtime-baseline",
        "moduleIds": ["runtime-baseline"],
        "subsystemIds": ["workflow"],
        "docRefs": ["wiki/06-测试验证/测试分层.md"],
    },
]


def find_public_test_suite_spec(family: str | None, suite: str | None) -> dict | None:
    for item in PUBLIC_TEST_SPECS:
        if item["family"] == family and item["suite"] == suite:
            return item
    return None


def find_public_test_suite_by_id(suite_id: str) -> dict | None:
    for item in PUBLIC_TEST_SPECS:
        if item["id"] == suite_id:
            return item
    return None


def list_public_test_suites(host_platform: str) -> list[dict]:
    return [
        {
            "id": item["id"],
            "type": "suite",
            "family": item["family"],
            "suite": item["suite"],
            "stages": list(item["stages"]),
            "level": item.get("level", "code"),
            "primaryModuleId": item.get("primaryModuleId"),
            "moduleIds": list(item.get("moduleIds", [])),
            "subsystemIds": list(item.get("subsystemIds", [])),
            "docRefs": list(item.get("docRefs", [])),
            "supportedHosts": list(item["supported_hosts"]),
        }
        for item in PUBLIC_TEST_SPECS
        if host_platform in item["supported_hosts"]
    ]


def resolve_legacy_test_command_id(
    family: str,
    suite: str,
    *,
    stage: str,
    host_platform: str,
) -> str | None:
    suite_spec = find_public_test_suite_spec(family, suite)
    if suite_spec is None:
        return None

    if host_platform not in suite_spec["supported_hosts"]:
        return None

    if stage not in suite_spec["stages"]:
        return None

    return suite_spec["legacy_commands"].get(stage)
