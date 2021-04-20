#include <stdio.h>
#include <string.h>

#define TRAY_WINAPI 1

#include "pegas_tray.h"
#include "3rd-party/tray.h"


#define TRAY_ICON "icon.ico"

static struct tray tray;
static struct menu_context ctx;

static void quit_cb(struct tray_menu *item) {
  (void)item;
  tray_exit();
}

static struct tray tray = {
    .icon = TRAY_ICON,
    .menu = (struct tray_menu[]){ {
                            .text = "servers",
                        },
                        {.text = "-" },
                        {.text = "quit", .cb = quit_cb },
                        {.text = NULL } },
};

static void timer_cb() {
    update_submenu(&tray, &ctx);
}

static void on_click(struct tray_menu* item) {
    int idx = item->context;
    set_server(&ctx, idx);
    update_submenu(&tray, &ctx);
}

int main(int argc, char* argv[]) {
  if (tray_init(&tray) < 0) {
    printf("failed to create tray\n");
    return 1;
  }
  HWND hWnd = GetConsoleWindow();
  // ShowWindow(hWnd, SW_MINIMIZE);
  ShowWindow(hWnd, SW_HIDE);

  init_menu_context(&ctx, "127.0.0.1", 11080);
  ctx.cb = on_click;
  update_submenu(&tray, &ctx);

  UINT_PTR timerid = SetTimer(NULL, 0, 30000, &timer_cb);

  while (tray_loop(1) == 0) {}

  return 0;
}
