#include "dm_ws_dispatchers.hpp"
#include "globals.hpp"
#include <cstddef>
#include <cstdint>
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/config/ConfigManager.hpp>
#include <hyprland/src/helpers/Color.hpp>
#include <hyprland/src/helpers/Vector2D.hpp>
#include <hyprland/src/helpers/Workspace.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#include <map>
#include <string>
#include <vector>
std::map<uint64_t, typename std::vector<std::string>> monitor_workspace_map;
static const Vector2D parse_hyprctl_cursorpos(std::string input) {
  int split_point = input.find(",");
  return {static_cast<double>(std::stoi(input.substr(0, split_point))),
          static_cast<double>(
              std::stoi(input.substr(split_point, input.length())))};
}
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

  HyprlandAPI::addNotification(
      PHANDLE, HyprlandAPI::invokeHyprctlCommand("cursorpos", ""),
      catppuccin_mocha_mauve, 5000);
  monitor_workspace_map.clear();
  const int ws_idx = 1;
  const int num_monitors = g_pCompositor->m_vMonitors.size();
  for (int j = 0; j < num_monitors; j++) {

    const auto monitor = g_pCompositor->m_vMonitors.at(j);
    const int ws_count =
        g_pConfigManager->getConfigValuePtrSafe(k_ws_count)->intValue;
    for (int i = ws_idx; i <= ws_count; i++) {
      // i love off-by-one errors.
      std::string ws_name = std::to_string((j + 1) + num_monitors * (i - 1));
      monitor_workspace_map[monitor->ID].push_back(ws_name);

      // for whatever reason, this completely did not work.
      // HyprlandAPI::invokeHyprctlCommand("keyword", "workspace " + ws_name +
      //                                                  "," +
      //                                                  monitor->szName);

      CWorkspace *workspace = g_pCompositor->getWorkspaceByName(ws_name);
      if (workspace != nullptr) {
        g_pCompositor->moveWorkspaceToMonitor(workspace, monitor.get());
      }
    }
  }
  /* now unnecessary debug print :3
    for (auto monitor : g_pCompositor->m_vMonitors) {
    std::stringstream log_msg;
    log_msg << "[dm_ws] Mapped workspaces (";
    for (auto mapping : monitor_workspace_map[monitor->ID]) {
      log_msg << mapping << ",";
    }
    log_msg << ") to monitor " + monitor->szName;
    HyprlandAPI::addNotification(PHANDLE, log_msg.str(), catppuccin_mocha_mauve,
                                 5000);
  }*/
}
void refresh_mapping(void *, SCallbackInfo &, std::any) {
  map_workspaces_to_monitors();
}
void clear_mapping() { monitor_workspace_map.clear(); }
void dm_workspace(std::string workspace) {
  const int ws = std::stoi(workspace);
  const auto monitors = g_pCompositor->m_vMonitors;
  const CMonitor *active_monitor = g_pCompositor->getMonitorFromCursor();
  Vector2D cursor_coords = parse_hyprctl_cursorpos(
      HyprlandAPI::invokeHyprctlCommand("cursorpos", ""));
  int active_monitor_pos = 0;
  for (uint64_t i = 0; i < monitors.size(); i++) {
    if (monitors.at(i)->ID == active_monitor->ID) {
      active_monitor_pos = i;
      continue;
    }

    /*
       HyprlandAPI::addNotification(
        PHANDLE,
        "going to: " + std::to_string((ws - 1) * monitors.size() + (i + 1)),
        catppuccin_mocha_mauve, 5000);
    */
    HyprlandAPI::invokeHyprctlCommand(
        "dispatch",
        "workspace " + std::to_string((ws - 1) * monitors.size() + (i + 1)));
    // g_pCompositor->warpCursorTo(cursor_coords);
  }

  /*
    HyprlandAPI::addNotification(
      PHANDLE,
      "going to: " +
          std::to_string((ws - 1) * monitors.size() + (active_monitor_pos + 1)),
      catppuccin_mocha_mauve, 5000);
  */
  HyprlandAPI::invokeHyprctlCommand(
      "dispatch", "workspace " + std::to_string((ws - 1) * monitors.size() +
                                                (active_monitor_pos + 1)));
}
void dm_move_to_workspace(std::string workspace) {
  CMonitor *monitor = g_pCompositor->getMonitorFromCursor();
  int ws = std::stoi(workspace);
  HyprlandAPI::invokeHyprctlCommand(
      "dispatch",
      "movetoworkspace " + monitor_workspace_map[monitor->ID][ws - 1]);
}
void dm_move_to_workspace_silent(std::string workspace) {
  CMonitor *monitor = g_pCompositor->getMonitorFromCursor();
  int ws = std::stoi(workspace);
  HyprlandAPI::invokeHyprctlCommand(
      "dispatch",
      "movetoworkspacesilent " + monitor_workspace_map[monitor->ID][ws - 1]);
}
