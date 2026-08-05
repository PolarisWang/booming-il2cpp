param()

$ErrorActionPreference = 'Continue'
$repoRoot = git -C $PSScriptRoot rev-parse --show-toplevel
$chapterRoot = Join-Path $repoRoot '.ai/skills'
$formalSkillsDir = Join-Path $chapterRoot 'library/skills'
$bootstrapScript = Join-Path $chapterRoot 'runtime/bootstrap-skills.ps1'
$catalogScript = Join-Path $chapterRoot 'tooling/catalog/generate_skill_catalog.py'

function Has-ScaffoldPlaceholder {
    param([string]$Text)
    if ([string]::IsNullOrWhiteSpace($Text)) { return $false }
    $ph = @('TODO: 用 1-2 句话描述该 skill 的核心职责、输入信号和输出边界。')
    foreach ($p in $ph) {
        if ($Text.Contains($p)) { return $true }
    }
    return $false
}

function Has-TodoText {
    param([string]$Text)
    if ([string]::IsNullOrWhiteSpace($Text)) { return $false }
    return $Text -match '(?i)todo'
}

function Get-FormalSkillIssues {
    param([string]$SkillDir)
    $issues = New-Object System.Collections.Generic.List[string]
    $manifestPath = Join-Path $SkillDir 'skill.manifest.json'
    $skillFile = Join-Path $SkillDir 'SKILL.md'
    $metadataPath = Join-Path $SkillDir 'scaffold.meta.json'
    $candidateNotesPath = Join-Path $SkillDir 'candidate-notes.md'

    if (Test-Path -LiteralPath $metadataPath)   { $issues.Add('contains scaffold.meta.json') }
    if (Test-Path -LiteralPath $candidateNotesPath) { $issues.Add('contains candidate-notes.md') }
    if (-not (Test-Path -LiteralPath $manifestPath)) { $issues.Add('missing skill.manifest.json'); return @($issues) }
    if (-not (Test-Path -LiteralPath $skillFile))    { $issues.Add('missing SKILL.md'); return @($issues) }

    $manifest = Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
    $skillContent = Get-Content -LiteralPath $skillFile -Raw -Encoding UTF8

    $description = $null
    $lines = $skillContent -split "`r?`n"
    if ($lines.Count -ge 3 -and $lines[0].Trim() -eq '---') {
        for ($i = 1; $i -lt $lines.Count; $i++) {
            if ($lines[$i].Trim() -eq '---') { break }
            if ($lines[$i] -match '^description\s*:\s*(.*)$') { $description = $Matches[1].Trim() }
        }
    }

    $summary = [string]$manifest.summary
    if ([string]$manifest.status -eq 'draft') { $issues.Add('status must not be draft') }
    if (Has-TodoText -Text $summary) { $issues.Add('summary contains TODO') }
    if (Has-TodoText -Text $description) { $issues.Add('description contains TODO') }

    $triggerCount = @($manifest.trigger_signals).Count
    if ($triggerCount -lt 1 -or $triggerCount -gt 5) { $issues.Add("trigger_signals count $triggerCount out of range (1-5)") }

    if (Has-ScaffoldPlaceholder -Text $skillContent) { $issues.Add('SKILL.md body contains scaffold placeholder') }

    return @($issues)
}

# Step 1: Verify scripts exist
if (-not (Test-Path -LiteralPath $bootstrapScript)) { throw "[skill-verify] Missing bootstrap: $bootstrapScript" }
if (-not (Test-Path -LiteralPath $catalogScript))  { throw "[skill-verify] Missing catalog: $catalogScript" }

# Step 2: Run bootstrap
Write-Host "[skill-verify] Running bootstrap..."
& $bootstrapScript
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Step 3: Check catalog
Write-Host "[skill-verify] Checking catalog..."
python $catalogScript --repo-root $repoRoot --check
if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }

# Step 4: Verify routing stub
Write-Host "[skill-verify] Checking routing stub..."
$entryStubPath = Join-Path $repoRoot '.claude/skills/dev-il2cpp/SKILL.md'
$libraryEntry  = Join-Path $repoRoot '.ai/skills/library/skills/dev-il2cpp/SKILL.md'
$stubErrors = @()

if (-not (Test-Path -LiteralPath $entryStubPath)) {
    $stubErrors += "Routing stub missing: $entryStubPath"
} else {
    $stubContent = Get-Content -LiteralPath $entryStubPath -Raw
    if ($stubContent -notmatch '路由桩') {
        $stubErrors += "Routing stub marker not found in $entryStubPath"
    }
}
if (-not (Test-Path -LiteralPath $libraryEntry)) {
    $stubErrors += "Library entry SKILL.md missing: $libraryEntry"
}

if ($stubErrors.Count -gt 0) {
    Write-Warning "[skill-verify] Routing stub issues:"
    foreach ($e in $stubErrors) { Write-Warning "  - $e" }
} else {
    Write-Host "[skill-verify] Routing stub verified OK."
}

# Step 5: Verify formal skills
Write-Host "[skill-verify] Checking formal skills..."
$formalSkillIssues = New-Object System.Collections.Generic.List[string]
if (Test-Path -LiteralPath $formalSkillsDir) {
    foreach ($skillDir in Get-ChildItem -LiteralPath $formalSkillsDir -Directory) {
        foreach ($issue in (Get-FormalSkillIssues -SkillDir $skillDir.FullName)) {
            $formalSkillIssues.Add(('{0}: {1}' -f $skillDir.Name, $issue))
        }
    }
}
if ($formalSkillIssues.Count -gt 0) {
    Write-Warning "[skill-verify] Formal skills issues:"
    foreach ($issue in $formalSkillIssues) { Write-Warning "  - $issue" }
} else {
    Write-Host "[skill-verify] All formal skills clean."
}

# Step 6: Verify evolution infrastructure
Write-Host "[skill-verify] Checking evolution infrastructure..."
$evolutionDirs = @(
    'lifecycle/evolution/proposals',
    'lifecycle/evolution/lineage',
    'lifecycle/telemetry/health'
)
foreach ($relDir in $evolutionDirs) {
    $fullPath = Join-Path $chapterRoot $relDir
    if (-not (Test-Path -LiteralPath $fullPath)) {
        Write-Warning "[skill-verify] Missing evolution directory: $relDir"
    } else {
        Write-Host "[skill-verify] Evolution directory ok: $relDir"
    }
}

# Step 7: Check manifest migration (version_history + evolution_lineage)
Write-Host "[skill-verify] Checking manifest evolution fields..."
$manifestIssues = New-Object System.Collections.Generic.List[string]
if (Test-Path -LiteralPath $formalSkillsDir) {
    foreach ($skillDir in Get-ChildItem -LiteralPath $formalSkillsDir -Directory) {
        $manifestPath = Join-Path $skillDir.FullName 'skill.manifest.json'
        if (-not (Test-Path -LiteralPath $manifestPath)) { continue }
        $manifest = Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
        if (-not $manifest.version_history) { $manifestIssues.Add("$($skillDir.Name): missing version_history") }
        if (-not $manifest.evolution_lineage) { $manifestIssues.Add("$($skillDir.Name): missing evolution_lineage") }
    }
}
if ($manifestIssues.Count -gt 0) {
    Write-Warning "[skill-verify] Manifest evolution field issues:"
    foreach ($issue in $manifestIssues) { Write-Warning "  - $issue" }
} else {
    Write-Host "[skill-verify] All manifests have evolution fields."
}

Write-Host '[skill-verify] Skill creation/loading pipeline looks consistent.'
exit 0
