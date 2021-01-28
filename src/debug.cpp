#include "B+Tree.hpp"
BplusTree tr("storage");

char s[100010][30];

int main()
{
    int seed = time(0);
    srand(seed);
    int n = 10000;
    for (int i = 1; i <= n; i++)
    {
        int len = 1 + rand() % 10;
        for (int j = 0; j < len; j++)
            s[i][j] = 'a' + rand() % 26;
        //   printf("in :: i:%d s[i]:%s\n", i, s[i]);
        // tr.dfs();
        tr.insert(i, s[i]);

        /*if (i >= 5 && (tr.query(s[5]) == -1 || strcmp(s[5], s[tr.query(s[5])])))
        {
            printf("return i:%d\n", i);
            //tr.dfs();
             return 0;
        }*/

         if (i == 36 || i == 37)
        {
        //  tr.dfs();
        }

        // printf("i:%d\n", i);
    }

    for (int i = 1; i <= n; i++)
    {
        int x = i;
        tr.query(s[x]);
        // printf("%d %d\n", x, tr.query(s[x]));
        if (tr.query(s[x]) == -1 || strcmp(s[x], s[tr.query(s[x])]))
        {
            printf("seed:%d x:%d %d\n", seed, x, tr.query(s[x]));
            puts("QAAQ");
            return 0;
        }
    }

   

    // printf("%s\n", s[30]);

    // printf("%d\n", tr.query(s[30]));

    puts("AC");
}