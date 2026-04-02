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

function Invoke-Run {
    param(
        [string[]]$Arguments,
        [hashtable]$Environment
    )

    $scriptPath = Join-Path $repoRoot "run.ps1"
    $originalValues = @{}

    foreach ($key in $Environment.Keys) {
        $originalValues[$key] = [Environment]::GetEnvironmentVariable($key, "Process")
        [Environment]::SetEnvironmentVariable($key, [string]$Environment[$key], "Process")
    }

    try {
        $psi = New-Object System.Diagnostics.ProcessStartInfo
        $psi.FileName = "powershell.exe"
        $quotedArgs = @(
            "-NoLogo",
            "-NoProfile",
            "-NonInteractive",
            "-ExecutionPolicy", "Bypass",
            "-File", ('"{0}"' -f $scriptPath)
        )
        $quotedArgs += ($Arguments | ForEach-Object { '"{0}"' -f ($_ -replace '"', '\"') })
        $psi.Arguments = [string]::Join(" ", $quotedArgs)
        $psi.WorkingDirectory = $repoRoot
        $psi.RedirectStandardOutput = $true
        $psi.RedirectStandardError = $true
        $psi.UseShellExecute = $false

        $process = [System.Diagnostics.Process]::Start($psi)
        $stdout = $process.StandardOutput.ReadToEnd()
        $stderr = $process.StandardError.ReadToEnd()
        $process.WaitForExit()
        $exitCode = $process.ExitCode
    }
    finally {
        foreach ($key in $Environment.Keys) {
            [Environment]::SetEnvironmentVariable($key, $originalValues[$key], "Process")
        }
    }

    return [pscustomobject]@{
        ExitCode = $exitCode
        Output   = (@($stdout.TrimEnd(), $stderr.TrimEnd()) | Where-Object { $_ }) -join [Environment]::NewLine
    }
}

function New-FakeRuntimeArchive {
    param(
        [string]$ArchivePath
    )

    $payloadRoot = Join-Path $testRoot "payload"
    $pythonDir = Join-Path $payloadRoot "python"
    New-Item -ItemType Directory -Force -Path $pythonDir | Out-Null

    $pythonCmdPath = Join-Path $pythonDir "python.cmd"
    @'
@echo off
if not "%BOOM_RUN_TEST_LOG%"=="" echo %*>>"%BOOM_RUN_TEST_LOG%"
exit /b 0
'@ | Set-Content -Encoding Ascii -Path $pythonCmdPath

    if (Test-Path $ArchivePath) {
        Remove-Item -Force $ArchivePath
    }

    Compress-Archive -Path (Join-Path $payloadRoot "*") -DestinationPath $ArchivePath
}

$repoRoot = Get-RepoRoot
$pythonCommand = (Get-Command python -ErrorAction Stop).Source
$sessionId = [guid]::NewGuid().ToString("N")
$testRoot = Join-Path $repoRoot "artifacts\.tmp_run-wrapper-bootstrap\$sessionId"
$cacheRootRelative = "artifacts/.tmp_run-wrapper-bootstrap/$sessionId/python-cache"
$cacheRootAbsolute = Join-Path $repoRoot "artifacts\.tmp_run-wrapper-bootstrap\$sessionId\python-cache"
$manifestPath = Join-Path $testRoot "runtime_manifest.test.json"
$archivePath = Join-Path $testRoot "fake-python-runtime.zip"

New-Item -ItemType Directory -Force -Path $testRoot | Out-Null
New-FakeRuntimeArchive -ArchivePath $archivePath

$archiveHash = (Get-FileHash -Algorithm SHA256 -Path $archivePath).Hash.ToLowerInvariant()
$archiveUri = [System.Uri]::new($archivePath).AbsoluteUri

$manifest = [ordered]@{
    provider         = "test-runtime"
    provider_release = "local"
    python_version   = "9.9.9"
    cache_root       = $cacheRootRelative
    platforms        = [ordered]@{
        "windows-x64" = [ordered]@{
            platform_id    = "windows-x64"
            download_url   = $archiveUri
            sha256         = $archiveHash
            archive_type   = "zip"
            extract_dir    = "."
            python_relpath = "python/python.cmd"
        }
    }
}

$manifest | ConvertTo-Json -Depth 8 | Set-Content -Encoding utf8 -Path $manifestPath

$runtimePython = Join-Path $cacheRootAbsolute "windows-x64\9.9.9\python\python.cmd"
$currentState = Join-Path $cacheRootAbsolute "current.json"

$interactivePrompt = Invoke-Run -Arguments @("list") -Environment @{
    BOOM_RUN_RUNTIME_MANIFEST = $manifestPath
    BOOM_RUN_FORCE_INTERACTIVE = "1"
    BOOM_RUN_CONFIRM_RESPONSE = "n"
    BOOM_RUN_BOOTSTRAP_PYTHON = $pythonCommand
}

Assert-True ($interactivePrompt.ExitCode -ne 0) "interactive missing-runtime branch should not continue without confirmation"
Assert-Contains -Text $interactivePrompt.Output -Expected "bootstrap" -Message "interactive missing-runtime branch should mention bootstrap"

$nonInteractive = Invoke-Run -Arguments @("list") -Environment @{
    BOOM_RUN_RUNTIME_MANIFEST = $manifestPath
    BOOM_RUN_FORCE_INTERACTIVE = "0"
    BOOM_RUN_BOOTSTRAP_PYTHON = $pythonCommand
}

Assert-True ($nonInteractive.ExitCode -ne 0) "non-interactive missing-runtime branch should fail"
Assert-Contains -Text $nonInteractive.Output -Expected "run bootstrap --yes" -Message "non-interactive branch should require explicit bootstrap"

$bootstrapJson = Invoke-Run -Arguments @("bootstrap", "--yes", "--json") -Environment @{
    BOOM_RUN_RUNTIME_MANIFEST = $manifestPath
    BOOM_RUN_FORCE_INTERACTIVE = "0"
    BOOM_RUN_BOOTSTRAP_PYTHON = $pythonCommand
}

$bootstrapPayload = $bootstrapJson.Output | ConvertFrom-Json
Assert-True ($bootstrapJson.ExitCode -eq 0) "bootstrap --yes --json should succeed"
Assert-True ($bootstrapPayload.command -eq "bootstrap") "bootstrap --json should identify the command"
Assert-True ($bootstrapPayload.status -eq "ok") "bootstrap --json should report ok status"
Assert-True ($bootstrapPayload.hostPlatform -eq "windows") "bootstrap --json should identify the Windows host"
Assert-True ($bootstrapPayload.runtimeStatus -eq "installed") "bootstrap --json should report an installed runtime"
Assert-True ($bootstrapPayload.pythonPath -eq $runtimePython) "bootstrap --json should report the installed runtime path"
Assert-True (Test-Path $runtimePython) "bootstrap should materialize runtime python path"
Assert-True (Test-Path $currentState) "bootstrap should write current.json"

$runtimeTicksBefore = (Get-Item $runtimePython).LastWriteTimeUtc.Ticks
Start-Sleep -Milliseconds 25

$bootstrapAgain = Invoke-Run -Arguments @("bootstrap", "--yes") -Environment @{
    BOOM_RUN_RUNTIME_MANIFEST = $manifestPath
    BOOM_RUN_FORCE_INTERACTIVE = "0"
    BOOM_RUN_BOOTSTRAP_PYTHON = $pythonCommand
}

Assert-True ($bootstrapAgain.ExitCode -eq 0) "second bootstrap --yes should also succeed"
Assert-Contains -Text $bootstrapAgain.Output -Expected "already installed" -Message "existing runtime should not be downloaded again"
Assert-True ((Get-Item $runtimePython).LastWriteTimeUtc.Ticks -eq $runtimeTicksBefore) "existing runtime should not be replaced"

Write-Host "wrapper bootstrap tests passed"
