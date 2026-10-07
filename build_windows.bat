@echo off
setlocal

where cmake >nul 2>nul
if errorlevel 1 (
  echo CMake was not found. Install CMake and add it to PATH.
  exit /b 1
)

if not exist build (
  cmake -B build -G "Visual Studio 17 2022" -A x64
  if errorlevel 1 exit /b 1
)

cmake --build build --config Release
if errorlevel 1 exit /b 1

echo.
echo Paper Drums VST3 build completed.
echo Look in: build\PaperDrums_artefacts\Release\VST3
