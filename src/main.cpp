#include "dm_ws_dispatchers.hpp"
#include "globals.hpp"
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
APICALL EXPORT std::string PLUGIN_API_VERSION() { return HYPRLAND_API_VERSION; }
#pragma clang diagnostic pop

static HOOK_CALLBACK_FN *e_monitorAddedHandle = nullptr;
static HOOK_CALLBACK_FN *e_monitorRemovedHandle = nullptr;

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wreturn-type-c-linkage"
APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;

  HyprlandAPI::addConfigValue(PHANDLE, k_ws_count, SConfigValue{.intValue = 5});

  HyprlandAPI::addDispatcher(PHANDLE, "dm-workspace", dm_workspace);
  HyprlandAPI::addDispatcher(PHANDLE, "dm-movetoworkspace",
                             dm_move_to_workspace);
  HyprlandAPI::addDispatcher(PHANDLE, "dm-movetoworkspacesilent",
                             dm_move_to_workspace_silent);

  HyprlandAPI::reloadConfig();

  map_workspaces_to_monitors();

  HyprlandAPI::addNotification(PHANDLE, "[dm-ws] Initialized successfully!",
                               catppuccin_mocha_mauve, 5000);

  e_monitorAddedHandle = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "monitorAdded", refresh_mapping);
  e_monitorRemovedHandle = HyprlandAPI::registerCallbackDynamic(
      PHANDLE, "monitorRemoved", refresh_mapping);

  return {"HyprlandDualMonitorWorkspaces",
          "A plugin that provides quality of "
          "life features for paired "
          "workspaces on dual monitor setups."};
}
#pragma clang diagnostic pop
APICALL EXPORT void PLUGIN_EXIT() {
  HyprlandAPI::addNotification(PHANDLE, "[dm-ws] Unloaded successfully!",
                               catppuccin_mocha_mauve, 5000);
  clear_mapping();
}
