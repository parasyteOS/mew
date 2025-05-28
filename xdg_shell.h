#ifndef MEW_XDG_SHELL_H
#define MEW_XDG_SHELL_H

#include <wayland-server-core.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/types/wlr_xdg_shell.h>

#include "view.h"

struct mew_xdg_shell_view {
	struct mew_view view;
	struct wlr_xdg_toplevel *xdg_toplevel;

	struct wl_listener destroy;
	struct wl_listener commit;
	struct wl_listener unmap;
	struct wl_listener map;
	struct wl_listener request_fullscreen;
};

struct mew_xdg_decoration {
	struct wlr_xdg_toplevel_decoration_v1 *wlr_decoration;
	struct mew_server *server;
	struct wl_listener destroy;
	struct wl_listener commit;
	struct wl_listener request_mode;
};

struct mew_xdg_popup {
	struct wlr_xdg_popup *xdg_popup;

	struct wl_listener destroy;
	struct wl_listener commit;
};

void handle_new_xdg_toplevel(struct wl_listener *listener, void *data);
void handle_new_xdg_popup(struct wl_listener *listener, void *data);

void handle_xdg_toplevel_decoration(struct wl_listener *listener, void *data);

#endif
