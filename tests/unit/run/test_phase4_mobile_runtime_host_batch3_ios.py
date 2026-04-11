from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
IOS_ROOT = REPO_ROOT / "src" / "mobile" / "ios"


class Phase4MobileRuntimeHostBatch3IosTests(unittest.TestCase):
    def test_ios_host_skeleton_files_exist_and_cmake_reuses_shared_host(self) -> None:
        required_paths = [
            IOS_ROOT / "CMakeLists.txt",
            IOS_ROOT / "bridge" / "mobile_host_bridge.mm",
            IOS_ROOT / "app" / "AppDelegate.mm",
            IOS_ROOT / "app" / "Info.plist",
        ]

        for required_path in required_paths:
            self.assertTrue(required_path.is_file(), required_path)

        cmake_text = (IOS_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        for required_fragment in [
            "chaos_mobile_ios_host",
            "bridge/mobile_host_bridge.mm",
            "src/mobile/shared/host_main.cpp",
            "src/mobile/shared/result_reporter.cpp",
        ]:
            self.assertIn(required_fragment, cmake_text)

    def test_ios_app_delegate_invokes_bridge_entry(self) -> None:
        app_delegate_text = (IOS_ROOT / "app" / "AppDelegate.mm").read_text(encoding="utf-8")

        for required_fragment in [
            "@interface AppDelegate",
            "UIApplicationDelegate",
            "chaos_mobile_ios_run_host()",
            "NSLog",
        ]:
            self.assertIn(required_fragment, app_delegate_text)

    def test_ios_bridge_forwards_to_shared_il2cpp_host_run(self) -> None:
        bridge_text = (IOS_ROOT / "bridge" / "mobile_host_bridge.mm").read_text(encoding="utf-8")

        for required_fragment in [
            '#include "host_main.h"',
            "chaos_mobile_ios_run_host",
            "il2cpp_host_run(0, nullptr)",
        ]:
            self.assertIn(required_fragment, bridge_text)


if __name__ == "__main__":
    unittest.main()
