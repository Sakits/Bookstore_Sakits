#include "B+Tree.hpp"
BplusTree tr("storage");

char s[100010][30];

int main()
{
    // int n = 100000;
    // cin >> n;
    // tr.time_test(n);

    int seed = time(0);
    srand(seed);
    // cout << seed << endl;
    int n = 100;
    for (int i = 1; i <= n; i++)
    {
        int len = 1 + rand() % 10;
        for (int j = 0; j < len; j++)
            s[i][j] = 'a' + rand() % 26;
        //    printf("in :: i:%d s[i]:%s\n", i, s[i]);
        tr.insert(i, s[i]);
        //  tr.dfs();
/*
        if (i >= 5 && (tr.query(s[5]) == -1 || strcmp(s[5], s[tr.query(s[5])])))
        {
            printf("return i:%d\n", i);
            tr.dfs();
             return 0;
        }
*/
        //  tr.dfs();
        // printf("i:%d\n", i);
    }

    //  tr.dfs();
    for (int i = 1; i <= n; i++)
    {
        tr.erase(s[i]);
        //  printf("delete: i:%d s[i]:%s\n", i, s[i]);
        //  tr.dfs();
        for (int j = i + 1; j <= n; j++)
        {
            int x = tr.query(s[j]);
            if ( x == -1 || strcmp(s[j], s[x]))
            {
                printf("seed:%d i:%d j:%d %d\n", seed, i, j, x);
                puts("QAAQ");
                return 0;
            }
        }
    }

    // printf("%s\n", s[30]);

    // printf("%d\n", tr.query(s[30]));

    puts("AC");
    cout << tr.file_read_cnt << " " << tr.file_write_cnt << endl;
}