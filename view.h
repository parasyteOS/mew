#ifndef MEW_VIEW_H
#define MEW_VIEW_H

#include <stdbool.h>
#include <wayland-server-core.h>
#include <wlr/types/wlr_compositor.h>
#include <wlr/types/wlr_xdg_shell.h>
#include <wlr/util/box.h>

#include "server.h"

struct mew_view {
	struct mew_server *server;
	struct wl_list link; // server::views
	struct wlr_surface *wlr_surface;
	struct wlr_scene_tree *scene_tree;

	/* The view has a position in layout coordinates. */
	int lx, ly;

	const struct mew_view_impl *impl;
};

struct mew_view_impl {
	char *(*get_title)(struct mew_view *view);
	void (*get_geometry)(struct mew_view *view, int *width_out, int *height_out);
	bool (*is_primary)(struct mew_view *view);
	bool (*is_transient_for)(struct mew_view *child, struct mew_view *parent);
	void (*activate)(struct mew_view *view, bool activate);
	void (*maximize)(struct mew_view *view, int output_width, int output_height);
	void (*destroy)(struct mew_view *view);
};

char *view_get_title(struct mew_view *view);
bool view_is_primary(struct mew_view *view);
bool view_is_transient_for(struct mew_view *child, struct mew_view *parent);
void view_activate(struct mew_view *view, bool activate);
void view_position(struct mew_view *view);
void view_position_all(struct mew_server *server);
void view_unmap(struct mew_view *view);
void view_map(struct mew_view *view, struct wlr_surface *surface);
void view_destroy(struct mew_view *view);
void view_init(struct mew_view *view, struct mew_server *server, const struct mew_view_impl *impl);

struct mew_view *view_from_wlr_surface(struct wlr_surface *surface);

#endif
