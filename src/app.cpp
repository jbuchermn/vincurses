#include <ncurses.h>

#include "app.h"
#include "window.h"
#include "box.h"

#include "stdout_window.h"

using namespace ViNCurses;

App::App(): 
    _w_input(0), 
    _w_working(0), 
    _w_status(0), 
    _main_height(0),
    _main_width(0),
    _active_index(0),
    _w_stdout(new StdoutWindow()){

}


App::~App(){
    for(unsigned int i=0; i<_windows.size(); i++) delete _windows[i];
}

bool App::command(std::string cmd, bool before_window){ return false; }

void App::run(){
    // Reroute std::cout and std::cerr
    _w_stdout->start();

    // Setup and start ncurses
    initscr();
    getmaxyx(stdscr, _main_height, _main_width);
    _main_height-=2;
    
    // Setup colors
    start_color();
    init_pair(VIN_ACTIVE_WINDOW_BORDER_, COLOR_CYAN, COLOR_BLACK   );
    init_pair(VIN_SELECTION_,            COLOR_BLACK, COLOR_CYAN   );
    init_pair(VIN_HIGHLIGHT_,            COLOR_BLACK, COLOR_MAGENTA);

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
     
    // Call abstract init procedure
    init_windows();

    // Initial select and refresh
    set_active_window(1);
    refresh();

    while(true){ 
        // Read in command
        std::string cmd = input();
        status("cmd: "+cmd);

        // Process command
        bool processed = false;
        
        if(cmd==":q"){ // Quit
            break;
        }else if(cmd.length()==1 and cmd[0]==23){ // <C-w> to switch, resize or move active window
            std::string c = input();
            if(c.length()!=1){
                status("Usage: <C-w><hjklHJKL+->");
            }else{
                char cmd = c[0];
                if(cmd=='H' or cmd=='J' or cmd=='K' or cmd=='L'){
                    // Move
                    if(_active_index>0 and _active_index<=_windows.size()){
                        _windows[_active_index-1]->box()->move(cmd);
                        setup_windows();
                    }
                }else if(cmd=='h' or cmd=='j' or cmd=='k' or cmd=='l'){
                    // Switch active
                    if(_active_index>0 and _active_index<=_windows.size()){
                        Box* b = _windows[_active_index-1]->box()->find_leaf_in_dir(cmd);
                        for(unsigned int i=0; i<_windows.size(); i++){
                            if(_windows[i]->box()==b){
                                set_active_window(i+1);
                            }
                        }
                    }
                }else if(cmd=='+' or cmd=='-' or cmd=='0'){
                    // Resize box
                    if(_active_index>0 and _active_index<=_windows.size()){
                        _windows[_active_index-1]->box()->resize(cmd);
                        setup_windows();
                    }
                }
            }

            processed=true;
        }else if(cmd==":stdout"){
            if(_w_stdout->assigned()){
                remove_window(_w_stdout);
            }else{
                if(_windows.size()>0){
                    add_window(_w_stdout, _windows[0]->box()->root()->split('J',0.2));
                }else{
                    add_window(_w_stdout);
                }
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
    
    // Free std::cout and std::cerr
    _w_stdout->stop();
}

void App::refresh(){
    for(unsigned int i=0; i<_windows.size(); i++) _windows[i]->refresh();
}

void App::set_active_window(int index){
    int i1 = _active_index;

    _active_index = index;
    if(_active_index<1 or _active_index>_windows.size()) _active_index=1;

    int i2 = _active_index;

    if(i1!=i2){
        if(i1>0 and i1<=_windows.size()) _windows[i1-1]->on_active_change();
        if(i2>0 and i2<=_windows.size()) _windows[i2-1]->on_active_change();
    }
}

void App::setup_windows(){
    for(unsigned int i=0; i<_windows.size(); i++) _windows[i]->setup();
}

bool App::active(const Window* window) const{
    for(unsigned int i=0; i<_windows.size(); i++){
        if(_windows[i]==window){
            return (i+1 == _active_index);
        }
    }

    return false;
}


void App::add_window(Window* window, Box* box){
    if(_windows.size()==0){
        box = new Box(_main_height, _main_width);
    }else if(box==0) return; // If not root, box needs to be given

    window->assign(this, box);
    _windows.push_back(window);

    setup_windows();
    set_active_window(_windows.size());
    refresh();
}


void App::remove_window(Window* window){
    for(unsigned int i=0; i<_windows.size(); i++){
        if(_windows[i]==window){
            _windows.erase(_windows.begin()+i);
            break;
        }
    }

    window->clear();

    Box* rem = window->box();
    window->assign(0,0);

    if(_windows.size()==0){
        delete rem;
    }else{
        _windows[0]->box()->root()->remove(rem);
    }

    setup_windows();
    set_active_window(_active_index);
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
