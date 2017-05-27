#include "box.h"

using namespace ViNCurses;

Box::Box(int _align, int pref_height, int pref_width):
    _align(_align),
    _pref_width(pref_width),
    _pref_height(pref_height){}

int Box::row() const{ return _row; }
int Box::col() const{ return _col; }
int Box::width() const{ return _width; }
int Box::height() const{ return _height; }


void Box::match(std::vector<Box*> boxes, int total_height, int total_width){
    for(unsigned int i=0; i<boxes.size(); i++){
        boxes[i]->_height = boxes[i]->_pref_height * total_height / 100;
        boxes[i]->_width  = boxes[i]->_pref_width  * total_width  / 100;

        if(boxes[i]->_align == VIN_ALIGN_TR or boxes[i]->_align == VIN_ALIGN_R or boxes[i]->_align == VIN_ALIGN_BR){
            boxes[i]->_col = total_width - boxes[i]->_width;
        }else{
            boxes[i]->_col = 0;
        }

        if(boxes[i]->_align == VIN_ALIGN_BL or boxes[i]->_align == VIN_ALIGN_B or boxes[i]->_align == VIN_ALIGN_BR){
            boxes[i]->_row = total_height - boxes[i]->_height;
        }else{
            boxes[i]->_row = 0;
        }
    }
}
