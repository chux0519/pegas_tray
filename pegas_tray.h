#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <Ws2tcpip.h>
#include <stdio.h>
#include <stdlib.h>
#include "3rd-party/tray.h"

#define DEFAULT_BUFLEN 2048

struct menu_context {
	struct pegas_control_endpoint {
		const char* host;
		int port;
	} endpoint;
	SOCKET sock;
};

struct server_info {
	char title[256];
	char desc[256];
	struct server_info* next;
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
}

static void update_submemu_by_info(struct tray_menu *root, const char * info) {
	struct server_info* prev = root->context;
	struct server_info* cur = prev->next;
	const char s[2] = "\n";
	char* token = NULL;
	int server_len = 0;
	do {
		if(token == NULL)
			token = strtok(info, s);
		else
			token = strtok(NULL, s);
		if (token == NULL) break;
		server_len += 1;
		if (cur == NULL) {
			cur = malloc(sizeof(struct server_info));
			cur->next = NULL;
		}
		printf("title: %s\n", token);
		sprintf(cur->title, "%s\n", token);
		token = strtok(NULL, s);
		printf("desc: %s\n", token);
		sprintf(cur->desc, "%s\n", token);
		prev->next = cur;
		prev = cur;
		cur = cur->next;
	} while (1);
	printf("server len: %d\n", server_len);

	struct tray_menu* submenu = root->submenu;
	if (submenu == NULL) {
		// init submenu
		submenu = malloc(3 * server_len * sizeof(struct tray_menu));
	}

	struct server_info* dummy =root->context;
	struct server_info* sinfo = dummy->next;
	for (int i = 0; i < server_len * 3; i += 3, sinfo = sinfo->next) {
		if (sinfo != NULL) {
			submenu[i].text = sinfo->title;
			submenu[i].checked = sinfo->title[0] == '*';
			submenu[i].disabled = 0;
			submenu[i].cb = NULL; // TODO: pick server
			submenu[i].submenu = NULL;
			submenu[i].context = i / 3;

			submenu[i + 1].text = sinfo->desc;
			submenu[i + 1].disabled = 1;
			submenu[i + 1].checked = 0;
			submenu[i + 1].submenu = NULL;

			submenu[i + 2].text = "-";
			submenu[i + 2].submenu = NULL;
		}
	}
	submenu[server_len * 3 - 1].text = NULL;
	if (root->submenu == NULL)
		root->submenu = submenu;
}

static void update_submenu(struct tray *tray, struct menu_context * ctx) {
	if (tray->menu[0].context == NULL) {
		struct server_info* sinfo = malloc(sizeof(struct server_info));
		sinfo->next = NULL;
		tray->menu[0].context = sinfo;
	}
	char* sendbuf = "get servers";
	int i_res = send(ctx->sock, sendbuf, (int)strlen(sendbuf), 0);
	if (i_res == SOCKET_ERROR) {
		printf("send failed: %d\n", WSAGetLastError());
		closesocket(ctx->sock);
		WSACleanup();
		return 1;
	}
	printf("Bytes Sent: %ld\n", i_res);

	char recvbuf[DEFAULT_BUFLEN] = { 0 };
	int recvbuflen = DEFAULT_BUFLEN;

	// blocking
	i_res = recv(ctx->sock, recvbuf, recvbuflen, 0);
	if (i_res > 0) {
		if (i_res >= recvbuflen) {
			printf("Not implement yet.\n");
		}
		else {
			printf("Bytes received: %d\n%s\n", i_res, recvbuf);
			update_submemu_by_info(&tray->menu[0], recvbuf);
			tray_update(tray);
		}
	}
	else if (i_res == 0)
		printf("Connection closed\n");
	else
		printf("recv failed: %d\n", WSAGetLastError());

}