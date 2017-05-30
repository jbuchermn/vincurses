#include "box.h"

#include <iostream>
#include <math.h>

using namespace ViNCurses;

Box::Box(unsigned int root_height, unsigned int root_width):
    _root_height(root_height),
    _root_width(root_width),
    _parent(0),
    _first_child(0),
    _second_child(0){

}

Box::Box(Box* parent):
    _root_height(parent->_root_height),
    _root_width(parent->_root_width),
    _parent(parent),
    _first_child(0),
    _second_child(0){

}

Box* Box::split(char cmd, double new_box_percentage){
    if(new_box_percentage>1. or new_box_percentage<0.) new_box_percentage=0.5;
    
    Box* new_parent = 0;
    if(_parent==0){
        new_parent = new Box(_root_height, _root_width);
    }else{
        new_parent = new Box(_parent);
        if(_parent->_first_child==this) _parent->_first_child=new_parent;
        else if(_parent->_second_child==this) _parent->_second_child=new_parent;
    }

    Box* new_box = new Box(new_parent);
    _parent=new_parent;
    
    if(cmd=='H' or cmd=='K'){
        _parent->_first_child = new_box;
        _parent->_second_child = this;
        _parent->_first_child_percentage = new_box_percentage;
    }else{
        _parent->_first_child = this;
        _parent->_second_child= new_box;
        _parent->_first_child_percentage = 1. - new_box_percentage;
    }

    _parent->_split_horizontal = (cmd=='H' or cmd=='L');

    return new_box;
}

void Box::move(char cmd){
    if(_parent==0) return;

    _parent->_split_horizontal = (cmd=='H' or cmd=='L');
    if((cmd=='H' or cmd=='K')!=(_parent->_first_child==this)){
        Box* tmp=_parent->_first_child;
        _parent->_first_child=_parent->_second_child;
        _parent->_second_child=tmp;
        _parent->_first_child_percentage=1.-_parent->_first_child_percentage;
    }
}

void Box::resize(char cmd){
    if(_parent==0) return;

    if(cmd=='0') _parent->_first_child_percentage=0.5;
    else if((_parent->_first_child==this)==(cmd=='+')){
        _parent->_first_child_percentage=std::min(0.9, _parent->_first_child_percentage+0.05);
    }else{
        _parent->_first_child_percentage=std::max(0.1, _parent->_first_child_percentage-0.05);
    }
}

Box* Box::find_leaf_in_dir(char dir){
    if(_parent==0) return 0;
    if((_parent->_split_horizontal)!=(dir=='h' or dir=='l')){
        return _parent->find_leaf_in_dir(dir);
    }

    if((_parent->_first_child==this)==(dir=='h' or dir=='k')){
        return _parent->find_leaf_in_dir(dir);
    }

    Box* current;
    if(_parent->_first_child==this) current=_parent->_second_child;
    else current=_parent->_first_child;

    while(current->_first_child!=0) current=current->_first_child;

    return current;
}

Box* Box::remove(Box* box){ 
    if(_first_child==box){
        _second_child->_parent=_parent;
        return _second_child;
    }
    
    if(_second_child==box){
        _first_child->_parent=_parent;
        return _first_child;
    }

    if(_first_child!=0){
        Box* tmp = _first_child->remove(box);
        if(tmp!=_first_child){
            delete _first_child;
            _first_child = tmp;
        }
    }

    if(_second_child!=0){
        Box* tmp = _second_child->remove(box);
        if(tmp!=_second_child){
            delete _second_child;
            _second_child = tmp;
        }
    }

    return this;
}

Box* Box::root(){
    if(_parent==0) return this;
    return _parent->root();
}

Box* Box::parent() const{ return _parent; }

unsigned int Box::row() const{
    if(_parent==0) return 0;
    unsigned int tmp = _parent->row();

    if(not _parent->_split_horizontal and _parent->_second_child==this){
        tmp+= round(double(_parent->height())*_parent->_first_child_percentage);
    }

    return tmp;
}

unsigned int Box::col() const{
    if(_parent==0) return 0;
    unsigned int tmp = _parent->col();
    if(_parent->_split_horizontal and _parent->_second_child==this){
        tmp+= round(double(_parent->width())*_parent->_first_child_percentage);
    }

    return tmp;
}

unsigned int Box::width() const{
    if(_parent==0) return _root_width;

    double p=_parent->_first_child_percentage;
    if(_parent->_second_child==this) p=1.-p;
    if(not _parent->_split_horizontal) p=1.;

    return round(double(_parent->width())*p);
}

unsigned int Box::height() const{
    if(_parent==0) return _root_height;

    double p=_parent->_first_child_percentage;
    if(_parent->_second_child==this) p=1.-p;
    if(_parent->_split_horizontal) p=1.;

    return round(double(_parent->height())*p);
}










