#include <string>
#include <iostream>
#include <sstream>
#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>

#include "buffer.h"
#include "window.h"

using namespace ViNCurses;

static inline std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}

static inline std::string &rtrim(std::string &s) {
    s.erase(std::find_if(s.rbegin(), s.rend(),
            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
    return s;
}

static inline std::string &trim(std::string &s) {
    return ltrim(rtrim(s));
}

Buffer::Buffer():
    _buffer_row(0),
    _buffer_col(0),
    _buffer_attr(0),
    _coutbuf(0){

}

Buffer::~Buffer(){
    for(unsigned int i=0; i<_content.size(); i++) delete _content[i];
}

Buffer::Element::Element(std::string content, unsigned int row, unsigned int col, unsigned int attr):
    _content(content), 
    _row(row), 
    _col(col), 
    _attr(attr){

}

Buffer::Element::~Element(){}

unsigned int Buffer::Element::row() const{ return _row; }
unsigned int Buffer::Element::col() const{ return _col; }
unsigned int Buffer::Element::attr() const{ return _attr; }
std::string Buffer::Element::content() const { return _content; }


void Buffer::buffer_cout(unsigned int row, unsigned int col, unsigned int attr){
    if(_coutbuf==0){
        _coutbuf=std::cout.rdbuf();
        std::cout.rdbuf(rdbuf());
    }

    _buffer_row=row;
    _buffer_col=col;
    _buffer_attr=attr;
}

Buffer& Buffer::operator()(unsigned int row, unsigned int col, unsigned int attr){
    flush_buffer();

    _buffer_row=row;
    _buffer_col=col;
    _buffer_attr=attr;

    return *this;
}


void Buffer::flush_buffer(){
    if(_coutbuf!=0){
        std::cout.rdbuf(_coutbuf);
        _coutbuf=0;
    }
    
    std::string s;
    for(unsigned int i=0; std::getline(*this, s, '\n'); i++){
        trim(s);
        if(s!="") _content.push_back(new Element(s, _buffer_row+i, _buffer_col, _buffer_attr));
    }

    _buffer_row=0;
    _buffer_col=0;
    _buffer_attr=0;

    str("");
    clear();
}

unsigned int Buffer::width() const{
    unsigned int width=0;
    for(unsigned int i=0; i<_content.size(); i++){
        width = std::max(width, _content[i]->col()+(unsigned int)_content[i]->content().length());
    }

    return width;
}

unsigned int Buffer::height() const{
    unsigned int height=0;
    for(unsigned int i=0; i<_content.size(); i++){
        height = std::max(height, _content[i]->row());
    }

    return height;
}

std::vector<Buffer::Element*> Buffer::content() const{ return _content; }

void Buffer::clear_buffer(){
    flush_buffer();
    for(unsigned int i=0; i<_content.size(); i++) delete _content[i];
    _content.erase(_content.begin(), _content.begin() + _content.size());
}
