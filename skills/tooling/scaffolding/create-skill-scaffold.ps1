param(
    [Parameter(Mandatory = $true, Position = 0)]
    [string]$DirectoryName,

    [Parameter(Mandatory = $true, Position = 1)]
    [string]$Domain,

    [string]$SkillName,
    [string]$Title,
    [string]$Owner = '00-skills-maintainers',
    [ValidateSet('small', 'medium')][string]$TokenBudget = 'small',
    [ValidateSet('draft', 'active')][string]$Status = 'draft',
    [string]$Summary,
    [string]$Description,
    [string[]]$TriggerSignals,
    [string[]]$Resources,
    [switch]$RefreshCatalog
)

$ErrorActionPreference = 'Stop'

$repoRoot = git -C $PSScriptRoot rev-parse --show-toplevel
$incubatorRoot = Join-Path $repoRoot 'skills\lifecycle\incubator'
$domainCatalogPath = Join-Path $repoRoot 'skills\discovery\domain-catalog.json'
$catalogScript = Join-Path $repoRoot 'skills\tooling\catalog\generate_skill_catalog.py'

function Assert-HyphenatedName {
    param([Parameter(Mandatory = $true)][string]$Label, [Parameter(Mandatory = $true)][string]$Value)
    if ($Value -notmatch '^[a-z0-9]+(?:-[a-z0-9]+)*$') {
        throw "[skill-scaffold] $Label must use lowercase letters, digits, and hyphens only: $Value"
    }
}

function Normalize-ResourceList {
    param([string[]]$Values)
    $allowed = @('references', 'scripts', 'assets')
    $result = New-Object System.Collections.Generic.List[string]
    foreach ($value in @($Values)) {
        if ($null -eq $value) { continue }
        foreach ($item in ($value -split ',')) {
            $normalized = $item.Trim().ToLowerInvariant()
            if ([string]::IsNullOrWhiteSpace($normalized)) { continue }
            if ($allowed -notcontains $normalized) {
                throw "[skill-scaffold] Unsupported resource '$normalized'. Allowed: $($allowed -join ', ')"
            }
            if (-not $result.Contains($normalized)) { $null = $result.Add($normalized) }
        }
    }
    return @($result)
}

function Set-Utf8Content {
    param([string]$Path, [string]$Content)
    $parentDir = Split-Path -Parent $Path
    if (-not (Test-Path -LiteralPath $parentDir)) { New-Item -ItemType Directory -Path $parentDir -Force | Out-Null }
    $utf8NoBom = New-Object System.Text.UTF8Encoding($false)
    [System.IO.File]::WriteAllText($Path, $Content, $utf8NoBom)
}

if (-not (Test-Path -LiteralPath $domainCatalogPath)) {
    throw "[skill-scaffold] Missing domain catalog: $domainCatalogPath"
}

Assert-HyphenatedName -Label 'DirectoryName' -Value $DirectoryName

if ([string]::IsNullOrWhiteSpace($SkillName)) {
    $SkillName = if ($DirectoryName.StartsWith('dev-')) { $DirectoryName.Substring(4) } else { $DirectoryName }
}
Assert-HyphenatedName -Label 'SkillName' -Value $SkillName

if ([string]::IsNullOrWhiteSpace($Title)) { $Title = $SkillName }
if ([string]::IsNullOrWhiteSpace($Summary)) { $Summary = "TODO: summary for $SkillName" }
if ([string]::IsNullOrWhiteSpace($Description)) {
    $Description = "Use for tasks related to $SkillName. Fill in trigger signals and scope before publishing."
}
if ($null -eq $TriggerSignals -or $TriggerSignals.Count -eq 0) {
    $TriggerSignals = @("TODO: signal 1 for $SkillName", "TODO: signal 2 for $SkillName")
}

$normalizedResources = Normalize-ResourceList -Values $Resources
$domainCatalog = Get-Content -LiteralPath $domainCatalogPath -Raw -Encoding UTF8 | ConvertFrom-Json
$knownDomains = @($domainCatalog.domains | ForEach-Object { $_.id })
if ($knownDomains -notcontains $Domain) {
    throw "[skill-scaffold] Unknown domain '$Domain'. Allowed: $($knownDomains -join ', ')"
}
if ($Status -ne 'draft') {
    throw '[skill-scaffold] Manual scaffolds must stay in draft status until promoted.'
}

$scaffoldId = "manual-$DirectoryName"
$skillDir = Join-Path $incubatorRoot $scaffoldId
if (Test-Path -LiteralPath $skillDir) {
    throw "[skill-scaffold] Scaffold already exists: $skillDir"
}

$skillFilePath = Join-Path $skillDir 'SKILL.md'
$manifestPath = Join-Path $skillDir 'skill.manifest.json'
$metadataPath = Join-Path $skillDir 'scaffold.meta.json'
$createdPaths = New-Object System.Collections.Generic.List[string]

New-Item -ItemType Directory -Path $skillDir -Force | Out-Null
$null = $createdPaths.Add($skillDir)

$resourceLines = New-Object System.Collections.Generic.List[string]
if ($normalizedResources.Count -eq 0) {
    $null = $resourceLines.Add('Create references/, scripts/, assets/ directories on demand.')
} else {
    foreach ($resource in $normalizedResources) {
        $resourceDir = Join-Path $skillDir $resource
        New-Item -ItemType Directory -Path $resourceDir -Force | Out-Null
        $null = $createdPaths.Add($resourceDir)
        Set-Utf8Content -Path (Join-Path $resourceDir '.gitkeep') -Content ''
        switch ($resource) {
            'references' { $null = $resourceLines.Add('- references/: reference docs and long-form specs.') }
            'scripts'    { $null = $resourceLines.Add('- scripts/: executable scripts and tools.') }
            'assets'     { $null = $resourceLines.Add('- assets/: templates, samples, output artifacts.') }
        }
    }
}

$skillLines = @(
    '---',
    "name: $SkillName",
    "description: $Description",
    '---',
    '',
    "# $Title",
    '',
    '## Overview',
    '',
    'TODO: 1-2 sentence description of this skill purpose, inputs, and boundaries.',
    '',
    '## Usage',
    '',
    '1. TODO: preconditions and entry checks.',
    '2. TODO: core execution steps.',
    '3. TODO: completion, verification, and delivery.',
    '',
    '## Resources',
    ''
)
$skillLines += $resourceLines
$skillLines += @(
    '',
    '## Red Flags',
    '',
    '- TODO: non-applicable scenarios.',
    '- TODO: common misuse patterns.',
    '',
    '## Next Steps',
    '',
    '1. Fill in description, summary, and trigger_signals.',
    '2. Populate references/, scripts/, assets/ as needed.',
    "3. Before promoting, run: promote-manual $scaffoldId --dry-run",
    "4. After dry-run passes: promote-manual $scaffoldId",
    ''
)
Set-Utf8Content -Path $skillFilePath -Content ($skillLines -join "`n")
$null = $createdPaths.Add($skillFilePath)

$manifestObject = [ordered]@{
    kind            = 'project'
    domain          = $Domain
    summary         = $Summary
    trigger_signals = @($TriggerSignals)
    owner           = $Owner
    status          = $Status
    startup_visible = $false
    token_budget    = $TokenBudget
}
Set-Utf8Content -Path $manifestPath -Content ((ConvertTo-Json -InputObject $manifestObject -Depth 4) + "`n")
$null = $createdPaths.Add($manifestPath)

$metadataObject = [ordered]@{
    source_type           = 'manual-scaffold'
    scaffold_id           = $scaffoldId
    target_directory_name = $DirectoryName
    target_skill_name     = $SkillName
    title                 = $Title
    domain                = $Domain
    owner                 = $Owner
    status                = 'draft'
    token_budget          = $TokenBudget
    resources             = @($normalizedResources)
    created_at            = (Get-Date).ToUniversalTime().ToString('o')
}
Set-Utf8Content -Path $metadataPath -Content ((ConvertTo-Json -InputObject $metadataObject -Depth 4) + "`n")
$null = $createdPaths.Add($metadataPath)

if ($RefreshCatalog) {
    if (-not (Test-Path -LiteralPath $catalogScript)) {
        throw "[skill-scaffold] Missing catalog script: $catalogScript"
    }
    python $catalogScript --repo-root $repoRoot
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}

Write-Host "[skill-scaffold] Created manual scaffold '$scaffoldId' in domain '$Domain'."
Write-Host '[skill-scaffold] Created paths:'
foreach ($path in $createdPaths) {
    $relative = $path.Substring($repoRoot.Length).TrimStart('\')
    Write-Host "  - $relative"
}
if (-not $RefreshCatalog) {
    Write-Host "[skill-scaffold] After filling placeholders, run promote-manual $scaffoldId --dry-run before promotion."
}
