#ifndef DM_WS_DISPATCHERS_HPP
#define DM_WS_DISPATCHERS_HPP
#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/SharedDefs.hpp>
const std::string k_ws_count = "plugin:dm-ws:count";
void map_workspaces_to_monitors();
void refresh_mapping(void *, SCallbackInfo &, std::any);
void clear_mapping();
void dm_workspace(std::string workspace);
void dm_move_to_workspace(std::string workspace);
void dm_move_to_workspace_silent(std::string workspace);
#endif
