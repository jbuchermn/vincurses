#ifndef VINCURSES_WINDOW_H
#define VINCURSES_WINDOW_H

#include <ncurses.h>
#include <string>

#include "buffer.h"

namespace ViNCurses{
    class App;
    class Box;

    class Window{
        std::string _title;

        // Managed externally
        App* _parent;
        Box* _box;
        
        // Managed by this class
        WINDOW* _window;
        int _height;
        int _width;

        bool _stale;

        int _offset_row;
        int _offset_col;

        void print(Buffer::Element* element);
    protected:
        // Preimplemented
        bool command_move_buffer(std::string command);

        // Render 
        Buffer buffer;
        virtual void render()=0;

    public:
        // Internal methods
        Window(std::string title);
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

        // Setter and rendering control
        // To handle rendering:
        //      -> Either call stale() upon change, then during refresh() - once a cycle - render() will be called.
        //         Don't forget to clean the buffer if using this method
        //      -> Alternatively just write to the buffer, whenever you like. Currently flush does not issue a refresh,
        //         but during refresh any flushed changes will be written
        void stale();
        void set_offset(int row, int col);
        void title(std::string title);

        // Command flow
        virtual bool command(std::string cmd);
        virtual void on_active_change();

    };
}

#endif //VINCURSES_WINDOW_H
