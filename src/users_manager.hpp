#ifndef _users_manager_hpp
#define _users_manager_hpp

#include "B+Tree.hpp"
#include <vector>
using namespace std;

class user
{
    private:
        int pri;
        char pw[30], name[30], uid[30];

    public:

        user(int _pri = 0, const char* _uid = "", const char* _pw = "", const char* _name = "") : pri(_pri)
        {
            strcpy(pw, _pw);
            strcpy(name, _name);
            strcpy(uid, _uid);
        }

        user &operator = (const user &other)
        {
            if (this == &other) return *this;
            pri = other.pri;
            memcpy(pw, other.pw, sizeof(pw));
            memcpy(name, other.name, sizeof(name));
            memcpy(uid, other.uid, sizeof(uid));
            return *this;
        }

        const char* get_uid() const {return uid;}

        const int get_pri() const {return pri;}

        const bool check_pw(const char* _pw) {return !strcmp(pw, _pw);}
};

void Invalid() {puts("Invalid");}

namespace um
{
    enum privilege {base = 0, customer = 1, clerk = 3, root = 7};
    vector <user> log_st;
    fstream fio;
    BplusTree bpt_uid("index_users_uid");
    user current_user;

// --------------------------- debug area ---------------------------

    void pwerror()
    {
        return Invalid();
    }

    void logout_limit_exceed()
    {
        return Invalid();
    }

    void user_add_have_no_permission()
    {
        return Invalid();
    }

    void user_add_pri_error()
    {
        return Invalid();
    }

// --------------------------- debug area ---------------------------


    void file_write(user &p)
    {
        fio.seekp(0, ios :: end);
        int pos = fio.tellp();
        fio.write(reinterpret_cast<char *>(&p), sizeof(p));
        bpt_uid.insert(pos, p.get_uid());
    }

    void file_read(const char* uid, user &p)
    {
        int pos = bpt_uid.query(uid);
        fio.seekg(pos, ios :: beg);
        fio.read(reinterpret_cast<char *>(&p), sizeof(p));
    }

    void init()
    {
        log_st.clear();
        log_st.push_back(current_user);
        fstream fin("storage_users", ios :: in | ios :: binary);
        if (!fin.is_open())
        {
            fstream fout("storage_users", ios :: out | ios :: binary);
            fout.close();
        }

        fio.open("storage_users", ios :: in | ios :: out | ios :: binary);
        if (!fin.is_open()) 
        {
            user root(7, "root", "sjtu", "root");
            file_write(root);
        }
        fin.close();
    }

    void login(const char* uid, const char* pw = nullptr)
    {
        user now; file_read(uid, now);
        if (!pw && current_user.get_pri() <= now.get_pri()) return Invalid();

        if (!pw || now.check_pw(pw)) 
        {
            log_st.push_back(now);
            current_user = now;
        }
        else pwerror();
    }

    void logout()
    {
        if (log_st.size() <= 1) return logout_limit_exceed();

        log_st.pop_back();
        current_user = log_st.back();
    }

    void user_add(const char* uid, const char* pw, int pri, const char* name)
    {
        if (!pri) return user_add_pri_error();
        if (current_user.get_pri() <= pri) return user_add_have_no_permission();
        user now(pri, uid, pw, name);
        file_write(now);
    }

    void user_register(const char* uid, const char*pw, const char* name)
    {
        user now(0, uid, pw, name);
        file_write(now);
    }

    void user_delete(const char* uid)
    {
        bpt_uid.erase()
    }
}

#endif

