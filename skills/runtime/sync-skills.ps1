param(
    [string]$RepoRoot,
    [switch]$Verbose
)

if (-not $RepoRoot) {
    $RepoRoot = git -C $PSScriptRoot rev-parse --show-toplevel
}

$sourceDir       = Join-Path $RepoRoot 'skills\library\skills'
$claudeSkillsDir = Join-Path $RepoRoot '.claude\skills'
$codexSkillsDir  = Join-Path $RepoRoot '.codex\skills'

if (-not (Test-Path -LiteralPath $sourceDir)) {
    Write-Error "[skill-sync] Source skill directory not found: $sourceDir"
    exit 1
}

function Get-NormalizedPath {
    param([string]$Path)
    return [System.IO.Path]::GetFullPath($Path.TrimEnd('\'))
}

function Test-JunctionToSource {
    param([string]$Path, [string]$ExpectedTarget)
    if (-not (Test-Path -LiteralPath $Path)) { return $false }
    $item = Get-Item -LiteralPath $Path -Force
    if (-not ($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint)) { return $false }
    $expected = Get-NormalizedPath $ExpectedTarget
    foreach ($target in @($item.Target)) {
        if ($null -eq $target) { continue }
        if ((Get-NormalizedPath $target.ToString()) -eq $expected) { return $true }
    }
    return $false
}

function Sync-Directory {
    param([string]$Name, [string]$TargetDir, [string]$SourceDir)

    if (Test-JunctionToSource -Path $TargetDir -ExpectedTarget $SourceDir) {
        Write-Host "[skill-sync] $Name is a valid junction, no copy needed."
        return $true
    }

    Write-Host "[skill-sync] $Name is not a junction (or missing), copying files..."

    if (-not (Test-Path -LiteralPath $TargetDir)) {
        New-Item -ItemType Directory -Path $TargetDir -Force | Out-Null
    }

    # Robocopy: /MIR mirrors directory tree, /NJH /NJP /NDL suppresses verbose output
    $robocopyArgs = @(
        $SourceDir, $TargetDir, "/MIR", "/NJH", "/NJP", "/NDL",
        "/XD", ".git", "__pycache__", "node_modules"
    )
    $result = Start-Process -FilePath "robocopy" -ArgumentList $robocopyArgs -NoNewWindow -Wait -PassThru

    if ($result.ExitCode -ge 8) {
        Write-Error "[skill-sync] Robocopy failed with exit code $($result.ExitCode)."
        return $false
    }

    Write-Host "[skill-sync] $Name synced successfully."
    return $true
}

$synced = 0
if (Sync-Directory -Name 'Claude skills' -TargetDir $claudeSkillsDir -SourceDir $sourceDir) { $synced++ }
if (Sync-Directory -Name 'Codex skills' -TargetDir $codexSkillsDir -SourceDir $sourceDir) { $synced++ }

Write-Host "[skill-sync] Done. $synced / 2 targets synced."
