currently developing
## Description points
- As it is now, this repo is a skeleton for a reverse shell.
- Connects to a control server and spawns a shell on a windows machine
- Utilizes the win32 library
- Anti-virus and analysis evasion techniques to be implemented

## TODO
- Connect to C2 over HTTPS
- Encrypt important info (host name, process name, ...)
- Add stager/packer which uses process injection/hollowing 
- Add file grabbing and other functionality that doesn't require creating processes
- Research anti-virus evasion and malware polymorphism (sandbox detection, ...)
- Research windows kernel programming and kernel mode malware
