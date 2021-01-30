#ifndef _Bookstore_hpp
#define _Bookstore_hpp

#include "users_manager.hpp"
#include "book.hpp"

#include <vector>

namespace bm
{
    book current_book;
    int key_cnt;
    char key[61][61];

    fstream fio;
    BplusTree bpt_isbn("index_books_isbn"), bpt_name("index_books_name"), bpt_aut("index_books_author"), bpt_key("index_books_keyword");

// --------------------------- debug area ---------------------------

    void Invalid() {puts("Invalid");}

    void modify_wrong_infor() {return Invalid();}

// --------------------------- debug area ---------------------------

    void init()
    {
        fstream fin("storage_books", ios :: in | ios :: binary);
        if (!fin.is_open())
        {
            fstream fout("storage_books", ios :: out | ios :: binary);
            fout.close();
        }
        fin.close();

        fio.open("storage_books", ios :: in | ios :: out | ios :: binary);
    }

    void split_key(const char* keyword)
    {
        key_cnt = 0; int len = strlen(keyword);

        for (int i = 0; i < len; i++)
        {
            int j = i, len2 = 0;
            while (j < len && keyword[j] != '|') 
                key[key_cnt][len2++] = keyword[j], j++;
            key[key_cnt][len2] = '\0';

            key_cnt++;
            i = j;
        }
    }

    void file_write(book &now, int pos = -1, bool isbn = 1, bool name = 1, bool author = 1, bool keyword = 1)
    {
        if (pos == -1)
        {
            fio.seekp(0, ios :: end);
            pos = fio.tellp();
        }
        else fio.seekp(pos, ios :: beg);

        if (isbn) bpt_isbn.insert(pos, now.get_ISBN());
        if (name) bpt_name.insert(pos, now.get_name());
        if (author) bpt_aut.insert(pos, now.get_author());
        if (keyword)
        {
            split_key(now.get_keyword());
            for (int i = 0; i < key_cnt; i++)
                bpt_key.insert(pos, key[i]);
        }
        fio.write(reinterpret_cast<char *>(&now), sizeof(now));
    }

    void file_read(const char* ISBN, book &now)
    {
        int pos = bpt_isbn.query(ISBN);
        if (pos == -1) now.init(ISBN), file_write(now);
        else 
        {
            fio.seekg(pos, ios :: beg);
            fio.read(reinterpret_cast<char *>(&now), sizeof(now));
        }
    }

    void select(const char* ISBN)
    {
        file_read(ISBN, current_book);
    }

    int get_type(const char* s)
    {
        if (s[0] == '-' && s[1] == 'I' && s[2] == 'S' && s[3] == 'B' && s[4] == 'N' && s[5] == '=') return 1;
        if (s[0] == '-' && s[1] == 'n' && s[2] == 'a' && s[3] == 'm' && s[4] == 'e' && s[5] == '=') return 2;
        if (s[0] == '-' && s[1] == 'a' && s[2] == 'u' && s[3] == 't' && s[4] == 'h' && s[5] == 'o' && s[6] == 'r' && s[7] == '=') return 3;
        if (s[0] == '-' && s[1] == 'k' && s[2] == 'e' && s[3] == 'y' && s[4] == 'w' && s[5] == 'o' && s[6] == 'r' && s[7] == 'd' && s[8] == '=') return 4;
        if (s[0] == '-' && s[1] == 'p' && s[2] == 'r' && s[3] == 'i' && s[4] == 'c' && s[5] == 'e' && s[6] == '=') return 5;
        return 0;
    }

    int get_price(const char* s)
    {
        int len = strlen(s); int price = 0;

        for (int i = 0; i < len; i++)
        if ('0' <= s[i] && s[i] <= '9')
            price = price * 10 + s[i] - '0';
        else 
            return -1;

        return price;
    }

    void modify(int argc, char **argv)
    {
        char ISBN[70] = "", name[70] = "", author[70] = "", keyword[70] = "";
        int price = -1;
        
        for (int i = 0; i < argc; i++)
        {
            int type = get_type(argv[i]);
            if (type == 1 && !strcmp(ISBN, "")) strcpy(ISBN, argv[i] + 6);
            else if (type == 2 && !strcmp(name, "")) strcpy(name, argv[i] + 6);
            else if (type == 3 && !strcmp(author, "")) strcpy(author, argv[i] + 8);
            else if (type == 4 && !strcmp(keyword, "")) strcpy(keyword, argv[i] + 9);
            else if (type == 5 && price == -1)
            {
                price = get_price(argv[i] + 7);
                if (price == -1) return modify_wrong_infor();
            }
            else return modify_wrong_infor();
        }

        int pos = bpt_isbn.query(current_book.get_ISBN());

        current_book.modify(ISBN, name, author, keyword, price);
        file_write(current_book, pos);
    }
}

#endif