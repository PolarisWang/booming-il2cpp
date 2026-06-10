[CmdletBinding()]
param(
    [Parameter(ValueFromRemainingArguments = $true)]
    [string[]]$Arguments
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RepoRoot {
    if ($env:CHAOS_RUN_REPO_ROOT) {
        return (Resolve-Path $env:CHAOS_RUN_REPO_ROOT).Path
    }

    return (Resolve-Path $PSScriptRoot).Path
}

function Get-ManifestPath {
    param([string]$RepoRoot)

    if ($env:CHAOS_RUN_RUNTIME_MANIFEST) {
        return (Resolve-Path $env:CHAOS_RUN_RUNTIME_MANIFEST).Path
    }

    return (Join-Path $RepoRoot "build\toolchains\run\runtime_manifest.json")
}

function Get-HostPlatformId {
    param(
        [type]$RuntimeInformationType = [System.Runtime.InteropServices.RuntimeInformation],
        [System.Collections.IDictionary]$EnvironmentVariables = $null
    )

    $os = Get-HostOsDescription -RuntimeInformationType $RuntimeInformationType
    $arch = Get-HostArchitectureName -RuntimeInformationType $RuntimeInformationType -EnvironmentVariables $EnvironmentVariables

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

function Get-RuntimeInformationPropertyValue {
    param(
        [string]$PropertyName,
        [type]$RuntimeInformationType = [System.Runtime.InteropServices.RuntimeInformation]
    )

    $property = $RuntimeInformationType.GetProperty(
        $PropertyName,
        [System.Reflection.BindingFlags]::Public -bor [System.Reflection.BindingFlags]::Static
    )
    if ($null -eq $property) {
        return $null
    }

    $value = $property.GetValue($null, $null)
    if ($null -eq $value) {
        return $null
    }

    return $value.ToString()
}

function Get-HostOsDescription {
    param([type]$RuntimeInformationType = [System.Runtime.InteropServices.RuntimeInformation])

    $os = Get-RuntimeInformationPropertyValue -PropertyName "OSDescription" -RuntimeInformationType $RuntimeInformationType
    if (-not [string]::IsNullOrWhiteSpace($os)) {
        return $os.ToLowerInvariant()
    }

    return [Environment]::OSVersion.VersionString.ToLowerInvariant()
}

function Convert-ArchitectureName {
    param([string]$Architecture)

    if ([string]::IsNullOrWhiteSpace($Architecture)) {
        return $null
    }

    switch ($Architecture.Trim().ToLowerInvariant()) {
        { $_ -in @("amd64", "x64") } { return "x64" }
        { $_ -in @("arm64", "aarch64") } { return "arm64" }
        { $_ -in @("x86", "i386", "i486", "i586", "i686") } { return "x86" }
        { $_ -eq "arm" } { return "arm" }
        default { return $Architecture.Trim().ToLowerInvariant() }
    }
}

function Get-HostArchitectureName {
    param(
        [type]$RuntimeInformationType = [System.Runtime.InteropServices.RuntimeInformation],
        [System.Collections.IDictionary]$EnvironmentVariables = $null
    )

    $arch = Get-RuntimeInformationPropertyValue -PropertyName "OSArchitecture" -RuntimeInformationType $RuntimeInformationType
    if ([string]::IsNullOrWhiteSpace($arch)) {
        $arch = Get-RuntimeInformationPropertyValue -PropertyName "ProcessArchitecture" -RuntimeInformationType $RuntimeInformationType
    }

    if ([string]::IsNullOrWhiteSpace($arch)) {
        if ($null -eq $EnvironmentVariables) {
            $EnvironmentVariables = [Environment]::GetEnvironmentVariables()
        }

        if ($EnvironmentVariables.Contains("PROCESSOR_ARCHITEW6432")) {
            $arch = [string]$EnvironmentVariables["PROCESSOR_ARCHITEW6432"]
        }
        elseif ($EnvironmentVariables.Contains("PROCESSOR_ARCHITECTURE")) {
            $arch = [string]$EnvironmentVariables["PROCESSOR_ARCHITECTURE"]
        }
    }

    $normalized = Convert-ArchitectureName -Architecture $arch
    if (-not [string]::IsNullOrWhiteSpace($normalized)) {
        return $normalized
    }

    if ([Environment]::Is64BitOperatingSystem -or [Environment]::Is64BitProcess) {
        return "x64"
    }

    return "x86"
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
    if ($env:CHAOS_RUN_FORCE_INTERACTIVE -eq "1") {
        return $true
    }

    if ($env:CHAOS_RUN_FORCE_INTERACTIVE -eq "0") {
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

    $response = $env:CHAOS_RUN_CONFIRM_RESPONSE
    if (-not $response) {
        $response = Read-Host
    }

    return @("y", "yes", "1", "true") -contains $response.ToLowerInvariant()
}

function Get-BootstrapPython {
    if ($env:CHAOS_RUN_BOOTSTRAP_PYTHON) {
        return $env:CHAOS_RUN_BOOTSTRAP_PYTHON
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
    if ($env:CHAOS_RUN_WINDOWS_TERMINAL_COMMAND) {
        $resolved = Resolve-Path $env:CHAOS_RUN_WINDOWS_TERMINAL_COMMAND -ErrorAction SilentlyContinue
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

    if ($env:CHAOS_RUN_REQUEST_WINDOWS_TERMINAL -ne "1") {
        return $false
    }

    if ($env:CHAOS_RUN_WINDOWS_TERMINAL_HANDOFF -eq "1") {
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

function ConvertTo-WindowsProcessArgumentList {
    param([string[]]$Arguments)

    $segments = New-Object System.Collections.Generic.List[string]

    foreach ($argument in @($Arguments | Where-Object { $null -ne $_ })) {
        if ($segments.Count -gt 0) {
            $segments.Add(" ")
        }

        $requiresQuotes = $argument.Length -eq 0 -or $argument.Contains(" ") -or $argument.Contains("`t")
        if ($requiresQuotes) {
            $segments.Add('"')
        }

        $pendingBackslashes = 0
        foreach ($character in $argument.ToCharArray()) {
            if ($character -eq '\') {
                $pendingBackslashes += 1
                continue
            }

            if ($character -eq '"') {
                if ($pendingBackslashes -gt 0) {
                    $segments.Add(('\' * ($pendingBackslashes * 2)))
                }
                $segments.Add('\"')
                $pendingBackslashes = 0
                continue
            }

            if ($pendingBackslashes -gt 0) {
                $segments.Add(('\' * $pendingBackslashes))
                $pendingBackslashes = 0
            }

            $segments.Add([string]$character)
        }

        if ($pendingBackslashes -gt 0) {
            $segments.Add(('\' * ($pendingBackslashes * $(if ($requiresQuotes) { 2 } else { 1 }))))
        }

        if ($requiresQuotes) {
            $segments.Add('"')
        }
    }

    return [string]::Concat($segments.ToArray())
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

    $forwardedArguments = @($Arguments | Where-Object { $null -ne $_ })

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
    $wtArguments += $forwardedArguments

    $originalRequest = $env:CHAOS_RUN_REQUEST_WINDOWS_TERMINAL
    $originalHandoff = $env:CHAOS_RUN_WINDOWS_TERMINAL_HANDOFF

    $env:CHAOS_RUN_REQUEST_WINDOWS_TERMINAL = "0"
    $env:CHAOS_RUN_WINDOWS_TERMINAL_HANDOFF = "1"

    try {
        $launcherPath = $wtCommandPath
        $launcherArguments = $wtArguments
        $extension = [System.IO.Path]::GetExtension($wtCommandPath)
        if ($extension -eq ".cmd" -or $extension -eq ".bat") {
            $launcherPath = $env:ComSpec
            $launcherArguments = @("/d", "/c", $wtCommandPath) + $wtArguments
        }

        $launcherArgumentLine = ConvertTo-WindowsProcessArgumentList -Arguments $launcherArguments
        Start-Process -FilePath $launcherPath -ArgumentList $launcherArgumentLine -WorkingDirectory $RepoRoot | Out-Null
    }
    finally {
        $env:CHAOS_RUN_REQUEST_WINDOWS_TERMINAL = $originalRequest
        $env:CHAOS_RUN_WINDOWS_TERMINAL_HANDOFF = $originalHandoff
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
