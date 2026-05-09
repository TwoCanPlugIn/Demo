Shipdriver template update to plugin API levels
===============================================

The shipdriver template supports using different Plugin API levels. This allows
developers to adopt the API level appropriate for their plugin feature set. The 
benefits of newer plugin API levels include:

  - Better version handling, plugin carries the complete version info. This
    avoids the needs for various hacks in main openpcn used for older API
    levels.

  - The new communication interface described in
    https://opencpn-manuals.github.io/main/opencpn-dev/plugin-messaging.html.
    This can handle NMEA2000 messages and also offers more performance
    and flexibility compared to the old interfaces.

  - Improved scalable interface support

  - Enhanced waypoint and route support.

The update process:
-------------------

1. Ensure that the template is updated to 3.3.0 or higher

2. In Plugin.cmake, change `set(PKG_API_LIB api-16)` to `the required API level`

3. Add the following snippet to config.h.in:
   ```
   #define PLUGIN_VERSION_TWEAK @PROJECT_VERSION_TWEAK@

   #define PKG_PRERELEASE "@PKG_PRERELEASE@"
   #define PKG_BUILD_INFO "@pkg_vers_build_info@"

   #define PLUGIN
   ```

4. In the main header file for the plugin (demo_plugin.h) find the snippet
   ```
   class DemoPlugin : public opencpn_plugin_118 {
   ```
   and update as required
   ```
   public opencpn_plugin_120
   ```

6. In the file which implements the constructor src/demo_plugin.cpp change
   ```
   DemoPlugin::DemoPlugin(void* ppimgr) : opencpn_plugin_118(ppimgr)
   ```
   to match
   ```
   opencpn_plugin_120(ppimgr)
   ```

8. Implement new methods, typically in the file which implements
   ```
   GetAPIVersionMajor()
   ```
   which can be found in src/demo_plugin.cpp
   ```
   int GetPlugInVersionPatch() { return PLUGIN_VERSION_PATCH; }
   int GetPlugInVersionPost() { return PLUGIN_VERSION_TWEAK; }
   const char *GetPlugInVersionPre() { return PKG_PRERELEASE; }
   const char *GetPlugInVersionBuild() { return PKG_BUILD_INFO; }
   ```
   This is boilerplate code, and can be used for all plugins based on this Shipdriver template. 

10. Build and test plugin. 
