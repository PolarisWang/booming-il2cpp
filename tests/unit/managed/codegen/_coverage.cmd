@echo off
cd /d "%~dp0"
dotnet test --no-build -p:CollectCoverage=true -p:CoverletOutputFormat=cobertura -p:CoverletOutput=./TestResults/coverage/ -p:ExcludeByFile="**/obj/**/*.cs^;**/ReferenceProof/**/*.cs^;**/PatchData*.cs^;**/NativeReferenceProofEmitter*.cs^;**/RuntimeSkeleton*.cs^;**/NativeReferenceLoweringPlanner.PlanBuilders*.cs^;**/NativeReferenceLoweringPlanner.GeneratedPluginExports*.cs"
