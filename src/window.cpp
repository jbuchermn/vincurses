#include "window.h"

#include <ncurses.h>

#include "app.h"
#include "buffer.h"
#include "box.h"

using namespace ViNCurses;

Window::Window(Box box): 
    _window(0),
    _parent(0),
    _box(box){}

Window::~Window(){
    clear();
    if(_window!=0) delwin(_window);
}

void Window::clear(){
    if(_window!=0){
        werase(_window);
        wrefresh(_window);
    }
}

void Window::print(int row, int col, int attr, std::string text){
    if(_window==0) return;
    if(attr!=0) wattron(_window, attr);
    mvwprintw(_window, row, col, text.c_str());
    if(attr!=0) wattroff(_window, attr);
}

void Window::assign(App* parent){
    _parent=parent;
}

Box& Window::box() { return _box; }

void Window::setup(){
    clear();
    if(_window!=0) delwin(_window);

    _window = newwin(_box.height(), _box.width(), _box.row(), _box.col());
}

void Window::refresh(){
    int index;
    bool active;
    _parent->index_active(this, index, active);

    // TODO: Stale or sth like that
    render(_buffer);

    // Erase
    werase(_window);

    // Write content
    _buffer.write(this);
   
    // Write border
    if(active) wattron(_window, VIN_ACTIVE_WINDOW_BORDER);
    wborder(_window,'|','|','-','-','0'+index,' ',' ',' ');
    if(active) wattroff(_window, VIN_ACTIVE_WINDOW_BORDER);
    
    // Refresh
    wrefresh(_window);
}

bool Window::command(std::string command){
    return false;
}
