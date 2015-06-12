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
#include <app_manager.h>
#include "geofence_client.h"
#include "geofence_client_priv.h"

typedef struct _geofence_client_dbus_s {
	GDBusConnection *conn;
	gchar *service_name;
	gchar *service_path;
	int geofence_evt_id;
	int geofence_evt_status_id;
	int geofence_id;
	geofence_client_cb user_cb;
	geofence_client_event_cb user_event_cb;
	void *user_data;
} geofence_client_dbus_s;

static void __geofence_signal_callback(GDBusConnection *conn, const gchar *name, const gchar *path, const gchar *interface, const gchar *sig, GVariant *param, gpointer user_data)
{
	GEOFENCE_CLIENT_SECLOG("name : %s, path : %s, interface : %s, sig : %s, handle[%p]", name, path, interface, sig, user_data);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *) user_data;
	if (handle == NULL) {
		GEOFENCE_CLIENT_LOGD("Invalid handle");
		return;
	}
	if (handle->user_cb)
		handle->user_cb(sig, param, handle->user_data);
}

EXPORT_API int geo_client_add_geofence(geofence_client_dbus_h geofence_client, gint place_id, gint geofence_type, gdouble latitude, gdouble longitude, gint radius, const gchar *address, const gchar *bssid, const gchar *ssid)
{
	/* add fence interface between App & geofence-server */
	GEOFENCE_CLIENT_LOGD("geo_client_add_fence");
	g_return_val_if_fail(geofence_client, 0);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	gint fence_id = -1;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	int ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]", ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);
	gchar *signal_path = NULL;
	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		GEOFENCE_CLIENT_LOGD(" proxy exited !");
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
		}
		g_object_unref(proxy);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
	}

	if (app_id)
		g_free(app_id);

	return fence_id;
}

EXPORT_API int geo_client_add_place(geofence_client_dbus_h geofence_client, const gchar *place_name)
{
	/* add fence interface between App & geofence-server */
	GEOFENCE_CLIENT_LOGD("geo_client_add_place");
	g_return_val_if_fail(geofence_client, 0);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int place_id = -1;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	int ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);
	gchar *signal_path = NULL;
	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		GEOFENCE_CLIENT_LOGD(" proxy exited !");
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
		}
		g_object_unref(proxy);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
		}
	}
	if (app_id)
		g_free(app_id);

	return place_id;
}

EXPORT_API int geo_client_update_place(geofence_client_dbus_h geofence_client, gint place_id, const gchar *place_name)
{
	GEOFENCE_CLIENT_LOGD("geo_client_update_place");
	g_return_val_if_fail(geofence_client, 0);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	int ret = app_manager_get_app_id(pid, &app_id);
	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);
	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	ret = GEOFENCE_CLIENT_ERROR_NONE;
	gchar *signal_path = NULL;
	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		GEOFENCE_CLIENT_LOGD(" proxy exited !");
		g_dbus_proxy_call(proxy, "UpdatePlace",	g_variant_new("(iss)", place_id, app_id, place_name), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_delete_geofence(geofence_client_dbus_h geofence_client, gint fence_id)
{
	GEOFENCE_CLIENT_LOGD("geo_client_delete_geofence");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *) geofence_client;

	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	gchar *signal_path = NULL;
	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		g_dbus_proxy_call(proxy, "DeleteGeofence", g_variant_new("(is)", fence_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_delete_place(geofence_client_dbus_h geofence_client, gint place_id)
{
	GEOFENCE_CLIENT_LOGD("geo_client_delete_place");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);
	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	gchar *signal_path = NULL;
	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);
	if (proxy) {
		g_dbus_proxy_call(proxy, "DeletePlace",	g_variant_new("(is)", place_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_enable_service(geofence_client_dbus_h geofence_client, gint geofence_id, gboolean bEnable)
{
	GEOFENCE_CLIENT_LOGD("geo_client_enable_geofence, id(%d)", geofence_id);
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	gchar *signal_path = NULL;
	pid_t pid = 0;
	gboolean enable = bEnable;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	GEOFENCE_CLIENT_LOGD("handle->user_cb : %x", handle->user_cb);

	/* Start geofence */
	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		g_dbus_proxy_call(proxy, "EnableGeofence", g_variant_new("(isb)", geofence_id, app_id, enable),	G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_start_geofence(geofence_client_dbus_h geofence_client, int geofence_id)
{
	GEOFENCE_CLIENT_LOGD("geo_client_start_geofence, id(%d)", geofence_id);
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	gchar *signal_path = NULL;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	GEOFENCE_CLIENT_LOGD("handle->user_cb : %x", handle->user_cb);

	/* Start geofence */
	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		g_dbus_proxy_call(proxy, "StartGeofence", g_variant_new("(is)", geofence_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_stop_geofence(geofence_client_dbus_h geofence_client, int geofence_id)
{
	GEOFENCE_CLIENT_LOGD("geo_client_stop_geofence");

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;
	g_return_val_if_fail(handle, GEOFENCE_CLIENT_ERROR_PARAMETER);

	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	GError *error = NULL;
	gchar *app_id = NULL;
	pid_t pid = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);
	GEOFENCE_CLIENT_LOGD("handle->user_cb : %x", handle->user_cb);

	gchar *signal_path = NULL;
	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	/* Stop*/
	GDBusProxy *proxy = NULL;
	GEOFENCE_CLIENT_LOGD("handle->conn: %p", handle->conn);
	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		g_dbus_proxy_call(proxy, "StopGeofence", g_variant_new("(is)", geofence_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, NULL, &error);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_get_place_name(geofence_client_dbus_h geofence_client, int place_id, char **placeName, int *errorCode)
{
	GEOFENCE_CLIENT_LOGD("geo_client_get_place_name");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	gchar *signal_path = NULL;
	pid_t pid = 0;
	char *place_name = NULL;
	int error_code = -1;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	GEOFENCE_CLIENT_LOGD("handle->user_cb : %x", handle->user_cb);
	/* Start geofence */
	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		reg = g_dbus_proxy_call_sync(proxy, "GetPlaceName", g_variant_new("(is)", place_id, app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(is)", &error_code, &place_name);
			*errorCode = error_code;
			*placeName = strdup(place_name);
			if (place_name)
				g_free(place_name);
			g_variant_unref(reg);
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to get the place name Error[%s]", error->message);
				g_error_free(error);
				ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
			}
		}
		g_object_unref(proxy);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}

	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_get_list(geofence_client_dbus_h geofence_client, int place_id, GVariantIter **iter, int *fenceCnt, int *errorCode)
{
	GEOFENCE_CLIENT_LOGD("geo_client_get_list");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	gchar *signal_path = NULL;
	pid_t pid = 0;
	GVariantIter *iterator;
	int error_code = -1;
	int fence_cnt = 1;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);

	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	GEOFENCE_CLIENT_LOGD("handle->user_cb : %x", handle->user_cb);

	/* Start geofence */
	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);

	if (proxy) {
		reg = g_dbus_proxy_call_sync(proxy, "GetList", g_variant_new("(is)", place_id, app_id),	G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(iiaa{sv})", &fence_cnt, &error_code, &iterator);
			*fenceCnt = fence_cnt;
			*errorCode = error_code;
			if (iterator == NULL)
				GEOFENCE_CLIENT_LOGE("Iterator is null");
			*iter = iterator;
			g_variant_unref(reg);
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to get the list Error[%s]", error->message);
				g_error_free(error);
				ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
			}
		}
		g_object_unref(proxy);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

EXPORT_API int geo_client_get_place_list(geofence_client_dbus_h geofence_client, GVariantIter **iter, int *placeCnt, int *errorCode)
{
	GEOFENCE_CLIENT_LOGD("geo_client_get_place_list");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *reg = NULL;
	GError *error = NULL;
	GDBusProxy *proxy = NULL;
	gchar *app_id = NULL;
	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	gchar *signal_path = NULL;
	pid_t pid = 0;
	GVariantIter *iterator;
	int error_code = -1;
	int place_cnt = 0;

	GEOFENCE_CLIENT_LOGI("Getting the app id in client");
	pid = getpid();
	ret = app_manager_get_app_id(pid, &app_id);
	if (ret != APP_MANAGER_ERROR_NONE)
		GEOFENCE_CLIENT_LOGE("Fail to get app_id from module_geofence_server. Err[%d]",	ret);

	GEOFENCE_CLIENT_LOGI("APP ID is : %s", app_id);

	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_SECLOG("GEOFENCE signal subscribe Object Path [%s]", signal_path);

	GEOFENCE_CLIENT_LOGD("handle->user_cb : %x", handle->user_cb);

	/* Start geofence */
	proxy = g_dbus_proxy_new_sync(handle->conn, G_DBUS_PROXY_FLAGS_NONE, NULL, handle->service_name, signal_path, "org.tizen.lbs.Geofence", NULL, &error);
	g_free(signal_path);
	if (proxy) {
		reg = g_dbus_proxy_call_sync(proxy, "GetPlaceList", g_variant_new("(s)", app_id), G_DBUS_CALL_FLAGS_NONE, -1, NULL, &error);
		if (reg) {
			g_variant_get(reg, "(iiaa{sv})", &place_cnt, &error_code, &iterator);
			*placeCnt = place_cnt;
			*errorCode = error_code;
			if (iterator == NULL)
				GEOFENCE_CLIENT_LOGE("Iterator is null");
			*iter = iterator;
			g_variant_unref(reg);
		} else {
			if (error) {
				GEOFENCE_CLIENT_LOGE("Fail to get the place list Error[%s]", error->message);
				g_error_free(error);
				ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
			}
		}
		g_object_unref(proxy);
	} else {
		GEOFENCE_CLIENT_LOGD(" proxy NOT exited !");
		if (error) {
			GEOFENCE_CLIENT_LOGE("Fail to get proxy Error[%s]", error->message);
			g_error_free(error);
			ret = GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	if (app_id)
		g_free(app_id);

	return ret;
}

static void geo_client_signal_unsubcribe(geofence_client_dbus_h geofence_client)
{
	GEOFENCE_CLIENT_LOGD("geo_client_signal_unsubcribe");

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
}

EXPORT_API int geo_client_start(char *service_name, char *service_path, geofence_client_dbus_h *geofence_client, geofence_client_cb callback, geofence_client_event_cb evt_callback, void *user_data)
{
	GEOFENCE_CLIENT_LOGD("geo_client_start");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	GVariant *param = NULL;
	GVariantBuilder *builder = NULL;
	gchar *signal_path = NULL;

	handle->service_name = g_strdup(service_name);
	handle->service_path = g_strdup(service_path);

	signal_path = g_strdup_printf("%s/%s", handle->service_path, "SAMSUNG");
	GEOFENCE_CLIENT_LOGD("Geofence signal subscribe Object Path [%s]", signal_path);

	if (callback) {
		handle->user_cb = callback;
		handle->user_data = user_data;
		handle->geofence_evt_id = g_dbus_connection_signal_subscribe(handle->conn, handle->service_name, "org.tizen.lbs.Geofence", "GeofenceInout", signal_path, NULL, G_DBUS_SIGNAL_FLAGS_NONE, __geofence_signal_callback, handle, NULL);

		if (handle->geofence_evt_id) {
			GEOFENCE_CLIENT_LOGD("Listening Geofence Changed");
		} else {
			GEOFENCE_CLIENT_LOGD("Fail to listen Geofence Changed");
		}
	}
	if (callback) {
		handle->user_event_cb = evt_callback;
		handle->user_data = user_data;
		handle->geofence_evt_status_id = g_dbus_connection_signal_subscribe(handle->conn, handle->service_name,	"org.tizen.lbs.Geofence", "GeofenceEvent", signal_path,	NULL, G_DBUS_SIGNAL_FLAGS_NONE, __geofence_signal_callback, handle, NULL);

		if (handle->geofence_evt_status_id) {
			GEOFENCE_CLIENT_LOGD("Listening Geofence event changed");
		} else {
			GEOFENCE_CLIENT_LOGD("Fail to listen Geofence event changed");
			return GEOFENCE_CLIENT_ERROR_DBUS_CALL;
		}
	}
	g_free(signal_path);

	/* Start */
	GEOFENCE_CLIENT_LOGD("START: CMD-START");
	builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(builder, "{sv}", "CMD", g_variant_new_string("START"));

	param = g_variant_ref_sink(g_variant_new("(@a{sv})", g_variant_builder_end(builder)));

	g_variant_unref(param);

	return GEOFENCE_CLIENT_ERROR_NONE;
}

EXPORT_API int geo_client_stop(geofence_client_dbus_h *geofence_client)
{
	GEOFENCE_CLIENT_LOGD("geo_client_stop");

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;;
	g_return_val_if_fail(handle, GEOFENCE_CLIENT_ERROR_PARAMETER);

	GVariant *param = NULL;
	GVariantBuilder *builder = NULL;
	geo_client_signal_unsubcribe(handle);
	/* Stop*/
	GEOFENCE_CLIENT_LOGD("STOP: CMD-STOP");
	builder = g_variant_builder_new(G_VARIANT_TYPE("a{sv}"));
	g_variant_builder_add(builder, "{sv}", "CMD", g_variant_new_string("STOP"));
	param = g_variant_ref_sink(g_variant_new("(@a{sv})", g_variant_builder_end(builder)));

	g_variant_unref(param);

	return GEOFENCE_CLIENT_ERROR_NONE;
}

static int __geofence_client_create_connection(geofence_client_dbus_s *client)
{
	GEOFENCE_CLIENT_LOGD("__geofence_client_create_connection");
	g_return_val_if_fail(client, GEOFENCE_CLIENT_ERROR_PARAMETER);
	GError *error = NULL;

	client->conn = g_bus_get_sync(G_BUS_TYPE_SYSTEM, NULL, &error);
	if (!client->conn) {
		if (error && error->message) {
			GEOFENCE_CLIENT_LOGD("Fail to get GBus. ErrCode[%d], Msg[%s]", error->code, error->message);
			g_error_free(error);
			error = NULL;
		}
		GEOFENCE_CLIENT_LOGD("Fail to get addr of bus.");
		return GEOFENCE_CLIENT_ERROR_CONNECTION;
	}
	GEOFENCE_CLIENT_LOGD("client->conn: %p", client->conn);

	return GEOFENCE_CLIENT_ERROR_NONE;
}

static void _glib_log(const gchar *log_domain, GLogLevelFlags log_level, const gchar *msg, gpointer user_data)
{
	geofence_client_dbus_s *client = (geofence_client_dbus_s *)user_data;
	if (client != NULL) {
		GEOFENCE_CLIENT_LOGD("client->conn: %p", client->conn);
	}
	GEOFENCE_CLIENT_LOGE("GLIB[%d] : %s", log_level, msg);
}

/* The reason why we seperate this from start is to support IPC for db operation between a server and a client.*/
EXPORT_API int geo_client_create(geofence_client_dbus_h *geofence_client)
{
	GEOFENCE_CLIENT_LOGD("geo_client_create");

	int ret = GEOFENCE_CLIENT_ERROR_NONE;
	geofence_client_dbus_s *client = g_new0(geofence_client_dbus_s, 1);
	g_return_val_if_fail(client, GEOFENCE_CLIENT_ERROR_MEMORY);
	g_log_set_default_handler(_glib_log, client);

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
	GEOFENCE_CLIENT_LOGD("geo_client_destroy");
	g_return_val_if_fail(geofence_client, GEOFENCE_CLIENT_ERROR_PARAMETER);

	geofence_client_dbus_s *handle = (geofence_client_dbus_s *)geofence_client;

	if (handle->conn) {
		g_object_unref(handle->conn);
		handle->conn = NULL;
	}
	if (handle->service_path)
		g_free(handle->service_path);
	if (handle->service_name)
		g_free(handle->service_name);
	g_free(handle);

	return GEOFENCE_CLIENT_ERROR_NONE;
}
