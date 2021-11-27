#ifndef _BPlusTree_h
#define _BPlusTree_h

#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>
#include "MemoryPool.hpp"

class BPlusTree
{
    static const int max_size = 40, block_size = max_size / 2;
    using ull = unsigned long long;

private:
    class Node
    {
    public:
        int nxtptr = -1, preptr = -1, size = 0;
        int isleaf = 1;
        int child[max_size + 1], mxpos[max_size + 1];
        ull key[max_size + 1];

        Node()
        {
            memset(child, 0, sizeof(child));
            memset(mxpos, 0, sizeof(mxpos));
            memset(key, 0, sizeof(key));
        }

        Node &operator = (const Node &other)
        {
            if (this == &other) return *this;

            nxtptr = other.nxtptr;
            preptr = other.preptr;
            size = other.size;
            isleaf = other.isleaf;

            memcpy(child, other.child, sizeof(child));
            memcpy(key, other.key, sizeof(key));
            memcpy(mxpos, other.mxpos, sizeof(mxpos));

            return *this;
        }
    };

    static const int root_file_pos = 8 + sizeof(Node);
    int prex = -1, prec, presize, prenxt, size = 0;
    MemoryPool<Node> node_pool;

    int get_pos(const Node &now, const ull _key, int pos, bool unique = 0)
    {
        for (int i = 0; i < now.size; i++)
        {
            if (unique && now.key[i] == _key) return -1;
            if (now.key[i] > _key || (now.key[i] == _key && now.mxpos[i] >= pos)) return i;
        }
        return now.size;
    }

    void insertone(Node &now, const int pos, const int file_pos, const ull _key, int mxpos)
    {
        for (int i = now.size; i > pos; i--)
        {
            now.child[i] = now.child[i - 1];
            now.key[i] = now.key[i - 1];
            now.mxpos[i] = now.mxpos[i - 1];
        }

        now.child[pos] = file_pos;
        now.mxpos[pos] = mxpos;
        now.key[pos] = _key;
        now.size++;
    }

public:
    BPlusTree (const char* file_name, int cache_size = 0) : node_pool(file_name)
    {
        if (node_pool.get_file_end() == sizeof(int))
        {
            node_pool.file_insert(size);
            Node initnode;
            node_pool.insert(initnode, 0);
        }
        else
            node_pool.file_read(4, size);
    }

    ~BPlusTree() 
    {
        node_pool.file_write(4, size);
    }

    void clear()
    {
        prex = -1; size = 0;

        node_pool.clear();
        node_pool.file_insert(size);
        Node initnode;
        node_pool.insert(initnode, 0);
    }

    // void check(const Node& now)
    // {
    //     for (int i = 0; i < now.size; i++)
    //         printf("%llu ", now.key[i]);
    //     puts("");
    // }

    int get_size() const {return size;}

    bool insert(const ull _key, int file_pos, bool unique = 0, int x = root_file_pos, Node* const faptr = nullptr, int dep = 0)
    {
        Node &now = node_pool.read(x, dep);
        // printf("x:%d\n", x);
        // check(now);

        int pos = get_pos(now, _key, file_pos, unique);
        if (pos == -1) return 0;

        if (now.isleaf)
        {
            insertone(now, pos, file_pos, _key, file_pos);
            size++;
        }
        else
        {
            pos -= (pos == now.size);
            if (!insert(_key, file_pos, unique, now.child[pos], &now, dep + 1))
                return 0;
        }

        if (faptr)
        {
            Node &fa = *faptr;

            for (int i = 0; i < fa.size; i++)
                if (fa.child[i] == x)
                    fa.key[i] = now.key[now.size - 1], fa.mxpos[i] = now.mxpos[now.size - 1];
        }

        if (now.size > max_size)
        {
            Node nxt; int nxt_pos = node_pool.get_nxt_pos();
            nxt.isleaf = now.isleaf;
            nxt.size = now.size - block_size;
            nxt.nxtptr = now.nxtptr;
            nxt.preptr = x;

            if (~nxt.nxtptr) 
                node_pool.write(nxt.nxtptr, nxt_pos, dep, sizeof(int));

            for (int i = block_size; i < now.size; i++)
            {
                nxt.child[i - block_size] = now.child[i];
                nxt.key[i - block_size] = now.key[i];
                nxt.mxpos[i - block_size] = now.mxpos[i];
            }

            node_pool.insert(nxt, dep);

            now.size = block_size;
            now.nxtptr = nxt_pos; 

            if (!faptr)
            {
                Node root; int root_pos = node_pool.get_nxt_pos();
                x = root_pos; node_pool.write(nxt_pos, x, dep, sizeof(int));
                root_pos = root_file_pos;
                root.isleaf = 0;
                root.size = 2;
                root.child[0] = x; root.child[1] = nxt_pos;
                root.key[0] = now.key[now.size - 1];
                root.key[1] = nxt.key[nxt.size - 1];
                root.mxpos[0] = now.mxpos[now.size - 1];
                root.mxpos[1] = nxt.mxpos[nxt.size - 1];

                node_pool.insert(now, dep);
                node_pool.write(root_pos, root, dep);
                return 1;
            }
            else
            {
                Node &fa = *faptr;

                int pos = 0;
                for (int i = 0; i < fa.size; i++)
                    if (fa.child[i] == x)
                    {
                        pos = i;
                        fa.child[i] = nxt_pos;
                        fa.key[i] = nxt.key[nxt.size - 1];
                        fa.mxpos[i] = nxt.mxpos[nxt.size - 1];
                        break;
                    }

                insertone(fa, pos, x, now.key[now.size - 1], now.mxpos[now.size - 1]);
            }
        }

        node_pool.write(x, now, dep);
        return 1;
    }

    bool insert(const char* _skey, int file_pos)
    {
        ull _key = 0;
        int len = strlen(_skey);
        for (int i = 0; i < len; i++)
            _key = _key * 233 + _skey[i];
        
        return insert(_key, file_pos);
    }

    bool erase(const ull _key, int file_pos = -1, int x = root_file_pos, Node* const faptr = nullptr, int dep = 0)
    {
        Node &now = node_pool.read(x, dep);

        if (now.isleaf)
        {
            int pos = -1;
            if (~file_pos)
            {
                for (int i = 0; i < now.size; i++)
                    if (now.key[i] == _key && now.child[i] == file_pos)
                    {
                        pos = i;
                        break;
                    }
            }
            else
            {
                for (int i = 0; i < now.size; i++)
                {
                    if (now.key[i] == _key)
                    {
                        pos = i;
                        break;
                    }
                }
            }

            if (pos == -1) return 0;

            for (int i = pos; i < now.size - 1; i++)
            {
                now.child[i] = now.child[i + 1];
                now.key[i] = now.key[i + 1];
                now.mxpos[i] = now.mxpos[i + 1];
            }

            now.size--;
            size--;
        }
        else
        {
            int pos = get_pos(now, _key, file_pos);
            if (pos == now.size || !erase(_key, file_pos, now.child[pos], &now, dep + 1)) return 0;
        }


        int nxtptr = -1, preptr = -1;
        if (faptr)
        {
            int pos = 0;
            Node &fa = *faptr;
            for (int i = 0; i < fa.size; i++)
                if (fa.child[i] == x)
                {
                    pos = i;
                    break;
                }
            if (pos) preptr = fa.child[pos - 1];
            if (pos != fa.size - 1) nxtptr = fa.child[pos + 1];
        }

        if (now.size < block_size)
        {
            if (~nxtptr)
            {
                Node &nxt = node_pool.read(now.nxtptr, dep);

                if (nxt.size > block_size)
                {
                    now.child[now.size] = nxt.child[0];
                    now.key[now.size] = nxt.key[0];
                    now.mxpos[now.size] = nxt.mxpos[0];
                    now.size++;

                    for (int i = 0; i < nxt.size - 1; i++)
                    {
                        nxt.child[i] = nxt.child[i + 1];
                        nxt.key[i] = nxt.key[i + 1];
                        nxt.mxpos[i] = nxt.mxpos[i + 1];
                    }
                    nxt.size--;

                    node_pool.write(now.nxtptr, nxt, dep);
                }
                else
                {
                    for (int i = 0; i < nxt.size; i++)
                    {
                        now.child[now.size] = nxt.child[i];
                        now.key[now.size] = nxt.key[i];
                        now.mxpos[now.size] = nxt.mxpos[i];
                        now.size++;
                    }

                    Node &fa = *faptr;
                    for (int i = 0; i < fa.size; i++)
                    {
                        if (fa.child[i] == now.nxtptr)
                        {
                            for (int j = i; j < fa.size - 1; j++)
                            {
                                fa.child[j] = fa.child[j + 1];
                                fa.key[j] = fa.key[j + 1];
                                fa.mxpos[j] = fa.mxpos[j + 1];
                            }
                            fa.size--;
                            break;
                        }
                    }

                    if (~nxt.nxtptr)
                    {
                        now.nxtptr = nxt.nxtptr;
                        node_pool.write(nxt.nxtptr, x, dep, sizeof(int));
                    }
                    else now.nxtptr = -1;

                    node_pool.erase(nxtptr);
                }
            }
            else if (~preptr)
            {
                Node &pre = node_pool.read(now.preptr, dep);

                if (pre.size > block_size)
                {
                    for (int i = now.size; i; i--)
                    {
                        now.child[i] = now.child[i - 1];
                        now.key[i] = now.key[i - 1];
                        now.mxpos[i] = now.mxpos[i - 1];
                    }
                    now.child[0] = pre.child[pre.size - 1];
                    now.key[0] = pre.key[pre.size - 1];
                    now.mxpos[0] = pre.mxpos[pre.size - 1];
                    now.size++;
                    pre.size--;

                    node_pool.write(now.preptr, pre, dep);

                    Node &fa = *faptr;
                    for (int i = 0; i < fa.size; i++)
                        if (fa.child[i] == now.preptr)
                        {
                            fa.key[i] = pre.key[pre.size - 1];
                            fa.mxpos[i] = pre.mxpos[pre.size - 1];
                            break;
                        }
                }
                else
                {
                    for (int i = 0; i < now.size; i++)
                    {
                        now.child[i + pre.size] = now.child[i];
                        now.key[i + pre.size] = now.key[i];
                        now.mxpos[i + pre.size] = now.mxpos[i];
                    }

                    for (int i = 0; i < pre.size; i++)
                    {
                        now.child[i] = pre.child[i];
                        now.key[i] = pre.key[i];
                        now.mxpos[i] = pre.mxpos[i];
                    }
                    now.size += pre.size;

                    Node &fa = *faptr;
                    for (int i = 0; i < fa.size; i++)
                    {
                        if (fa.child[i] == now.preptr)
                        {
                            for (int j = i; j < fa.size - 1; j++)
                            {
                                fa.child[j] = fa.child[j + 1];
                                fa.key[j] = fa.key[j + 1];
                                fa.mxpos[j] = fa.mxpos[j + 1];
                            }
                            fa.size--;
                            break;
                        }
                    }

                    if (~pre.preptr)
                    {
                        now.preptr = pre.preptr;
                        node_pool.write(pre.preptr, x, dep, 0);
                    }
                    else now.preptr = -1;

                    node_pool.erase(preptr);
                }
            }
        }

        if (faptr)
        {
            Node &fa = *faptr;
            if (fa.size > 1 || ~fa.preptr || ~fa.nxtptr)
            {
                for (int i = 0; i < fa.size; i++)
                    if (fa.child[i] == x)
                    {
                        fa.key[i] = now.key[now.size - 1];
                        fa.mxpos[i] = now.mxpos[now.size - 1];
                        break;
                    }
            }
            else
                x = -1, fa = now;
        }

        if (~x) node_pool.write(x, now, dep);

        return 1;
    }

    bool erase(const char* _skey, int file_pos = -1)
    {
        ull _key = 0;
        int len = strlen(_skey);
        for (int i = 0; i < len; i++)
            _key = _key * 233 + _skey[i];
        
        return erase(_key, file_pos);
    }

    int query(const ull _key, int x = root_file_pos, int dep = 0)
    {
        Node &now = node_pool.read(x, dep);

        if (now.isleaf)
        {
            for (int i = 0; i < now.size; i++)
                if (now.key[i] == _key)
                {
                    prex = x; prec = i; presize = now.size; prenxt = now.nxtptr;
                    return now.child[i];
                }
            return -1;
        }

        int pos = get_pos(now, _key, -1);
        return (pos == now.size) ? -1 : query(_key, now.child[pos], dep + 1);
    }

    int query(const char* _skey)
    {
        ull _key = 0;
        int len = strlen(_skey);
        for (int i = 0; i < len; i++)
            _key = _key * 233 + _skey[i];
        
        return query(_key);
    }

    int get_next()
    {
        if (prex == -1) return -1;

        if (prec < presize - 1)
        {
            prec++;
            int ans = node_pool.read(prex, 0).child[prec];
            return ans;
        }
        else
        {
            if (prenxt == -1) return -1;
            Node now = node_pool.read(prenxt, 0); 
            
            prex = prenxt; prec = 0; presize = now.size; prenxt = now.nxtptr; 
            return now.child[0];
        }
    }
};

#endif