currently developing

## Description
This is a Remote Access Trojan tool developed as a fun project to learn about malware and evasion.
To communicate with the bot once executed on the target system, you must use a tool like 
netcat to act as the command server (will be adding a listener).

## Bot execution and modules
Once the bot is run it will attempt to copy itself to the temp directory (from temp environment variable).
It will then add the new path to a registry run key, either admin or user depending on priviledges. Finally it will execute from within the temp directory and begin beaconing the command server (C2 properties in Bot/config.h). Once a connection is made, the bot will wait to receive module codes, which determine which module is run through the bot. Currently the only module implemented is spawn_shell, which will create a cmd.exe process in the background and pipe stdin, stdout, stderr to the command server.

### Notes:
- To compile windows executables (.exe) on linux, use mingw64
- Bot/Makefile contains compilation flags for a compact executable
- Run "make" from within the Bot directory to compile the bot (uses mingw64)

## TODO
- ~~registry key persistance ~~
- upload and execute files 
- get antivirus products 
- Add cryptor with decryption stub and anti-virus evasion
- java tcp listener


