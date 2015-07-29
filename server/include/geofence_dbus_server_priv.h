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
 * @file	geofence_dbus_server_priv.h
 * @brief	This file contains the log tags for the geofence dbus server.
 */

#ifndef __GEOFENCE_DBUS_SERVER_PRIV_H__
#define __GEOFENCE_DBUS_SERVER_PRIV_H__

__BEGIN_DECLS
#ifdef FEATURE_DLOG_DEBUG
#include <dlog.h>
#ifdef LOG_TAG
#undef LOG_TAG
#define LOG_TAG "GEOFENCE_DBUS_SERVER"
#endif
#define GEOFENCE_DBUS_SERVER_LOGI(fmt, args...)  { LOGI(fmt, ##args); }
#define GEOFENCE_DBUS_SERVER_LOGD(fmt, args...)  { LOGD(fmt, ##args); }
#define GEOFENCE_DBUS_SERVER_LOGW(fmt, args...)  { LOGW(fmt, ##args); }
#define GEOFENCE_DBUS_SERVER_LOGE(fmt, args...)  { LOGE(fmt, ##args); }
#define GEOFENCE_DBUS_SERVER_SECLOG(fmt, args...)  { SECURE_LOGD(fmt, ##args); }
#else
#define GEOFENCE_DBUS_SERVER_LOGI(fmt, args...)
#define GEOFENCE_DBUS_SERVER_LOGD(fmt, args...)
#define GEOFENCE_DBUS_SERVER_LOGW(fmt, args...)
#define GEOFENCE_DBUS_SERVER_LOGE(fmt, args...)
#define GEOFENCE_DBUS_SERVER_SECLOG(fmt, args...)
#endif
__END_DECLS
#endif /*__GEOFENCE_DBUS_SERVER_PRIV_H__*/
