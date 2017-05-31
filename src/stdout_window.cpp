#include "stdout_window.h"

using namespace ViNCurses;

StdoutWindow::StdoutWindow(): Window("stdout"){}

void StdoutWindow::render(){}

void StdoutWindow::start(){
    buffer.stream(&std::cout);
    buffer.stream(&std::cerr);
}

void StdoutWindow::stop(){
    buffer.free_stream();
}

bool StdoutWindow::command(std::string command){
    return command_move_buffer(command);
}
