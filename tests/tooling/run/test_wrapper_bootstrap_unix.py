from __future__ import annotations

import json
import os
import platform
import pty
import shutil
import stat
import subprocess
import sys
import tarfile
import tempfile
import unittest
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[3]
RUN_SH = REPO_ROOT / "run.sh"


def detect_host_platform_id() -> str:
    system = platform.system().lower()
    machine = platform.machine().lower()

    if system == "darwin":
        if machine in {"arm64", "aarch64"}:
            return "macos-arm64"
        if machine in {"x86_64", "amd64"}:
            return "macos-x64"
    elif system == "linux":
        if machine in {"x86_64", "amd64"}:
            return "linux-x64"
        if machine in {"arm64", "aarch64"}:
            return "linux-arm64"

    raise unittest.SkipTest(f"unsupported Unix host: system={system} machine={machine}")


def create_fake_runtime_archive(temp_root: Path) -> Path:
    archive_path = temp_root / "fake-python-runtime.tar.gz"
    payload_root = temp_root / "payload"
    python_path = payload_root / "python" / "bin" / "python3.12"
    python_path.parent.mkdir(parents=True, exist_ok=True)
    python_path.write_text(
        "#!/bin/sh\n"
        f'exec "{sys.executable}" "$@"\n',
        encoding="utf-8",
    )
    python_path.chmod(0o755)

    with tarfile.open(archive_path, "w:gz") as archive:
        archive.add(payload_root / "python", arcname="python")

    return archive_path


def build_cache_root_relative(temp_root: Path) -> str:
    return f"artifacts/.tmp_run-wrapper/{temp_root.name}/python-cache"


def run_with_tty(arguments: list[str], env: dict[str, str]) -> subprocess.CompletedProcess[str]:
    master_fd, slave_fd = pty.openpty()
    try:
        process = subprocess.Popen(
            ["bash", str(RUN_SH), *arguments],
            cwd=REPO_ROOT,
            env=env,
            stdin=slave_fd,
            stdout=slave_fd,
            stderr=slave_fd,
            text=False,
            close_fds=True,
        )
    finally:
        os.close(slave_fd)

    try:
        return_code = process.wait(timeout=30)
    finally:
        chunks: list[bytes] = []
        while True:
            try:
                chunk = os.read(master_fd, 4096)
            except OSError:
                break
            if not chunk:
                break
            chunks.append(chunk)
        os.close(master_fd)

    output = b"".join(chunks).decode("utf-8", errors="replace")
    return subprocess.CompletedProcess(process.args, return_code, output, "")


@unittest.skipUnless(os.name == "posix", "Unix wrapper tests require a POSIX host")
class UnixWrapperBootstrapTests(unittest.TestCase):
    def test_run_sh_has_executable_bit(self) -> None:
        mode = RUN_SH.stat().st_mode
        self.assertTrue(mode & stat.S_IXUSR, "run.sh should be executable on Unix hosts")

    def test_missing_runtime_requires_explicit_bootstrap(self) -> None:
        platform_id = detect_host_platform_id()

        with tempfile.TemporaryDirectory(prefix="booming-run-wrapper-") as temp_dir:
            temp_root = Path(temp_dir)
            manifest_path = temp_root / "runtime_manifest.test.json"
            cache_root = build_cache_root_relative(temp_root)
            manifest = {
                "provider": "test-runtime",
                "provider_release": "local",
                "python_version": "9.9.9",
                "cache_root": cache_root,
                "platforms": {
                    platform_id: {
                        "platform_id": platform_id,
                        "download_url": "file:///unused/runtime.tar.gz",
                        "sha256": "0" * 64,
                        "archive_type": "tar.gz",
                        "extract_dir": ".",
                        "python_relpath": "python/bin/python3.12",
                    }
                },
            }
            manifest_path.write_text(json.dumps(manifest), encoding="utf-8")
            self.addCleanup(shutil.rmtree, REPO_ROOT / "artifacts" / ".tmp_run-wrapper" / temp_root.name, True)

            env = os.environ.copy()
            env["CHAOS_RUN_RUNTIME_MANIFEST"] = str(manifest_path)
            env["CHAOS_RUN_FORCE_INTERACTIVE"] = "0"
            env["CHAOS_RUN_BOOTSTRAP_PYTHON"] = sys.executable

            result = subprocess.run(
                ["bash", str(RUN_SH), "list"],
                cwd=REPO_ROOT,
                env=env,
                capture_output=True,
                text=True,
                check=False,
            )

        output = "\n".join(part for part in (result.stdout.strip(), result.stderr.strip()) if part)

        self.assertNotEqual(0, result.returncode, "missing runtime should fail in non-interactive mode")
        self.assertIn(
            "Run 'run bootstrap --yes' first.",
            output,
            "wrapper should require explicit bootstrap when the cached runtime is absent",
        )

    def test_interactive_bootstrap_accepts_uppercase_confirmation_on_bash_3(self) -> None:
        platform_id = detect_host_platform_id()

        with tempfile.TemporaryDirectory(prefix="booming-run-wrapper-") as temp_dir:
            temp_root = Path(temp_dir)
            archive_path = create_fake_runtime_archive(temp_root)
            manifest_path = temp_root / "runtime_manifest.test.json"
            cache_root = build_cache_root_relative(temp_root)
            manifest = {
                "provider": "test-runtime",
                "provider_release": "local",
                "python_version": "9.9.9",
                "cache_root": cache_root,
                "platforms": {
                    platform_id: {
                        "platform_id": platform_id,
                        "download_url": archive_path.as_uri(),
                        "sha256": subprocess.check_output(
                            ["shasum", "-a", "256", str(archive_path)],
                            text=True,
                        ).split()[0],
                        "archive_type": "tar.gz",
                        "extract_dir": ".",
                        "python_relpath": "python/bin/python3.12",
                    }
                },
            }
            manifest_path.write_text(json.dumps(manifest), encoding="utf-8")
            self.addCleanup(shutil.rmtree, REPO_ROOT / "artifacts" / ".tmp_run-wrapper" / temp_root.name, True)
            runtime_python = (
                REPO_ROOT
                / cache_root
                / platform_id
                / "9.9.9"
                / "python"
                / "bin"
                / "python3.12"
            )

            env = os.environ.copy()
            env["CHAOS_RUN_RUNTIME_MANIFEST"] = str(manifest_path)
            env["CHAOS_RUN_CONFIRM_RESPONSE"] = "Y"
            env["CHAOS_RUN_BOOTSTRAP_PYTHON"] = sys.executable

            result = run_with_tty(["bootstrap"], env)

        self.assertEqual(0, result.returncode, "interactive bootstrap should succeed with uppercase confirmation")
        self.assertTrue(runtime_python.is_file(), "interactive bootstrap should materialize the cached runtime")
        self.assertNotIn(
            "bad substitution",
            result.stdout,
            "interactive bootstrap should stay compatible with Bash 3.x",
        )


if __name__ == "__main__":
    unittest.main()
