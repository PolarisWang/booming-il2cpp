import subprocess
import sys

bat = r"D:\agent\booming-il2cpp\build\toolchains\run\testing\foundation_dll\_build_fact_combined.bat"
# Use shell=True -> runs under cmd.exe which finds MSVC tools
proc = subprocess.run(bat, capture_output=True, text=True, shell=True, timeout=120)
out = proc.stdout + proc.stderr
print(out[-3000:])
print("RC:", proc.returncode)