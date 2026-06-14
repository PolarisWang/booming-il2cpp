param(
    [string]$RepoRoot,
    [switch]$SkipCatalogGen,
    [switch]$Verbose
)

if (-not $RepoRoot) {
    $RepoRoot = git -C $PSScriptRoot rev-parse --show-toplevel
}

$sourceDir       = Join-Path $RepoRoot 'skills\library\skills'
$catalogScript   = Join-Path $RepoRoot 'skills\tooling\catalog\generate_skill_catalog.py'
$entryStubPath   = Join-Path $RepoRoot '.claude\skills\dev-il2cpp\SKILL.md'
$libraryEntry    = Join-Path $sourceDir 'dev-il2cpp\SKILL.md'

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

# Step 2: Verify routing stub
$stubOk = $true
if (-not (Test-Path -LiteralPath $entryStubPath)) {
    Write-Error "[skill-bootstrap] Routing stub missing: $entryStubPath"
    $stubOk = $false
} else {
    $stubContent = Get-Content -LiteralPath $entryStubPath -Raw
    if ($stubContent -notmatch "路由桩") {
        Write-Warning "[skill-bootstrap] Entry SKILL.md is not a routing stub (missing '路由桩' marker)."
        $stubOk = $false
    }
}

if (-not (Test-Path -LiteralPath $libraryEntry)) {
    Write-Error "[skill-bootstrap] Library entry SKILL.md missing: $libraryEntry"
    $stubOk = $false
}

if ($stubOk) {
    Write-Host "[skill-bootstrap] Routing stub and library entry verified OK."
} else {
    Write-Warning "[skill-bootstrap] Routing stub issues found. Check warnings above."
}

# Step 3: Sync template files for gitignored run-time essentials
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
Write-Host "[skill-bootstrap] Done. (Junction mechanism removed; routing via skill registry)"
