#ifndef _Bookstore_hpp
#define _Bookstore_hpp

#include "users_manager.hpp"
#include "book.hpp"

#include <vector>

namespace finance
{
    int income, cost;
    fstream fio;

// --------------------------- debug area ---------------------------

    void Invalid() {puts("Invalid");}

    void log_is_not_enough() {return Invalid();}

// --------------------------- debug area ---------------------------

    void init()
    {
        fstream fin("storage_finance", ios :: in | ios :: binary);
        if (!fin.is_open())
        {
            fstream fout("storage_finance", ios :: out | ios :: binary);
            income = cost = 0;
            fout.write(reinterpret_cast<char *>(&income), sizeof(income));
            fout.write(reinterpret_cast<char *>(&cost), sizeof(cost));
            fout.close();
        }
        fin.close();

        fio.open("storage_finance", ios :: in | ios :: out | ios :: binary);
        fio.seekg(0, ios :: beg);
        fio.read(reinterpret_cast<char *>(&income), sizeof(income));
        fio.read(reinterpret_cast<char *>(&cost), sizeof(cost));
    }

    void add_income(int delta)
    {
        income += delta;
        fio.seekp(0, ios :: beg);
        fio.write(reinterpret_cast<char *>(&income), sizeof(income));
        fio.seekp(0, ios :: end);
        fio.write(reinterpret_cast<char *>(&delta), sizeof(delta));
    }    

    void add_cost(int delta)
    {
        cost += delta;
        fio.seekp(sizeof(int), ios :: beg);
        fio.write(reinterpret_cast<char *>(&cost), sizeof(cost));

        delta = -delta;
        fio.seekp(0, ios :: end);
        fio.write(reinterpret_cast<char *>(&delta), sizeof(delta));
    }

    void show(int cnt = 0)
    {
        if (!cnt) printf("+ %.2lf - %.2lf\n", income / 100.0, cost / 100.0);
        else
        {
            fio.seekg(0, ios :: end);
            int tot = fio.tellg() / sizeof(int) - 2;
            if (cnt > tot) return log_is_not_enough();

            int pos = fio.tellg(); 
            pos -= cnt * sizeof(int);
            fio.seekg(pos, ios :: beg);
            int totin = 0, totout = 0, delta;
            for (int i = 0; i < cnt; i++)
            {
                fio.read(reinterpret_cast<char *>(&delta), sizeof(delta));
                if (delta >= 0) totin += delta;
                else totout -= delta;
            }

            printf("+ %.2lf - %.2lf\n", totin / 100.0, totout / 100.0);
        }
    }
}

namespace bm
{
    book current_book;
    int key_cnt;
    char key[61][61];

    fstream fio;
    BplusTree bpt_isbn("index_books_isbn"), bpt_name("index_books_name"), bpt_aut("index_books_author"), bpt_key("index_books_keyword");

// --------------------------- debug area ---------------------------

    void Invalid() {puts("Invalid");}

    void modify_wrong_infor() 
    {
        // printf("modify_wrong_infor "); 
        return Invalid();
    }

    void wrong_show_type() 
    {
        // printf("wrong_show_type "); 
        return Invalid();
    }

    void show_book_not_found() {return (void)puts("");}

    void wrong_import_price() 
    {
        // printf("wrong_import_price "); 
        return Invalid();
    }

    void select_have_no_permission() 
    {
        // printf("select_have_no_permission "); 
        return Invalid();
    }

    void modify_have_no_permission() 
    {
        // printf("modify_have_no_permission "); 
        return Invalid();
    }

    void import_have_no_permission() 
    {
        // printf("import_have_no_permission "); 
        return Invalid();
    }

    void show_have_no_permission() 
    {
        // printf("show_have_no_permission "); 
        return Invalid();
    }

    void buy_have_no_permission() 
    {
        // printf("buy_have_no_permission "); 
        return Invalid();
    }

    void buy_book_not_found() 
    {
        // printf("buy_book_not_found "); 
        return Invalid();
    }

    void book_is_not_enough() 
    {
        // printf("book_is_not_enough "); 
        return Invalid();
    }

    void ISBN_already_exist() 
    {
        // printf("ISBN_already_exist "); 
        return Invalid();
    }

    void null_current_book()
    {
        // printf("null_current_book");
        return Invalid();
    }

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

    bool check_keyword(const char* keyword, const char* _keyword)
    {
        split_key(keyword);
        for (int i = 0; i < key_cnt; i++)
            if (!strcmp(key[i], _keyword)) return 1;
        return 0;
    }

    void cut(char *s)
    {
        s[strlen(s) - 1] = '\0';
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

    int find_book(const char* ISBN, book &now)
    {
        int pos = bpt_isbn.query(ISBN);
        if (pos == -1) return -1;
        fio.seekg(pos, ios :: beg);
        fio.read(reinterpret_cast<char *>(&now), sizeof(now));
        return pos;
    }

    void select(const char* ISBN)
    {
        // printf("select: current_user:"); um :: current_user.print();
        if (um :: current_user.get_pri() < um :: Clerk) return select_have_no_permission();
        file_read(ISBN, current_book);
    }

    int get_type(const char* s)
    {
        if (s[0] == '-' && s[1] == 'I' && s[2] == 'S' && s[3] == 'B' && s[4] == 'N' && s[5] == '=') return 1;
        if (s[0] == '-' && s[1] == 'n' && s[2] == 'a' && s[3] == 'm' && s[4] == 'e' && s[5] == '=') return 2;
        if (s[0] == '-' && s[1] == 'a' && s[2] == 'u' && s[3] == 't' && s[4] == 'h' && s[5] == 'o' && s[6] == 'r' && s[7] == '=') return 3;
        if (s[0] == '-' && s[1] == 'k' && s[2] == 'e' && s[3] == 'y' && s[4] == 'w' && s[5] == 'o' && s[6] == 'r' && s[7] == 'd' && s[8] == '=') return 4;
        if (s[0] == '-' && s[1] == 'p' && s[2] == 'r' && s[3] == 'i' && s[4] == 'c' && s[5] == 'e' && s[6] == '=') return 5;
        if (!strcmp(s, "finance")) return 6;
        return 0;
    }

    int get_price(const char* s)
    {
        int len = strlen(s); 

        int num1 = 0, num2 = 0, i = 0;
        for (i = 0; i < len; i++)
        if ('0' <= s[i] && s[i] <= '9')
            num1 = num1 * 10 + s[i] - '0';
        else if (s[i] == '.') break;
        else return -1;

        int cnt = 0;
        for (i = i + 1; i < len; i++)
        if ('0' <= s[i] && s[i] <= '9')
            num2 = num2 * 10 + s[i] - '0', cnt++;
        else return -1;
        if (cnt == 1) num2 *= 10;

        return num1 * 100 + num2;
    }

    void modify(int argc, char (*argv)[100])
    {
        if (current_book.get_ISBN()[0] == '\0') return null_current_book();
        // printf("modify: current_user:"); um :: current_user.print();
        if (um :: current_user.get_pri() < um :: Clerk) return modify_have_no_permission();

        char ISBN[70] = "", name[70] = "", author[70] = "", keyword[70] = "";
        int price = -1;
        
        bool flag1 = 0, flag2 = 0, flag3 = 0, flag4 = 0;
        for (int i = 1; i < argc; i++)
        {
            int type = get_type(argv[i]);
            if (type == 1 && !flag1) strcpy(ISBN, argv[i] + 6), flag1 = 1;
            else if (type == 2 && !flag2) cut(argv[i]), strcpy(name, argv[i] + 7), flag2 = 1;
            else if (type == 3 && !flag3) cut(argv[i]), strcpy(author, argv[i] + 9), flag3 = 1;
            else if (type == 4 && !flag4) cut(argv[i]), strcpy(keyword, argv[i] + 10), flag4 = 1;
            else if (type == 5 && price == -1)
            {
                price = get_price(argv[i] + 7);
                if (price == -1) return modify_wrong_infor();
            }
            else return modify_wrong_infor();
        }

        if (flag1 && bpt_isbn.query(ISBN) != -1) return ISBN_already_exist();

        int pos = bpt_isbn.query(current_book.get_ISBN());
        if (flag1) bpt_isbn.erase(current_book.get_ISBN(), pos);
        if (flag2) bpt_name.erase(current_book.get_name(), pos);
        if (flag3) bpt_aut.erase(current_book.get_author(), pos);
        if (flag4) bpt_key.erase(current_book.get_keyword(), pos);

        current_book.modify(ISBN, name, author, keyword, price);
        file_write(current_book, pos, flag1, flag2, flag3, flag4);
    }

    void import(int delta, const char* s)
    {
        if (current_book.get_ISBN()[0] == '\0') return null_current_book();
        // printf("import: current_user:"); um :: current_user.print();
        if (um :: current_user.get_pri() < um :: Clerk) return import_have_no_permission();

        int cost = get_price(s);
        if (cost == -1) return wrong_import_price();
        finance :: add_cost(cost);
        current_book.import(delta);
        int pos = bpt_isbn.query(current_book.get_ISBN());
        fio.seekp(pos, ios :: beg);
        int cnt = current_book.get_cnt();
        fio.write(reinterpret_cast<char *>(&cnt), sizeof(cnt));
    }

    void show(char* ty = nullptr, int cnt = 0)
    {
        // printf("show: current_user:"); um :: current_user.print();
        if (um :: current_user.get_pri() < um :: Customer) return show_have_no_permission();

        book now;
        vector <book> v;

        if (!ty)
        {
            fio.seekg(0, ios :: beg);
            if (fio.peek() == EOF) return show_book_not_found();
            while (fio.peek() != EOF)
            {
                fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                v.push_back(now);
            }
        }
        else
        {
            int type = get_type(ty);
            if (type == 1) 
            {
                int pos = bpt_isbn.query(ty + 6);
                if (pos == -1) return show_book_not_found();
                fio.seekg(pos, ios :: beg);
                fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                v.push_back(now);
            }
            else if (type == 2)
            {
                cut(ty);
                int pos = bpt_name.query(ty + 7);
                if (pos == -1) return show_book_not_found();

                fio.seekg(pos, ios :: beg);
                fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                while (!strcmp(now.get_name(), ty + 7))
                {
                    v.push_back(now);

                    if (fio.peek() == EOF) break;
                    fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                }
            }
            else if (type == 3)
            {
                cut(ty);
                int pos = bpt_aut.query(ty + 9);
                if (pos == -1) return show_book_not_found();

                fio.seekg(pos, ios :: beg);
                fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                while (!strcmp(now.get_author(), ty + 9))
                {
                    v.push_back(now);

                    if (fio.peek() == EOF) break;
                    fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                }
            }
            else if (type == 4)
            {
                cut(ty);
                int pos = bpt_key.query(ty + 10);
                if (pos == -1) return show_book_not_found();

                fio.seekg(pos, ios :: beg);
                fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                while (check_keyword(now.get_keyword(), ty + 10))
                {
                    v.push_back(now);

                    if (fio.peek() == EOF) break;
                    fio.read(reinterpret_cast<char *>(&now), sizeof(now));
                }
            }
            else if (type == 6) finance :: show(cnt);
            else return wrong_show_type();
        }

        sort(v.begin(), v.end());
        for (int i = 0; i < v.size(); i++)
            v[i].print();
    }

    void buy(const char* ISBN, int cnt)
    {
        if (current_book.get_ISBN()[0] == '\0') return null_current_book();
        if (um :: current_user.get_pri() < um :: Customer) return buy_have_no_permission();
        
        book now; int pos = find_book(ISBN, now);
        if (pos == -1) return buy_book_not_found();
        if (cnt > now.get_cnt()) return book_is_not_enough();

        finance :: add_income(now.get_price() * cnt);
        printf("%.2lf\n", now.get_price() * cnt / 100.0);

        fio.seekp(pos, ios :: beg);
        cnt = now.get_cnt() - cnt;
        fio.write(reinterpret_cast<char *>(&cnt), sizeof(cnt));
    }
}

#endif