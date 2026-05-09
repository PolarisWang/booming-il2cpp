$exe = 'D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\build\RelWithDebInfo\entry.exe'
$p = Start-Process -NoNewWindow -FilePath $exe -Wait -PassThru
Write-Host ('CODE=' + $p.ExitCode)
Start-Sleep -Seconds 1
Write-Host '---DEBUG LOG---'
Get-Content 'D:\agent\booming-il2cpp\_crash_debug.txt'