# ~~~
# Summary:      Local, generic plugin setup
# Copyright (c) 2020-2021 Mike Rossiter
# License:      GPLv3+
# ~~~

# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3 of the License, or
# (at your option) any later version.


# -------- Options ----------

set(OCPN_TEST_REPO
    "opencpn/demo-alpha"
    CACHE STRING "Default repository for untagged builds"
)
set(OCPN_BETA_REPO
    "opencpn/demo-beta"
    CACHE STRING
    "Default repository for tagged builds matching 'beta'"
)
set(OCPN_RELEASE_REPO
    "opencpn/demo-prod"
    CACHE STRING
    "Default repository for tagged builds not matching 'beta'"
)

#
#
# -------  Plugin setup --------
#
set(PKG_NAME demo_pi) #or _pi
set(PKG_VERSION  1.0)
set(PKG_PRERELEASE "")  # Empty, or a tag like 'beta'

set(DISPLAY_NAME Demo)    # Dialogs, installer artifacts, ...
set(PLUGIN_API_NAME Demo) # As of GetCommonName() in plugin API
set(PKG_SUMMARY "Demonstration plugin")
set(PKG_DESCRIPTION "Demonstration plugin, illustrates the use of the different OpenCPN Plugin API's ")
set(PKG_AUTHOR "Demo Author")
set(PKG_IS_OPEN_SOURCE "yes")
set(PKG_HOMEPAGE https://github.com/twocanplugin/demoplugin)
set(PKG_INFO_URL https://opencpn.org/OpenCPN/plugins/demoplugin.html)

include_directories(${CMAKE_SOURCE_DIR}/inc)


set(SRC
    ${CMAKE_SOURCE_DIR}/src/demo_plugin.cpp
    ${CMAKE_SOURCE_DIR}/src/demo_globals.cpp
    ${CMAKE_SOURCE_DIR}/src/demo_wizard.cpp
    ${CMAKE_SOURCE_DIR}/src/demo_toolbox.cpp
     ${CMAKE_SOURCE_DIR}/src/demo_settings.cpp
    ${CMAKE_SOURCE_DIR}/src/demo_ui_base.cpp
)
set (INC
    ${CMAKE_SOURCE_DIR}/inc/demo_plugin.h
    ${CMAKE_SOURCE_DIR}/inc/demo_globals.h
    ${CMAKE_SOURCE_DIR}/inc/demo_wizard.h
    ${CMAKE_SOURCE_DIR}/inc/demo_toolbox.h
    ${CMAKE_SOURCE_DIR}/inc/demo_settings.h
    ${CMAKE_SOURCE_DIR}/inc/demo_ui_base.h
)

set (SOURCE_FILES ${SRC} ${INC})

set(PKG_API_LIB api-20)  

macro(late_init)
  # Perform initialization after the PACKAGE_NAME library, compilers
  # and ocpn::api is available.
  if (APPLE)
    target_compile_definitions(${PACKAGE_NAME} PUBLIC OCPN_GHC_FILESYSTEM)
  endif ()
endmacro ()

macro(add_plugin_libraries)

    add_subdirectory(opencpn-libs/nmea0183)
    target_link_libraries(${PACKAGE_NAME} ocpn::nmea0183)

    #add_subdirectory(opencpn-libs/n2kparser)
    #target_link_libraries(${PACKAGE_NAME} ocpn::N2KParser)

    #add_subdirectory(opencpn-libs/wxJSON)
    #target_link_libraries(${PACKAGE_NAME} ocpn::wxjson)

endmacro ()
