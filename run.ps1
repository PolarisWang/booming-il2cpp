[CmdletBinding()]
param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RepoRoot {
    if ($env:BOOM_RUN_REPO_ROOT) {
        return (Resolve-Path $env:BOOM_RUN_REPO_ROOT).Path
    }

    return (Resolve-Path $PSScriptRoot).Path
}

function Get-ManifestPath {
    param([string]$RepoRoot)

    if ($env:BOOM_RUN_RUNTIME_MANIFEST) {
        return (Resolve-Path $env:BOOM_RUN_RUNTIME_MANIFEST).Path
    }

    return (Join-Path $RepoRoot "build\toolchains\run\runtime_manifest.json")
}

function Get-HostPlatformId {
    $os = [System.Runtime.InteropServices.RuntimeInformation]::OSDescription.ToLowerInvariant()
    $arch = [System.Runtime.InteropServices.RuntimeInformation]::OSArchitecture.ToString().ToLowerInvariant()

    if ($os.Contains("windows")) {
        if ($arch -eq "x64") {
            return "windows-x64"
        }
        if ($arch -eq "arm64") {
            return "windows-arm64"
        }
    }

    if ($os.Contains("darwin") -or $os.Contains("mac")) {
        if ($arch -eq "arm64") {
            return "macos-arm64"
        }
        if ($arch -eq "x64") {
            return "macos-x64"
        }
    }

    if ($os.Contains("linux")) {
        if ($arch -eq "x64") {
            return "linux-x64"
        }
        if ($arch -eq "arm64") {
            return "linux-arm64"
        }
    }

    throw "unsupported host platform for run.ps1: os='$os' arch='$arch'"
}

function Read-RuntimeManifest {
    param([string]$ManifestPath)

    $json = Get-Content -Raw -Encoding utf8 $ManifestPath
    return $json | ConvertFrom-Json
}

function Join-RelativePath {
    param(
        [string]$BasePath,
        [string]$RelativePath
    )

    $current = $BasePath
    foreach ($segment in ($RelativePath -split "[/\\]")) {
        if ([string]::IsNullOrWhiteSpace($segment)) {
            continue
        }

        $current = Join-Path $current $segment
    }

    return $current
}

function Get-RuntimePythonPath {
    param(
        [string]$RepoRoot,
        [string]$ManifestPath
    )

    $manifest = Read-RuntimeManifest -ManifestPath $ManifestPath
    $platformId = Get-HostPlatformId

    if (-not $manifest.platforms.PSObject.Properties.Name.Contains($platformId)) {
        throw "runtime manifest does not define host platform '$platformId'"
    }

    $cacheRoot = Join-Path $RepoRoot $manifest.cache_root
    $runtimeRoot = Join-Path (Join-Path $cacheRoot $platformId) $manifest.python_version
    return Join-RelativePath -BasePath $runtimeRoot -RelativePath $manifest.platforms.$platformId.python_relpath
}

function Test-InteractiveSession {
    if ($env:BOOM_RUN_FORCE_INTERACTIVE -eq "1") {
        return $true
    }

    if ($env:BOOM_RUN_FORCE_INTERACTIVE -eq "0") {
        return $false
    }

    if ($env:CI -or $env:GITHUB_ACTIONS -or $env:TF_BUILD) {
        return $false
    }

    return [Environment]::UserInteractive
}

function Test-JsonOutputRequested {
    param([string[]]$Arguments)

    return @($Arguments) -contains "--json"
}

function Get-CommandText {
    param([string[]]$Arguments)

    $filtered = @($Arguments | Where-Object { $_ -ne "--json" })
    if ($filtered.Length -eq 0) {
        return "help"
    }

    return ($filtered -join " ")
}

function Get-HostPlatformFamily {
    param([string]$PlatformId)

    if ($PlatformId.StartsWith("windows")) {
        return "windows"
    }
    if ($PlatformId.StartsWith("macos")) {
        return "macos"
    }
    if ($PlatformId.StartsWith("linux")) {
        return "linux"
    }

    return $PlatformId
}

function Write-JsonFailure {
    param(
        [string]$CommandText,
        [string]$HostPlatform,
        [string]$Message,
        [int]$DurationMs
    )

    $payload = [ordered]@{
        command    = $CommandText
        status     = "error"
        hostPlatform = $HostPlatform
        target     = $null
        durationMs = $DurationMs
        checks     = @()
        errors     = @($Message)
    }

    $payload | ConvertTo-Json -Depth 8 | Write-Output
}

function Confirm-Bootstrap {
    $prompt = "Python runtime is not installed. Bootstrap now? [y/N]"
    Write-Host $prompt

    $response = $env:BOOM_RUN_CONFIRM_RESPONSE
    if (-not $response) {
        $response = Read-Host
    }

    return @("y", "yes", "1", "true") -contains $response.ToLowerInvariant()
}

function Get-BootstrapPython {
    if ($env:BOOM_RUN_BOOTSTRAP_PYTHON) {
        return $env:BOOM_RUN_BOOTSTRAP_PYTHON
    }

    $python = Get-Command python -ErrorAction SilentlyContinue
    if ($python) {
        return $python.Source
    }

    return $null
}

function Invoke-RuntimeBootstrap {
    param(
        [string]$PythonExecutable,
        [string]$RepoRoot,
        [string]$ManifestPath,
        [switch]$Force,
        [switch]$JsonOutput
    )

    $runtimeScript = Join-Path $RepoRoot "build\toolchains\run\runtime.py"
    $command = @(
        $runtimeScript,
        "bootstrap",
        "--repo-root", $RepoRoot,
        "--manifest", $ManifestPath
    )

    if ($Force) {
        $command += "--force"
    }

    if ($JsonOutput) {
        $command += "--json"
    }

    $output = & $PythonExecutable @command
    return [pscustomobject]@{
        ExitCode = $LASTEXITCODE
        Output   = (@($output) -join [Environment]::NewLine)
    }
}

function Get-RunScriptPath {
    param([string]$RepoRoot)

    return Join-Path $RepoRoot "build\toolchains\run\run.py"
}

function Get-WindowsTerminalCommandPath {
    if ($env:BOOM_RUN_WINDOWS_TERMINAL_COMMAND) {
        $resolved = Resolve-Path $env:BOOM_RUN_WINDOWS_TERMINAL_COMMAND -ErrorAction SilentlyContinue
        if ($resolved) {
            return $resolved.Path
        }

        return $null
    }

    $command = Get-Command "wt.exe" -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    $command = Get-Command "wt" -ErrorAction SilentlyContinue
    if ($command) {
        return $command.Source
    }

    return $null
}

function Test-WindowsTerminalHandoffRequested {
    param([bool]$JsonOutput)

    if ($env:BOOM_RUN_REQUEST_WINDOWS_TERMINAL -ne "1") {
        return $false
    }

    if ($env:BOOM_RUN_WINDOWS_TERMINAL_HANDOFF -eq "1") {
        return $false
    }

    if ($env:WT_SESSION) {
        return $false
    }

    if ($JsonOutput) {
        return $false
    }

    if ($env:CI -or $env:GITHUB_ACTIONS -or $env:TF_BUILD) {
        return $false
    }

    return $null -ne (Get-WindowsTerminalCommandPath)
}

function Invoke-WindowsTerminalHandoff {
    param(
        [string]$RepoRoot,
        [string]$ScriptPath,
        [string[]]$Arguments
    )

    $wtCommandPath = Get-WindowsTerminalCommandPath
    if (-not $wtCommandPath) {
        return $false
    }

    $wtArguments = @(
        "-w", "new",
        "new-tab",
        "-d", $RepoRoot,
        "--title", "boom run",
        "powershell.exe",
        "-NoLogo",
        "-NoProfile",
        "-ExecutionPolicy", "Bypass",
        "-NoExit",
        "-File", $ScriptPath
    )
    $wtArguments += @($Arguments)

    $originalRequest = $env:BOOM_RUN_REQUEST_WINDOWS_TERMINAL
    $originalHandoff = $env:BOOM_RUN_WINDOWS_TERMINAL_HANDOFF

    $env:BOOM_RUN_REQUEST_WINDOWS_TERMINAL = "0"
    $env:BOOM_RUN_WINDOWS_TERMINAL_HANDOFF = "1"

    try {
        $launcherPath = $wtCommandPath
        $launcherArguments = $wtArguments
        $extension = [System.IO.Path]::GetExtension($wtCommandPath)
        if ($extension -eq ".cmd" -or $extension -eq ".bat") {
            $launcherPath = $env:ComSpec
            $launcherArguments = @("/d", "/c", $wtCommandPath) + $wtArguments
        }

        Start-Process -FilePath $launcherPath -ArgumentList $launcherArguments -WorkingDirectory $RepoRoot | Out-Null
    }
    finally {
        $env:BOOM_RUN_REQUEST_WINDOWS_TERMINAL = $originalRequest
        $env:BOOM_RUN_WINDOWS_TERMINAL_HANDOFF = $originalHandoff
    }

    return $true
}

$jsonOutput = $false
$commandText = ""
$hostPlatformFamily = "unknown"
$stopwatch = [System.Diagnostics.Stopwatch]::StartNew()

try {
    $repoRoot = Get-RepoRoot
    $manifestPath = Get-ManifestPath -RepoRoot $repoRoot
    $jsonOutput = Test-JsonOutputRequested -Arguments $Arguments
    $commandText = Get-CommandText -Arguments $Arguments
    $hostPlatformId = Get-HostPlatformId
    $hostPlatformFamily = Get-HostPlatformFamily -PlatformId $hostPlatformId
    if (Test-WindowsTerminalHandoffRequested -JsonOutput $jsonOutput) {
        $scriptPath = Join-Path $repoRoot "run.ps1"
        if (Invoke-WindowsTerminalHandoff -RepoRoot $repoRoot -ScriptPath $scriptPath -Arguments $Arguments) {
            exit 0
        }
    }
    $runtimePython = Get-RuntimePythonPath -RepoRoot $repoRoot -ManifestPath $manifestPath
    $argumentsList = @($Arguments)
    $isBootstrapCommand = $argumentsList.Length -gt 0 -and $argumentsList[0] -eq "bootstrap"
    $hasYes = $argumentsList -contains "--yes"
    $runtimeReady = Test-Path $runtimePython

    if ($isBootstrapCommand) {
        if ($runtimeReady) {
            if ($jsonOutput) {
                $response = Invoke-RuntimeBootstrap -PythonExecutable $runtimePython -RepoRoot $repoRoot -ManifestPath $manifestPath -JsonOutput
                if ($response.Output) {
                    $response.Output | Out-Host
                }
                exit $response.ExitCode
            }

            Write-Host "Python runtime already installed."
            exit 0
        }

        if (-not $hasYes) {
            if (-not (Test-InteractiveSession)) {
                throw "non-interactive bootstrap requires 'run bootstrap --yes'"
            }

            if (-not (Confirm-Bootstrap)) {
                throw "bootstrap cancelled"
            }
        }

        $bootstrapPython = Get-BootstrapPython
        if (-not $bootstrapPython) {
            throw "system Python was not found. Install Python temporarily or pre-seed the cached runtime before bootstrapping."
        }

        $response = Invoke-RuntimeBootstrap -PythonExecutable $bootstrapPython -RepoRoot $repoRoot -ManifestPath $manifestPath -JsonOutput:$jsonOutput
        if ($jsonOutput) {
            if ($response.Output) {
                $response.Output | Out-Host
            }
            exit $response.ExitCode
        }

        if ($response.ExitCode -ne 0) {
            throw $response.Output
        }

        if ($response.Output) {
            $response.Output | Out-Host
        }
        exit 0
    }

    if (-not $runtimeReady) {
        if (-not (Test-InteractiveSession)) {
            throw "Python runtime is not installed. Run 'run bootstrap --yes' first."
        }

        if (-not (Confirm-Bootstrap)) {
            throw "bootstrap cancelled"
        }

        $bootstrapPython = Get-BootstrapPython
        if (-not $bootstrapPython) {
            throw "system Python was not found. Install Python temporarily or pre-seed the cached runtime before bootstrapping."
        }

        $response = Invoke-RuntimeBootstrap -PythonExecutable $bootstrapPython -RepoRoot $repoRoot -ManifestPath $manifestPath
        if ($response.ExitCode -ne 0) {
            throw $response.Output
        }

        if ($response.Output) {
            $response.Output | Out-Host
        }
        $runtimeReady = Test-Path $runtimePython
    }

    $runScript = Get-RunScriptPath -RepoRoot $repoRoot
    if (-not (Test-Path $runScript)) {
        throw "runtime bootstrap is ready, but the command layer is not implemented yet."
    }

    & $runtimePython $runScript @argumentsList
    exit $LASTEXITCODE
}
catch {
    if ($jsonOutput) {
        $stopwatch.Stop()
        Write-JsonFailure -CommandText $commandText -HostPlatform $hostPlatformFamily -Message $_.Exception.Message -DurationMs ([int]$stopwatch.Elapsed.TotalMilliseconds)
        exit 1
    }

    [Console]::Error.WriteLine($_.Exception.Message)
    exit 1
}
