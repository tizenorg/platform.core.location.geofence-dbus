CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(geofence-dbus-server C)

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR} ${CMAKE_CURRENT_SOURCE_DIR}/include ${CMAKE_CURRENT_SOURCE_DIR}/cmake_tmp ${CMAKE_BINARY_DIR})

### Build ###
SET(SRCS_S
	src/geofence_dbus_server.c
	${CMAKE_CURRENT_SOURCE_DIR}/generated-code.c
)
MESSAGE(Server_CMAKE_BINARY_DIR: ${CMAKE_BINARY_DIR})
MESSAGE(CMAKE_CURRENT_SOURCE_DIR: ${CMAKE_CURRENT_SOURCE_DIR})
MESSAGE(CMAKE_SOURCE_DIR: ${CMAKE_SOURCE_DIR})

ADD_CUSTOM_COMMAND(
	OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/generated-code.c
	COMMAND gdbus-codegen --interface-prefix org.tizen.lbs. --generate-c-code generated-code --c-generate-object-manager --c-namespace SLoc --generate-docbook generated-docs ${CMAKE_SOURCE_DIR}/introspection/geofence.xml
	COMMENT "Generating GDBus .c/.h")

ADD_LIBRARY(geofence-dbus-server SHARED ${SRCS_S})
TARGET_LINK_LIBRARIES(geofence-dbus-server ${pkgs_LDFLAGS})
SET_TARGET_PROPERTIES(geofence-dbus-server PROPERTIES VERSION 0.1.0 SOVERSION 0 OUTPUT_NAME geofence-dbus-server)

INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/res/geofence-dbus-server.conf DESTINATION ${PREFIX}/etc/dbus-1/system.d)
INSTALL(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/geofence_dbus_server.h DESTINATION include/geofence-dbus)
INSTALL(TARGETS geofence-dbus-server DESTINATION ${LIBDIR} COMPONENT Runtime)
#ADD_SUBDIRECTORY(test_src)
