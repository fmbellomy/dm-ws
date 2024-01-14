#include <hyprland/src/plugins/PluginAPI.hpp>
inline HANDLE  PHANDLE = nullptr;
APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    return {"HyprlandDualMonitorWorkspaces", "A plugin that provides quality of life features for paired workspaces on dual monitor setups."};
}

APICALL EXPORT void PLUGIN_EXIT() {}
