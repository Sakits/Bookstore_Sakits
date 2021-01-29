#include "Bookstore.hpp"

const int maxn = 100;
int argc;
char argv[maxn][maxn];
string s;

void Invalid() {puts("Invalid");}

bool get_line()
{
    argc = 0;

    if (!getline(cin, s)) return 0;
    int len = s.length();

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

void init()
{
    um :: init();
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
        else if (!strcmp(argv[0], "exit")) return 0;
        else if (!strcmp(argv[0], "quit")) return 0;
        else Invalid();
    }
    return 0;
}