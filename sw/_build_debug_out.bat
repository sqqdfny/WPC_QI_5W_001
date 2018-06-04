@echo off
del  hex\*debug*.hex
copy obj\debug.hex hex\WPC_QI_5W_001_debug_%date:~0,4%_%date:~5,2%_%date:~8,2%.hex











