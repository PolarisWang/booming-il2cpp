from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
SHARED_ROOT = REPO_ROOT / "src" / "mobile" / "shared"
ANDROID_GATE_ROOT = REPO_ROOT / "tests" / "gate" / "android-smoke"
IOS_GATE_ROOT = REPO_ROOT / "tests" / "gate" / "ios-smoke"
ANDROID_CANONICAL_ROOT = REPO_ROOT / "tests" / "integration" / "platform-hosts" / "android-smoke"
IOS_CANONICAL_ROOT = REPO_ROOT / "tests" / "integration" / "platform-hosts" / "ios-smoke"


class MobileRuntimeHostFoundationTests(unittest.TestCase):
    def test_shared_mobile_host_files_exist_and_gate_cmake_consumes_them(self) -> None:
        required_paths = [
            SHARED_ROOT / "host_main.h",
            SHARED_ROOT / "host_main.cpp",
            SHARED_ROOT / "result_reporter.h",
            SHARED_ROOT / "result_reporter.cpp",
            ANDROID_CANONICAL_ROOT / "CMakeLists.txt",
            IOS_CANONICAL_ROOT / "CMakeLists.txt",
            ANDROID_GATE_ROOT / "CMakeLists.txt",
            IOS_GATE_ROOT / "CMakeLists.txt",
        ]

        for required_path in required_paths:
            self.assertTrue(required_path.is_file(), required_path)

        android_cmake = (ANDROID_CANONICAL_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        ios_cmake = (IOS_CANONICAL_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        android_wrapper = (ANDROID_GATE_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        ios_wrapper = (IOS_GATE_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")

        for cmake_text in [android_cmake, ios_cmake]:
            self.assertIn("src/mobile/shared/host_main.cpp", cmake_text)
            self.assertIn("src/mobile/shared/result_reporter.cpp", cmake_text)

        self.assertIn("../../integration/platform-hosts/android-smoke", android_wrapper)
        self.assertIn("../../integration/platform-hosts/ios-smoke", ios_wrapper)

    def test_shared_mobile_host_contract_exposes_il2cpp_host_run_and_reporter(self) -> None:
        host_header = (SHARED_ROOT / "host_main.h").read_text(encoding="utf-8")
        host_cpp = (SHARED_ROOT / "host_main.cpp").read_text(encoding="utf-8")
        reporter_header = (SHARED_ROOT / "result_reporter.h").read_text(encoding="utf-8")
        reporter_cpp = (SHARED_ROOT / "result_reporter.cpp").read_text(encoding="utf-8")

        for required_fragment in [
            "struct HostRunResult",
            "int exitCode;",
            'extern "C" int il2cpp_host_run(int argc, const char** argv);',
        ]:
            self.assertIn(required_fragment, host_header)

        for required_fragment in [
            '#include "host_main.h"',
            '#include "result_reporter.h"',
            "report_host_result(result);",
            'return result.exitCode;',
        ]:
            self.assertIn(required_fragment, host_cpp)

        for required_fragment in [
            '#include "host_main.h"',
            "void report_host_result(const HostRunResult& result);",
        ]:
            self.assertIn(required_fragment, reporter_header)

        for required_fragment in [
            '#include "result_reporter.h"',
            "report_host_result",
            "std::fprintf",
            "result.exitCode",
        ]:
            self.assertIn(required_fragment, reporter_cpp)

    def test_android_and_ios_gate_entries_forward_to_shared_il2cpp_host_run(self) -> None:
        android_entry = (ANDROID_CANONICAL_ROOT / "smoke_entry.cpp").read_text(encoding="utf-8")
        ios_entry = (IOS_CANONICAL_ROOT / "smoke_entry.mm").read_text(encoding="utf-8")

        for source_text in [android_entry, ios_entry]:
            self.assertIn("il2cpp_host_run(0, nullptr)", source_text)

        self.assertIn("chaos_android_startup_smoke_entry", android_entry)
        self.assertIn("chaos_ios_packaging_smoke_entry", ios_entry)


if __name__ == "__main__":
    unittest.main()
