if exist "%cd%\build\Debug\rltos_test.exe" (
REM Now we run the coverage check - excluding the test framework, test code & port files (we only want to know how much of our rltos source is being tested)
OpenCppCoverage --sources "%cd%" --excluded_source "%CPPUTEST_SOURCE%\*" --excluded_source "test\*" --excluded_source "rltos\port\*" -- "build\Debug\rltos_test.exe"

REM Find the coverage report, copy into the report/coverage folder and delete the auto generated one.

set find_folder_result=---

for /D %%d in ("%cd%\CoverageReport*") do (
  set find_folder_result=%%d
)

if exist "%find_folder_result%" (goto WINDIR) else (goto NOWINDIR)

:WINDIR
    if exist "%cd%\report\coverage\" (
        del "%cd%\report\coverage\*" /q /s
    ) else (
        mkdir "%cd%\report\coverage"
    )

    for /F "delims=" %%i in ('dir /b /ad-h /t:c /od') do set a=%%i
    xcopy "%a%" "%cd%\report\coverage" /E/H/C/

    rmdir "%find_folder_result%" /q /s
  goto end

:NOWINDIR
  echo No Coverage Report Generated

:end
)