#ifndef MEW_SEAT_H
#define MEW_SEAT_H

#include <wayland-server-core.h>
#include <wlr/types/wlr_cursor.h>
#include <wlr/types/wlr_data_device.h>
#include <wlr/types/wlr_input_device.h>
#include <wlr/types/wlr_seat.h>
#include <wlr/types/wlr_xcursor_manager.h>

#include "server.h"
#include "view.h"

#define DEFAULT_XCURSOR "left_ptr"
#define XCURSOR_SIZE 24

struct mew_seat {
	struct wlr_seat *seat;
	struct mew_server *server;
	struct wl_listener destroy;

	struct wl_list keyboards;
	struct wl_list keyboard_groups;
	struct wl_list pointers;
	struct wl_list touch;
	struct wl_listener new_input;

	struct wlr_cursor *cursor;
	struct wlr_xcursor_manager *xcursor_manager;
	struct wl_listener cursor_motion_relative;
	struct wl_listener cursor_motion_absolute;
	struct wl_listener cursor_button;
	struct wl_listener cursor_axis;
	struct wl_listener cursor_frame;

	int32_t touch_id;
	double touch_lx;
	double touch_ly;
	struct wl_listener touch_down;
	struct wl_listener touch_up;
	struct wl_listener touch_motion;
	struct wl_listener touch_frame;

	struct wl_list drag_icons;
	struct wl_listener request_start_drag;
	struct wl_listener start_drag;

	struct wl_listener request_set_cursor;
	struct wl_listener request_set_selection;
	struct wl_listener request_set_primary_selection;
};

struct mew_keyboard_group {
	struct wlr_keyboard_group *wlr_group;
	struct mew_seat *seat;
	struct wl_listener key;
	struct wl_listener modifiers;
	struct wl_list link; // mew_seat::keyboard_groups
	bool is_virtual;
};

struct mew_pointer {
	struct wl_list link; // seat::pointers
	struct mew_seat *seat;
	struct wlr_pointer *pointer;

	struct wl_listener destroy;
};

struct mew_touch {
	struct wl_list link; // seat::touch
	struct mew_seat *seat;
	struct wlr_touch *touch;

	struct wl_listener destroy;
};

struct mew_drag_icon {
	struct wl_list link; // seat::drag_icons
	struct mew_seat *seat;
	struct wlr_drag_icon *wlr_drag_icon;
	struct wlr_scene_tree *scene_tree;

	/* The drag icon has a position in layout coordinates. */
	double lx, ly;

	struct wl_listener destroy;
};

struct mew_seat *seat_create(struct mew_server *server, struct wlr_backend *backend);
void seat_destroy(struct mew_seat *seat);
struct mew_view *seat_get_focus(struct mew_seat *seat);
void seat_set_focus(struct mew_seat *seat, struct mew_view *view);
void seat_center_cursor(struct mew_seat *seat);

#endif
