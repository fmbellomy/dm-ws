#include "globals.hpp"
#include <cstddef>
#include <cstdint>
#include <dm_ws_dispatchers.hpp>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/Workspace.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <map>
#include <string>
#include <vector>
std::map<uint64_t, typename std::vector<std::string>> monitor_workspace_map;

const std::string &get_workspace_from_monitor(CMonitor *monitor,
                                              const std::string &workspace) {
  int ws_idx = 0;
  try {
    ws_idx = std::stoi(workspace) - 1;
  } catch (std::invalid_argument &) {
    Debug::log(WARN, "invalid workspace index: %s", workspace.c_str());
    return workspace;
  }

  if (ws_idx < 0) {
    return workspace;
  }
  if ((size_t)ws_idx >= monitor_workspace_map.size()) {
    return workspace;
  }
  return monitor_workspace_map[monitor->ID][ws_idx];
}
void map_workspaces_to_monitors() {
  monitor_workspace_map.clear();
  int ws_idx = 1;
  for (auto &monitor : g_pCompositor->m_vMonitors) {
    int ws_count =
        g_pConfigManager->getConfigValuePtrSafe(k_ws_count)->intValue;
    std::string log_msg = "[dm_ws] Mapping workspaces " +
                          std::to_string(ws_idx) + "-" +
                          std::to_string(ws_idx + ws_count - 1) +
                          " to monitor " + monitor->szName;
    HyprlandAPI::addNotification(PHANDLE, log_msg, catppuccin_mocha_mauve,
                                 5000);

    for (int i = ws_idx; i < ws_idx + ws_count; i++) {
      std::string ws_name = std::to_string(i);
      monitor_workspace_map[monitor->ID].push_back(ws_name);
      HyprlandAPI::invokeHyprctlCommand("keyword", "workspace " + ws_name +
                                                       "," + monitor->szName);
      CWorkspace *workspace = g_pCompositor->getWorkspaceByName(ws_name);

      if (workspace != nullptr) {
        g_pCompositor->moveWorkspaceToMonitor(workspace, monitor.get());
      }
      // prepare for the next set of workspaces
      ws_idx += ws_count;
    }
  }
}
void refresh_mapping(void *, SCallbackInfo &, std::any) {
  map_workspaces_to_monitors();
}
void clear_mapping() { monitor_workspace_map.clear(); }
void dm_workspace(std::string workspace) {
  std::vector<std::shared_ptr<CMonitor>> monitors = g_pCompositor->m_vMonitors;
  int ws_count = g_pConfigManager->getConfigValuePtrSafe(k_ws_count)->intValue;
  CMonitor *active_monitor = g_pCompositor->getMonitorFromCursor();
  int active_monitor_pos = 0;
  for (int i = 0; i < monitors.size(); i++) {
    if (monitors.at(i)->ID == active_monitor->ID) {
      active_monitor_pos = i;
      continue;
    }
    HyprlandAPI::invokeHyprctlCommand(
        "dispatch",
        "workspace " + std::to_string(std::stoi(workspace) + (i * ws_count)));
  }
  HyprlandAPI::invokeHyprctlCommand(
      "dispatch",
      "workspace " + std::to_string(std::stoi(workspace) +
                                    (active_monitor_pos * ws_count)));
}
void dm_move_to_workspace(std::string workspace) {
  CMonitor *monitor = g_pCompositor->getMonitorFromCursor();
  HyprlandAPI::invokeHyprctlCommand(
      "dispatch", "movetoworkspace " +
                      monitor_workspace_map[monitor->ID][std::stoi(workspace)]);
}
void dm_move_to_workspace_silent(std::string workspace) {
  CMonitor *monitor = g_pCompositor->getMonitorFromCursor();
  HyprlandAPI::invokeHyprctlCommand(
      "dispatch", "movetoworkspacesilent " +
                      monitor_workspace_map[monitor->ID][std::stoi(workspace)]);
}
