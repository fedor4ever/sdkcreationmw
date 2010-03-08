REM ==============================================================================
REM
REM Script for generating the documentation for Ecmt.
REM
REM doxygen and the dot tool must be available in path. They can be downloaded from
REM http://www.doxygen.org and http://www.graphviz.org/
REM
REM 
REM ==============================================================================
REM 
doxygen DoxySettings.cfg
xcopy /y *.gif output
xcopy /y main.html output

