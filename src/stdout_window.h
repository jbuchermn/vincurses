#ifndef STD_OUT_WINDOW_H
#define STD_OUT_WINDOW_H

#include <string>

#include "window.h"

namespace ViNCurses{
    class StdoutWindow: public Window{
    protected:
        void render();
    public:
        StdoutWindow();

        void start();
        void stop();

        bool command(std::string cmd);
    };
}

#endif //STD_OUT_WINDOW_H
