# vincurses

Simple wrapper around ncurses to implement the most basic ViM like behaviour. That is
- creation of different windows by recursively splitting them,
- selecting an active window,
- moving and resizing windows.

Also a basic command loop is implemented, where a string (either one char or a string starting with ":") is read in and then
processed by the main application and the active window.
