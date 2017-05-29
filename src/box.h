#ifndef VINCURSES_BOX_H
#define VINCURSES_BOX_H

#include <iostream>
#include <vector>


namespace ViNCurses{
    class App;

    class Box{
        friend class App; // Make constructor accessible from App

        // Stored in any node in the tree
        const unsigned int _root_height;
        const unsigned int _root_width;

        // Tree structure
        Box* _parent;
        Box* _first_child;
        Box* _second_child;

        bool _split_horizontal;
        double _first_child_percentage;

        // Root constructor
        Box(unsigned int root_height, unsigned int root_width);
        
        // Children constructor
        Box(Box* parent);
    
    public:
        // Mutation methods
        Box* split(char cmd, double new_box_percentage);
        void move(char cmd);
        Box* remove(Box* box);

        // Traversal
        Box* find_leaf_in_dir(char dir);

        // Getters
        Box* root();
        Box* parent() const;
        unsigned int row() const;
        unsigned int col() const;
        unsigned int width() const;
        unsigned int height() const;
    };
}


#endif // VINCURSES_BOX_H
