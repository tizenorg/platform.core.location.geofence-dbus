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
#include "geofence_client.h"
#include "geofence_client_priv.h"

#define GEOFENCE_SERVICE_NAME	"org.tizen.lbs.Providers.GeofenceServer"
#define GEOFENCE_SERVICE_PATH	"/org/tizen/lbs/Providers/GeofenceServer"
#define GEOFENCE_INTERFACE_NAME "org.tizen.lbs.Geofence"


typedef struct _geofence_client_dbus_s {
	GDBusConnection *conn;
	gchar *service_name;
	gchar *service_path;
	gchar *signal_path;
	int geofence_evt_id;
	int geofence_proximity_id;
	int geofence_evt_status_id;
	geofence_client_cb user_cb;
	void *user_data;
} geofence_client_dbus_s;

static void __geofence_signal_callback(GDBusConnection *conn, const gchar *name, const gchar *path, const gchar *interface, const gchar *sig, GVariant *param, gpointer user_data)
{
	GEOFENCE_CLIENT_SECLOG("name: %s, path: %s, interface: %s, sig: %s, handle[%p]", name, path, interface, sig, user_data);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *) user_data;
	if (handle == NULL) {
		GEOFENCE_CLIENT_LOGD("Invalid handle");
		return;
	}
	if (handle->user_cb)
		handle->user_cb(sig, param, handle->user_data);
}

EXPORT_API int geo_client_add_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, gint geofence_type, gdouble latitude, gdouble longitude, gint radius, const gchar *address, const gchar *bssid, const gchar *ssid, gint *error_code)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int fence_id = -1;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		GEOFENCE_CLIENT_LOGD("proxy: %p", proxy);

		reg = g_dbus_proxy_call_sync(proxy, "AddGeofence", g_variant_new("(siiddisss)", app_id,	place_id, geofence_type, latitude, longitude, radius, address, bssid, ssid), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(i)", &fence_id);
			g_variant_unref(reg);
			reg = NULL;
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to add geofence Error[%s]", error->message);
				g_error_free(error);
			}
			if (error_code != NULL)
				*error_code = GEOFENCE_CLIENT_ACCESS_DENIED;
		}
		g_object_unref(proxy);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}

	}
	GEOFENCE_CLIENT_LOGD("fence_id: %d", fence_id);

	return fence_id;
}

EXPORT_API int geo_client_delete_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint fence_id)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *) geofence_client;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		g_dbus_proxy_call(proxy, "DeleteGeofence", g_variant_new("(is)", fence_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ACCESS_DENIED;
	}

	return ret;
}

EXPORT_API int geo_client_get_geofences(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, GVariantIter **iter, gint *fence_cnt, gint *error_code)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	GVariantIter *iterator;
	int new_error_code = 0;
	int new_fence_cnt = 0;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		GEOFENCE_CLIENT_LOGD("proxy: %p", proxy);

		reg = g_dbus_proxy_call_sync(proxy, "GetGeofences", g_variant_new("(is)", place_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(iiaa{sv})", &new_fence_cnt, &new_error_code, &iterator);
			if (error_code != NULL)
				*error_code = new_error_code;
			*fence_cnt = new_fence_cnt;
			if (iterator == NULL)
				GEOFENCE_CLIENT_LOGE("Iterator is null");
			*iter = iterator;
			g_variant_unref(reg);
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to get the list Error[%s]", error->message);
				g_error_free(error);
			}
			ret = GEOFENCE_CLIENT_ACCESS_DENIED;
		}
		g_object_unref(proxy);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}

	return ret;
}

EXPORT_API int geo_client_enable_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint geofence_id, gboolean onoff)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		g_dbus_proxy_call(proxy, "EnableGeofence", g_variant_new("(isb)", geofence_id, app_id, onoff),	G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ACCESS_DENIED;
	}

	return ret;
}

EXPORT_API int geo_client_start_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint geofence_id)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;

	GEOFENCE_CLIENT_LOGD("handle->conn: %p, geofence_id", handle->conn, geofence_id);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		g_dbus_proxy_call(proxy, "StartGeofence", g_variant_new("(is)", geofence_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ACCESS_DENIED;
	}
	return ret;
}

EXPORT_API int geo_client_stop_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint geofence_id)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;

	GEOFENCE_CLIENT_LOGD("handle->conn: %p", handle->conn);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		g_dbus_proxy_call(proxy, "StopGeofence", g_variant_new("(is)", geofence_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ACCESS_DENIED;
	}

	return ret;
}

EXPORT_API int geo_client_add_place(geofence_client_dbus_h geofence_client, gchar *app_id, const gchar *place_name, gint *error_code)
{
	/* add fence interface between App & geofence-server */
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int place_id = -1;

	GEOFENCE_CLIENT_LOGI("APP ID: %s", app_id);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		GEOFENCE_CLIENT_LOGD("proxy: %p", proxy);
		reg = g_dbus_proxy_call_sync(proxy, "AddPlace", g_variant_new("(ss)", app_id, place_name), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(i)", &place_id);
			g_variant_unref(reg);
			reg = NULL;
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to add place Error[%s]", error->message);
				g_error_free(error);
			}
			if (error_code != NULL)
				*error_code = GEOFENCE_CLIENT_ACCESS_DENIED;
		}
		g_object_unref(proxy);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
	}

	GEOFENCE_CLIENT_LOGD("place_id: %d", place_id);

	return place_id;
}

EXPORT_API int geo_client_update_place(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, const gchar *place_name)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		g_dbus_proxy_call(proxy, "UpdatePlace",	g_variant_new("(iss)", place_id, app_id, place_name), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ACCESS_DENIED;
	}

	return ret;
}

EXPORT_API int geo_client_delete_place(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		g_dbus_proxy_call(proxy, "DeletePlace",	g_variant_new("(is)", place_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ACCESS_DENIED;
	}

	return ret;
}

EXPORT_API int geo_client_get_place_name(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, gchar **place_name, gint *error_code)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	char *new_place_name = NULL;
	int new_error_code = 0;

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		reg = g_dbus_proxy_call_sync(proxy, "GetPlaceName", g_variant_new("(is)", place_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(is)", &new_error_code, &new_place_name);
			if (error_code != NULL)
				*error_code = new_error_code;
			*place_name = g_strdup(new_place_name);
			g_free(new_place_name);
			g_variant_unref(reg);
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to get the place name Error[%s]", error->message);
				g_error_free(error);
			}
			ret = GEOFENCE_CLIENT_ACCESS_DENIED;
		}
		g_object_unref(proxy);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}

	return ret;
}

EXPORT_API int geo_client_get_places(geofence_client_dbus_h geofence_client, gchar *app_id, GVariantIter **iter, gint *place_cnt, gint *error_code)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;

	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	GVariantIter *iterator;
	int new_error_code = 0;
	int new_place_cnt = 0;

	GEOFENCE_CLIENT_LOGD("handle->conn: %p", handle->conn);
	GEOFENCE_CLIENT_LOGI("APP ID: %s", app_id);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, handle->signal_path, GEOFENCE_INTERFACE_NAME, NULL, &error);
	if (proxy) {
		reg = g_dbus_proxy_call_sync(proxy, "GetPlaces", g_variant_new("(s)", app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(iiaa{sv})", &new_place_cnt, &new_error_code, &iterator);
			*place_cnt = new_place_cnt;
			if (error_code != NULL)
				*error_code = new_error_code;
			if (iterator == NULL)
				GEOFENCE_CLIENT_LOGE("Iterator is null");
			*iter = iterator;
			g_variant_unref(reg);
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to get the place list Error[%s]", error->message);
				g_error_free(error);
			}
			ret = GEOFENCE_CLIENT_ACCESS_DENIED;
		}
		g_object_unref(proxy);
	} else {
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
		ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
	}

	return ret;
}

EXPORT_API int geo_client_start(geofence_client_dbus_h geofence_client, geofence_client_cb callback, void *user_data)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	gchar *signal_path = NULL;

	handle->service_name = g_strdup(GEOFENCE_SERVICE_NAME);
	handle->service_path = g_strdup(GEOFENCE_SERVICE_PATH);
	handle->signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_LOGD("Object Path [%s]", handle->signal_path);

	if (callback) {
		handle->user_cb = callback;
		handle->user_data = user_data;
		handle->geofence_evt_id = g_dbus_connection_signal_subscribe(handle->conn, handle->service_name, GEOFENCE_INTERFACE_NAME, "GeofenceInout", handle->signal_path, NULL, G_DBUS_SIGNAL_FLAGS_NONE, __geofence_signal_callback, handle, NULL);

		if (handle->geofence_evt_id) {
			GEOFENCE_CLIENT_LOGD("Listening GeofenceInout");
		} else {
			GEOFENCE_CLIENT_LOGD("Fail to listen GeofenceInout");
		}

		handle->geofence_proximity_id = g_dbus_connection_signal_subscribe(handle->conn, handle->service_name, GEOFENCE_INTERFACE_NAME, "GeofenceProximity", handle->signal_path, NULL, G_DBUS_SIGNAL_FLAGS_NONE, __geofence_signal_callback, handle, NULL);

		if (handle->geofence_proximity_id) {
			GEOFENCE_CLIENT_LOGD("Listening GeofenceProximity");
		} else {
			GEOFENCE_CLIENT_LOGD("Fail to listen GeofenceProximity");
		}

		handle->geofence_evt_status_id = g_dbus_connection_signal_subscribe(handle->conn, handle->service_name,	GEOFENCE_INTERFACE_NAME, "GeofenceEvent", handle->signal_path,	NULL, G_DBUS_SIGNAL_FLAGS_NONE, __geofence_signal_callback, handle, NULL);

		if (handle->geofence_evt_status_id) {
			GEOFENCE_CLIENT_LOGD("Listening Geofence event");
		} else {
			GEOFENCE_CLIENT_LOGD("Fail to listen Geofence event");
			return GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	g_free(signal_path);


#if SUPPORT_MULTI_CLIENT
	GVariant *param = NULL;
	GVariantBuilder *builder = NULL;


	GEOFENCE_CLIENT_LOGD("START: CMD-START");
	builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(builder, "{sv}", "CMD", g_variant_new_string("START"));

	param = g_variant_ref_sink(g_variant_new("(@a{sv})", g_variant_builder_end(builder)));

	g_variant_unref(param);
#endif

	return GEOFENCE_CLIENT_ERROR_NONE;
}

static void __geo_client_signal_unsubcribe(geofence_client_dbus_h geofence_client)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	if (handle == NULL) {
		GEOFENCE_CLIENT_LOGE("Invalid handle");
		return;
	}
	if (handle->conn == NULL) {
		GEOFENCE_CLIENT_LOGE("Invalid dbus_connection");
		return;
	}
	if (handle->geofence_evt_id) {
		g_dbus_connection_signal_unsubscribe(handle->conn, handle->geofence_evt_id);
		handle->geofence_evt_id = 0;
	}
	if (handle->geofence_evt_status_id) {
		g_dbus_connection_signal_unsubscribe(handle->conn, handle->geofence_evt_status_id);
		handle->geofence_evt_status_id = 0;
	}
	if (handle->geofence_proximity_id) {
		g_dbus_connection_signal_unsubscribe(handle->conn, handle->geofence_proximity_id);
		handle->geofence_proximity_id = 0;
	}
}

EXPORT_API int geo_client_stop(geofence_client_dbus_h geofence_client)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;;
	g_return_val_if_fail(handle, GEOFENCE_CLIENT_ERROR_PARAMETER);

	__geo_client_signal_unsubcribe(handle);

#if SUPPORT_MULTI_CLIENT
	GVariant *param = NULL;
	GVariantBuilder *builder = NULL;

	/* Stop*/
	GEOFENCE_CLIENT_LOGD("STOP: CMD-STOP");
	builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(builder, "{sv}", "CMD", g_variant_new_string("STOP"));
	param = g_variant_ref_sink(g_variant_new("(@a{sv})", g_variant_builder_end(builder)));

	g_variant_unref(param);

#endif

	return GEOFENCE_CLIENT_ERROR_NONE;
}

static int __geofence_client_create_connection(geofence_client_dbus_s *client)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");

	g_return_val_if_fail(client, GEOFENCE_CLIENT_ERROR_PARAMETER);
	GError *error = NULL;

	char *bus_addr = NULL;
	bus_addr = g_dbus_address_get_for_bus_sync(G_BUS_TYPE_SESSION, NULL, &error);
	if (!bus_addr) {
		GEOFENCE_CLIENT_LOGD("Fail to get addr of bus.");
		return GEOFENCE_CLIENT_ERROR_CONNECTION;
	}

	GEOFENCE_CLIENT_LOGD("bus_addr: %s", bus_addr);

	client->conn = g_dbus_connection_new_for_address_sync(bus_addr,
	                                                      G_DBUS_CONNECTION_FLAGS_AUTHENTICATION_CLIENT |
	                                                      G_DBUS_CONNECTION_FLAGS_MESSAGE_BUS_CONNECTION,
	                                                      NULL, NULL, &error);
	g_free(bus_addr);

	if (!client->conn) {
		if (error && error->message) {
			GEOFENCE_CLIENT_LOGD("Fail to get GBus. ErrCode[%d], Msg[%s]", error->code, error->message);
			g_error_free(error);
			error = NULL;
		}
		return GEOFENCE_CLIENT_ERROR_CONNECTION;
	}
	GEOFENCE_CLIENT_LOGD("client->conn: %p", client->conn);

	return GEOFENCE_CLIENT_ERROR_NONE;
}

static void __glib_log(const gchar *log_domain, GLogLevelFlags log_level, const gchar *msg, gpointer user_data)
{
	geofence_client_dbus_s *client = (geofence_client_dbus_s *)user_data;
	if (client != NULL) {
		GEOFENCE_CLIENT_LOGD("client->conn: %p", client->conn);
	}
	GEOFENCE_CLIENT_LOGE("GLIB[%d]: %s", log_level, msg);
}

/* The reason why we seperate this from start is to support IPC for db operation between a server and a client.*/
EXPORT_API int geo_client_create(geofence_client_dbus_h *geofence_client)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");

	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	geofence_client_dbus_s *client = g_new0(geofence_client_dbus_s, 1);
	g_return_val_if_fail(client, GEOFENCE_CLIENT_ERROR_MEMORY);
	g_log_set_default_handler(__glib_log, client);

	ret = __geofence_client_create_connection(client);
	if (ret != GEOFENCE_CLIENT_ERROR_NONE) {
		g_free(client);
		return ret;
	}
	*geofence_client = (geofence_client_dbus_s *) client;

	return GEOFENCE_CLIENT_ERROR_NONE;
}

EXPORT_API int geo_client_destroy(geofence_client_dbus_h geofence_client)
{
	GEOFENCE_CLIENT_LOGD("ENTER >>>");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	if (handle->conn) {
		g_object_unref(handle->conn);
		handle->conn = NULL;
	}
	g_free(handle->service_path);
	g_free(handle->service_name);
	g_free(handle->signal_path);
	g_free(handle);

	return GEOFENCE_CLIENT_ERROR_NONE;
}
