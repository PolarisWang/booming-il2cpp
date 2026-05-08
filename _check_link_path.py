import subprocess

vcvars = r'C:\Program Files\Microsoft Visual Studio\2022\Professional\VC\Auxiliary\Build\vcvarsall.bat'
cmd = f'call "{vcvars}" x64 >nul 2>nul && where link.exe'
r = subprocess.run(['cmd.exe', '/c', cmd], capture_output=True, text=True, shell=False)
print("where link.exe output:")
for line in r.stdout.splitlines():
    print(f"  {line}")
print(f"stderr: {r.stderr[:500]}")

cmd2 = f'call "{vcvars}" x64 >nul 2>nul && for %i in (link.exe) do @echo %~$PATH:i'
r2 = subprocess.run(['cmd.exe', '/c', cmd2], capture_output=True, text=True, shell=False)
print(f"\nfor loop PATH search:\n{r2.stdout[:500]}")