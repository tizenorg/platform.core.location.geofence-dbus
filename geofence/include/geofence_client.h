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
typedef enum {
	GEOFENCE_STATUS_CB = 0x01,
	GEOFENCE_INOUT_CB = GEOFENCE_STATUS_CB << 0x01,
} geofence_client_callback_e;

typedef enum {
	GEOFENCE_CLIENT_ERROR_NONE = 0x0,
	GEOFENCE_CLIENT_ERROR_UNKNOWN,
	GEOFENCE_CLIENT_ERROR_PARAMETER,
	GEOFENCE_CLIENT_ERROR_MEMORY,
	GEOFENCE_CLIENT_ERROR_CONNECTION,
	GEOFENCE_CLIENT_ERROR_STATUS,
	GEOFENCE_CLIENT_ERROR_DBUS_CALL,
	GEOFENCE_CLIENT_ERROR_NO_RESULT,
	GEOFENCE_CLIENT_ACCESS_DENIED
} geofence_client_error_e;

typedef void *geofence_client_dbus_h;

/**
* @brief	Callback registered by the client to get the callback for the geofence in/out status 	
*/
typedef void (*geofence_client_cb) (const char *sig, GVariant *param, void *user_data);

/**
* @brief        Callback registered by the client to get the callback for the geofence event status    
*/
typedef void (*geofence_client_event_cb) (const char *sig, GVariant *param, void *user_data);

/**
* @brief	API to add the geofence. This will call the server API.
* @param[in]	geofence_client - Client dbus handle
* @param[in]	latitude - latitude value if is a geopoint geofence
* @param[in]	longitude - longitude value if it is a geopint geofence
* @param[in]	radius - radius value if it is a geopoint geofence
* @param[in]	bssid - bssid value if it is a wifi/BT geofence
* @return	int
* @retval	returns the fence-id of the geofence added
* @see geo_client_delete_geofence
*/
int geo_client_add_geofence(geofence_client_dbus_h geofence_client, gint place_id, gint geofence_type, gdouble latitude, gdouble longitude, gint radius, const gchar *address, const gchar *bssid, const gchar *ssid);

/**
* @brief	API to add the place. This will call the server API
* @param[in]	geofence_client - Client dbus handle
* @param[in]	place_name - Name of the place
* retval	returns the place_id of the place added
* @see geo_client_delete_place
*/
int geo_client_add_place(geofence_client_dbus_h geofence_client, const gchar *place_name);

/**
* @brief	API to enable the geofence. This will call the server API
* @param[in]	geofence_client - Client dbus handle
* @param[in]	fence_id - Unique id of the fence
* @param[in]	bEnable - enable flag for geofence
*/
int geo_client_enable_service(geofence_client_dbus_h geofence_client, gint geofence_id, gboolean bEnable);

/**
* @brief        API to update the place. This will call the server API
* @param[in]    geofence_client - Client dbus handle
* @param[in]    place_id - Unique id of the place
* @param[in]	place_name - Name of the place[may be a new value]
* @return       int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see  geo_client_add_place
*/
int geo_client_update_place(geofence_client_dbus_h geofence_client, gint place_id, const gchar *place_name);

/**
* @brief	API to delete the geofence from the server
* @param[in]	geofence_client - Client dbus handle
* @param[in]	fence-id - Fence ID of the fence which needs to be deleted
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see	geo_client_add_geofence
*/
int geo_client_delete_geofence(geofence_client_dbus_h geofence_client, gint fence_id);

/**
* @brief        API to delete the place from the server
* @param[in]    geofence_client - Client dbus handle
* @param[in]    place_id - Place ID of the place which needs to be deleted
* @return       int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see  geo_client_add_place
*/
int geo_client_delete_place(geofence_client_dbus_h geofence_client, gint place_id);

/**
* @brief	API to get the place name
* @param[in]	geofence_client - Client dbus handle
* @param[in]	place_id - Place ID of the place whose name should be retrieved
* @param[out]	place_name - Name of the place
* @param[out]	error_code - Error that has occured on the server side
* @return	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
*/
int geo_client_get_place_name(geofence_client_dbus_h geofence_client, gint place_id, char **placeName, int *errorCode);

/**
* @brief	API to get the fence list
* @param[in]	geofence_client - Client dbus handle
* @param[in]	place_id - ID of the place whose list has to be retrived
* @param[out]	iter - iterator for the array of fences
* @param[out]	fenceCnt - total count of the fence
* @param[out]	errorCode - error code for the api
* @return	int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
*/
int geo_client_get_list(geofence_client_dbus_h geofence_client, int place_id, GVariantIter **iter, gint *fenceCnt, int *errorCode);

/**
* @brief        API to get the place list
* @param[in]    geofence_client - Client dbus handle
* @param[out]   iter - iterator for the array of fences
* @param[out]   placeCnt - total count of the fence
* @param[out]	errorCode - error code for the API
* @return       int
* @retval       GEOFENCE_CLIENT_ERROR_NONE if success
                GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
*/
int geo_client_get_place_list(geofence_client_dbus_h geofence_client, GVariantIter **iter, gint *fenceCnt, int *errorCode);

/**
* @brief	API to start the geofence service on a particular geofence
* @param[in]	geofence_client - Client dbus handle
* @param[in]	fence-id - Fence ID of the fence for which geofence service should be started
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see	geo_client_stop_geofence		
*/
int geo_client_start_geofence(geofence_client_dbus_h geofence_client, gint fence_id);

/**
* @brief	API to stop the geofence service on a particular geofence
* @param[in]	geofence_client - Client dbus handle
* @param[in]	fence-id - Fence ID of the fence for which geofence service should be stopped
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE if success
		GEOFENCE_CLIENT_ERROR_DBUS_CALL if failure
* @see	geo_client_start_geofence		
*/
int geo_client_stop_geofence(geofence_client_dbus_h geofence_client, gint fence_id);

/**
* @brief	API to start the client and make a subscription to the server
* @param[in]	service_name - Name of the service required by the client
* @param[in]	service_path - Path of the service for dbus connection
* @param[in]	geofence_client - Client dbus handle
* @param[in]	callback - callback registered by the client
* @param[in]	user_data - Data that has to come back in the callback
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE - if success
		GEOFENCE_CLIENT_ACCESS_DENIED - if failure
		GEOFENCE_CLIENT_ERROR_DBUS_CALL - if failure
* @see	geo_client_stop
*/
int geo_client_start(char *service_name, char *service_path, geofence_client_dbus_h *geofence_client, geofence_client_cb callback, geofence_client_event_cb evt_callback, void *user_data);

/**
* @brief	API to stop the client and remove the subscription to the server
* @param[in]	geofence_client - Client dbus handle
* @return	int
* @retval	GEOFENCE_CLIENT_ERROR_NONE - if success
		GEOFENCE_CLIENT_ACCESS_DENIED - if failure
		GEOFENCE_CLIENT_ERROR_DBUS_CALL - if failure
* @see	geo_client_start
*/
int geo_client_stop(geofence_client_dbus_h *geofence_client);

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
