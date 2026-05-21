$xml = New-Object XML
$xml.Load('coverage.xml')

$codegenModule = $xml.results.modules.module | Where-Object { $_.name -eq 'Chaos.IL2CPP.CodeGen.dll' }

$totalUncovered = 0
$results = @()
foreach ($function in $codegenModule.functions.function) {
    $typeName = $function.type_name
    $funcTotal = [int]$function.lines_covered + [int]$function.lines_partially_covered + [int]$function.lines_not_covered
    $funcCovered = [int]$function.lines_covered

    $isSkeleton = $typeName -like "RuntimeSkeleton*" -or $typeName -like "PatchData*"
    $isRefProof = $typeName -like "NativeReferenceProof*" -or $typeName -like "NativeReferenceLoweringPlanner*"

    if (-not $isSkeleton -and -not $isRefProof -and $funcTotal -gt 0 -and $funcCovered -eq 0) {
        $totalUncovered += $funcTotal
        $results += [PSCustomObject]@{
            Type = $typeName
            Func = $function.name.Substring(0, [Math]::Min(90, $function.name.Length))
            Lines = $funcTotal
        }
    }
}

$results | Sort-Object Lines -Descending | Select-Object -First 50
Write-Host ""
Write-Host "TOTAL uncovered lines (non-RefProof, non-Skeleton): $totalUncovered"
