#define _POSIX_C_SOURCE 200809L

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <wayland-server-core.h>
#include <wlr/types/wlr_output.h>
#include <wlr/types/wlr_scene.h>

#include "output.h"
#include "seat.h"
#include "server.h"
#include "view.h"

char *
view_get_title(struct mew_view *view)
{
	const char *title = view->impl->get_title(view);
	if (!title) {
		return NULL;
	}
	return strndup(title, strlen(title));
}

bool
view_is_primary(struct mew_view *view)
{
	return view->impl->is_primary(view);
}

bool
view_is_transient_for(struct mew_view *child, struct mew_view *parent)
{
	return child->impl->is_transient_for(child, parent);
}

void
view_activate(struct mew_view *view, bool activate)
{
	view->impl->activate(view, activate);
}

static bool
view_extends_output_layout(struct mew_view *view, struct wlr_box *layout_box)
{
	int width, height;
	view->impl->get_geometry(view, &width, &height);

	return (layout_box->height < height || layout_box->width < width);
}

static void
view_maximize(struct mew_view *view, struct wlr_box *layout_box)
{
	view->lx = layout_box->x;
	view->ly = layout_box->y;

	if (view->scene_tree) {
		wlr_scene_node_set_position(&view->scene_tree->node, view->lx, view->ly);
	}

	view->impl->maximize(view, layout_box->width, layout_box->height);
}

static void
view_center(struct mew_view *view, struct wlr_box *layout_box)
{
	int width, height;
	view->impl->get_geometry(view, &width, &height);

	view->lx = (layout_box->width - width) / 2;
	view->ly = (layout_box->height - height) / 2;

	if (view->scene_tree) {
		wlr_scene_node_set_position(&view->scene_tree->node, view->lx, view->ly);
	}
}

void
view_position(struct mew_view *view)
{
	struct wlr_box layout_box;
	wlr_output_layout_get_box(view->server->output_layout, NULL, &layout_box);

	if (view_is_primary(view) || view_extends_output_layout(view, &layout_box)) {
		view_maximize(view, &layout_box);
	} else {
		view_center(view, &layout_box);
	}
}

void
view_position_all(struct mew_server *server)
{
	struct mew_view *view;
	wl_list_for_each (view, &server->views, link) {
		view_position(view);
	}
}

void
view_unmap(struct mew_view *view)
{
	wl_list_remove(&view->link);

	wlr_scene_node_destroy(&view->scene_tree->node);

	view->wlr_surface->data = NULL;
	view->wlr_surface = NULL;
}

void
view_map(struct mew_view *view, struct wlr_surface *surface)
{
	view->scene_tree = wlr_scene_subsurface_tree_create(&view->server->scene->tree, surface);
	if (!view->scene_tree) {
		wl_resource_post_no_memory(surface->resource);
		return;
	}
	view->scene_tree->node.data = view;

	view->wlr_surface = surface;
	surface->data = view;

	view_position(view);

	wl_list_insert(&view->server->views, &view->link);
	seat_set_focus(view->server->seat, view);
}

void
view_destroy(struct mew_view *view)
{
	struct mew_server *server = view->server;

	if (view->wlr_surface != NULL) {
		view_unmap(view);
	}

	view->impl->destroy(view);

	/* If there is a previous view in the list, focus that. */
	bool empty = wl_list_empty(&server->views);
	if (!empty) {
		struct mew_view *prev = wl_container_of(server->views.next, prev, link);
		seat_set_focus(server->seat, prev);
	}
}

void
view_init(struct mew_view *view, struct mew_server *server, const struct mew_view_impl *impl)
{
	view->server = server;
	view->impl = impl;
}

struct mew_view *
view_from_wlr_surface(struct wlr_surface *surface)
{
	assert(surface);
	return surface->data;
}
