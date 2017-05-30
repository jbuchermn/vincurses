#ifndef VINCURSES_BUFFER_H
#define VINCURSES_BUFFER_H

#include <vector>
#include <iostream>
#include <sstream>

namespace ViNCurses{
    class Window;

    class Buffer: public std::stringstream{
        unsigned int _buffer_row;
        unsigned int _buffer_col;
        unsigned int _buffer_attr;

        std::streambuf* _coutbuf;

    public:
        class Element{
            std::string _content;
            unsigned int _row;
            unsigned int _col;
            unsigned int _attr;

        public:
            Element(std::string content, unsigned int row, unsigned int col, unsigned int attr);
            ~Element();

            unsigned int row() const;
            unsigned int col() const;
            unsigned int attr() const;
            std::string content() const;
        };
    private:
        std::vector<Element*> _content;
    public:
        Buffer();
        ~Buffer();

        // Controls
        void buffer_cout(  unsigned int row, unsigned int col, unsigned int attr=0);
        Buffer& operator()(unsigned int row, unsigned int col, unsigned int attr=0);
        void clear_buffer();
        void flush_buffer();

        // Getters
        unsigned int width() const;
        unsigned int height() const;
        std::vector<Element*> content() const;
    };
}

#endif //VINCURSES_BUFFER_H
