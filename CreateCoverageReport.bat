REM Now we run the coverage check - excluding the test framework, test code & port files (we only want to know how much of our rltos source is being tested)
echo off
OpenCppCoverage --sources "%cd%" --excluded_source "%CPPUTEST_SOURCE%\*" --excluded_source "test\*" --excluded_source "rltos\port\*" -- "build\Debug\rltos_test.exe"

echo on
REM Now we copy the latest coverage report in the docs coverage folder
echo off

if exist "%cd%\CoverageReport*" (
del "%cd%\report\coverage\*" /q /s

FOR /F "delims=" %%i IN ('dir /b /ad-h /t:c /od') DO SET a=%%i
xcopy "%a%" "%cd%\report\coverage" /E/H/C/
)