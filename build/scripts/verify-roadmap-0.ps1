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

function Test-JsonPropertyExists {
    param(
        $Value,
        [string]$PropertyName
    )

    if (-not ($Value -is [pscustomobject])) {
        return $false
    }

    return $null -ne $Value.PSObject.Properties[$PropertyName]
}

function Get-JsonPropertyValue {
    param(
        $Value,
        [string]$PropertyName
    )

    Write-Output -NoEnumerate $Value.PSObject.Properties[$PropertyName].Value
}

function Test-JsonSchemaType {
    param(
        $Value,
        [string]$SchemaType
    )

    switch ($SchemaType) {
        "object" { return $Value -is [pscustomobject] }
        "array" { return $Value -is [System.Array] -or ($Value -is [System.Collections.IList] -and -not ($Value -is [string])) }
        "string" { return $Value -is [string] }
        "integer" { return $Value -is [sbyte] -or $Value -is [byte] -or $Value -is [int16] -or $Value -is [int32] -or $Value -is [int64] }
        default { throw "unsupported schema type: $SchemaType" }
    }
}

function Assert-JsonMatchesSchema {
    param(
        $Value,
        $Schema,
        [string]$Path
    )

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "const") {
        $expectedConst = Get-JsonPropertyValue -Value $Schema -PropertyName "const"
        if ($Value -ne $expectedConst) {
            throw "schema const mismatch at ${Path}: expected '$expectedConst' actual '$Value'"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "type") {
        $schemaType = [string](Get-JsonPropertyValue -Value $Schema -PropertyName "type")
        if (-not (Test-JsonSchemaType -Value $Value -SchemaType $schemaType)) {
            throw "schema type mismatch at ${Path}: expected '$schemaType'"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "enum") {
        $allowedValues = Get-JsonPropertyValue -Value $Schema -PropertyName "enum"
        if ($allowedValues -notcontains $Value) {
            throw "schema enum mismatch at ${Path}: value '$Value' not allowed"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "minLength") {
        $minLength = [int](Get-JsonPropertyValue -Value $Schema -PropertyName "minLength")
        if ($null -eq $Value -or ([string]$Value).Length -lt $minLength) {
            throw "schema minLength mismatch at ${Path}: expected >= $minLength"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "minimum") {
        $minimum = [int](Get-JsonPropertyValue -Value $Schema -PropertyName "minimum")
        if ([int]$Value -lt $minimum) {
            throw "schema minimum mismatch at ${Path}: expected >= $minimum"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "pattern") {
        $pattern = [string](Get-JsonPropertyValue -Value $Schema -PropertyName "pattern")
        if (-not [regex]::IsMatch([string]$Value, $pattern)) {
            throw "schema pattern mismatch at ${Path}: value '$Value' does not match '$pattern'"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "required") {
        foreach ($requiredName in (Get-JsonPropertyValue -Value $Schema -PropertyName "required")) {
            if (-not (Test-JsonPropertyExists -Value $Value -PropertyName $requiredName)) {
                throw "schema required property missing at ${Path}: '$requiredName'"
            }
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "properties") {
        $properties = Get-JsonPropertyValue -Value $Schema -PropertyName "properties"
        foreach ($property in $properties.PSObject.Properties) {
            if (Test-JsonPropertyExists -Value $Value -PropertyName $property.Name) {
                Assert-JsonMatchesSchema `
                    -Value (Get-JsonPropertyValue -Value $Value -PropertyName $property.Name) `
                    -Schema $property.Value `
                    -Path "$Path.$($property.Name)"
            }
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "minItems") {
        $minItems = [int](Get-JsonPropertyValue -Value $Schema -PropertyName "minItems")
        if (@($Value).Count -lt $minItems) {
            throw "schema minItems mismatch at ${Path}: expected >= $minItems"
        }
    }

    if (Test-JsonPropertyExists -Value $Schema -PropertyName "items") {
        $itemSchema = Get-JsonPropertyValue -Value $Schema -PropertyName "items"
        $index = 0
        foreach ($item in @($Value)) {
            Assert-JsonMatchesSchema -Value $item -Schema $itemSchema -Path "$Path[$index]"
            $index++
        }
    }
}

function Assert-JsonFilesParse {
    param([string[]]$Paths)

    foreach ($path in $Paths) {
        [void](Read-JsonFile -Path $path)
    }
}

function Assert-AnalysisContracts {
    $schemaDir = Join-Path $repoRoot "analysis\contracts\schemas"
    $exampleDir = Join-Path $repoRoot "analysis\contracts\examples"
    $snapshotDir = Join-Path $repoRoot "tests\contracts\schema"

    foreach ($schemaFile in Get-ChildItem $schemaDir -Filter *.schema.json) {
        $baseName = $schemaFile.Name -replace "\.schema\.json$", ""
        $schema = Read-JsonFile -Path $schemaFile.FullName
        $example = Read-JsonFile -Path (Join-Path $exampleDir "$baseName.min.json")
        $snapshot = Read-JsonFile -Path (Join-Path $snapshotDir "$baseName.snapshot.json")

        Assert-JsonMatchesSchema -Value $example -Schema $schema -Path "$baseName.example"
        Assert-JsonMatchesSchema -Value $snapshot -Schema $schema -Path "$baseName.snapshot"
    }
}

function Assert-TraceContracts {
    $schema = Read-JsonFile -Path (Join-Path $repoRoot "tests\contracts\trace\schema\warmup-trace.schema.json")

    foreach ($snapshotPath in Get-ChildItem (Join-Path $repoRoot "tests\contracts\trace\snapshots") -Filter *.json | ForEach-Object FullName) {
        $snapshot = Read-JsonFile -Path $snapshotPath
        Assert-JsonMatchesSchema -Value $snapshot -Schema $schema -Path ([System.IO.Path]::GetFileName($snapshotPath))
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

function Invoke-DotNetRuntimeSmoke {
    param(
        [string]$DllPath,
        [string[]]$ExpectedPatterns
    )

    $output = & dotnet $DllPath 2>&1
    $output | Out-Host

    if ($LASTEXITCODE -ne 0) {
        throw "dotnet runtime smoke failed: $DllPath"
    }

    $joinedOutput = ($output -join [Environment]::NewLine)
    foreach ($pattern in $ExpectedPatterns) {
        if ($joinedOutput -notmatch [regex]::Escape($pattern)) {
            throw "runtime smoke output mismatch for ${DllPath}: missing '$pattern'"
        }
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
Set-Location $repoRoot
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

Write-Step "Validate analysis schema/example/snapshot contracts"
Assert-AnalysisContracts

Write-Step "Validate trace schema and snapshots"
Assert-TraceContracts

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

Write-Step "Run managed smoke projects"
Invoke-DotNetRuntimeSmoke `
    -DllPath (Join-Path $repoRoot "artifacts\smoke\bin\HelloWorld\Release\net8.0\HelloWorld.dll") `
    -ExpectedPatterns @("HelloWorld smoke entry reached.", "register:Main")
Invoke-DotNetRuntimeSmoke `
    -DllPath (Join-Path $repoRoot "artifacts\smoke\bin\GenericEcho\Release\net8.0\GenericEcho.dll") `
    -ExpectedPatterns @("roadmap0", "42", "roadmap0:roadmap0")
Invoke-DotNetRuntimeSmoke `
    -DllPath (Join-Path $repoRoot "artifacts\smoke\bin\ReflectionLite\Release\net8.0\ReflectionLite.dll") `
    -ExpectedPatterns @("field=BackingField:Int32", "generic-method=String")
Invoke-DotNetRuntimeSmoke `
    -DllPath (Join-Path $repoRoot "artifacts\smoke\bin\PInvokeLite\Release\net8.0\PInvokeLite.dll") `
    -ExpectedPatterns @("marshal=interop-smoke", "export=boom_smoke_add:7", "symbol=True")
Invoke-DotNetRuntimeSmoke `
    -DllPath (Join-Path $repoRoot "artifacts\smoke\bin\HostEmbeddingLite\Release\net8.0\HostEmbeddingLite.dll") `
    -ExpectedPatterns @("HostEmbeddingSession:InvokeManagedEntry:True", "guards=invalid-detach:True|double-start:True|unattached-entry:True")

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
