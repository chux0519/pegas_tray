#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include "3rd-party/tray.h"

struct menu_context {
	struct pegas_control_endpoint {
		const char* host;
		int port;
	} endpoint;
	SOCKET sock;
};

static void init_menu_context(struct menu_context * ctx, const char* host, int port) {
	ctx->endpoint.host = host;
	ctx->endpoint.port = port;

	WSADATA wsa_data;
	int i_res = WSAStartup(MAKEWORD(2, 2), &wsa_data);
	if (i_res != NO_ERROR) {
		wprintf(L"WSAStartup function failed with error: %d\n", i_res);
		exit(1);
	}

	SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sock == INVALID_SOCKET) {
		wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}

	struct sockaddr_in endpoint;
	endpoint.sin_family = AF_INET;
	endpoint.sin_addr.s_addr = inet_addr(host);
	endpoint.sin_port = htons(port);

	i_res = connect(sock, (SOCKADDR*)&endpoint, sizeof(endpoint));
	if (i_res == SOCKET_ERROR) {
		wprintf(L"connect function failed with error: %ld\n", WSAGetLastError());
		i_res = closesocket(sock);
		if (i_res == SOCKET_ERROR)
			wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}

	wprintf(L"Connected to server.\n");
	ctx->sock = sock;
	/*i_res = closesocket(sock);
	if (i_res == SOCKET_ERROR) {
		wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
		WSACleanup();
		exit(1);
	}*/

	//WSACleanup();
}

static void init_menu(struct tray_menu *root, struct menu_context * ctx) {

}