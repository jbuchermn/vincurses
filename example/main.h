#include <vincurses.h>



class ExampleWindow: public ViNCurses::Window{
protected:
    void render(){
        buffer.cursor(0,0,VIN_HIGHLIGHT);
        buffer()<<"Test1"<<std::endl;
        buffer(10,10)<<"Halloooo";
       
        buffer.stream(&std::cout, 10,0, VIN_SELECTION);
        std::cout<<"abcd"<<std::endl<<"efghasd";
        buffer.free_stream();
    }

public:
    ExampleWindow(){}
};


class ExampleApp: public ViNCurses::App{
    ExampleWindow* _main_window;
    ExampleWindow* _left_window;
    ExampleWindow* _bottom_window;
protected:
    void init_windows(){
        _main_window = new ExampleWindow();
        _left_window = new ExampleWindow();
        _bottom_window = new ExampleWindow();

        add_window(_main_window);
    }

    bool command(std::string cmd, bool before_window){

        if(cmd=="-"){
            std::cout<<"DEBUG MESSAGE: LEFT WINDOW"<<std::endl;
            if(_left_window->assigned()) remove_window(_left_window);
            else add_window(_left_window, _main_window->box()->split('H', 0.2));
            
            return true;
        }

        if(cmd=="_"){
            std::cout<<"DEBUG MESSAGE: BOTTOM WINDOW"<<std::endl;
            if(_bottom_window->assigned()) remove_window(_bottom_window);
            else add_window(_bottom_window, _main_window->box()->split('J', 0.2));
        
            return true;
        }

        return false;
    }
};
