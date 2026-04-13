from __future__ import annotations

import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
ANDROID_ROOT = REPO_ROOT / "src" / "mobile" / "android"


class Phase4MobileRuntimeHostBatch2AndroidTests(unittest.TestCase):
    def test_android_host_skeleton_files_exist_and_cmake_reuses_shared_host(self) -> None:
        required_paths = [
            ANDROID_ROOT / "CMakeLists.txt",
            ANDROID_ROOT / "jni" / "mobile_host_jni.cpp",
            ANDROID_ROOT / "app" / "build.gradle.kts",
            ANDROID_ROOT / "app" / "src" / "main" / "AndroidManifest.xml",
            ANDROID_ROOT / "app" / "src" / "main" / "java" / "com" / "chaos" / "mobilehost" / "MainActivity.kt",
        ]

        for required_path in required_paths:
            self.assertTrue(required_path.is_file(), required_path)

        cmake_text = (ANDROID_ROOT / "CMakeLists.txt").read_text(encoding="utf-8")
        for required_fragment in [
            "chaos_mobile_android_host",
            "jni/mobile_host_jni.cpp",
            "src/mobile/shared/host_main.cpp",
            "src/mobile/shared/result_reporter.cpp",
        ]:
            self.assertIn(required_fragment, cmake_text)

    def test_android_main_activity_loads_native_library_and_invokes_run_host(self) -> None:
        activity_text = (
            ANDROID_ROOT
            / "app"
            / "src"
            / "main"
            / "java"
            / "com"
            / "chaos"
            / "mobilehost"
            / "MainActivity.kt"
        ).read_text(encoding="utf-8")

        for required_fragment in [
            "class MainActivity",
            'System.loadLibrary("chaos_mobile_android_host")',
            "external fun runHost(): Int",
            "runHost()",
        ]:
            self.assertIn(required_fragment, activity_text)

    def test_android_jni_bridge_forwards_to_shared_il2cpp_host_run(self) -> None:
        jni_text = (ANDROID_ROOT / "jni" / "mobile_host_jni.cpp").read_text(encoding="utf-8")

        for required_fragment in [
            "#include <jni.h>",
            '#include "host_main.h"',
            "Java_com_chaos_mobilehost_MainActivity_runHost",
            "il2cpp_host_run(0, nullptr)",
        ]:
            self.assertIn(required_fragment, jni_text)


if __name__ == "__main__":
    unittest.main()
