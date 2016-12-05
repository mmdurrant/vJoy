echo off
SET VS=14.0
SET BUILDER=%ProgramFiles(x86)%\MSBuild\%VS%\Bin\MSBuild.exe

SET DigiCertUtil="C:\Program Files (x86)\Windows Kits\10\bin\x64\signtool.exe"
SET InnoCompiler=%ProgramFiles(x86)%\Inno Setup 5\ISCC.exe
SET BuildConfig=Debug
Set BuildPlatform=x64
Set SignSha1=304FE425D193434E879957FCEE60D485E5C6EB4E
SET Target64=%BuildPlatform%\%BuildConfig%\Package
Set TIMESTAMP_URL=http://timestamp.verisign.com/scripts/timstamp.dll

:build64
echo %DATE% %TIME%: Cleaning vJoy (x64)
"%BUILDER%"  vjoy.sln  /maxcpucount:1 /t:clean /p:Platform=x64;Configuration=%BuildConfig%
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

echo %DATE% %TIME%: Building vJoy (x64)
"%BUILDER%"  vjoy.sln  /maxcpucount:4  /p:Platform=x64;Configuration=%BuildConfig%
set BUILD_STATUS=%ERRORLEVEL%
if not %BUILD_STATUS%==0 goto fail

:signapps
echo %DATE% %TIME%: Signing the applications
IF NOT EXIST %DigiCertUtil% GOTO NOUTIL
%DigiCertUtil% sign /debug /v /t %TIMESTAMP_URL% /sha1 %SignSha1% .\%Target64%\vJoyList.exe* .\%Target64%\vJoyConf.exe*  .\%Target64%\vJoyFeeder.exe .\%Target64%\vJoyInstall.dll .\%Target64%\vJoyInstall.exe
%DigiCertUtil% timestamp /debug /v /t %TIMESTAMP_URL% .\%Target64%\hidkmdf.sys .\%Target64%\vjoy.cat  .\%Target64%\vJoy.sys
REM %DigiCertUtil% sign /sha1 %SignSha1% /noInput .\%Target64%\vJoyList.exe*.\%Target32%\vJoyList.exe*.\%Target64%\vJoyConf.exe*.\%Target32%\vJoyConf.exe*.\%Target64%\vJoyFeeder.exe*.\%Target32%\vJoyFeeder.exe
set SIGN_STATUS=%ERRORLEVEL%
if not %SIGN_STATUS%==0 goto fail
echo %DATE% %TIME%: Signing the applications - OK

:inno
echo %DATE% %TIME%: Compiling the Inno Setup Script
IF NOT EXIST "%InnoCompiler%" GOTO NOINNO
"%InnoCompiler%" install\vJoyInstallerSigned.iss 
set INNO_STATUS=%ERRORLEVEL%
if not %INNO_STATUS%==0 goto fail
echo %DATE% %TIME%: Compiling the Inno Setup Script - OK
exit /b 0

:NOUTIL
echo %DATE% %TIME%: Could not find DigiCertUtil on the desktop
goto fail

:NOINNO
echo %DATE% %TIME%: Could not find Inno Setup Compiler
goto fail

:fail
exit /b 1
