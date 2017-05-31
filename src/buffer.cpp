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

/*
 * Static helpers
 */
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

/*
 * Streambuf implementation
 */
Buffer::buf::buf(Buffer* owner): 
    _owner(owner){

}

int Buffer::buf::sync(){
    _owner->flush();
    return 0;
}

/*
 * Element POCPPO
 */
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

/*
 * Main class
 */
Buffer::Buffer():
    _cursor_row(0),
    _cursor_col(0),
    _cursor_attr(0),
    _cursor_string(""),
    _buf(this),
    _buf_stream(&_buf){

}

Buffer::~Buffer(){
    for(unsigned int i=0; i<_content.size(); i++) delete _content[i];
}

/*
 * Controls
 */ 
void Buffer::cursor(unsigned int row, unsigned int col, unsigned int attr){
    flush();

    _cursor_row = row;
    _cursor_col = col;
    _cursor_attr = attr;
}

std::ostream& Buffer::operator()(){
    return _buf_stream;
}

void Buffer::stream(std::ostream* os){
    if(_streaming.find(os)==_streaming.end()){
        _streaming[os]=os->rdbuf(&_buf);
    }
}


void Buffer::free_stream(std::ostream* os){
    if(os==0){
        for(std::map<std::ostream*,std::streambuf*>::iterator it=_streaming.begin(); it!=_streaming.end(); it++){
            it->first->rdbuf(it->second);
        }
        _streaming.clear();
    }else{
        std::map<std::ostream*,std::streambuf*>::iterator it = _streaming.find(os);
        if(it!=_streaming.end()){
            it->first->rdbuf(it->second);
            _streaming.erase(it);
        }
    }
}


void Buffer::flush(){

    std::string s;
    std::istream is(&_buf);

    while(std::getline(is, s, '\n')){
        trim(s);
        
        if(s!="") _content.push_back(new Element(s, _cursor_row, _cursor_col, _cursor_attr));
        _cursor_row++;
    }

    _buf.str("");
}

void Buffer::clear(){
    flush();
    for(unsigned int i=0; i<_content.size(); i++) delete _content[i];
    _content.erase(_content.begin(), _content.begin() + _content.size());
}


/*
 * Shortcut controls
 */
void Buffer::stream(std::ostream* os, unsigned int row, unsigned int col, unsigned int attr){
    cursor(row, col, attr);
    stream(os);
}

std::ostream& Buffer::operator()(unsigned int row, unsigned int col, unsigned int attr){
    cursor(row, col, attr);
    return operator()();
}

/*
 * Getters
 */
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

