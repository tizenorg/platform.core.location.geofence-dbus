CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(geofence-dbus-client C)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}/include)

### Build ###
SET(SRCS
	src/geofence_client.c
)

ADD_LIBRARY(geofence-dbus-client SHARED ${SRCS})
TARGET_LINK_LIBRARIES(geofence-dbus-client ${pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(geofence-dbus-client PROPERTIES VERSION 0.1.0 SOVERSION 0 OUTPUT_NAME geofence-dbus-client)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/geofence_client.h DESTINATION include/geofence-dbus)
INSTALL(TARGETS geofence-dbus-client DESTINATION ${LIBDIR} COMPONENT Runtime)
