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

/**
 * @file	geofence_client.h
 * @brief	This file contains the internal definitions and structures related to geofence client.
 */
#ifndef __GEOFENCE_CLIENT_H__
#define __GEOFENCE_CLIENT_H__

__BEGIN_DECLS
#include <gio/gio.h>
#include <tizen_error.h>

typedef enum {
    GEOFENCE_STATUS_CB = 0x01,
    GEOFENCE_INOUT_CB = GEOFENCE_STATUS_CB << 0x01,
} geofence_client_callback_e;

typedef enum {
    GEOFENCE_CLIENT_ERROR_NONE = 0x0,
    GEOFENCE_CLIENT_ERROR_PARAMETER = TIZEN_ERROR_INVALID_PARAMETER,
    GEOFENCE_CLIENT_ERROR_MEMORY = TIZEN_ERROR_OUT_OF_MEMORY,
    GEOFENCE_CLIENT_ACCESS_DENIED = TIZEN_ERROR_PERMISSION_DENIED,
    GEOFENCE_CLIENT_ERROR_CONNECTION = TIZEN_ERROR_GEOFENCE_MANAGER | 0x01,
    GEOFENCE_CLIENT_ERROR_DBUS_CALL = TIZEN_ERROR_GEOFENCE_MANAGER | 0x06,
} geofence_client_error_e;

typedef void *geofence_client_dbus_h;

/**
* @brief	Callback registered by the client to get the callback for the geofence in/out status
*/
typedef void (*geofence_client_cb)(const char *sig, GVariant *param, void *user_data);

#if 0
/**
* @brief        Callback registered by the client to get the callback for the geofence event status
*/
typedef void (*geofence_client_event_cb)(const char *sig, GVariant *param, void *user_data);
#endif

/**
* @brief	API to add the geofence. This will call the server API.
* @param[in]	geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]	place_id - place id for add geofence
* @param[in]	latitude - latitude value if is a geopoint geofence
* @param[in]	longitude - longitude value if it is a geopint geofence
* @param[in]	radius - radius value if it is a geopoint geofence
* @param[in]	bssid - bssid value if it is a wifi/BT geofence
* @param[out] error_code - Error that has occured on the server side
* @return	int
* @retval	returns the fence-id of the geofence added
* @see geo_client_delete_geofence
*/
int geo_client_add_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, gint geofence_type, gdouble latitude, gdouble longitude, gint radius, const gchar *address, const gchar *bssid, const gchar *ssid, gint *error_code);

/**
* @brief	API to add the place. This will call the server API
* @param[in]	geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]	place_name - Name of the place
* @param[out] error_code - Error that has occured on the server side
* retval	returns the place_id of the place added
* @see geo_client_delete_place
*/
int geo_client_add_place(geofence_client_dbus_h geofence_client, gchar *app_id, const gchar *place_name, gint *error_code);

/**
* @brief        API to update the place. This will call the server API
* @param[in]    geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]    place_id - Unique id of the place
* @param[in]	place_name - Name of the place[may be a new value]
* @return       int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see  geo_client_add_place
*/
int geo_client_update_place(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, const gchar *place_name);

/**
* @brief	API to delete the geofence from the server
* @param[in]	geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]	fence-id - Fence ID of the fence which needs to be deleted
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see	geo_client_add_geofence
*/
int geo_client_delete_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint fence_id);

/**
* @brief        API to delete the place from the server
* @param[in]    geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]    place_id - Place ID of the place which needs to be deleted
* @return       int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see  geo_client_add_place
*/
int geo_client_delete_place(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id);

/**
* @brief	API to get the place name
* @param[in]	geofence_client - Client dbus handle
* @param[in]	place_id - Place ID of the place whose name should be retrieved
* @param[out]	place_name - Name of the place
* @param[out]	error_code - Error that has occured on the server side
* @return	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
*/
int geo_client_get_place_name(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, gchar **place_name, gint *error_code);

/**
* @brief	API to get the fence list
* @param[in]	geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]	place_id - ID of the place whose list has to be retrived
* @param[out]	iter - iterator for the array of fences
* @param[out]	fenceCnt - total count of the fence
* @param[out]	errorCode - error code for the api
* @return	int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
*/
int geo_client_get_geofences(geofence_client_dbus_h geofence_client, gchar *app_id, gint place_id, GVariantIter **iter, gint *fence_cnt, gint *error_code);

/**
* @brief        API to get the place list
* @param[in]    geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[out]   iter - iterator for the array of fences
* @param[out]   placeCnt - total count of the fence
* @param[out]	errorCode - error code for the API
* @return       int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
*/
int geo_client_get_places(geofence_client_dbus_h geofence_client, gchar *app_id, GVariantIter **iter, gint *fence_cnt, int *error_code);

/**
* @brief	API to enable the geofence. This will call the server API
* @param[in]	geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]	fence_id - Unique id of the fence
* @param[in]	bEnable - enable flag for geofence
*/
int geo_client_enable_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint geofence_id, gboolean onoff);

/**
* @brief	API to start the geofence service on a particular geofence
* @param[in]	app_id - Caller's application ID
* @param[in]	geofence_client - Client dbus handle
* @param[in]	fence-id - Fence ID of the fence for which geofence service should be started
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see	geo_client_stop_geofence
*/
int geo_client_start_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint fence_id);

/**
* @brief	API to stop the geofence service on a particular geofence
* @param[in]	geofence_client - Client dbus handle
* @param[in]	app_id - Caller's application ID
* @param[in]	fence-id - Fence ID of the fence for which geofence service should be stopped
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see	geo_client_start_geofence
*/
int geo_client_stop_geofence(geofence_client_dbus_h geofence_client, gchar *app_id, gint fence_id);

/**
* @brief	API to start the client and make a subscription to the server
* @param[in]	geofence_client - Client dbus handle
* @param[in]	callback - callback registered by the client
* @param[in]	user_data - Data that has to come back in the callback
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE - if success
		GEOFENCE_CLIENT_ACCESS_DENIED - if failure
		GEOFENCE_CLIENT_ERROR_DBUS_CALL - if failure
* @see	geo_client_stop
*/
int geo_client_start(geofence_client_dbus_h geofence_client, geofence_client_cb callback, void *user_data);

/**
* @brief	API to stop the client and remove the subscription to the server
* @param[in]	geofence_client - Client dbus handle
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE - if success
		GEOFENCE_CLIENT_ACCESS_DENIED - if failure
		GEOFENCE_CLIENT_ERROR_DBUS_CALL - if failure
* @see	geo_client_start
*/
int geo_client_stop(geofence_client_dbus_h geofence_client);

/**
* @brief	API to create the connection with the server
* @param[in]	geofence_client - Client dbus handle
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE - if success
		GEOFENCE_CLIENT_ERROR_CONNECTION - if failure
* @see	geo_client_destroy
*/
int geo_client_create(geofence_client_dbus_h *geofence_client);

/**
* @brief	API to destroy the connection with the server
* @param[in]	geofence_client - Client dbus handle
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE - if success
		GEOFENCE_CLIENT_ERROR_CONNECTION - if failure
* @see	geo_client_create
*/
int geo_client_destroy(geofence_client_dbus_h geofence_client);

__END_DECLS
#endif /* __GEOFENCE_CLIENT_H__ */
