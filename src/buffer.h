#ifndef BUFFER_H
#define BUFFER_H

#include <vector>
#include <iostream>
#include <sstream>

namespace ViNCurses{
    class Window;

    class Buffer{
        std::stringstream* _buffer;
        int _buffer_row;
        int _buffer_col;
        int _buffer_attr;

        std::streambuf* _coutbuf;

        class Element{
            std::string _content;
            int _row;
            int _col;
            int _attr;

        public:
            Element(std::string content, int row, int col, int attr);
            ~Element();

            void write(Window* window);
        };

        std::vector<Element*> _content;
    public:
        Buffer();
        ~Buffer();

        void clear();
        void buffer_cout(int row, int col, int attr=0);
        void finish_cout();
        std::ostream& operator()(int row, int col, int attr=0);
        void finish();

        void write(Window* window);
    };
}

#endif //BUFFER_H
