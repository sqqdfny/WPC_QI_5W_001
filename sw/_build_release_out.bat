@echo off
del  hex\*release*.hex
copy obj\release.hex hex\WPC_QI_5W_001_release_%date:~0,4%_%date:~5,2%_%date:~8,2%.hex
copy obj\release.hex hex_history\WPC_QI_5W_001_release_%date:~0,4%_%date:~5,2%_%date:~8,2%.hex











