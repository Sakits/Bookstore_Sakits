#ifndef _BplusTree_h
#define _BplusTree_h

#include <iostream>
#include <fstream>
using namespace std;

inline void Invalid() {puts("Invalid");} // to be merged

class BplusTree
{
    static const int max_size = 4, block_size = max_size / 2;

    private:
        char file[30];

        class node
        {
            public:
                bool isleaf = 1;
                int size = 0, nextptr = -1;
                int child[max_size + 1];
                char key[max_size + 1][30];

                node() 
                {
                    memset(child, 0, sizeof(child));
                    memset(key, 0, sizeof(key));
                }
        };

    public:
        BplusTree (const char* file_name)
        {
            strcpy(file, file_name);

            fstream fin(file, ios :: in | ios :: binary);
            if (!fin.is_open())
            {
                fstream fout(file, ios :: out | ios :: binary);
                node initnode;
                fout.write(reinterpret_cast<char *>(&initnode), sizeof(initnode));
                fout.close();
            }
            fin.close();
        }

        template <typename T>
        void file_read(const int pos, T &p)
        {
            fstream fin(file, ios :: in | ios :: binary);
            fin.seekg(pos, ios :: beg);
            fin.read(reinterpret_cast<char *>(&p), sizeof(p));
            fin.close();
        }

        template <typename T>
        void file_write(const int pos, T &p)
        {
            fstream fout(file, ios :: in | ios :: out | ios :: binary);
            fout.seekp(pos, ios :: beg);
            fout.write(reinterpret_cast<char *>(&p), sizeof(p));
            fout.close();
        }

// --------------------------- debug area ---------------------------

        void printall(const node &p)
        {
            printf("p.isleaf:%d p.size:%d p.nextptr:%d\n", p.isleaf, p.size, p.nextptr);
            for (int i = 0; i < p.size; i++)
                printf("i:%d child[i]:%d key[i]:%s\n", i, p.child[i], p.key[i]);
            puts("");
        }

        void dfs(int x = 0)
        {
            printf("x:%d\n", x);
            node now; file_read(x, now);
            printall(now);

            if (!now.isleaf)
            for (int i = 0; i < now.size; i++)
                dfs(now.child[i]);
        }

// --------------------------- debug area ---------------------------

        int get_file_end()
        {
            fstream fout(file, ios :: app | ios :: binary);
            int pos = fout.tellp();
            fout.close();
            return pos;
        }

        int get_pos(const node &now, const char* _key)
        {
            for (int i = 0; i < now.size; i++)
            if (strcmp(now.key[i], _key) >= 0) return i;
            return now.size;
        }

        void insertone(node &now, const int pos, const int file_pos, const char* _key)
        {
            for (int i = now.size; i > pos; i--)
                now.child[i] = now.child[i - 1], strcpy(now.key[i], now.key[i - 1]);

            now.child[pos] = file_pos;
            strcpy(now.key[pos], _key);
            now.size++;
        }

        void insert(int file_pos, const char* _key, int x = 0, int fa = -1)
        {
            node now; file_read(x, now);
            int pos = get_pos(now, _key);

            if (now.isleaf) insertone(now, pos, file_pos, _key);
            else 
            {
                pos -= (pos == now.size);
                insert(file_pos, _key, now.child[pos], x);
                file_read(x, now);
                node son; file_read(now.child[pos], son);
                strcpy(now.key[pos], son.key[son.size - 1]);
            }

            if (now.size > max_size)
            {
                node nxt; int nxt_pos = get_file_end();
                nxt.isleaf = now.isleaf;
                nxt.size = now.size - block_size;
                nxt.nextptr = now.nextptr;
                
                for (int i = block_size; i < now.size; i++)
                {
                    nxt.child[i - block_size] = now.child[i];
                    strcpy(nxt.key[i - block_size], now.key[i]);
                }

                file_write(nxt_pos, nxt);

                now.size = block_size;
                now.nextptr = nxt_pos;

                if (fa == -1)
                {
                    node root; int root_pos = get_file_end();
                    x = root_pos; root_pos = 0; 
                    root.isleaf = 0;
                    root.size = 2;
                    root.child[0] = x; root.child[1] = nxt_pos;
                    strcpy(root.key[0], now.key[now.size - 1]);
                    strcpy(root.key[1], nxt.key[nxt.size - 1]);

                    file_write(root_pos, root); 
                }
                else
                {
                    node root; file_read(fa, root);

                    int pos = 0;
                    for (int i = 0; i < root.size; i++)
                        if (root.child[i] == x)
                        {
                            pos = i;
                            root.child[i] = nxt_pos;
                            strcpy(root.key[i], nxt.key[nxt.size - 1]);
                            break;
                        }

                    insertone(root, pos, x, now.key[now.size - 1]);
                    file_write(fa, root);
                }
            }

            file_write(x, now);
        }

        int query(const char* _key, int x = 0)
        {
            node now; file_read(x, now);

            if (now.isleaf)
            {
                for (int i = 0; i < now.size; i++)
                    if (!strcmp(now.key[i], _key)) return now.child[i];
                return -1;
            }
            
            int pos = get_pos(now, _key);
            return (pos == now.size) ? -1 : query(_key, now.child[pos]);
        }
};

#endif
