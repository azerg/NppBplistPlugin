echo off
SET ShortConfiguration=%~1
SET TargetPath=%~2
SET TargetFileName=%~3
SET Configuration=%~4
SET TargetName=%~5
SET SolutionDir=%~6

SET OutputBinFilePath="%SolutionDir%\..\..\bin\%Configuration%\*"
SET OutputPluginFilePath="C:\Program Files (x86)\Notepad++\plugins\%TargetName%_%ShortConfiguration%.dll"



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