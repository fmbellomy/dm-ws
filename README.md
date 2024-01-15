
# Installation

Don't.

I'm not joking, this plugin is only really usable with some hacks in your `hyprland.conf`

It's currently very hacked together and liable to crash
because I'm not a particularly great modern C++ user.
The codebase looks a lot like a C99 university assignment,
because I'm more familiar with programming in that style, bug prone as it may be.

## Do you really want to install this janky disaster?

If you *really* want to use this plugin, you can install it with hyprpm as follows.

```bash
hyprpm add https://github.com/fmbellomy/dm-ws
hyprpm enable dm-ws
```

### disclaimer for if you're silly enough to still want to use this

Be warned, though!
Before you start replacing all your usages of `workspace` with `dm-workspace`
in your config, know that this plugin expects a very specific mapping of workspaces
to monitors in order to work correctly, and *you* have to set it in your config.

If you had 3 monitors, DP-1, DP-2, and DP-3, and you wanted to have 5 workspaces,
you would need to include these lines in your config.

```conf
monitor=DP-1, ... # resolution, position, etc
monitor=DP-2, ...
monitor=DP-3, ...

# ...
# make sure that plugins are loaded when starting hyprland
exec-once hyprpm reload -n

# set number of workspaces
plugin:dm-ws:count = 5

# map hyprland's workspaces to monitors
workspace=1,monitor:DP-1
workspace=2,monitor:DP-2
workspace=3,monitor:DP-3
# from this point onward,
# monitors will repeat until you've done this 
# for as many dm-workspaces as you want to have.
workspace=4,monitor:DP-1
workspace=5,monitor=DP-2
workspace=6,monitor=DP-3

# ...
# example usage of this plugin's dispatchers

bind = $mainMod, 1, dm-workspace, 1
bind = $mainMod, 2, dm-workspace, 2
bind = $mainMod, 3, dm-workspace, 3
bind = $mainMod, 4, dm-workspace, 4
bind = $mainMod, 5, dm-workspace, 5
```

## Why is it so scuffed?

Originally, this plugin was meant to call
`HyprlandAPI::invokeHyprctlCommand("keyword", "workspace {workspace_number}, {monitor}")`
to assign workspaces to the correct monitors.
For some reason though, this command seems to not really do anything.
