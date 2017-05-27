#ifndef WINDOW_H
#define WINDOW_H

#include <ncurses.h>
#include <string>

#include "buffer.h"
#include "box.h"

namespace ViNCurses{
    class App;

    class Window{
        App* _parent;
        Buffer _buffer;
        Box _box;
        WINDOW* _window;

    protected:
        App* parent();
        virtual void render(Buffer& buffer)=0;
    public:
        Window(Box box);
        virtual ~Window();
        void assign(App* parent);
        void setup();
        Box& box();

        void clear();
        void print(int row, int col, int attr, std::string text);
        virtual void refresh();
        virtual bool command(std::string cmd);
    };
}

#endif
