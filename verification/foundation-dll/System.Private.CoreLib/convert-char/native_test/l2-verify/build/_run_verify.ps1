Set-Location "D:\agent\booming-il2cpp\verification\foundation-dll\System.Private.CoreLib\convert-char\native_test\l2-verify\build"
$result = & ".\verify_convert-char.exe" 2>&1
Write-Host $result
Write-Host ("EXIT_CODE=" + $LASTEXITCODE)
