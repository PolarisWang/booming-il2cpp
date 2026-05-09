$exe = 'D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\il2cpp_dist\genuine\build\RelWithDebInfo\entry.exe'
$errfile = 'D:\agent\booming-il2cpp\_tmp_stderr2.txt'

$p = Start-Process -NoNewWindow -FilePath $exe -Wait -PassThru -RedirectStandardError $errfile
Write-Host ('CODE=' + $p.ExitCode)
Write-Host '---STDERR---'
Get-Content $errfile