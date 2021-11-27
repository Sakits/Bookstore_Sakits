#ifndef _Bookstore_hpp
#define _Bookstore_hpp

#include "users_manager.hpp"
#include "book.hpp"
#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>
#include <vector>

namespace finance
{
    typedef long long ll;
    ll income, cost;
    std::FILE *fio;

// --------------------------- debug area ---------------------------

    void Invalid() {puts("Invalid");}

    void log_is_not_enough() 
    {
        // puts("log_is_not_enough ");
        return Invalid();
    }

// --------------------------- debug area ---------------------------

    void init()
    {
        fio = std::fopen("storage_finance", "rb+");
        if (fio == nullptr)
        {
            fio = std::fopen("storage_finance", "wb+");
            income = cost = 0;
            std::fwrite(std::addressof(income), sizeof(income), 1, fio);
            std::fwrite(std::addressof(cost), sizeof(cost), 1, fio);
        }
        else
        {
            std::fread(std::addressof(income), sizeof(income), 1, fio);
            std::fread(std::addressof(cost), sizeof(cost), 1, fio);
        }
    }

    void add_income(ll delta)
    {
        income += delta;
        std::fseek(fio, 0, SEEK_SET);
        std::fwrite(std::addressof(income), sizeof(income), 1, fio);
        std::fseek(fio, 0, SEEK_END);
        std::fwrite(std::addressof(delta), sizeof(delta), 1, fio);
    }    

    void add_cost(ll delta)
    {
        cost += delta;
        std::fseek(fio, sizeof(ll), SEEK_SET);
        std::fwrite(std::addressof(cost), sizeof(cost), 1, fio);

        delta = -delta;
        std::fseek(fio, 0, SEEK_END);
        std::fwrite(std::addressof(delta), sizeof(delta), 1, fio);
    }

    void show(int cnt = 0)
    {
        if (!cnt) printf("+ %.2lf - %.2lf\n", income / 100.0, cost / 100.0);
        else
        {
            std::fseek(fio, 0, SEEK_END);
            int tot = (int)ftell(fio) / sizeof(ll) - 2;
            if (cnt > tot) return log_is_not_enough();

            int pos = (int)ftell(fio) - cnt * sizeof(ll);
            std::fseek(fio, pos, SEEK_SET);
            ll totin = 0, totout = 0;
            ll delta[cnt]; 
            std::fread(delta, sizeof(delta), 1, fio);
            for (int i = 0; i < cnt; i++)
            {
                if (delta[i] >= 0) totin += delta[i];
                else totout -= delta[i];
            }

            printf("+ %.2lf - %.2lf\n", totin / 100.0, totout / 100.0);
        }
    }
}

namespace bm
{
    int key_cnt;
    char key[61][61];

    FILE* fio;
    BPlusTree bpt_isbn("index_books_isbn"), bpt_name("index_books_name"), bpt_aut("index_books_author"), bpt_key("index_books_keyword");

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
        // um :: current_user.print();
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
        // um :: current_user.print();
        // printf("null_current_book ");
        return Invalid();
    }

    void keyword_occur_more_than_once()
    {
        return Invalid();
    }

// --------------------------- debug area ---------------------------

    void init()
    {
        fio = std::fopen("storage_books", "rb+");
        if (fio == nullptr)
            fio = std::fopen("storage_books", "wb+");
    }

    int split_key(const char* keyword)
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

        for (int i = 0; i < key_cnt; i++)
        {
            for (int j = i + 1; j < key_cnt; j++)
                if (!strcmp(key[i], key[j])) return 0;
        }

        return 1;
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

    int file_write(book &now, int pos = -1, bool isbn = 1, bool name = 1, bool author = 1, bool keyword = 1)
    {
        if (pos == -1)
        {
            std::fseek(fio, 0, SEEK_END);
            pos = ftell(fio);
        }
        else std::fseek(fio, pos, SEEK_SET);

        if (isbn) bpt_isbn.insert(now.get_ISBN(), pos);
        if (name) bpt_name.insert(now.get_name(), pos);
        if (author) bpt_aut.insert(now.get_author(), pos);
        if (keyword)
        {
            split_key(now.get_keyword());
            for (int i = 0; i < key_cnt; i++)
                bpt_key.insert(key[i], pos);
        }

        fwrite(std::addressof(now), sizeof(now), 1, fio);
        return pos;
    }

    int file_read(const char* ISBN)
    {
        int pos = bpt_isbn.query(ISBN);
        if (pos == -1) 
        {
           book newbk; newbk.init(ISBN); 
           return file_write(newbk);
        }
        return pos;
    }

    void file_read(int pos, book &now)
    {
        std::fseek(fio, pos, SEEK_SET);
        fread(std::addressof(now), sizeof(now), 1, fio);
    }

    int find_book(const char* ISBN, book &now)
    {
        int pos = bpt_isbn.query(ISBN);
        if (pos == -1) return -1;
        std::fseek(fio, pos, SEEK_SET);
        fread(std::addressof(now), sizeof(now), 1, fio);
        return pos;
    }

    void select(const char* ISBN)
    {
        if (um :: current_user.get_pri() < um :: Clerk) return select_have_no_permission();
        um :: current_user.curbk = file_read(ISBN);
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
        if (um :: current_user.curbk == -1) return null_current_book();
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
            else if (type == 4 && !flag4) 
            {
                cut(argv[i]), strcpy(keyword, argv[i] + 10), flag4 = 1;
                if (!split_key(keyword)) return keyword_occur_more_than_once();
            }
            else if (type == 5 && price == -1)
            {
                price = get_price(argv[i] + 7);
                if (price == -1) return modify_wrong_infor();
            }
            else return modify_wrong_infor();
        }

        if (flag1 && bpt_isbn.query(ISBN) != -1) return ISBN_already_exist();

        book current_book; 
        int pos = um :: current_user.curbk;
        file_read(pos, current_book);
        if (flag1) bpt_isbn.erase(current_book.get_ISBN(), pos);
        if (flag2) bpt_name.erase(current_book.get_name(), pos);
        if (flag3) bpt_aut.erase(current_book.get_author(), pos);
        if (flag4) 
        {
            split_key(current_book.get_keyword());
            for (int i = 0; i < key_cnt; i++)
                bpt_key.erase(key[i], pos);
        }

        current_book.modify(ISBN, name, author, keyword, price);
        file_write(current_book, pos, flag1, flag2, flag3, flag4);
    }

    void import(int delta, const char* s)
    {
        if (um :: current_user.curbk == -1) return null_current_book();
        // printf("import: current_user:"); um :: current_user.print();
        if (um :: current_user.get_pri() < um :: Clerk) return import_have_no_permission();

        // um :: current_user.print();
        // book current_book;
        // file_read(um :: current_user.curbk, current_book);
        // current_book.print();
        // um :: current_user.bk.print();
        // printf("import delta:%d\n", delta);
        // bpt_isbn.dfs();

        int cost = get_price(s);
        if (cost == -1) return wrong_import_price();
        finance :: add_cost(cost);

        int pos = um :: current_user.curbk, cnt;
        std::fseek(fio, pos, SEEK_SET);
        fread(std::addressof(cnt), sizeof(cnt), 1, fio);
        cnt += delta;
        std::fseek(fio, pos, SEEK_SET);
        fwrite(std::addressof(cnt), sizeof(cnt), 1, fio);
        // printf("import delta:%d\n", delta);
        // current_book.print();
    }

    char fpeek(FILE* stream)
    {
        char c;
        c = fgetc(stream);
        ungetc(c, stream);
        return c;
    }

    void show(char* ty = nullptr, int cnt = 0)
    {
        // printf("show: current_user:"); um :: current_user.print();
        if (um :: current_user.get_pri() < um :: Customer) return show_have_no_permission();

        book now;
        vector <book> v;

        if (!ty)
        {
            std::fseek(fio, 0, SEEK_SET);
            if (fpeek(fio) == EOF) return show_book_not_found();
            while (fpeek(fio) != EOF)
            {
                fread(std::addressof(now), sizeof(now), 1, fio);
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
                std::fseek(fio, pos, SEEK_SET);
                fread(std::addressof(now), sizeof(now), 1, fio);
                v.push_back(now);
            }
            else if (type == 2)
            {
                cut(ty);
                int pos = bpt_name.query(ty + 7);
                if (pos == -1) return show_book_not_found();

                std::fseek(fio, pos, SEEK_SET);
                fread(std::addressof(now), sizeof(now), 1, fio);
                while (!strcmp(now.get_name(), ty + 7))
                {
                    v.push_back(now);

                    pos = bpt_name.get_next();
                    if (pos == -1) break;
                    std::fseek(fio, pos, SEEK_SET);
                    fread(std::addressof(now), sizeof(now), 1, fio);
                }
            }
            else if (type == 3)
            {
                cut(ty);
                int pos = bpt_aut.query(ty + 9);
                if (pos == -1) return show_book_not_found();

                std::fseek(fio, pos, SEEK_SET);
                fread(std::addressof(now), sizeof(now), 1, fio);
                while (!strcmp(now.get_author(), ty + 9))
                {
                    v.push_back(now);

                    pos = bpt_aut.get_next();
                    if (pos == -1) break;
                    std::fseek(fio, pos, SEEK_SET);
                    fread(std::addressof(now), sizeof(now), 1, fio);
                }
            }
            else if (type == 4)
            {
                cut(ty);
                int pos = bpt_key.query(ty + 10);
                if (pos == -1) return show_book_not_found();

                std::fseek(fio, pos, SEEK_SET);
                fread(std::addressof(now), sizeof(now), 1, fio);
                while (check_keyword(now.get_keyword(), ty + 10))
                {
                    v.push_back(now);

                    pos = bpt_key.get_next();
                    if (pos == -1) break;
                    std::fseek(fio, pos, SEEK_SET);
                    fread(std::addressof(now), sizeof(now), 1, fio);
                }
            }
            else if (type == 6) 
            {
                if (um :: current_user.get_pri() < um :: Root) return show_have_no_permission();
                finance :: show(cnt);
            }
            else return wrong_show_type();
        }

        sort(v.begin(), v.end());
        v.erase(unique(v.begin(), v.end()), v.end());
        for (int i = 0; i < v.size(); i++)
            v[i].print();
    }

    void buy(const char* ISBN, int cnt)
    {
        if (um :: current_user.get_pri() < um :: Customer) return buy_have_no_permission();
        
        book now; int pos = find_book(ISBN, now);
        if (pos == -1) return buy_book_not_found();

        // printf("ISBN:%s cnt:%d nowcnt:%d ", ISBN, cnt, now.get_cnt());

        if (cnt > now.get_cnt()) return book_is_not_enough();

        finance :: add_income(1ll * now.get_price() * cnt);
        printf("%.2lf\n", 1ll * now.get_price() * cnt / 100.0);

        std::fseek(fio, pos, SEEK_SET);
        cnt = now.get_cnt() - cnt;
        fwrite(std::addressof(cnt), sizeof(cnt), 1, fio);
    }
}

#endif