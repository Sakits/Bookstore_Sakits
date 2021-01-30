#ifndef _book_hpp
#define _book_hpp

#include <iostream>
using namespace std;

class book
{
    private:
        char ISBN[70], name[70], author[70], keyword[70];
        int price, cnt;

    public:
        book()
        {
            cnt = price = 0;
            ISBN[0] = name[0] = author[0] = keyword[0] = '\0';
        }

        void init(const char* _ISBN)
        {
            cnt = price = 0;
            strcpy(ISBN, _ISBN);
            name[0] = author[0] = keyword[0] = '\0';
        }

        void modify(const char* _ISBN, const char* _name, const char* _author, const char* _keyword, int _price)
        {
            if (strcmp(_ISBN, "")) strcpy(ISBN, _ISBN);
            if (strcmp(_name, "")) strcpy(name, _name);
            if (strcmp(_author, "")) strcpy(name, _author);
            if (strcmp(_keyword, "")) strcpy(keyword, _keyword);
            if (~_price) price = price;
        }

        void import(int delta) {cnt += delta;}

        void print()
        {
            printf("%s\t%s\t%s\t%s\t%d\t%d\n", ISBN, name, author, keyword, price, cnt);
        }

        const char* get_ISBN() {return ISBN;}

        const char* get_name() {return ISBN;}

        const char* get_author() {return ISBN;}

        const char* get_keyword() {return ISBN;}
};

#endif