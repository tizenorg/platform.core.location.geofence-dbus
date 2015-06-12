#!/bin/bash -x

gdbus-codegen --interface-prefix org.tizen.lbs. \
	--generate-c-code generated-code                        \
	--c-generate-object-manager                 \
	--c-namespace Geofence                        \
	--generate-docbook generated-docs                       \
	geofence.xml 
