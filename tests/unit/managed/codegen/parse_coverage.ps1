Add-Type -AssemblyName System.Xml.Linq
$doc = [System.Xml.Linq.XDocument]::Load('coverage.cobertura.xml')

# Try without namespace first, then with
$packages = $doc.Descendants('package')
if ($packages.Count -eq 0) {
    $ns = [System.Xml.Linq.XNamespace]'http://cobertura.sourceforge.net/xml/coverage-04.dtd'
    $packages = $doc.Descendants($ns + 'package')
}

foreach ($p in $packages) {
    $name = $p.Attribute('name').Value
    $lineRate = [double]($p.Attribute('line-rate').Value)
    $branchRate = [double]($p.Attribute('branch-rate').Value)
    $complexity = $p.Attribute('complexity').Value
    Write-Host ("{0,-50} lines={1,7:P1}  branches={2,7:P1}  complexity={3}" -f $name, $lineRate, $branchRate, $complexity)
}

$coverage = $doc.Descendants('coverage') | Select-Object -First 1
if ($coverage -eq $null) {
    $coverage = $doc.Descendants($ns + 'coverage') | Select-Object -First 1
}
$totalLineRate = [double]($coverage.Attribute('line-rate').Value)
$totalBranchRate = [double]($coverage.Attribute('branch-rate').Value)
$linesCovered = $coverage.Attribute('lines-covered').Value
$linesValid = $coverage.Attribute('lines-valid').Value
Write-Host ""
Write-Host "TOTAL: lines=$totalLineRate ($linesCovered/$linesValid) branches=$totalBranchRate"
