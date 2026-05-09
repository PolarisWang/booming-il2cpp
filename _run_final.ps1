$exe = 'D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\build\RelWithDebInfo\entry.exe'
$dst = 'D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\ConvertCharNativeEntry\generated\entry.exe'
Copy-Item $exe $dst -Force
Write-Host 'Copied'

$p = Start-Process -NoNewWindow -FilePath $dst -Wait -PassThru -RedirectStandardOutput 'D:\agent\booming-il2cpp\_tmp_stdout.txt' -RedirectStandardError 'D:\agent\booming-il2cpp\_tmp_stderr.txt'
Write-Host ('CODE=' + $p.ExitCode)
Write-Host '---STDOUT---'
Get-Content 'D:\agent\booming-il2cpp\_tmp_stdout.txt'
if ($p.ExitCode -eq 0) { Write-Host 'ALL PASSED' } else { Write-Host '---STDERR---'; Get-Content 'D:\agent\booming-il2cpp\_tmp_stderr.txt' }