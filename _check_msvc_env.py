import subprocess, os

# Try a direct Python approach: run vcvars first to set env, then run cl/link
vcvars = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat'

# Create a cmd script that dumps the environment after vcvars
cmd = f'call "{vcvars}" x64 >nul 2>nul && set'
r = subprocess.run(['cmd.exe', '/c', cmd], capture_output=True, text=True, shell=False)

# Parse env vars
env = {}
for line in r.stdout.splitlines():
    if '=' in line:
        k, v = line.split('=', 1)
        env[k] = v

# Check if PATH has MSVC tools
paths = env.get('PATH', '').split(';')
msvc_paths = [p for p in paths if 'MSVC' in p.upper() or 'VC' in p.upper() or 'VISUAL STUDIO' in p.upper()]
print("MSVC-related PATH entries:")
for p in msvc_paths[:5]:
    print(f"  {p}")
print(f"\nLIB = {env.get('LIB', 'NOT SET')[:200]}")
print(f"INCLUDE = {env.get('INCLUDE', 'NOT SET')[:200]}")

# Check if link.exe is in one of these paths
import glob
for p in msvc_paths:
    link_exe = os.path.join(p, 'link.exe')
    if os.path.exists(link_exe):
        print(f"\nFound link.exe at: {link_exe}")
        break
else:
    print("\nlink.exe NOT found in MSVC paths!")
    # Search more broadly
    for p in paths:
        le = os.path.join(p, 'link.exe')
        if os.path.exists(le) and 'git' not in p.lower():
            print(f"Found link.exe (non-git) at: {le}")