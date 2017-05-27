#include <ncurses.h>

#include "app.h"
#include "window.h"
#include "box.h"


using namespace ViNCurses;

App::App(): 
    _w_input(0), 
    _w_working(0), 
    _w_status(0), 
    _main_height(0),
    _main_width(0),
    _active_index(0){

}


App::~App(){
    for(unsigned int i=0; i<_windows.size(); i++) delete _windows[i];
}

bool App::command(std::string cmd, bool before_window){ return false; }

void App::run(){
    
    // Setup and start ncurses
    initscr();
    getmaxyx(stdscr, _main_height, _main_width);
    _main_height-=2;
    
    // Setup colors
    start_color();
    init_pair(VIN_ACTIVE_WINDOW_BORDER_, COLOR_CYAN, COLOR_BLACK);
    init_pair(VIN_SELECTION_,            COLOR_BLACK, COLOR_CYAN);

    // Setup base windows
    _w_input =   newwin(1, _main_width,   _main_height,   0             );
    _w_status =  newwin(1, _main_width-1, _main_height+1, 0             );
    _w_working = newwin(1, 1,             _main_height+1, _main_width-1 );

    wrefresh(_w_input);
    wrefresh(_w_status);
    wrefresh(_w_working);

    // Configuration
    cbreak();
    curs_set(0);
    keypad(_w_input, TRUE); // Recognize arrow keys and CTRL
     
    // Setup windows
    setup_windows();

    // Initial select and refresh
    ensure_active_index();
    refresh();

    while(true){ 
        // Read in command
        std::string cmd = input();
        status("cmd: "+cmd);

        // Process command
        bool processed = false;
        
        if(cmd==":q"){ // Quit
            break;
        }else if(cmd=="w"){ // Switch active window
            std::string index = input();
            if(index.length()!=1){
                status("Usage: w<window index>");
            }else{
                _active_index = index[0]-'0';
                ensure_active_index();
            }

            processed=true;
        }

        working(true);
        
        if(not processed) processed = command(cmd, true);
        if(not processed and _active_index>0 and _active_index<=_windows.size()) processed = _windows[_active_index-1]->command(cmd);
        if(not processed) processed = command(cmd, false);

        refresh();
        working(false);

    }

    endwin();
}

void App::refresh(){
    for(unsigned int i=0; i<_windows.size(); i++) _windows[i]->refresh();
}

void App::ensure_active_index(){
    if(_active_index<1 or _active_index>_windows.size()) _active_index=1;
}

void App::rescale_windows(){
    std::vector<Box*> boxes;
    for(unsigned int i=0; i<_windows.size(); i++) boxes.push_back(&_windows[i]->box());
    Box::match(boxes, _main_height, _main_width);
    for(unsigned int i=0; i<_windows.size(); i++) _windows[i]->setup();
}

void App::index_active(const Window* window, int& index, bool& active) const{
    for(unsigned int i=0; i<_windows.size(); i++){
        if(_windows[i]==window){
            index  = i+1;
            active = (index==_active_index);
            return;
        }
    }
}


void App::add_window(Window* window){
    window->assign(this);
    _windows.push_back(window);

    rescale_windows();
    ensure_active_index();
    refresh();
}


void App::remove_window(Window* window){
    for(unsigned int i=0; i<_windows.size(); i++){
        if(_windows[i]==window){
            _windows.erase(_windows.begin()+i);
            break;
        }
    }

    delete window;

    rescale_windows();
    ensure_active_index();
    refresh();
}

void App::status(std::string status){
    werase(_w_status);
    wprintw(_w_status,status.c_str());
    wrefresh(_w_status);
}

void App::working(bool working){
    werase(_w_working);
    if(working){
        wattron(_w_working, A_BLINK);
        wprintw(_w_working, "*");
        wattroff(_w_working, A_BLINK);
    }
    wrefresh(_w_working);
}

std::string App::input(){
    std::string result;
    
    int c = wgetch(_w_input);
    if(c==':'){
        char cs[VIN_INPUT_MAX]; 
        wgetnstr(_w_input, cs, VIN_INPUT_MAX-1);

        std::string res(cs);
        size_t endpos = res.find_last_not_of(" \t");
        if(endpos != std::string::npos){
            res= res.substr(0, endpos+1 );
        }

        result=":"+res;
    }else if(c==KEY_LEFT){
        result="h";
    }else if(c==KEY_RIGHT){
        result="l";
    }else if(c==KEY_UP){
        result="k";
    }else if(c==KEY_DOWN){
        result="j";
    }else if(c<256){
        char cs[] = { (char)c , 0 };
        std::string res(cs);
        result=res;
    }else{
        status("Unknown key");
        result="";
    }
 
    werase(_w_input);
    wrefresh(_w_input);

    return result;
}
