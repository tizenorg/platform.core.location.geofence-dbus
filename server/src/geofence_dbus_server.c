/* Copyright 2014 Samsung Electronics Co., Ltd All Rights Reserved
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <glib.h>

#include "generated-code.h"
#include "geofence_dbus_server.h"
#include "geofence_dbus_server_priv.h"


#define GEOFENCE_SERVICE_NAME	"org.tizen.lbs.Providers.GeofenceServer"
#define GEOFENCE_SERVICE_PATH	"/org/tizen/lbs/Providers/GeofenceServer"

typedef struct _geofence_dbus_signal_s {
	guint add_geofence_h;
	guint delete_geofence_h;
	guint get_geofences_h;
	guint enable_geofence_h;
	guint start_geofence_h;
	guint stop_geofence_h;

	guint add_place_h;
	guint update_place_h;
	guint delete_place_h;
	guint get_place_name_h;
	guint get_places_h;
} geofence_dbus_signal_s;

typedef struct _geofence_dbus_server_s {
	/* Geofence Dbus Server info */
	gchar *service_name;
	gchar *prev_owner;
	gchar *service_path;
	gint status;
	GDBusObjectManagerServer *manager;
	SLocObjectSkeleton *obj_skeleton;
	GHashTable *connections;
	gpointer userdata;
	guint owner_changed_id;
	guint owner_id;
	guint get_providerinfo_h;
	guint get_status_h;
	guint set_option_h;
	guint add_reference_h;
	guint remove_reference_h;

	geofence_dbus_signal_s *dbus_signal;
	geofence_dbus_callback_s *dbus_callback;
} geofence_dbus_server_s;

static gboolean geofence_dbus_setup_geofence_interface(SLocObjectSkeleton *object, geofence_dbus_server_s *ctx)
{
	GEOFENCE_DBUS_SERVER_LOGD("geofence_dbus_setup_geofence_interface");
	if (!object || !ctx)
		return FALSE;

	SLocGeofence *geofence = NULL;
	geofence = sloc_geofence_skeleton_new();
	sloc_object_skeleton_set_geofence(object, geofence);
	g_object_unref(geofence);

	return TRUE;
}

/*
 * For geofence methods
 */
static gboolean on_add_geofence(SLocGeofence *geofence, GDBusMethodInvocation *invocation, const gchar *app_id, gint place_id, gint geofence_type, gdouble latitude, gdouble longitude, gint radius, const gchar *address, const gchar *bssid, const gchar *ssid, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("ENTER >>>");

	/* call geofence-server's callback, add_geofence_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	gint fence_id = -1;
	if (ctx->dbus_callback && ctx->dbus_callback->add_geofence_cb) {
		fence_id = ctx->dbus_callback->add_geofence_cb(app_id, place_id, geofence_type, latitude, longitude, radius, address, bssid, ssid, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("add_geofence_cb called");
	}
	GEOFENCE_DBUS_SERVER_LOGD("fence_id: %d", fence_id);
	sloc_geofence_complete_add_geofence(geofence, invocation, fence_id);
	return TRUE;
}

static gboolean on_delete_geofence(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint fence_id, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_delete_geofence");

	/* call geofence-server's callback, delete_geofence_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	if (ctx->dbus_callback && ctx->dbus_callback->delete_geofence_cb) {
		ctx->dbus_callback->delete_geofence_cb(fence_id, app_id, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("delete_geofence_cb called");
	}
	sloc_geofence_complete_delete_geofence(geofence, invocation);
	return TRUE;
}

static gboolean on_get_geofences(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint place_id, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("ENTER >>>");

	/* call geofence-server's callback, getlist_geofence_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	GVariant *geofence_info = NULL;
	gint fence_cnt = 0;
	gint error_code = -1;

	if (ctx->dbus_callback && ctx->dbus_callback->get_geofences_cb) {
		geofence_info =	ctx->dbus_callback->get_geofences_cb(place_id, app_id, &fence_cnt, &error_code, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("getlist_geofence_cb called");
	}
	sloc_geofence_complete_get_geofences(geofence, invocation, fence_cnt, error_code, geofence_info);
	return TRUE;
}

static gboolean on_enable_geofence(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint fence_id, const gchar *app_id, gboolean enable, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_enable_geofence");

	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	/* call geofence-server's callback, enable_geofence_cb */
	if (ctx->dbus_callback && ctx->dbus_callback->enable_geofence_cb) {
		ctx->dbus_callback->enable_geofence_cb(fence_id, app_id, enable, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("enable_geofence_cb called");
	}
	sloc_geofence_complete_enable_geofence(geofence, invocation);
	return TRUE;
}

static gboolean on_start_geofence(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint fence_id, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_start_geofence");

	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;
	/* call geofence-server's callback, start_geofence_cb */
	if (ctx->dbus_callback && ctx->dbus_callback->start_geofence_cb) {
		ctx->dbus_callback->start_geofence_cb(fence_id, app_id, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("start_geofence_cb called");
	}
	sloc_geofence_complete_start_geofence(geofence, invocation);
	return TRUE;
}

static gboolean on_stop_geofence(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint fence_id, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_stop_geofence");

	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;
	/* call geofence-server's callback, stop_geofence_cb */
	if (ctx->dbus_callback && ctx->dbus_callback->stop_geofence_cb) {
		ctx->dbus_callback->stop_geofence_cb(fence_id, app_id, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("stop_geofence_cb called");
	}
	sloc_geofence_complete_stop_geofence(geofence, invocation);
	return TRUE;
}

static gboolean on_add_place(SLocGeofence *geofence, GDBusMethodInvocation *invocation, const gchar *app_id, const gchar *place_name, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_add_place");

	/* call geofence-server's callback, add_place_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;
	gint place_id = -1;
	if (ctx->dbus_callback && ctx->dbus_callback->add_place_cb) {
		place_id = ctx->dbus_callback->add_place_cb(app_id, place_name, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("add_place_cb called");
	}
	sloc_geofence_complete_add_place(geofence, invocation, place_id);
	return TRUE;
}

static gboolean on_update_place(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint place_id, const gchar *app_id, const gchar *place_name, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_update_place");

	/* call geofence-server's callback, update_place_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	if (ctx->dbus_callback && ctx->dbus_callback->update_place_cb) {
		ctx->dbus_callback->update_place_cb(place_id, app_id, place_name, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("update_place_cb called");
	}
	sloc_geofence_complete_update_place(geofence, invocation);
	return TRUE;
}

static gboolean on_delete_place(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint place_id, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_delete_place");

	/* call geofence-server's callback, delete_place_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	if (ctx->dbus_callback && ctx->dbus_callback->delete_place_cb) {
		ctx->dbus_callback->delete_place_cb(place_id, app_id, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("delete_place_cb called");
	}
	sloc_geofence_complete_delete_place(geofence, invocation);
	return TRUE;
}

static gboolean on_get_place_name(SLocGeofence *geofence, GDBusMethodInvocation *invocation, gint place_id, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("on_getplacename_geofence");

	/* call geofence-server's callback, getplacename_geofence_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	char *place_name = NULL;
	gint error_code = -1;
	if (ctx->dbus_callback && ctx->dbus_callback->add_place_cb) {
		ctx->dbus_callback->get_place_name_cb(place_id, app_id, &place_name, &error_code, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("getlist_geofence_cb called");
	}
	sloc_geofence_complete_get_place_name(geofence, invocation, error_code,	place_name);
	if (place_name)
		g_free(place_name);	/*This has to be freed as it has been allocated by the server DB side*/
	return TRUE;
}

static gboolean on_get_places(SLocGeofence *geofence, GDBusMethodInvocation *invocation, const gchar *app_id, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_LOGD("ENTER >>>");

	/* call geofence-server's callback, getplacelist_geofence_cb */
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	if (!ctx) return FALSE;

	GVariant *place_info = NULL;
	gint place_cnt = 0;
	gint error_code = -1;
	if (ctx->dbus_callback && ctx->dbus_callback->add_place_cb) {
		place_info = ctx->dbus_callback->get_places_cb(app_id, &place_cnt, &error_code, ctx->userdata);
		GEOFENCE_DBUS_SERVER_LOGD("get_places_cb called");
	}
	sloc_geofence_complete_get_places(geofence, invocation, place_cnt, error_code, place_info);
	return TRUE;
}

/*#if SUPPORT_MULTI_CLIENT */
static gboolean geofence_remove_client_by_force(const char *client, void *data)
{
	GEOFENCE_DBUS_SERVER_LOGD("remove client by force for client [%s]", client);

	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)data;

	int *count_arr = (int *)g_hash_table_lookup(ctx->connections, client);
	if (!count_arr) {
		GEOFENCE_DBUS_SERVER_LOGD("Client[%s] is already removed", client);
		return FALSE;
	} else {
		GEOFENCE_DBUS_SERVER_LOGD("[Client: %s]. Remove all clients in hash table", client);
		if (!g_hash_table_remove(ctx->connections, client))
			GEOFENCE_DBUS_SERVER_LOGE("g_hash_table_remove is Fail");
	}

#if SUPPORT_MULTI_CLIENT
	gboolean shutdown_arr = FALSE;

	if (g_hash_table_size(ctx->connections) == 0) {
		GEOFENCE_DBUS_SERVER_SECLOG("Hash table size is zero, Now shutdown provider[%s]", ctx->prev_owner);
		shutdown_arr = TRUE;
	} else {
		GEOFENCE_DBUS_SERVER_SECLOG("Hash table size is not zero");
	}

	if (ctx->shutdown_cb) {
		ctx->shutdown_cb(ctx->userdata, shutdown_arr);
		GEOFENCE_DBUS_SERVER_LOGD("shutdown_cb called..[%d]", shutdown_arr);
	}

#endif
	GEOFENCE_DBUS_SERVER_LOGD("###### A client[%s] is abnormally shut down ########", client);


	return TRUE;
}

static void geofence_scan_sender(char *key, char *value, gpointer user_data)
{
	geofence_dbus_server_s *handle = (geofence_dbus_server_s *)user_data;
	g_return_if_fail(handle);
	gchar *prev_owner = handle->prev_owner;
	g_return_if_fail(prev_owner);

	GEOFENCE_DBUS_SERVER_LOGD("geofence_scan_sender >>  key[%s] : prev_owner[%s]\n", key, prev_owner);

	if (g_strcmp0(prev_owner, key) == 0) {
		GEOFENCE_DBUS_SERVER_LOGD("disconnected sender name matched, remove client by force!");
		geofence_remove_client_by_force(prev_owner, handle);
	}
}

static void on_name_owner_changed(GDBusConnection *connection, const gchar *sender_name, const gchar *object_path, const gchar *interface_name, const gchar *signal_name, GVariant *parameters, gpointer user_data)
{
	geofence_dbus_server_s *handle = (geofence_dbus_server_s *)user_data;
	g_return_if_fail(handle);

	gchar *service_name = NULL, *prev_owner = NULL, *new_owner = NULL;
	g_variant_get(parameters, "(&s&s&s)", &service_name, &prev_owner, &new_owner);

	if (g_strcmp0(object_path, "/org/freedesktop/DBus") != 0 || g_strcmp0(interface_name, "org.freedesktop.DBus") != 0 || g_strcmp0(sender_name, "org.freedesktop.DBus") != 0)
		return;

	/* if the prev_owner matches the sender name, then remote sender(client) is crashed */
	if (g_strcmp0(new_owner, "") == 0 && (prev_owner != NULL && strlen(prev_owner) > 0) && handle->connections != NULL) {
		if (handle->prev_owner) {
			g_free(handle->prev_owner);
			handle->prev_owner = NULL;
		}
		handle->prev_owner = g_strdup(prev_owner);
		g_hash_table_foreach(handle->connections, (GHFunc) geofence_scan_sender, handle);
	}
}
/*#endif */

static void on_bus_acquired(GDBusConnection *conn, const gchar *name, gpointer user_data)
{
	geofence_dbus_server_s *ctx = (geofence_dbus_server_s *)user_data;
	g_return_if_fail(ctx);

	gchar *path = NULL;
	SLocGeofence *geofence = NULL;

	GEOFENCE_DBUS_SERVER_LOGD("geofence dbus registered");

	/* create object for each interfaces */
	path = g_strdup_printf("%s/%s", ctx->service_path, "SAMSUNG");
	ctx->obj_skeleton = sloc_object_skeleton_new(path);

	if (NULL != path) {
		GEOFENCE_DBUS_SERVER_LOGD("object path [%s], obj_skeleton [%p]", path, ctx->obj_skeleton);
		g_free(path);
		path = NULL;
	}

	/* add geofence interface */
	geofence_dbus_setup_geofence_interface(ctx->obj_skeleton, ctx);

	if (ctx->obj_skeleton != NULL) {
		g_dbus_object_manager_server_export(ctx->manager, G_DBUS_OBJECT_SKELETON(ctx->obj_skeleton));
		/* register callback for each methods for geofence */
		geofence = sloc_object_get_geofence(SLOC_OBJECT(ctx->obj_skeleton));
	}
	if (geofence == NULL) {
		GEOFENCE_DBUS_SERVER_LOGE("Can't create geofence object");
		return;
	}

	geofence_dbus_callback_s *dbus_callback = ctx->dbus_callback;
	geofence_dbus_signal_s *dbus_signal = g_new0(geofence_dbus_signal_s, 1);
	g_return_val_if_fail(dbus_signal, GEOFENCE_DBUS_SERVER_ERROR_MEMORY);

	if (dbus_callback->add_geofence_cb)
		dbus_signal->add_geofence_h = g_signal_connect(geofence, "handle-add-geofence", G_CALLBACK(on_add_geofence), ctx);
	if (dbus_callback->delete_geofence_cb)
		dbus_signal->delete_geofence_h = g_signal_connect(geofence, "handle-delete-geofence", G_CALLBACK(on_delete_geofence), ctx);
	if (dbus_callback->get_geofences_cb)
		dbus_signal->get_geofences_h = g_signal_connect(geofence, "handle-get-geofences", G_CALLBACK(on_get_geofences), ctx);
	if (dbus_callback->enable_geofence_cb)
		dbus_signal->enable_geofence_h = g_signal_connect(geofence, "handle-enable-geofence", G_CALLBACK(on_enable_geofence), ctx);
	if (dbus_callback->start_geofence_cb)
		dbus_signal->start_geofence_h = g_signal_connect(geofence, "handle-start-geofence", G_CALLBACK(on_start_geofence), ctx);
	if (dbus_callback->stop_geofence_cb)
		dbus_signal->stop_geofence_h = g_signal_connect(geofence, "handle-stop-geofence", G_CALLBACK(on_stop_geofence), ctx);

	if (dbus_callback->add_place_cb)
		dbus_signal->add_place_h = g_signal_connect(geofence, "handle-add-place", G_CALLBACK(on_add_place), ctx);
	if (dbus_callback->update_place_cb)
		dbus_signal->update_place_h = g_signal_connect(geofence, "handle-update-place", G_CALLBACK(on_update_place), ctx);
	if (dbus_callback->delete_place_cb)
		dbus_signal->delete_place_h = g_signal_connect(geofence, "handle-delete-place", G_CALLBACK(on_delete_place), ctx);
	if (dbus_callback->get_place_name_cb)
		dbus_signal->get_place_name_h = g_signal_connect(geofence, "handle-get-place-name", G_CALLBACK(on_get_place_name), ctx);
	if (dbus_callback->get_places_cb)
		dbus_signal->get_places_h = g_signal_connect(geofence, "handle-get-places", G_CALLBACK(on_get_places), ctx);

	ctx->dbus_signal = dbus_signal;

	g_object_unref(geofence);

	/*#if SUPPORT_MULTI_CLIENT //TODO: We need it? */
	ctx->owner_changed_id = g_dbus_connection_signal_subscribe(conn, "org.freedesktop.DBus", "org.freedesktop.DBus", "NameOwnerChanged", "/org/freedesktop/DBus", NULL, G_DBUS_SIGNAL_FLAGS_NONE, on_name_owner_changed, ctx, NULL);
	/*#endif */

	g_dbus_object_manager_server_set_connection(ctx->manager, conn);
	GEOFENCE_DBUS_SERVER_LOGD("done to acquire the dbus");
}

static void on_name_acquired(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_SECLOG("Geofence Server: Acquired the name <%s> on the system bus", name);
}

static void on_name_lost(GDBusConnection *connection, const gchar *name, gpointer user_data)
{
	GEOFENCE_DBUS_SERVER_SECLOG("Geofence Server: Lost the name <%s> on the system bus", name);
}

EXPORT_API int geofence_dbus_server_send_geofence_inout_changed(geofence_dbus_server_h geofence_dbus_server, const gchar *app_id, gint fence_id, gint access_type, gint fence_state)
{
	GEOFENCE_DBUS_SERVER_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_dbus_server, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	geofence_dbus_server_s *handle = (geofence_dbus_server_s *)geofence_dbus_server;
	g_return_val_if_fail(handle->obj_skeleton, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	SLocGeofence *geofence = NULL;
	geofence = sloc_object_get_geofence(SLOC_OBJECT(handle->obj_skeleton));
	g_return_val_if_fail(geofence, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	sloc_geofence_emit_geofence_inout(geofence, app_id, fence_id, access_type, fence_state);
	g_object_unref(geofence);

	return GEOFENCE_DBUS_SERVER_ERROR_NONE;
}

EXPORT_API int geofence_dbus_server_send_geofence_event_changed(geofence_dbus_server_h geofence_dbus_server, gint place_id, gint fence_id, gint access_type, const gchar *app_id, gint error, gint state)
{
	GEOFENCE_DBUS_SERVER_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_dbus_server, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);
	geofence_dbus_server_s *handle = (geofence_dbus_server_s *)geofence_dbus_server;
	g_return_val_if_fail(handle->obj_skeleton, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	SLocGeofence *geofence = NULL;
	geofence = sloc_object_get_geofence(SLOC_OBJECT(handle->obj_skeleton));
	g_return_val_if_fail(geofence, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	sloc_geofence_emit_geofence_event(geofence, place_id, fence_id, access_type, app_id, error, state);
	g_object_unref(geofence);

	return GEOFENCE_DBUS_SERVER_ERROR_NONE;
}

/* Because of this, geofence-server's handler doesn't work. */
#if 0
static void __glib_log(const gchar *log_domain, GLogLevelFlags log_level, const gchar *msg, gpointer user_data)
{
	geofence_dbus_server_s *server = (geofence_dbus_server_s *)user_data;
	if (server != NULL)
		GEOFENCE_DBUS_SERVER_LOGD("server->manager (%p)", server->manager);
	GEOFENCE_DBUS_SERVER_LOGE("GLIB[%d] : %s", log_level, msg);
}
#endif

EXPORT_API int geofence_dbus_server_create(geofence_dbus_server_h *geofence_dbus_server, geofence_dbus_callback_s *geofence_dbus_callback, gpointer userdata)
{
	GEOFENCE_DBUS_SERVER_LOGD("geofence_dbus_server_create");
	g_return_val_if_fail(geofence_dbus_server, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);
	g_return_val_if_fail(geofence_dbus_callback, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	int ret = GEOFENCE_DBUS_SERVER_ERROR_NONE;

	geofence_dbus_server_s *server = g_new0(geofence_dbus_server_s, 1);
	g_return_val_if_fail(server, GEOFENCE_DBUS_SERVER_ERROR_MEMORY);

	/* 	g_log_set_default_handler(__glib_log, server);	*/

	server->service_name = g_strdup(GEOFENCE_SERVICE_NAME);
	server->service_path = g_strdup(GEOFENCE_SERVICE_PATH);
	server->manager = g_dbus_object_manager_server_new(server->service_path);
	server->connections = g_hash_table_new_full(g_str_hash, g_str_equal, g_free, g_free);
	server->userdata = userdata;

	/* set gefence callbacks  */
	server->dbus_callback = geofence_dbus_callback;

	server->owner_id = g_bus_own_name(G_BUS_TYPE_SYSTEM, server->service_name, G_BUS_NAME_OWNER_FLAGS_REPLACE, on_bus_acquired, on_name_acquired, on_name_lost, server, NULL);
	GEOFENCE_DBUS_SERVER_LOGD("g_bus_own_name id=[%d]", server->owner_id);
	*geofence_dbus_server = (geofence_dbus_server_s *) server;

	return ret;
}
EXPORT_API int geofence_dbus_server_destroy(geofence_dbus_server_h geofence_dbus_server)
{
	GEOFENCE_DBUS_SERVER_LOGD("geofence_dbus_server_destroy");
	g_return_val_if_fail(geofence_dbus_server, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	geofence_dbus_server_s *server = (geofence_dbus_server_s *)geofence_dbus_server;
	int ret = GEOFENCE_DBUS_SERVER_ERROR_NONE;
	g_bus_unown_name(server->owner_id);
	if (server->prev_owner) {
		g_free(server->prev_owner);
		server->prev_owner = NULL;
	}
	/* disconnect geofence callbacks */
	SLocGeofence *geofence = NULL;
	geofence = sloc_object_get_geofence(SLOC_OBJECT(server->obj_skeleton));
	g_return_val_if_fail(geofence, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	geofence_dbus_signal_s *handle = server->dbus_signal;
	g_return_val_if_fail(handle, GEOFENCE_DBUS_SERVER_ERROR_PARAMETER);

	if (handle->add_geofence_h) {
		g_signal_handler_disconnect(geofence, handle->add_geofence_h);
		handle->add_geofence_h = 0;
	}
	if (handle->delete_geofence_h) {
		g_signal_handler_disconnect(geofence, handle->delete_geofence_h);
		handle->delete_geofence_h = 0;
	}
	if (handle->get_geofences_h) {
		g_signal_handler_disconnect(geofence, handle->get_geofences_h);
		handle->get_geofences_h = 0;
	}
	if (handle->enable_geofence_h) {
		g_signal_handler_disconnect(geofence, handle->enable_geofence_h);
		handle->enable_geofence_h = 0;
	}
	if (handle->start_geofence_h) {
		g_signal_handler_disconnect(geofence, handle->start_geofence_h);
		handle->start_geofence_h = 0;
	}
	if (handle->stop_geofence_h) {
		g_signal_handler_disconnect(geofence, handle->stop_geofence_h);
		handle->stop_geofence_h = 0;
	}

	if (handle->add_place_h) {
		g_signal_handler_disconnect(geofence, handle->add_place_h);
		handle->add_place_h = 0;
	}
	if (handle->update_place_h) {
		g_signal_handler_disconnect(geofence, handle->update_place_h);
		handle->update_place_h = 0;
	}
	if (handle->delete_place_h) {
		g_signal_handler_disconnect(geofence, handle->delete_place_h);
		handle->delete_place_h = 0;
	}
	if (handle->get_place_name_h) {
		g_signal_handler_disconnect(geofence, handle->get_place_name_h);
		handle->get_place_name_h = 0;
	}
	if (handle->get_places_h) {
		g_signal_handler_disconnect(geofence, handle->get_places_h);
		handle->get_places_h = 0;
	}
	g_free(handle);
	g_object_unref(geofence);

	if (server->manager) {
		if (server->owner_changed_id) {
			g_dbus_connection_signal_unsubscribe(g_dbus_object_manager_server_get_connection(server->manager), server->owner_changed_id);
			server->owner_changed_id = 0;
		}
		g_object_unref(server->manager);
		server->manager = NULL;
	}

	g_hash_table_destroy(server->connections);

	g_free(server->service_name);
	g_free(server->service_path);

	g_free(server);

	return ret;
}
