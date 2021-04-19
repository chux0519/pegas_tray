#include <stdio.h>
#include <string.h>

#define TRAY_WINAPI 1

#include "pegas_tray.h"
#include "3rd-party/tray.h"


#define TRAY_ICON "icon.ico"

static struct tray tray;

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

int main() {
  if (tray_init(&tray) < 0) {
    printf("failed to create tray\n");
    return 1;
  }

  struct menu_context ctx;
  init_menu_context(&ctx, "127.0.0.1", 11080);

  while (tray_loop(1) == 0) {
  }

  return 0;
}
