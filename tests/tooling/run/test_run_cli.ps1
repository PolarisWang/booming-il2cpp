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

        return [pscustomobject]@{
            ExitCode = $process.ExitCode
            Output   = (@($stdout.TrimEnd(), $stderr.TrimEnd()) | Where-Object { $_ }) -join [Environment]::NewLine
        }
    }
    finally {
        foreach ($key in $Environment.Keys) {
            [Environment]::SetEnvironmentVariable($key, $originalValues[$key], "Process")
        }
    }
}

function New-PythonProxyArchive {
    param(
        [string]$ArchivePath,
        [string]$PythonCommand
    )

    $payloadRoot = Join-Path $testRoot "payload"
    $pythonDir = Join-Path $payloadRoot "python"
    New-Item -ItemType Directory -Force -Path $pythonDir | Out-Null

    $escapedPython = $PythonCommand.Replace('"', '""')
    @"
@echo off
"$escapedPython" %*
exit /b %ERRORLEVEL%
"@ | Set-Content -Encoding Ascii -Path (Join-Path $pythonDir "python.cmd")

    Compress-Archive -Path (Join-Path $payloadRoot "*") -DestinationPath $ArchivePath
}

$repoRoot = Get-RepoRoot
$pythonCommand = (Get-Command python -ErrorAction Stop).Source
$sessionId = [guid]::NewGuid().ToString("N")
$testRoot = Join-Path $repoRoot "artifacts\.tmp_run-cli\$sessionId"
$cacheRootRelative = "artifacts/.tmp_run-cli/$sessionId/python-cache"
$manifestPath = Join-Path $testRoot "runtime_manifest.test.json"
$archivePath = Join-Path $testRoot "python-proxy.zip"

New-Item -ItemType Directory -Force -Path $testRoot | Out-Null
New-PythonProxyArchive -ArchivePath $archivePath -PythonCommand $pythonCommand

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

$baseEnv = @{
    CHAOS_RUN_RUNTIME_MANIFEST = $manifestPath
    CHAOS_RUN_FORCE_INTERACTIVE = "0"
    CHAOS_RUN_BOOTSTRAP_PYTHON = $pythonCommand
}

$bootstrap = Invoke-Run -Arguments @("bootstrap", "--yes") -Environment $baseEnv
Assert-True ($bootstrap.ExitCode -eq 0) "wrapper bootstrap should succeed before CLI tests"

$help = Invoke-Run -Arguments @() -Environment $baseEnv
Assert-True ($help.ExitCode -eq 0) "bare non-interactive run should print help"
Assert-True ($help.Output -match "Available commands") "bare non-interactive run should render help text"

$listJson = Invoke-Run -Arguments @("list", "--json") -Environment $baseEnv
Assert-True ($listJson.ExitCode -eq 0) "list --json should succeed"
$listPayload = $listJson.Output | ConvertFrom-Json
Assert-True ($listPayload.command -eq "list") "list --json should identify the command"
Assert-True ($listPayload.status -eq "ok") "list --json should report ok status"
Assert-True ($listPayload.items.Count -ge 10) "list --json should surface the current real capability set"
Assert-True ((@($listPayload.items.id) -contains "test-suite")) "list --json should include suite selector entry"
Assert-True ((@($listPayload.items.id) -contains "test-module")) "list --json should include module selector entry"
Assert-True ((@($listPayload.items.id) -contains "test-system")) "list --json should include system selector entry"
Assert-True ((@($listPayload.items.id) -contains "verify-verification-v1")) "list --json should include verification-v1 entry"
Assert-True (-not (@($listPayload.items.id) -contains "test-pipeline")) "list --json should hide the retired pipeline selector entry"
Assert-True ((@($listPayload.items.id) -contains "test-registry-list")) "list --json should include registry listing entry"
Assert-True (-not (@($listPayload.items.id) -contains "build-smoke-helloworld")) "list --json should hide deprecated hidden smoke build entries"
Assert-True (-not (@($listPayload.items.id) -contains "verify-roadmap-0-windows")) "list --json should hide removed verify entrypoints"

$capabilityJson = Invoke-Run -Arguments @("capability", "bootstrap", "--json") -Environment $baseEnv
Assert-True ($capabilityJson.ExitCode -eq 0) "capability bootstrap --json should succeed"
$capabilityPayload = $capabilityJson.Output | ConvertFrom-Json
Assert-True ($capabilityPayload.command -eq "capability") "capability --json should identify the command"
Assert-True ($capabilityPayload.item.id -eq "bootstrap") "capability bootstrap --json should return bootstrap metadata"

$interactiveMenu = Invoke-Run -Arguments @() -Environment @{
    CHAOS_RUN_RUNTIME_MANIFEST = $manifestPath
    CHAOS_RUN_FORCE_INTERACTIVE = "1"
    CHAOS_RUN_BOOTSTRAP_PYTHON = $pythonCommand
}
Assert-True ($interactiveMenu.ExitCode -eq 0) "bare interactive run should render menu"
Assert-True ($interactiveMenu.Output -match "prepare") "interactive menu should render the primary command list"
Assert-True ($interactiveMenu.Output -match "test") "interactive menu should include the unified test entry"

Write-Host "run CLI tests passed"
