﻿param(
    [string]$RepoRoot,
    [switch]$SkipCatalogGen,
    [switch]$Verbose
)

if (-not $RepoRoot) {
    $RepoRoot = git -C $PSScriptRoot rev-parse --show-toplevel
}

$sourceDir       = Join-Path $RepoRoot 'skills\library\skills'
$catalogScript   = Join-Path $RepoRoot 'skills\tooling\catalog\generate_skill_catalog.py'
$claudeSkillsDir = Join-Path $RepoRoot '.claude\skills'
$codexSkillsDir  = Join-Path $RepoRoot '.codex\skills'
$hermesSkillsDir = Join-Path $RepoRoot '.hermes\skills'
$hermesGlobalCategory = 'project-chaos'  # 全局 skills 分类名
$hermesGlobalSkillsDir = Join-Path $env:USERPROFILE ".hermes\skills\$hermesGlobalCategory"

if (-not (Test-Path -LiteralPath $sourceDir)) {
    Write-Error "[skill-bootstrap] Source skill directory not found: $sourceDir"
    exit 1
}

# Step 1: Generate catalog
if (-not $SkipCatalogGen) {
    if (Test-Path -LiteralPath $catalogScript) {
        Write-Host "[skill-bootstrap] Generating skill catalog..."
        python $catalogScript --repo-root $RepoRoot
        if ($LASTEXITCODE -ne 0) {
            Write-Error "[skill-bootstrap] Catalog generation failed."
            exit $LASTEXITCODE
        }
    } else {
        Write-Warning "[skill-bootstrap] Catalog generator not found: $catalogScript"
    }
}

# Step 2: Helper functions
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

function Backup-And-RemoveDir {
    param([string]$Path)
    if (-not (Test-Path -LiteralPath $Path)) { return }
    $item = Get-Item -LiteralPath $Path -Force
    if ($item.Attributes -band [System.IO.FileAttributes]::ReparsePoint) {
        # It's a junction - just remove it
        Remove-Item -LiteralPath $Path -Force -Recurse
        Write-Host "[skill-bootstrap] Removed existing junction: $Path"
        return
    }
    # It's a real directory - back it up
    $backupPath = "$Path.backup-$(Get-Date -Format 'yyyyMMdd-HHmmss')"
    Rename-Item -LiteralPath $Path -NewName (Split-Path -Leaf $backupPath)
    Write-Host "[skill-bootstrap] Backed up existing directory to: $backupPath"
}

function Ensure-Junction {
    param([string]$Name, [string]$LinkPath, [string]$TargetPath)
    $parentDir = Split-Path -Parent $LinkPath
    if (-not (Test-Path -LiteralPath $parentDir)) {
        New-Item -ItemType Directory -Path $parentDir -Force | Out-Null
    }
    if (Test-JunctionToSource -Path $LinkPath -ExpectedTarget $TargetPath) {
        Write-Host "[skill-bootstrap] $Name already points to $TargetPath"
        return $true
    }
    Backup-And-RemoveDir -Path $LinkPath
    New-Item -ItemType Junction -Path $LinkPath -Target $TargetPath | Out-Null
    Write-Host "[skill-bootstrap] Linked $Name -> $TargetPath"
    return $true
}

# Step 3: Create junctions
$linked = 0
if (Ensure-Junction -Name 'Claude skills' -LinkPath $claudeSkillsDir -TargetPath $sourceDir) { $linked++ }
if (Ensure-Junction -Name 'Codex skills' -LinkPath $codexSkillsDir -TargetPath $sourceDir) { $linked++ }
if (Ensure-Junction -Name 'Hermes skills' -LinkPath $hermesSkillsDir -TargetPath $sourceDir) { $linked++ }

Write-Host "[skill-bootstrap] Done. $linked / 3 junctions created/verified."

# Step 3b: Register project skills in Hermes global skills (A+C pattern)
# Hermes does NOT auto-scan .hermes/skills/ - only reads from ~/.hermes/skills/<category>/<name>/
# We create per-skill junctions in ~/.hermes/skills/project-chaos/ so Hermes discovers them.
$projectSkills = 0
Write-Host "[skill-bootstrap] Linking project skills into Hermes global: $hermesGlobalSkillsDir"
if (Test-Path -LiteralPath $sourceDir) {
    Get-ChildItem -Directory -LiteralPath $sourceDir | ForEach-Object {
        $skillName = $_.Name
        $linkPath = Join-Path $hermesGlobalSkillsDir $skillName
        if (Ensure-Junction -Name "Hermes global: $skillName" -LinkPath $linkPath -TargetPath $_.FullName) {
            $projectSkills++
        }
    }
}
Write-Host "[skill-bootstrap] Registered $projectSkills project skills in Hermes global."

# Step 4: Sync template files for gitignored run-time essentials
$templateDir = Join-Path $PSScriptRoot 'templates'
$syncTargets = @(
    @{ Source = Join-Path $templateDir '.claude\settings.json'; Dest = Join-Path $RepoRoot '.claude\settings.json' },
    @{ Source = Join-Path $templateDir '.codex\docs\testing.md'; Dest = Join-Path $RepoRoot '.codex\docs\testing.md' },
    @{ Source = Join-Path $templateDir '.codex\docs\windows\polyglot-hooks.md'; Dest = Join-Path $RepoRoot '.codex\docs\windows\polyglot-hooks.md' },
    @{ Source = Join-Path $templateDir '.hermes\config.yaml'; Dest = Join-Path $RepoRoot '.hermes\config.yaml' }
)
$synced = 0
foreach ($entry in $syncTargets) {
    if (-not (Test-Path -LiteralPath $entry.Source)) {
        Write-Warning "[skill-bootstrap] Template not found: $($entry.Source)"
        continue
    }
    $destParent = Split-Path -Parent $entry.Dest
    if (-not (Test-Path -LiteralPath $destParent)) {
        New-Item -ItemType Directory -Path $destParent -Force | Out-Null
    }
    if (Test-Path -LiteralPath $entry.Dest) {
        Write-Host "[skill-bootstrap] Already exists, skipping: $($entry.Dest)"
        $synced++
        continue
    }
    Copy-Item -LiteralPath $entry.Source -Destination $entry.Dest
    Write-Host "[skill-bootstrap] Synced template: $($entry.Dest)"
    $synced++
}
Write-Host "[skill-bootstrap] Templates: $synced / $($syncTargets.Count) synced."
