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

function Assert-Equal {
    param(
        [string]$Expected,
        [string]$Actual,
        [string]$Message
    )

    if ($Expected -ne $Actual) {
        throw "$Message`nExpected: $Expected`nActual: $Actual"
    }
}

function Get-RunFunctionsSource {
    param(
        [string]$ScriptPath,
        [string[]]$FunctionNames
    )

    $tokens = $null
    $errors = $null
    $ast = [System.Management.Automation.Language.Parser]::ParseFile($ScriptPath, [ref]$tokens, [ref]$errors)
    if ($errors.Count -gt 0) {
        throw "unable to parse run.ps1"
    }

    $definitions = foreach ($functionName in $FunctionNames) {
        $functionAst = $ast.Find(
            {
                param($node)
                $node -is [System.Management.Automation.Language.FunctionDefinitionAst] -and $node.Name -eq $functionName
            },
            $false
        )

        if ($null -eq $functionAst) {
            throw "function missing from run.ps1: $functionName"
        }

        $functionAst.Extent.Text
    }

    return ($definitions -join [Environment]::NewLine + [Environment]::NewLine)
}

$repoRoot = Get-RepoRoot
$runScriptPath = Join-Path $repoRoot "run.ps1"
Invoke-Expression (Get-RunFunctionsSource -ScriptPath $runScriptPath -FunctionNames @(
    "Get-RuntimeInformationPropertyValue",
    "Get-HostOsDescription",
    "Convert-ArchitectureName",
    "Get-HostArchitectureName",
    "Get-HostPlatformId"
))

Add-Type -TypeDefinition @"
public static class BoomRunFakeRuntimeInfoMissingOsArchitecture
{
    public static string OSDescription { get { return "Microsoft Windows 10.0"; } }
    public static string ProcessArchitecture { get { return "Arm64"; } }
}

public static class BoomRunFakeRuntimeInfoMissingArchitecture
{
    public static string OSDescription { get { return "Microsoft Windows 10.0"; } }
}
"@

$platformFromProcessArch = Get-HostPlatformId -RuntimeInformationType ([BoomRunFakeRuntimeInfoMissingOsArchitecture])
Assert-Equal -Expected "windows-arm64" -Actual $platformFromProcessArch -Message "Get-HostPlatformId should fall back to ProcessArchitecture when OSArchitecture is unavailable"

$platformFromEnvironment = Get-HostPlatformId -RuntimeInformationType ([BoomRunFakeRuntimeInfoMissingArchitecture]) -EnvironmentVariables @{
    PROCESSOR_ARCHITECTURE = "AMD64"
}
Assert-Equal -Expected "windows-x64" -Actual $platformFromEnvironment -Message "Get-HostPlatformId should fall back to PROCESSOR_ARCHITECTURE when RuntimeInformation lacks architecture properties"

Write-Host "run host platform compatibility tests passed"
