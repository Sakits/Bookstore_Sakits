#ifndef _users_manager_hpp
#define _users_manager_hpp

#include "B+Tree.hpp"
#include "book.hpp"
#include <vector>
using namespace std;

class user
{
    public:
        int curbk = -1;
    private:
        int pri;
        char pw[31], name[31], uid[31];

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
            curbk = other.curbk;
            memcpy(pw, other.pw, sizeof(pw));
            memcpy(name, other.name, sizeof(name));
            memcpy(uid, other.uid, sizeof(uid));
            return *this;
        }

// --------------------------- debug area ---------------------------

        void print()
        {
            printf("pri:%d\n", pri);
            printf("curbook:%d\n", curbk);
            printf("pw:%s\nname:%s\nuid:%s\n",pw, name, uid);
            puts("");
        }

// --------------------------- debug area ---------------------------


        const char* get_uid() const {return uid;}

        const int get_pri() const {return pri;}

        const bool check_pw(const char* _pw) {return !strcmp(pw, _pw);}

        void changepw(const char* npw) {strcpy(pw, npw);}
};

namespace um
{
    enum privilege {Base = 0, Customer = 1, Clerk = 3, Root = 7};
    vector <user> log_st;
    fstream fio;
    BplusTree bpt_uid("index_users_uid");
    user current_user;

// --------------------------- debug area ---------------------------
    void Invalid() {puts("Invalid");}

    void pwerror() 
    {
        // printf("password_error ");
        return Invalid();
    }

    void logout_limit_exceed() 
    {
        // printf("logout_limit_exceed ");
        return Invalid();
    }

    void user_add_have_no_permission() 
    {
        // printf("user_add_have_no_permission ");
        return Invalid();
    }

    void user_add_pri_error() 
    {
        // printf("user_add_pri_error ");
        return Invalid();
    }
    
    void user_delete_have_no_permission()
    {
        // printf("user_delete_have_no_permission ");
        return Invalid();
    }

    void user_changepw_have_no_permission()
    {
        // printf("user_changepw_have_no_permission ");
        return Invalid();
    }
    
    void user_changepw_wrong_opw()
    {
        // printf("user_changepw_wrong_opw ");
        return Invalid();
    }

    void user_id_has_been_used() 
    {
        // printf("user_id_has_been_used ");
        return Invalid();
    }

    void user_delete_user_not_found() 
    {
        // printf("user_delete_user_not_found ");
        return Invalid();
    }

    void delete_root()
    {
        return Invalid();
    }

// --------------------------- debug area ---------------------------

    void file_write(user &p)
    {
        fio.seekg(0, ios :: end);
        int pos = fio.tellp();
        fio.write(reinterpret_cast<char *>(&p), sizeof(p));
        bpt_uid.insert(pos, p.get_uid());
    }

    bool file_read(const char* uid, user &p)
    {
        int pos = bpt_uid.query(uid);
        if (pos == -1) return 0;
        fio.seekg(pos, ios :: beg);
        fio.read(reinterpret_cast<char *>(&p), sizeof(p));
        return 1;
    }

    void file_write(int pos, user &p)
    {
        fio.seekg(pos, ios :: beg);
        fio.write(reinterpret_cast<char *>(&p), sizeof(p));
    }

    void file_read(int pos, user &p)
    {
        fio.seekg(pos, ios :: beg);
        fio.read(reinterpret_cast<char *>(&p), sizeof(p));
    }

    void init()
    {
        log_st.clear();
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
        user now; if (!file_read(uid, now)) return Invalid();
        if (!pw && current_user.get_pri() <= now.get_pri()) return Invalid();

        // if (pw)
        // {
        //     printf("pw :: %s\n", pw);
        //     now.print();
        // }

        if (!pw || now.check_pw(pw)) 
        {
            log_st.push_back(current_user);
            current_user = now;
        }
        else pwerror();

        current_user.curbk = -1;
    }

    void logout()
    {
        if (!log_st.size()) return logout_limit_exceed();

        current_user = log_st.back();
        log_st.pop_back();
    }

    void user_add(const char* uid, const char* pw, int pri, const char* name)
    {
        if (current_user.get_pri() <= pri) return user_add_have_no_permission();
        if (bpt_uid.query(uid) != -1) return user_id_has_been_used();
        user now(pri, uid, pw, name);
        file_write(now);
    }

    void user_register(const char* uid, const char* pw, const char* name)
    {
        if (bpt_uid.query(uid) != -1) return user_id_has_been_used();
        user now(1, uid, pw, name);
        file_write(now);
    }

    void user_delete(const char* uid)
    {
        if (current_user.get_pri() != Root) return user_delete_have_no_permission();
        if (!strcmp(uid, "root")) return delete_root();
        if (!bpt_uid.erase(uid)) return user_delete_user_not_found();
    }

    void user_changepw(const char* uid, const char* npw, const char* opw = nullptr)
    {
        if (current_user.get_pri() < Customer) return user_changepw_have_no_permission();
        if (!opw && current_user.get_pri() != Root) return user_changepw_have_no_permission();
        

        user now; 
        int pos = bpt_uid.query(uid); 
        if (pos == -1) return Invalid();
        file_read(pos, now);

        if (opw && !now.check_pw(opw)) return user_changepw_wrong_opw();

        now.changepw(npw);

        file_write(pos, now);
    }
}

#endif