Add-Type -AssemblyName System.Xml.Linq
$doc = [System.Xml.Linq.XDocument]::Load('coverage.cobertura.xml')

# Find classes with low coverage in AotCoreIrLowering
$classes = $doc.Descendants('class') | Where-Object { $_.Attribute('filename').Value -like '*AotCoreIrLowering*' }
foreach ($c in $classes) {
    $name = $c.Attribute('name').Value
    $lineRate = [double]($c.Attribute('line-rate').Value)
    $lines = $c.Descendants('line')
    $covered = 0
    $total = 0
    foreach ($l in $lines) {
        $hits = [int]($l.Attribute('hits').Value)
        $total++
        if ($hits -gt 0) { $covered++ }
    }
    if ($lineRate -lt 0.90) {
        Write-Host ("{0,-55} lines={1,4}/{2,-4} rate={3:P1}" -f $name, $covered, $total, $lineRate)
    }
}
