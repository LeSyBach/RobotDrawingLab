$ErrorActionPreference = 'Stop'
Set-Location -LiteralPath $PSScriptRoot
$compiler = Get-Command g++.exe -ErrorAction SilentlyContinue
$compilerPath = if ($compiler) { $compiler.Source } else { 'C:\msys64\ucrt64\bin\g++.exe' }
if (-not (Test-Path -LiteralPath $compilerPath)) { throw 'Cần g++ của MSYS2 UCRT64 để biên dịch. Chương trình trong bin vẫn chạy độc lập.' }
New-Item -ItemType Directory -Force -Path bin | Out-Null
& $compilerPath -std=c++17 -O2 -Wall -Wextra -Wpedantic -municode -mwindows -static -static-libgcc -static-libstdc++ src/main.cpp src/svg_import.cpp -o bin/RobotDrawingLab.exe -lopengl32 -lglu32 -lgdiplus -lcomctl32 -lcomdlg32 -lgdi32 -lole32 -luuid
if ($LASTEXITCODE -ne 0) { throw 'Biên dịch thất bại.' }
Write-Host 'Đã biên dịch bin\RobotDrawingLab.exe'
