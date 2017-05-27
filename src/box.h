#ifndef BOX_H
#define BOX_H

#include <vector>

#define VIN_ALIGN_TL  1
#define VIN_ALIGN_T   2
#define VIN_ALIGN_TR  3
#define VIN_ALIGN_R   4
#define VIN_ALIGN_BR  5
#define VIN_ALIGN_B   6
#define VIN_ALIGN_BL  7
#define VIN_ALIGN_L   8

namespace ViNCurses{
    class Box{
        int _align;
        int _pref_width;
        int _pref_height;

        int _row;
        int _col;
        int _width;
        int _height;

    public:
        Box(int align, int pref_height, int pref_width);
    
        int row() const;
        int col() const;
        int width() const;
        int height() const;
    
        static void match(std::vector<Box*> boxes, int total_height, int total_width);
    };
}


#endif // BOX_H
