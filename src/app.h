#ifndef VINAPP_H
#define VINAPP_H

#include <ncurses.h>

#include <string>
#include <vector>

#define VIN_INPUT_MAX              100

#define VIN_ACTIVE_WINDOW_BORDER_  1
#define VIN_SELECTION_             2

#define VIN_ACTIVE_WINDOW_BORDER   COLOR_PAIR(VIN_ACTIVE_WINDOW_BORDER_)
#define VIN_SELECTION              COLOR_PAIR(VIN_SELECTION_           )

namespace ViNCurses{
    class Window;
    class Box;

    class App{
        WINDOW* _w_input;
        WINDOW* _w_status;
        WINDOW* _w_working;

        int _main_height;
        int _main_width;

        std::vector<ViNCurses::Window*> _windows;
        int _active_index;

        void refresh();
        void ensure_active_index();
        void rescale_windows();

    protected:
        void add_window(Window* window);
        void remove_window(Window* window);
        virtual void setup_windows()=0;
        virtual bool command(std::string command, bool before_window);

    public:
        App();
        virtual ~App();
        void run();

        void status(std::string status);
        void working(bool working);

        virtual std::string input();

        void index_active(const Window* window, int& index, bool& active) const;
    };
}

#endif //VINAPP_H