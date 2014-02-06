@echo off
echo Checking out project and filters file
p4 edit geo2.vcxproj
p4 edit geo2.vcxproj.filters
echo Regenerating
..\..\..\..\..\..\shared_tools\python\27\python.exe ..\..\tools\ProjectFileGenerator\ProjectFileGenerator.py -i geo2.ccpproj
pause