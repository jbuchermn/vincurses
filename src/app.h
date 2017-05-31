#ifndef VINCURSES_APP_H
#define VINCURSES_APP_H

#include <ncurses.h>

#include <string>
#include <vector>

#define VIN_INPUT_MAX              100

#define VIN_ACTIVE_WINDOW_BORDER_  1
#define VIN_SELECTION_             2
#define VIN_HIGHLIGHT_             3

#define VIN_ACTIVE_WINDOW_BORDER   COLOR_PAIR(VIN_ACTIVE_WINDOW_BORDER_)
#define VIN_SELECTION              COLOR_PAIR(VIN_SELECTION_           )
#define VIN_HIGHLIGHT              COLOR_PAIR(VIN_HIGHLIGHT_           )

namespace ViNCurses{
    class Window;
    class Box;
    class StdoutWindow;

    class App{
        WINDOW* _w_input;
        WINDOW* _w_status;
        WINDOW* _w_working;

        int _main_height;
        int _main_width;

        std::vector<ViNCurses::Window*> _windows;
        int _active_index;

        void refresh();
        void setup_windows();
        void set_active_window(int index);

        // Standard windows
        StdoutWindow* _w_stdout;
    protected:
        // Add and remove a window, box!=0 except for first window
        void add_window(Window* window, Box* box=0);
        void remove_window(Window* window);

        // Set up inital window configuration
        virtual void init_windows()=0;

        // Process command - called twice, once before and once after active window
        virtual bool command(std::string command, bool before_window);

    public:
        App();
        virtual ~App();
        
        // internal
        bool active(const Window* window) const;
        
        // Main function
        void run();

        // Set status, working or read input
        void status(std::string status);
        void working(bool working);
        std::string input();
    };
}

#endif //VINCURSES_APP_H
