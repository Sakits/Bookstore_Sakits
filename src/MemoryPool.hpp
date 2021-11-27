#ifndef _MemoryPool_h
#define _MemoryPool_h

#include <iostream>
#include <cstring>
#include <cstdio>
#include <fstream>

template<typename T>
class MemoryPool
{
private:
    std::FILE *fio;
    char file[50];
    int last, front, rear;

    struct edge
    {
        T node;
        int pos, pre, nxt, epre, enxt;
    };

    static const int maxn = 2000, mod = 10007;
    int tot;
    int elast[mod];
    edge e[maxn + 1];

    int find(int x)
    {
        int hs = x % mod;
        for (int i = elast[hs]; i; i = e[i].pre)
        {
            if (x == e[i].pos)
                return i;
        }
        
        return 0;
    }

    int mpinsert(int x, int pos = 0)
    {
        int hs = x % mod;
        if (!pos)
            pos = ++tot;
        e[pos] = (edge){T(), x, elast[hs], 0, 0, 0};

        if (elast[hs])
            e[elast[hs]].nxt = pos;
        elast[hs] = pos;

        return pos;
    }

public:
    MemoryPool(const char* file_name) 
    {
        tot = 0;
        memset(elast, 0, sizeof(elast));
        last = -1;
        front = rear = 0;
        strcpy(file, file_name);

        fio = std::fopen(file, "rb+");
        if (fio == nullptr)
        {
            fio = std::fopen(file, "wb+");
            std::fwrite(std::addressof(last), sizeof(last), 1, fio);
        }
        else
            std::fread(std::addressof(last), sizeof(last), 1, fio);
    }

    void cut(int x)
    {
        if (rear == x)
            rear = e[x].epre;

        if (e[x].epre)
            e[e[x].epre].enxt = e[x].enxt;
        if (e[x].enxt)
            e[e[x].enxt].epre = e[x].epre;

        e[x].epre = e[x].enxt = 0;
    }

    void release(int x)
    {
        std::fseek(fio, e[x].pos, SEEK_SET);
        std::fwrite(std::addressof(e[x].node), sizeof(e[x].node), 1, fio);
    }

    ~MemoryPool() 
    {
        while (front)
        {
            release(front);
            front = e[front].enxt;
        }
        std::fseek(fio, 0, SEEK_SET);
        std::fwrite(std::addressof(last), sizeof(last), 1, fio);
        std::fclose(fio);
    }

    int get_file_end()
    {
        std::fseek(fio, 0, SEEK_END);
        return std::ftell(fio);
    }

    int get_nxt_pos()
    {
        return ~last ? last : get_file_end();
    }

    void move_to_front(int x)
    {
        if (front == x)
            return;

        e[x].enxt = front;
        if (front)
            e[front].epre = x;
        front = x;
    }

    T& cache_read(const int pos, int dep)
    {
        int x = find(pos);
        
        if (x)
        {
            if (x != front)
            {
                cut(x);
                move_to_front(x);
            }
            return e[x].node;
        }

        if (tot < maxn)
        {
            x = mpinsert(pos);
            if (tot == 1)
                rear = x;
        }
        else
        {
            x = rear;
            int tmp = rear;

            cut(x);
            release(x);

            if (e[x].pre)
                e[e[x].pre].nxt = e[x].nxt;
            if (e[x].nxt)
                e[e[x].nxt].pre = e[x].pre;
            else
                elast[e[x].pos % mod] = e[x].pre;

            x = mpinsert(pos, tmp);
        }

        move_to_front(x);

        
        std::fseek(fio, e[x].pos, SEEK_SET);
        std::fread(std::addressof(e[x].node), sizeof(e[x].node), 1, fio);
        return e[x].node;
    }

    T& read(const int pos, int dep)
    {
        return cache_read(pos, dep);
    }

    void read(const int pos, int &p, int dep)
    {
        std::fseek(fio, pos, SEEK_SET);
        std::fread(std::addressof(p), sizeof(p), 1, fio);
    }

    void write(const int pos, T &p, int dep)
    {
        cache_read(pos, dep) = p;
    }

    void write(const int pos, int p, int dep, int offset)
    {
        if (offset)
            cache_read(pos, dep).preptr = p;
        else
            cache_read(pos, dep).nxtptr = p;
    }

    template<typename TT>
    void file_read(const int pos, TT &p)
    {
        std::fseek(fio, pos, SEEK_SET);
        std::fread(std::addressof(p), sizeof(p), 1, fio);
    }

    template<typename TT>
    void file_write(const int pos, TT &p)
    {
        std::fseek(fio, pos, SEEK_SET);
        std::fwrite(std::addressof(p), sizeof(p), 1, fio);
    }

    void insert(T &p, int dep)
    {
        if (~last)
        {
            cache_read(last, dep) = p;
            std::fseek(fio, last + sizeof(T), SEEK_SET);
            std::fread(std::addressof(last), sizeof(last), 1, fio);
        }
        else
        {
            std::fseek(fio, 0, SEEK_END);
            std::fwrite(std::addressof(p), sizeof(p), 1, fio);
            int tmp = -1;
            std::fwrite(std::addressof(tmp), sizeof(tmp), 1, fio);
        }
    }

    template<typename TT>
    void file_insert(TT &p)
    {
        if (~last)
        {
            std::fseek(fio, last, SEEK_SET);
            std::fwrite(std::addressof(p), sizeof(T), 1, fio);
            std::fread(std::addressof(last), sizeof(last), 1, fio);
        }
        else
        {
            std::fseek(fio, 0, SEEK_END);
            std::fwrite(std::addressof(p), sizeof(T), 1, fio);
            int tmp = -1;
            std::fwrite(std::addressof(tmp), sizeof(tmp), 1, fio);
        }
    }

    void erase(int pos)
    {
        std::fseek(fio, pos + sizeof(T), SEEK_SET);
        std::fwrite(std::addressof(last), sizeof(last), 1, fio);
        last = pos;
    }

    void clear()
    {
        tot = 0; last = -1; front = rear = 0;
        memset(elast, 0, sizeof(elast));

        std::fclose(fio);
        fio = std::fopen(file, "wb+");
        std::fwrite(std::addressof(last), sizeof(last), 1, fio);
    }
};

#endif
