#ifndef _book_hpp
#define _book_hpp

#include <iostream>
using namespace std;

class book
{
        friend bool operator < (const book &a, const book & b)
        {
            return strcmp(a.ISBN, b.ISBN) < 0;
        }

        friend bool operator == (const book &a, const book & b)
        {
            return !strcmp(a.ISBN, b.ISBN);
        }

    private:
        int cnt, price;
        char ISBN[70], name[70], author[70], keyword[70];

    public:
        book()
        {
            cnt = price = 0;
            ISBN[0] = name[0] = author[0] = keyword[0] = '\0';
        }

        inline void cut(char *s)
        {
            int len = strlen(s);
            s[len - 1] = '\0';
        }

        void init(const char* _ISBN)
        {
            cnt = price = 0;
            strcpy(ISBN, _ISBN);
            name[0] = author[0] = keyword[0] = '\0';
        }

        void modify(const char* _ISBN, const char* _name, const char* _author, const char* _keyword, int _price)
        {
            if (_ISBN[0] != '\0') strcpy(ISBN, _ISBN);
            if (_name[0] != '\0') strcpy(name, _name);
            if (_author[0] != '\0') strcpy(author, _author);
            if (_keyword[0] != '\0') strcpy(keyword, _keyword);
            if (~_price) price = _price;
        }

        void import(int delta) {cnt += delta;}

        void print()
        {
            printf("%s\t%s\t%s\t%s\t%.2lf\t%d\n", ISBN, name, author, keyword, price / 100.0, cnt);
        }

        const char* get_ISBN() {return ISBN;}

        const char* get_name() {return name;}

        const char* get_author() {return author;}

        const char* get_keyword() {return keyword;}

        int get_cnt() {return cnt;}

        int get_price() {return price;}
};

#endif