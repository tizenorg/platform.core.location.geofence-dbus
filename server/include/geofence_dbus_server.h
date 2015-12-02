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
 * @file	geofence_dbus_server.h
 * @brief	This file contains the APIs and callback functions for the dbus server.
 */

#ifndef __GEOFENCE_DBUS_SERVER_H__
#define __GEOFENCE_DBUS_SERVER_H__

__BEGIN_DECLS
#include <gio/gio.h>

typedef enum {
    GEOFENCE_DBUS_SERVER_ERROR_NONE = 0x0,
    GEOFENCE_DBUS_SERVER_ERROR_UNKNOWN,
    GEOFENCE_DBUS_SERVER_ERROR_PARAMETER,
    GEOFENCE_DBUS_SERVER_ERROR_MEMORY,
    GEOFENCE_DBUS_SERVER_ERROR_CONNECTION,
    GEOFENCE_DBUS_SERVER_ERROR_STATUS,
    GEOFENCE_DBUS_SERVER_ERROR_DBUS_CALL,
    GEOFENCE_DBUS_SERVER_ERROR_NO_RESULT,
} geofence_dbus_server_error_e;


typedef void *geofence_dbus_server_h;


#if SUPPORT_MULTI_CLIENT
/**
* @brief	Callback for setting the options
*/
typedef void (*GeofenceDbusSetOptionsCB)(GVariant *options, gpointer userdata);
#endif

/**
* @brief	Callback for shutting down the Dbus
*/
typedef void (*GeofenceDbusShutdownCB)(gpointer userdata, gboolean shutdown_arr);


/* for geofence callbacks */

/**
* @brief	Callback for adding the fence
*/
typedef gint(*GeofenceAddGeofenceCB)(const gchar *app_id, gint place_id, gint geofence_type, gdouble latitude, gdouble longitude, gint radius, const gchar *address, const gchar *bssid, const gchar *ssid, gpointer userdata);

/**
* @brief	Callback for deleting the geofence
*/
typedef void (*GeofenceDeleteGeofenceCB)(gint fence_id, const gchar *app_id, gpointer userdata);

/**
* @brief	Callback for getting the fence list
*/
typedef GVariant *(*GeofenceGetGeofencesCB)(int place_id, const gchar *app_id, int *fence_cnt, int *error_code, gpointer userdata);

/**
* @brief	Callback for enabling the geofence
*/
typedef void (*GeofenceEnableGeofenceCB)(gint fence_id, const gchar *app_id, gboolean enable, gpointer userdata);

/**
* @brief	Callback for Starting the Geofence service
*/
typedef void (*GeofenceStartGeofenceCB)(gint fence_id, const gchar *app_id, gpointer userdata);

/**
* @brief	Callback for Stopping the Geofence service
*/
typedef void (*GeofenceStopGeofenceCB)(gint fence_id, const gchar *app_id, gpointer userdata);


/* for place callbacks */

/**
* @brief        Callback for adding the place
*/
typedef gint(*GeofenceAddPlaceCB)(const gchar *app_id, const gchar *place_name, gpointer userdata);

/**
* @brief        Callback for deleting the place
*/
typedef void (*GeofenceDeletePlaceCB)(gint place_id, const gchar *app_id, gpointer userdata);

/**
* @brief        Callback for updating the place
*/
typedef void (*GeofenceUpdatePlaceCB)(gint place_id, const gchar *app_id, const gchar *place_name, gpointer userdata);

/**
* @brief	Callback for getting the place name
*/
typedef void (*GeofenceGetPlaceNameCB)(int place_id, const gchar *app_id, char **place_name, int *error_code, gpointer userdata);

/**
* @brief        Callback for getting the place list
*/
typedef GVariant *(*GeofenceGetPlacesCB)(const gchar *app_id, int *fence_cnt, int *error_code, gpointer userdata);

/**
* @brief	API to send the geofence status to the application
* @param[in]	geofence_dbus_server - The dbus server
* @param[in]	app_id - The application to which the status needs to be sent
* @param[in]	fence_state - The state of the fence whether in/out
* @return	int
* @retval	GEOFENCE_DBUS_SERVER_ERROR_NONE
*/
int geofence_dbus_server_send_geofence_inout_changed(geofence_dbus_server_h geofence_dbus_server, const gchar *app_id, gint fence_id, gint access_type, gint fence_state);

int geofence_dbus_server_send_geofence_proximity_changed(geofence_dbus_server_h geofence_dbus_server, const gchar *app_id, gint fence_id, gint access_type, gint fence_proximity_state, gint provider);

int geofence_dbus_server_send_geofence_event_changed(geofence_dbus_server_h geofence_dbus_server, gint place_id, gint fence_id, gint access_type, const gchar *app_id, gint error, gint state);

typedef struct {
#if SUPPORT_MULTI_CLIENT
	/* size_t size; */
	GeofenceDbusSetOptionsCB set_options_cb;
#endif
	GeofenceDbusShutdownCB shutdown_cb;

	GeofenceAddGeofenceCB add_geofence_cb;
	GeofenceDeleteGeofenceCB delete_geofence_cb;
	GeofenceGetGeofencesCB get_geofences_cb;
	GeofenceEnableGeofenceCB enable_geofence_cb;
	GeofenceStartGeofenceCB start_geofence_cb;
	GeofenceStopGeofenceCB stop_geofence_cb;

	GeofenceAddPlaceCB add_place_cb;
	GeofenceUpdatePlaceCB update_place_cb;
	GeofenceDeletePlaceCB delete_place_cb;
	GeofenceGetPlaceNameCB get_place_name_cb;
	GeofenceGetPlacesCB get_places_cb;
} geofence_dbus_callback_s;


/**
* @brief	API to create the server
* @param[in]	service_name - Name of the service for which the server has to be created
* @param[in]	service_path - Path for the service
* @param[in]	geofence_dbus_server - Dbus server pointer
* @param[in]	geofence_dbus_callback - Callback for all dbus interface
* @param[in]	userdata - Userdata if any which will come back in the callbacks
* return	int
* retval	GEOFENCE_DBUS_SERVER_ERROR_NONE if success
		GEOFENCE_DBUS_SERVER_ERROR_MEMORY if failure
*/
int geofence_dbus_server_create(geofence_dbus_server_h *geofence_dbus_server, geofence_dbus_callback_s *geofence_dbus_callback, gpointer userdata);

/**
* @brief	API to destroy the dbus server which was created
* @param[in]	geofence_dbus_server - Handle to the dbus server which was created
* @return 	int
* @retval	GEOFENCE_DBUS_SERVER_ERROR_NONE if sucess
		GEOFENCE_DBUS_SERVER_ERROR_PARAMETER if failure
*/
int geofence_dbus_server_destroy(geofence_dbus_server_h geofence_dbus_server);

__END_DECLS
#endif /* __GEOFENCE_DBUS_SERVER_H__ */
