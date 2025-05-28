#ifndef MEW_SERVER_H
#define MEW_SERVER_H

#include <wayland-server-core.h>
#include <wlr/types/wlr_idle_inhibit_v1.h>
#include <wlr/types/wlr_idle_notify_v1.h>
#include <wlr/types/wlr_output_layout.h>
#include <wlr/types/wlr_relative_pointer_v1.h>
#include <wlr/types/wlr_xdg_decoration_v1.h>
#include <wlr/util/log.h>

struct mew_server {
	struct wl_display *wl_display;
	struct wl_list views;
	struct wlr_backend *backend;
	struct wlr_renderer *renderer;
	struct wlr_allocator *allocator;
	struct wlr_session *session;
	struct wl_listener display_destroy;

	struct mew_seat *seat;

	struct wlr_output_layout *output_layout;
	struct wlr_scene_output_layout *scene_output_layout;

	struct wlr_scene *scene;
	/* Includes disabled outputs; depending on the output_mode
	 * some outputs may be disabled. */
	struct wl_list outputs; // mew_output::link
	struct wl_listener new_output;
	struct wl_listener output_layout_change;
	char* output_shm_template;
	int next_output_shm_index;

	struct wl_listener xdg_toplevel_decoration;
	struct wl_listener new_xdg_toplevel;
	struct wl_listener new_xdg_popup;

	struct wl_listener new_virtual_keyboard;
	struct wl_listener new_virtual_pointer;

	struct wlr_output_manager_v1 *output_manager_v1;
	struct wl_listener output_manager_apply;
	struct wl_listener output_manager_test;

	struct wlr_relative_pointer_manager_v1 *relative_pointer_manager;

	bool return_app_code;
	bool terminated;
	enum wlr_log_importance log_level;
};

void server_terminate(struct mew_server *server);

#endif
