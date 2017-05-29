#ifndef VINCURSES_WINDOW_H
#define VINCURSES_WINDOW_H

#include <ncurses.h>
#include <string>

#include "buffer.h"

namespace ViNCurses{
    class App;
    class Box;

    class Window{
        // Managed externally
        App* _parent;
        Box* _box;
        
        // Managed by this class
        WINDOW* _window;
        
        Buffer _buffer;
        bool _stale;

        int _offset_row;
        int _offset_col;

        void print(Buffer::Element* element);
    protected:

        // Render 
        virtual void render(Buffer& buffer)=0;

    public:
        // Internal methods
        Window();
        virtual ~Window();
        void assign(App* parent, Box* box);
        void setup();
        void clear();
        void refresh();
        
        // Getter
        bool assigned() const;
        App* parent() const;
        Box* box() const;
        void offset(int& row, int& col) const;
        bool active() const;

        // Rendering control
        void stale();
        void set_offset(int row, int col);

        // Command flow
        virtual bool command(std::string cmd);
        virtual void on_active_change();
    };
}

#endif //VINCURSES_WINDOW_H
