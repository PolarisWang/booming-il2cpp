"""device_detector.py — Auto-detect device info for benchmark record metadata.

Supports Windows, macOS, Android (via adb), iOS (via xcrun).
Results are cached in artifacts/.device-cache.json for 1 hour.
"""
from __future__ import annotations

import json
import platform
import subprocess
import time
from pathlib import Path
from typing import Any

_CACHE_TTL_SECONDS = 3600  # 1 hour


# ---------------------------------------------------------------------------
# CPU name normalisation
# ---------------------------------------------------------------------------

def _normalize_cpu(raw: str) -> str:
    """Lower-case, spaces to hyphens, remove special characters."""
    import re
    s = raw.strip().lower()
    s = re.sub(r"[^a-z0-9\- ]+", "", s)
    s = re.sub(r"\s+", "-", s)
    s = re.sub(r"-+", "-", s).strip("-")
    return s


def _make_device_id(plat: str, arch: str, cpu: str) -> str:
    return f"{plat}-{arch}-{_normalize_cpu(cpu)}"


# ---------------------------------------------------------------------------
# Windows
# ---------------------------------------------------------------------------

def _detect_windows() -> dict[str, Any]:
    cpu = "unknown-cpu"
    try:
        out = subprocess.check_output(
            ["wmic", "cpu", "get", "Name", "/value"],
            text=True,
            stderr=subprocess.DEVNULL,
            timeout=5,
        )
        for line in out.splitlines():
            if line.startswith("Name="):
                cpu = line.split("=", 1)[1].strip()
                break
    except Exception:
        cpu = platform.processor() or "unknown-cpu"

    arch = "x64" if platform.machine() in ("AMD64", "x86_64") else platform.machine().lower()
    os_name = f"Windows {platform.version()}"
    device_id = _make_device_id("windows", arch, cpu)
    return {
        "id": device_id,
        "name": f"Windows PC ({cpu})",
        "os": os_name,
        "cpu": cpu,
        "arch": arch,
        "isSimulator": False,
    }


# ---------------------------------------------------------------------------
# macOS
# ---------------------------------------------------------------------------

def _detect_macos() -> dict[str, Any]:
    cpu = "unknown-cpu"
    try:
        cpu = subprocess.check_output(
            ["sysctl", "-n", "machdep.cpu.brand_string"],
            text=True, timeout=5,
        ).strip()
    except Exception:
        cpu = platform.processor() or "unknown-cpu"

    arch = "arm64" if platform.machine() == "arm64" else "x86-64"
    mac_ver = platform.mac_ver()[0]
    os_name = f"macOS {mac_ver}"
    device_id = _make_device_id("macos", arch, cpu)
    return {
        "id": device_id,
        "name": f"Mac ({cpu})",
        "os": os_name,
        "cpu": cpu,
        "arch": arch,
        "isSimulator": False,
    }


# ---------------------------------------------------------------------------
# Linux
# ---------------------------------------------------------------------------

def _detect_linux() -> dict[str, Any]:
    cpu = platform.processor() or "unknown-cpu"
    try:
        with open("/proc/cpuinfo") as f:
            for line in f:
                if line.startswith("model name"):
                    cpu = line.split(":", 1)[1].strip()
                    break
    except Exception:
        pass

    arch = "x64" if platform.machine() == "x86_64" else platform.machine().lower()
    os_name = f"Linux {platform.release()}"
    device_id = _make_device_id("linux", arch, cpu)
    return {
        "id": device_id,
        "name": f"Linux ({cpu})",
        "os": os_name,
        "cpu": cpu,
        "arch": arch,
        "isSimulator": False,
    }


# ---------------------------------------------------------------------------
# Android (via adb)
# ---------------------------------------------------------------------------

def detect_android_device(serial: str | None = None) -> dict[str, Any]:
    """Detect Android device info via adb."""
    def _adb(prop: str) -> str:
        cmd = ["adb"]
        if serial:
            cmd += ["-s", serial]
        cmd += ["shell", "getprop", prop]
        try:
            return subprocess.check_output(cmd, text=True, timeout=5).strip()
        except Exception:
            return ""

    model = _adb("ro.product.model") or "Android Device"
    cpu = _adb("ro.hardware") or _adb("ro.product.board") or "unknown-cpu"
    abi = _adb("ro.product.cpu.abi") or "arm64-v8a"
    arch = "arm64" if "arm64" in abi else abi.replace("-", "").replace("_", "-")
    characteristics = _adb("ro.build.characteristics")
    is_simulator = "emulator" in characteristics.lower()
    os_ver = _adb("ro.build.version.release") or "unknown"
    plat = "android-sim" if is_simulator else "android"
    device_id = _make_device_id(plat, arch, cpu)
    return {
        "id": device_id,
        "name": f"{model} ({cpu})",
        "os": f"Android {os_ver}",
        "cpu": cpu,
        "arch": arch,
        "isSimulator": is_simulator,
    }


# ---------------------------------------------------------------------------
# iOS (via xcrun)
# ---------------------------------------------------------------------------

def detect_ios_device(udid: str | None = None) -> dict[str, Any]:
    """Detect iOS device / simulator info."""
    # Try real device via devicectl
    if udid:
        try:
            raw = subprocess.check_output(
                ["xcrun", "devicectl", "list", "devices", "--json"],
                text=True, timeout=10,
            )
            data = json.loads(raw)
            for dev in data.get("result", {}).get("devices", []):
                if udid in (dev.get("udid") or ""):
                    props = dev.get("deviceProperties", {})
                    hw = dev.get("hardwareProperties", {})
                    model = props.get("name", "iPhone")
                    cpu_type = hw.get("cpuType", {}).get("name", "arm64")
                    os_ver = props.get("osVersionNumber", "unknown")
                    chip = hw.get("platformIdentifier", cpu_type)
                    device_id = _make_device_id("ios", "arm64", chip)
                    return {
                        "id": device_id,
                        "name": f"{model} ({chip})",
                        "os": f"iOS {os_ver}",
                        "cpu": chip,
                        "arch": "arm64",
                        "isSimulator": False,
                    }
        except Exception:
            pass

    # Fall back to simulator list
    try:
        raw = subprocess.check_output(
            ["xcrun", "simctl", "list", "devices", "--json"],
            text=True, timeout=10,
        )
        data = json.loads(raw)
        for runtime, devices in (data.get("devices") or {}).items():
            for dev in devices:
                if dev.get("state") == "Booted":
                    name = dev.get("name", "Simulator")
                    # CPU = host machine's CPU
                    host_cpu = subprocess.check_output(
                        ["sysctl", "-n", "machdep.cpu.brand_string"],
                        text=True, timeout=3,
                    ).strip()
                    arch = platform.machine().lower()
                    device_id = _make_device_id("ios-sim", arch, host_cpu)
                    return {
                        "id": device_id,
                        "name": f"{name} Simulator ({host_cpu})",
                        "os": runtime.replace("com.apple.CoreSimulator.SimRuntime.", "").replace("-", " "),
                        "cpu": host_cpu,
                        "arch": arch,
                        "isSimulator": True,
                    }
    except Exception:
        pass

    return {
        "id": "ios-arm64-unknown",
        "name": "iOS Device (unknown)",
        "os": "iOS unknown",
        "cpu": "unknown",
        "arch": "arm64",
        "isSimulator": False,
    }


# ---------------------------------------------------------------------------
# Main entry: detect current host device
# ---------------------------------------------------------------------------

def detect_current_device() -> dict[str, Any]:
    """Detect the current host machine's device info."""
    sys = platform.system()
    if sys == "Windows":
        return _detect_windows()
    elif sys == "Darwin":
        return _detect_macos()
    else:
        return _detect_linux()


# ---------------------------------------------------------------------------
# Cache
# ---------------------------------------------------------------------------

def load_or_detect(repo_root: Path) -> dict[str, Any]:
    """Return cached device info if fresh, else detect and cache."""
    cache_path = repo_root / "artifacts" / ".device-cache.json"
    try:
        if cache_path.exists():
            cached = json.loads(cache_path.read_text(encoding="utf-8"))
            age = time.time() - float(cached.get("cachedAt", 0))
            if age < _CACHE_TTL_SECONDS:
                return dict(cached.get("device") or {})
    except Exception:
        pass

    device = detect_current_device()
    try:
        cache_path.parent.mkdir(parents=True, exist_ok=True)
        cache_path.write_text(
            json.dumps({"cachedAt": time.time(), "device": device}, indent=2, ensure_ascii=False),
            encoding="utf-8",
        )
    except Exception:
        pass
    return device
