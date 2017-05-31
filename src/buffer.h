#ifndef VINCURSES_BUFFER_H
#define VINCURSES_BUFFER_H

#include <map>
#include <vector>
#include <iostream>
#include <sstream>

namespace ViNCurses{
    class Window;

    class Buffer{
        unsigned int _cursor_row;
        unsigned int _cursor_col;
        unsigned int _cursor_attr;
        std::string _cursor_string;
        
        class buf: public std::stringbuf{
        private:
            Buffer* _owner;
        public:
            buf(Buffer* owner);

            int sync();
        };

        buf _buf;
        std::ostream _buf_stream;
        std::map<std::ostream*,std::streambuf*> _streaming;

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
        void cursor(unsigned int row, unsigned int col, unsigned int attr=0);
        std::ostream& operator()();
        void stream(std::ostream* os); 

        void free_stream(std::ostream* os=0); //nullptr means free all streams
        void clear();
        void flush();
        
        // Shortcut controls
        std::ostream& operator()(unsigned int row, unsigned int col, unsigned int attr=0);
        void stream(std::ostream* os, unsigned int row, unsigned int col, unsigned int attr=0);


        // Getters
        unsigned int width() const;
        unsigned int height() const;
        std::vector<Element*> content() const;
    };
}

#endif //VINCURSES_BUFFER_H
