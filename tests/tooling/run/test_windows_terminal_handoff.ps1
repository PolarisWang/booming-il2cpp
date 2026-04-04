[CmdletBinding()]
param()

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RepoRoot {
    return (Resolve-Path (Join-Path $PSScriptRoot "..\..\..")).Path
}

function Assert-True {
    param(
        [bool]$Condition,
        [string]$Message
    )

    if (-not $Condition) {
        throw $Message
    }
}

function Assert-Contains {
    param(
        [string]$Text,
        [string]$Expected,
        [string]$Message
    )

    if ($Text -notmatch [regex]::Escape($Expected)) {
        throw "$Message`nExpected fragment: $Expected`nActual text: $Text"
    }
}

$repoRoot = Get-RepoRoot
$sessionId = [guid]::NewGuid().ToString("N")
$testRoot = Join-Path $repoRoot "artifacts\.tmp_run-windows-terminal\$sessionId"
$fakeWtPath = Join-Path $testRoot "fake-wt.cmd"
$handoffLog = Join-Path $testRoot "handoff.log"
$scriptPath = Join-Path $repoRoot "run.ps1"

New-Item -ItemType Directory -Force -Path $testRoot | Out-Null

@'
@echo off
(
echo FILE=%~f0
echo ARGS=%*
echo HANDOFF=%BOOM_RUN_WINDOWS_TERMINAL_HANDOFF%
echo REQUEST=%BOOM_RUN_REQUEST_WINDOWS_TERMINAL%
)>"%BOOM_RUN_WT_LOG%"
exit /b 0
'@ | Set-Content -Encoding Ascii -Path $fakeWtPath

$originalRequest = [Environment]::GetEnvironmentVariable("BOOM_RUN_REQUEST_WINDOWS_TERMINAL", "Process")
$originalCommand = [Environment]::GetEnvironmentVariable("BOOM_RUN_WINDOWS_TERMINAL_COMMAND", "Process")
$originalLog = [Environment]::GetEnvironmentVariable("BOOM_RUN_WT_LOG", "Process")
$originalWtSession = [Environment]::GetEnvironmentVariable("WT_SESSION", "Process")

try {
    [Environment]::SetEnvironmentVariable("BOOM_RUN_REQUEST_WINDOWS_TERMINAL", "1", "Process")
    [Environment]::SetEnvironmentVariable("BOOM_RUN_WINDOWS_TERMINAL_COMMAND", $fakeWtPath, "Process")
    [Environment]::SetEnvironmentVariable("BOOM_RUN_WT_LOG", $handoffLog, "Process")
    [Environment]::SetEnvironmentVariable("WT_SESSION", $null, "Process")

    & powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File $scriptPath doctor
    $exitCode = $LASTEXITCODE
}
finally {
    [Environment]::SetEnvironmentVariable("BOOM_RUN_REQUEST_WINDOWS_TERMINAL", $originalRequest, "Process")
    [Environment]::SetEnvironmentVariable("BOOM_RUN_WINDOWS_TERMINAL_COMMAND", $originalCommand, "Process")
    [Environment]::SetEnvironmentVariable("BOOM_RUN_WT_LOG", $originalLog, "Process")
    [Environment]::SetEnvironmentVariable("WT_SESSION", $originalWtSession, "Process")
}

Assert-True ($exitCode -eq 0) "run.ps1 should exit successfully after handing off to Windows Terminal"

$deadline = [DateTime]::UtcNow.AddSeconds(2)
while (-not (Test-Path $handoffLog) -and [DateTime]::UtcNow -lt $deadline) {
    Start-Sleep -Milliseconds 100
}

Assert-True (Test-Path $handoffLog) "fake Windows Terminal command should be invoked"

$logText = Get-Content -Raw -Encoding utf8 $handoffLog
Assert-Contains -Text $logText -Expected "doctor" -Message "handoff should preserve original command arguments"
Assert-Contains -Text $logText -Expected "HANDOFF=1" -Message "handoff child should be marked to avoid recursive relaunch"
Assert-Contains -Text $logText -Expected "REQUEST=0" -Message "handoff child should clear the Windows Terminal request flag"

Write-Host "windows terminal handoff test passed"
