RUNTIME="D:/agent/booming-il2cpp/src/reference/Chaos.TestFramework.Runtime/"
OUT="D:/agent/booming-il2cpp/output_corelib"

TYPES=(
  "System.Boolean" "System.Int32" "System.Version" "System.Guid"
  "System.TimeSpan" "System.DateTime" "System.Decimal" "System.Double"
  "System.Math" "System.Convert" "System.Enum" "System.Random"
  "System.String" "System.Text.Encoding" "System.Text.StringBuilder"
  "System.BitConverter" "System.IO.Path" "System.IO.File" "System.Array"
)

for t in "${TYPES[@]}"; do
  dll=$(find "$OUT/$t/bin" -name "*.dll" -type f 2>/dev/null | grep -v "\.probe" | grep -v "Chaos.TestFramework" | head -1)

  if [ -z "$dll" ]; then
    echo "  $t: NO DLL"
    continue
  fi

  result=$(dotnet run --project "$RUNTIME" --no-build -- --assembly "$dll" --kind fact 2>&1)
  summary=$(echo "$result" | grep "Passed:")
  fails=$(echo "$result" | grep "FAIL")

  if [ -n "$summary" ]; then
    echo "  $t: $summary"
  fi
  if [ -n "$fails" ]; then
    echo "    $fails"
  fi
done
