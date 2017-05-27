#include "buffer.h"
#include "window.h"

using namespace ViNCurses;

Buffer::Buffer():
    _buffer(new std::stringstream()),
    _buffer_row(0),
    _buffer_col(0),
    _buffer_attr(0),
    _coutbuf(0){

}

Buffer::~Buffer(){
    delete _buffer;
    for(unsigned int i=0; i<_content.size(); i++) delete _content[i];
}

Buffer::Element::Element(std::string content, int row, int col, int attr):
    _content(content), _row(row), _col(col), _attr(attr){

}

Buffer::Element::~Element(){}

void Buffer::Element::write(Window* window){
    window->print(_row, _col, _attr, _content);
}

void Buffer::buffer_cout(int row, int col, int attr){
    if(_coutbuf==0){
        _coutbuf=std::cout.rdbuf();
        std::cout.rdbuf(_buffer->rdbuf());
    }

    _buffer_row=row;
    _buffer_col=col;
    _buffer_attr=attr;
}

std::ostream& Buffer::operator()(int row, int col, int attr){
    finish();

    _buffer_row=row;
    _buffer_col=col;
    _buffer_attr=attr;

    return *_buffer;
}

void Buffer::finish_cout(){
    if(_coutbuf!=0){
        std::cout.rdbuf(_coutbuf);
        _coutbuf=0;
    }

    finish();
}

void Buffer::finish(){
    std::string str = _buffer->str();
    if(str!=""){
        _content.push_back(new Element(str, _buffer_row, _buffer_col, _buffer_attr));
    }

    _buffer_row=0;
    _buffer_col=0;
    _buffer_attr=0;

    _buffer->str("");
    _buffer->clear();
}

void Buffer::write(Window* window){
    for(unsigned int i=0; i<_content.size(); i++){
        _content[i]->write(window);
    }
}

void Buffer::clear(){
    finish();
    for(unsigned int i=0; i<_content.size(); i++) delete _content[i];
    _content.erase(_content.begin(), _content.begin() + _content.size());
}
