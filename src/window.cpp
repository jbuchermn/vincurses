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
    _height(0),
    _width(0),
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
    if(_width<=0 or _height<=0) return;

    int y = _offset_row + element->row();
    int x = _offset_col + element->col();

    if(y<0 or y>=_height) return;

    std::string c = element->content();
    if(x<0){
        if(-x>c.length()) return;
        c=c.substr(-x);
        x=0;
    }
    
    if(x+(int)c.length() >= _width){
        c = c.substr(0, std::max(0, _width-x));
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
    getmaxyx(_window, _height, _width);

    // Borders
    _height-=2;
    _width-=2;
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

bool Window::command_move_buffer(std::string command){
    
    if(command=="h"){
        if(_offset_col<0){
            _offset_col++;
            return true;
        }
    }else if(command=="k"){
        if(_offset_row<0){
            _offset_row++;
            return true;
        }
    }else if(command=="j"){
        if(_buffer.height()+_offset_row>_height){
            _offset_row--;
            return true;
        }
    }else if(command=="l"){
        if(_buffer.width()+_offset_col>_width){
            _offset_col--;
            return true;
        }
    }

    return false;
}

bool Window::command(std::string command){
    return false;
}

void Window::on_active_change(){}
