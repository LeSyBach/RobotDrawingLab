$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot
$compiler = Get-Command g++.exe -ErrorAction SilentlyContinue
$compilerPath = if ($compiler) { $compiler.Source } else { 'C:\msys64\ucrt64\bin\g++.exe' }
if (-not (Test-Path -LiteralPath $compilerPath)) { throw 'Không tìm thấy g++ MSYS2 UCRT64.' }
New-Item -ItemType Directory -Force -Path bin | Out-Null
$commonOptions = @('-std=c++17', '-O2', '-Wall', '-Wextra', '-Wpedantic', '-static', '-static-libgcc', '-static-libstdc++')
foreach ($testName in @('math_test','image_test','simulation_test')) {
    $testSource = Join-Path 'tests' ($testName + '.cpp')
    $testBinary = Join-Path 'bin' ($testName + '.exe')
    $linkOptions = @()
    if ($testName -eq 'simulation_test') {
        $linkOptions = @('src/svg_import.cpp','-lopengl32','-lglu32','-lgdiplus','-lcomctl32','-lcomdlg32','-lgdi32','-lole32','-luuid')
    }
    & $compilerPath @commonOptions $testSource -o $testBinary @linkOptions
    if ($LASTEXITCODE -ne 0) { throw "Không biên dịch được $testName" }
    & (Join-Path $PSScriptRoot $testBinary)
    if ($LASTEXITCODE -ne 0) { throw "$testName thất bại" }
}
