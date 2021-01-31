#include "Bookstore.hpp"

#include <iostream>
#include <cstring>
#include <cstdio>
#include <algorithm>
#include <cmath>


const int maxn = 100;
int argc;
char argv[maxn][maxn];
string s;

void Invalid() 
{
    // puts("main"); 
    puts("Invalid");
}

bool get_line()
{
    argc = 0;

    if (!getline(cin, s)) return 0;
    int len = s.length();

    // cout << s << endl;

    for (int i = 0; i < len; i++)
    if (s[i] != ' ' && s[i] != '\n')
    {   
        int j = i, len2 = 0;
        while (j < len && s[j] != ' ' && s[j] != '\n') 
            argv[argc][len2++] = s[j], j++;
        argv[argc][len2] = '\0';

        i = j; argc++;
    }

    // printf("argc:%d ", argc);
    // for (int i = 0; i < argc; i++)
    //     printf("%s ", argv[i]);
    // puts("");

    return 1;
}

int StringtoInt(const char* s)
{
    if (strlen(s) > 1) return -1;
    return s[0] - '0';
}

int StringtoInt2(const char* s)
{
    int len = strlen(s);
    int num = 0;
    for (int i = 0; i < len; i++)
    if ('0' <= s[i] && s[i] <= '9') num = num * 10 + s[i] - '0';
    else return -1;
    return num;
}

void init()
{
    um :: init();
    bm :: init();
    finance :: init();


    // bm :: fio.seekg(576, ios :: beg);
    // book now; bm :: fio.read(reinterpret_cast<char *>(&now), sizeof(now));
    // now.print();
    // bm :: fio.seekg(0, ios :: beg);
}

int main() 
{
    init(); 
    while (get_line())
    {
        if (!argc) Invalid();
        else if (!strcmp(argv[0], "su"))
        {
            if (argc == 2) um :: login(argv[1]);
            else if (argc == 3) um :: login(argv[1], argv[2]);
            else Invalid();
        }
        else if (!strcmp(argv[0], "logout"))
        {
            if (argc == 1) um :: logout();
            else Invalid();
        }
        else if (!strcmp(argv[0], "useradd"))
        {
            if (argc == 5) 
            {
                int x = StringtoInt(argv[3]);
                if (x == 1 || x == 3 || x == 7) um :: user_add(argv[1], argv[2], x, argv[4]);
                else Invalid();
            }
            else Invalid();
        }
        else if (!strcmp(argv[0], "register"))
        {
            if (argc == 4) um :: user_register(argv[1], argv[2], argv[3]);
            else Invalid();
        }
        else if (!strcmp(argv[0], "delete"))
        {
            if (argc == 2) um :: user_delete(argv[1]);
            else Invalid();
        }
        else if (!strcmp(argv[0], "passwd"))
        {
            if (argc == 3) um :: user_changepw(argv[1], argv[2]);
            else if (argc == 4) um :: user_changepw(argv[1], argv[3], argv[2]);
            else Invalid();
        }
        else if (!strcmp(argv[0], "select"))
        {
            if (argc == 2) bm :: select(argv[1]);
            else Invalid();
        }
        else if (!strcmp(argv[0], "modify"))
        {
            if (argc <= 6) bm :: modify(argc, argv);
            else Invalid();
        }
        else if (!strcmp(argv[0], "show"))
        {
            if (argc == 1) bm :: show();
            else if (argc == 2) bm :: show(argv[1]);
            else if (argc == 3) 
            {
                int x = StringtoInt2(argv[2]);
                if (x == -1) Invalid();
                else bm :: show(argv[1], x);
            }
            else Invalid();
        }
        else if (!strcmp(argv[0], "import"))
        {
            if (argc == 3)
            {
                int x = StringtoInt2(argv[1]);
                if (x == -1) Invalid();
                else bm :: import(x, argv[2]);
            }
            else Invalid();
        }
        else if (!strcmp(argv[0], "buy")) 
        {
            if (argc == 3)
            {
                int x = StringtoInt2(argv[2]);
                if (x == -1) Invalid();
                else bm :: buy(argv[1], x);
            }
            else Invalid();
        }
        else if (!strcmp(argv[0], "exit")) return 0;
        else if (!strcmp(argv[0], "quit")) return 0;
        else Invalid();
    }

    return 0;
}