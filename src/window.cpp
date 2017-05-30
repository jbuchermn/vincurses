#include "window.h"

#include <ncurses.h>

#include "app.h"
#include "buffer.h"
#include "box.h"

using namespace ViNCurses;

Window::Window(): 
    _window(0),
    _parent(0),
    _box(0),
    _stale(true),
    _offset_row(0),
    _offset_col(0){

}

Window::~Window(){
    if(_window!=0) delwin(_window);
}

void Window::clear(){
    if(_window!=0){
        werase(_window);
        wrefresh(_window);
    }
}


void Window::print(Buffer::Element* element){
    if(_window==0) return;
    
    int height, width;
    getmaxyx(_window, height, width);
    height-=2; // Account for borders
    width-=2;
    if(width<=0 or height<=0) return;

    int y = _offset_row + element->row();
    int x = _offset_col + element->col();

    if(y<0 or y>=height) return;

    std::string c = element->content();
    if(x<0){
        if(-x>c.length()) return;
        c=c.substr(-x);
        x=0;
    }
    
    if(x+(int)c.length() >= width){
        c = c.substr(0, std::max(0, width-x));
    }

    if(element->attr()!=0) wattron(_window, element->attr());
    mvwprintw(_window, y+1, x+1, c.c_str()); // Account for borders
    if(element->attr()!=0) wattroff(_window, element->attr());
}

bool Window::assigned() const{ return _parent!=0 and _box!=0; }
App* Window::parent() const{ return _parent; }
Box* Window::box() const { return _box; }
bool Window::active() const { 
    bool active;
    int index;
    _parent->index_active(this, index, active);
    return active;
}
void Window::offset(int& row, int& col) const{ row=_offset_row; col=_offset_col; }


void Window::assign(App* parent, Box* box){
    _parent=parent;
    _box=box;

    stale();
}

void Window::setup(){
    clear();
    if(_window!=0) delwin(_window);

    _window = newwin(_box->height(), _box->width(), _box->row(), _box->col()); 
}

void Window::refresh(){
    int index;
    bool active;
    _parent->index_active(this, index, active);

    // Needs rerendering?
    if(_stale){
        _buffer.clear_buffer();
        render(_buffer);
        _buffer.flush_buffer();
        _stale=false;
    }

    // Erase
    werase(_window);

    // Write content
    std::vector<Buffer::Element*> content = _buffer.content();
    for(unsigned int i=0; i<content.size(); i++) print(content[i]);
   
    // Write border
    if(active) wattron(_window, VIN_ACTIVE_WINDOW_BORDER);
    wborder(_window,'|','|','-','-','0'+index,' ',' ',' ');
    if(active) wattroff(_window, VIN_ACTIVE_WINDOW_BORDER);
    
    // Refresh
    wrefresh(_window);
}

void Window::stale(){ _stale=true; }
void Window::set_offset(int row, int col){
    _offset_row=row;
    _offset_col=col;
}

bool Window::command(std::string command){
    return false;
}

void Window::on_active_change(){}
