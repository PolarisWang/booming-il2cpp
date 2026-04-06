[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidateSet("windows", "macos")]
    [string]$HostProfile
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

# Canonical trace contracts live under tests\contracts\trace.
$pythonCommand = Get-Command python -ErrorAction SilentlyContinue
if ($null -eq $pythonCommand) {
    $pythonCommand = Get-Command py -ErrorAction SilentlyContinue
}
if ($null -eq $pythonCommand) {
    throw "python is required to run verify-roadmap-0.ps1"
}

$pythonScript = Join-Path $PSScriptRoot "verify-roadmap-0.py"
if ($pythonCommand.Name -eq "py") {
    & py -3 $pythonScript --host-profile $HostProfile
}
else {
    & python $pythonScript --host-profile $HostProfile
}

exit $LASTEXITCODE
