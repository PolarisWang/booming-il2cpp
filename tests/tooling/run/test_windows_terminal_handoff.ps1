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

function Invoke-HandoffCase {
    param(
        [string]$RepoRoot,
        [string]$ScriptPath,
        [string]$FakeWtPath,
        [string]$HandoffLog,
        [string[]]$RunArguments
    )

    $originalRequest = [Environment]::GetEnvironmentVariable("BOOM_RUN_REQUEST_WINDOWS_TERMINAL", "Process")
    $originalCommand = [Environment]::GetEnvironmentVariable("BOOM_RUN_WINDOWS_TERMINAL_COMMAND", "Process")
    $originalLog = [Environment]::GetEnvironmentVariable("BOOM_RUN_WT_LOG", "Process")
    $originalWtSession = [Environment]::GetEnvironmentVariable("WT_SESSION", "Process")

    try {
        [Environment]::SetEnvironmentVariable("BOOM_RUN_REQUEST_WINDOWS_TERMINAL", "1", "Process")
        [Environment]::SetEnvironmentVariable("BOOM_RUN_WINDOWS_TERMINAL_COMMAND", $FakeWtPath, "Process")
        [Environment]::SetEnvironmentVariable("BOOM_RUN_WT_LOG", $HandoffLog, "Process")
        [Environment]::SetEnvironmentVariable("WT_SESSION", $null, "Process")

        & powershell.exe -NoLogo -NoProfile -ExecutionPolicy Bypass -File $ScriptPath @RunArguments
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
    while (-not (Test-Path $HandoffLog) -and [DateTime]::UtcNow -lt $deadline) {
        Start-Sleep -Milliseconds 100
    }

    Assert-True (Test-Path $HandoffLog) "fake Windows Terminal command should be invoked"
    return Get-Content -Raw -Encoding utf8 $HandoffLog
}

$repoRoot = Get-RepoRoot
$sessionId = [guid]::NewGuid().ToString("N")
$testRoot = Join-Path $repoRoot "artifacts\.tmp_run-windows-terminal\$sessionId"
$fakeWtPath = Join-Path $testRoot "fake-wt.cmd"
$handoffLog = Join-Path $testRoot "handoff-doctor.log"
$emptyHandoffLog = Join-Path $testRoot "handoff-empty.log"
$scriptPath = Join-Path $repoRoot "run.ps1"

New-Item -ItemType Directory -Force -Path $testRoot | Out-Null

@'
@echo off
setlocal EnableDelayedExpansion
set "ALL_ARGS=%*"
>"%BOOM_RUN_WT_LOG%" echo FILE=%~f0
>>"%BOOM_RUN_WT_LOG%" echo HANDOFF=%BOOM_RUN_WINDOWS_TERMINAL_HANDOFF%
>>"%BOOM_RUN_WT_LOG%" echo REQUEST=%BOOM_RUN_REQUEST_WINDOWS_TERMINAL%
>>"%BOOM_RUN_WT_LOG%" echo ARGS=!ALL_ARGS!
set ARG_INDEX=0
:next_arg
if "%~1"=="" goto end_args
set /a ARG_INDEX+=1
>>"%BOOM_RUN_WT_LOG%" echo ARG!ARG_INDEX!=[%~1]
shift
goto next_arg
:end_args
exit /b 0
'@ | Set-Content -Encoding Ascii -Path $fakeWtPath

$logText = Invoke-HandoffCase -RepoRoot $repoRoot -ScriptPath $scriptPath -FakeWtPath $fakeWtPath -HandoffLog $handoffLog -RunArguments @("doctor")
Assert-Contains -Text $logText -Expected "doctor" -Message "handoff should preserve original command arguments"
Assert-Contains -Text $logText -Expected "HANDOFF=1" -Message "handoff child should be marked to avoid recursive relaunch"
Assert-Contains -Text $logText -Expected "REQUEST=0" -Message "handoff child should clear the Windows Terminal request flag"
Assert-Contains -Text $logText -Expected "ARG7=[boom run]" -Message "handoff should preserve the Windows Terminal title as a single argument"
Assert-Contains -Text $logText -Expected "ARG8=[powershell.exe]" -Message "handoff should pass the shell executable immediately after the title value"

$emptyLog = Invoke-HandoffCase -RepoRoot $repoRoot -ScriptPath $scriptPath -FakeWtPath $fakeWtPath -HandoffLog $emptyHandoffLog -RunArguments @()
Assert-Contains -Text $emptyLog -Expected "ARGS=" -Message "handoff should support launching without command arguments"
Assert-Contains -Text $emptyLog -Expected "HANDOFF=1" -Message "empty handoff child should still be marked to avoid recursive relaunch"
Assert-Contains -Text $emptyLog -Expected "REQUEST=0" -Message "empty handoff child should clear the Windows Terminal request flag"

Write-Host "windows terminal handoff test passed"
