[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("windows", "macos")]
    [string]$HostProfile
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-RepoRoot {
    return (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path
}

function Write-Step {
    param([string]$Message)
    Write-Host "==> $Message"
}

function Get-HostRoutingGenerator {
    switch ($HostProfile) {
        "windows" { return "Visual Studio 17 2022" }
        "macos" { return "Xcode" }
        default { throw "unsupported host profile: $HostProfile" }
    }
}

function Read-JsonFile {
    param([string]$Path)

    $resolvedPath = (Resolve-Path $Path).Path
    $jsonText = [System.IO.File]::ReadAllText($resolvedPath)
    return ConvertFrom-Json -InputObject $jsonText
}

function Assert-JsonFilesParse {
    param([string[]]$Paths)

    foreach ($path in $Paths) {
        [void](Read-JsonFile -Path $path)
    }
}

function Invoke-NativeSmokeBuild {
    param(
        [string]$SourceDir,
        [string]$BinaryDir
    )

    if (Test-Path $BinaryDir) {
        Remove-Item -Recurse -Force $BinaryDir
    }

    & cmake -S $SourceDir -B $BinaryDir | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "cmake configure failed: $SourceDir"
    }

    & cmake --build $BinaryDir | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "cmake build failed: $BinaryDir"
    }
}

function Invoke-DotNetBuild {
    param([string]$ProjectPath)

    & dotnet build $ProjectPath -c Release | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "dotnet build failed: $ProjectPath"
    }
}

function Invoke-PresetBuildSmoke {
    param(
        [string]$PresetName,
        [bool]$ValidateOnly = $false
    )

    $arguments = @("--preset", $PresetName)
    if ($ValidateOnly) {
        $arguments += "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON"
    }

    $binaryDir = Join-Path $repoRoot "artifacts\presets\$PresetName"
    if (Test-Path $binaryDir) {
        Remove-Item -Recurse -Force $binaryDir
    }

    & cmake @arguments | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "cmake preset configure failed: $PresetName"
    }

    & cmake --build $binaryDir | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "cmake preset build failed: $PresetName"
    }
}

function Invoke-RoutingBuildSmoke {
    param(
        [string]$PresetTarget,
        [string]$ToolchainFile,
        [string]$BinaryDir,
        [string]$Generator
    )

    if (Test-Path $BinaryDir) {
        Remove-Item -Recurse -Force $BinaryDir
    }

    $arguments = @(
        "-S", $repoRoot,
        "-B", $BinaryDir,
        "-G", $Generator,
        "-DROADMAP0_PRESET_TARGET=$PresetTarget",
        "-DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON",
        "-DCMAKE_TOOLCHAIN_FILE=$ToolchainFile"
    )

    & cmake @arguments | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "cmake routing smoke failed: $PresetTarget"
    }

    & cmake --build $BinaryDir | Out-Host
    if ($LASTEXITCODE -ne 0) {
        throw "cmake routing build failed: $PresetTarget"
    }
}

function Write-GateRecord {
    param(
        [string]$OutputPath,
        [string]$GateName,
        [string]$Status,
        [string]$Preset,
        [string]$Notes
    )

    $record = [PSCustomObject]@{
        gateName     = $GateName
        hostProfile  = $HostProfile
        status       = $Status
        preset       = $Preset
        notes        = $Notes
    }

    $directory = Split-Path -Parent $OutputPath
    if (-not [string]::IsNullOrEmpty($directory)) {
        New-Item -ItemType Directory -Force -Path $directory | Out-Null
    }

    $json = $record | ConvertTo-Json -Depth 8
    [System.IO.File]::WriteAllText($OutputPath, $json)
}

$repoRoot = Get-RepoRoot
$artifactRoot = Join-Path $repoRoot "artifacts\verify-roadmap-0\$HostProfile"
$commonArtifactRoot = Join-Path $artifactRoot "common"
$compareScript = Join-Path $repoRoot "tests\contracts\trace\compare-warmup-trace.ps1"
$hostEmbeddingDll = Join-Path $repoRoot "artifacts\smoke\bin\HostEmbeddingLite\Release\net8.0\HostEmbeddingLite.dll"

New-Item -ItemType Directory -Force -Path $artifactRoot | Out-Null
New-Item -ItemType Directory -Force -Path $commonArtifactRoot | Out-Null

$isWindows = [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform([System.Runtime.InteropServices.OSPlatform]::Windows)
$isMacos = [System.Runtime.InteropServices.RuntimeInformation]::IsOSPlatform([System.Runtime.InteropServices.OSPlatform]::OSX)

if ($HostProfile -eq "windows" -and -not $isWindows) {
    throw "HostProfile=windows requires a Windows host"
}

if ($HostProfile -eq "macos" -and -not $isMacos) {
    throw "HostProfile=macos requires a macOS host"
}

Write-Step "List CMake presets"
$presetList = & cmake --list-presets=all 2>&1
if ($LASTEXITCODE -ne 0) {
    throw "cmake --list-presets=all failed"
}
[System.IO.File]::WriteAllLines((Join-Path $artifactRoot "cmake-presets.txt"), $presetList)
$presetList | Out-Host

Write-Step "Parse schema and trace JSON assets"
$jsonPaths = @(
    (Join-Path $repoRoot "CMakePresets.json")
)
$jsonPaths += (Get-ChildItem (Join-Path $repoRoot "analysis\contracts\schemas") -Filter *.json | ForEach-Object FullName)
$jsonPaths += (Get-ChildItem (Join-Path $repoRoot "analysis\contracts\examples") -Filter *.json | ForEach-Object FullName)
$jsonPaths += (Get-ChildItem (Join-Path $repoRoot "tests\contracts\schema") -Filter *.json | ForEach-Object FullName)
$jsonPaths += (Get-ChildItem (Join-Path $repoRoot "tests\contracts\trace") -Recurse -Filter *.json | ForEach-Object FullName)
Assert-JsonFilesParse -Paths $jsonPaths

Write-Step "Build native ABI compile-only smoke"
Invoke-NativeSmokeBuild `
    -SourceDir (Join-Path $repoRoot "tests\contracts\native\abi") `
    -BinaryDir (Join-Path $commonArtifactRoot "native-abi-config")

Write-Step "Build native bridge compile-only smoke"
Invoke-NativeSmokeBuild `
    -SourceDir (Join-Path $repoRoot "tests\contracts\native\bridge") `
    -BinaryDir (Join-Path $commonArtifactRoot "native-bridge-config")

Write-Step "Build smoke input projects"
foreach ($projectName in @("HelloWorld", "GenericEcho", "ReflectionLite", "PInvokeLite", "HostEmbeddingLite")) {
    Invoke-DotNetBuild -ProjectPath (Join-Path $repoRoot "tests\smoke\input\$projectName\$projectName.csproj")
}

Write-Step "Validate Linux packaging routing smoke"
Invoke-RoutingBuildSmoke `
    -PresetTarget "linux-x64-packaging" `
    -ToolchainFile (Join-Path $repoRoot "build\toolchains\linux-x64.cmake") `
    -BinaryDir (Join-Path $commonArtifactRoot "linux-packaging-routing") `
    -Generator (Get-HostRoutingGenerator)

Write-Step "Register Linux packaging shell gate"
Write-GateRecord `
    -OutputPath (Join-Path $artifactRoot "linux-packaging.gate.json") `
    -GateName "linux-packaging" `
    -Status "routing-validated" `
    -Preset "linux-x64-packaging" `
    -Notes "Linux preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; full cross toolchain remains outside Roadmap 0."

switch ($HostProfile) {
    "windows" {
        Write-Step "Build Windows reference preset smoke"
        Invoke-PresetBuildSmoke -PresetName "windows-x64-reference"

        Write-Step "Run Windows reference desktop trace compare"
        $tracePath = Join-Path $artifactRoot "windows-warmup-trace.runtime.json"
        & dotnet $hostEmbeddingDll --trace-platform windows --trace-output $tracePath | Out-Host
        if ($LASTEXITCODE -ne 0) {
            throw "HostEmbeddingLite windows trace export failed"
        }

        & $compareScript `
            -ExpectedPath (Join-Path $repoRoot "tests\contracts\trace\snapshots\windows-warmup-trace.snapshot.json") `
            -ActualPath $tracePath

        Write-GateRecord `
            -OutputPath (Join-Path $artifactRoot "windows-reference-desktop.gate.json") `
            -GateName "windows-reference-desktop" `
            -Status "passed" `
            -Preset "windows-x64-reference" `
            -Notes "Windows reference desktop gate passed with warmup trace compare."

        Write-Step "Validate Android startup smoke routing smoke"
        Invoke-RoutingBuildSmoke `
            -PresetTarget "android-arm64-smoke" `
            -ToolchainFile (Join-Path $repoRoot "build\toolchains\android-arm64.cmake") `
            -BinaryDir (Join-Path $commonArtifactRoot "android-smoke-routing") `
            -Generator (Get-HostRoutingGenerator)

        Write-Step "Register Android startup smoke shell gate"
        Write-GateRecord `
            -OutputPath (Join-Path $artifactRoot "android-startup-smoke.gate.json") `
            -GateName "android-startup-smoke" `
            -Status "routing-validated" `
            -Preset "android-arm64-smoke" `
            -Notes "Android preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; real Android toolchain execution remains outside Roadmap 0."
    }
    "macos" {
        Write-Step "Build macOS reference preset smoke"
        Invoke-PresetBuildSmoke -PresetName "macos-reference"

        Write-Step "Run macOS reference desktop trace compare"
        $tracePath = Join-Path $artifactRoot "macos-warmup-trace.runtime.json"
        & dotnet $hostEmbeddingDll --trace-platform macos --trace-output $tracePath | Out-Host
        if ($LASTEXITCODE -ne 0) {
            throw "HostEmbeddingLite macOS trace export failed"
        }

        & $compareScript `
            -ExpectedPath (Join-Path $repoRoot "tests\contracts\trace\snapshots\macos-warmup-trace.snapshot.json") `
            -ActualPath $tracePath

        Write-GateRecord `
            -OutputPath (Join-Path $artifactRoot "macos-reference-desktop.gate.json") `
            -GateName "macos-reference-desktop" `
            -Status "passed" `
            -Preset "macos-reference" `
            -Notes "macOS reference desktop gate passed with warmup trace compare."

        Write-Step "Validate iOS packaging routing smoke"
        Invoke-RoutingBuildSmoke `
            -PresetTarget "ios-arm64-packaging" `
            -ToolchainFile (Join-Path $repoRoot "build\toolchains\ios-arm64.cmake") `
            -BinaryDir (Join-Path $commonArtifactRoot "ios-packaging-routing") `
            -Generator "Xcode"

        Write-Step "Register iOS packaging shell gate"
        Write-GateRecord `
            -OutputPath (Join-Path $artifactRoot "ios-packaging.gate.json") `
            -GateName "ios-packaging" `
            -Status "routing-validated" `
            -Preset "ios-arm64-packaging" `
            -Notes "iOS preset remains visible in CMakePresets, and its toolchain/router/harness path was validated with a host-compatible generator; full Apple packaging flow remains outside Roadmap 0."
    }
}

Write-Step "verify-roadmap-0 completed for host profile '$HostProfile'"
