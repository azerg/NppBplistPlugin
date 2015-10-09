echo off
SET TargetPath=%~1
SET TargetFileName=%~2
SET Configuration=%~3
SET TargetName=%~4
SET SolutionDir=%~5

SET OutputBinFilePath="%SolutionDir%\..\..\bin\%Configuration%\*"
SET OutputPluginFilePath="E:\Work_Installs\Notepad++\plugins\%TargetName%.dll"



copy /Y  "%TargetPath%" %OutputPluginFilePath%
cd %SolutionDir%
cd ..
xcopy /Y /F /I  "%TargetName%\src\%Configuration%\%TargetFileName%" %OutputBinFilePath% 

rem -============ dummy check if everyting works fine ===============-

IF NOT EXIST %OutputPluginFilePath% (
  EXIT /B -1
)

IF NOT EXIST %OutputBinFilePath% (
  EXIT /B -2
)

EXIT /B 0