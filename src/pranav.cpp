#include <bits/stdc++.h>
#include <stdlib.h>
using namespace std;

class kvstore
{
public:
    struct Node
    {
        struct Node *l;
        struct Node *r;
        int left_children;
        string key, value;
    };

    struct Node *root = NULL;
    bool is_overwritten = false;
    bool is_found_flag = true;

    int num_nodes = 0;

    struct Node *makeNode(string key, string value)
    {
        num_nodes++;
        struct Node *tmp = new Node;
        tmp->left_children = 0;
        tmp->l = tmp->r = 0;
        tmp->key = key;
        tmp->value = value;
        return tmp;
    }

    struct Node *insert(struct Node *rt, string key, string value, bool should_increment)
    {
        if (!rt)
            return makeNode(key, value);

        if (key < rt->key)
        {
            if (should_increment)
                rt->left_children++;
            rt->l = insert(rt->l, key, value, should_increment);
        }
        else if (key > rt->key)
            rt->r = insert(rt->r, key, value, should_increment);
        else
        {
            rt->value = value;
            is_overwritten = true;
        }

        return rt;
    }

    string search(struct Node *rt, string key)
    {
        if (!rt)
        {
            is_found_flag = false;
            return "";
        }
        if (rt->key == key)
            return rt->value;
        else if (key < rt->key)
            return search(rt->l, key);
        else
            return search(rt->r, key);
    }

    struct Node *getnth(struct Node *rt, int n)
    {
        if (rt == NULL)
        {
            return NULL;
        }
        if (rt->left_children == n)
        {
            return rt;
        }
        else if (rt->left_children < n)
        {
            return getnth(rt->r, n - (rt->left_children + 1));
        }
        else
        {
            return getnth(rt->l, n);
        }
    }

    struct Node *delHelper(struct Node *rt)
    {
        if (!rt || !rt->r)
            return 0;

        rt = rt->r;

        while (rt->l)
            rt = rt->l;

        return rt;
    }

    struct Node *del(struct Node *rt, string key)
    {
        if (!rt)
            return 0;
        if (key < rt->key)
        {
            rt->left_children--;
            rt->l = del(rt->l, key);
        }
        else if (key > rt->key)
            rt->r = del(rt->r, key);
        else
        {
            if (!rt->l && !rt->r)
            {
                delete rt;
                return 0;
            }

            if (!rt->r)
            {
                struct Node *tmp = rt->l;
                delete rt;
                return tmp;
            }

            if (!rt->l)
            {
                struct Node *tmp = rt->r;
                delete rt;
                return tmp;
            }

            struct Node *succ = delHelper(rt);
            rt->key = succ->key;
            rt->value = succ->value;
            rt->r = del(rt->r, rt->key);
        }

        return rt;
    }

    bool get(string key)
    {
        string res = search(root, key);
        if (is_found_flag)
        {
            //value is valid
            return true;
        }
        is_found_flag = true;
        return false;
    }

    bool put(string key, string value)
    {
        bool should_increment = !get(key);
        root = insert(root, key, value, should_increment);
        bool retval = is_overwritten;
        if (!is_overwritten)
        {
            // num_nodes++;
        }
        is_overwritten = false;
        return retval;
    }

    bool del(string key)
    {
        bool key_exists = get(key);
        if (!key_exists)
        {
            num_nodes--;
            return false;
        }
        root = del(root, key);
        return true;
    }

    pair<string, string> get(int N)
    {
        struct Node *nth = getnth(root, N);
        if (nth == NULL)
        {
            return make_pair("", "");
        }
        return make_pair(nth->key, nth->value);
    }

    bool del(int N)
    {
        pair<string, string> nth = get(N);
        if (nth.first == "")
        {
            return false;
        }
        del(nth.first);
        return true;
    }
};

// int main()
// {
//     kvstore kv;

//     string s = "s";
//     for(int i=0;i<26;i++)
//     {
//         string chara = "";
//         chara.push_back((char)('a'+ i));
//         cout<<chara<<" "<<"s"<<endl;
//         kv.put(chara, s);
//     }
//     cout << endl;
//     for(int i=0;i<26;i++)
//     {
//         auto p = kv.get(i);
//         cout<<p.first<<" "<<p.second<<endl;
//     }

//     pair<string, string> res = kv.get(2);
//     cout << res.first << " " << res.second << "\n";

//     kv.del(1);
//     kv.del(1);

//     res = kv.get(1);
//     cout << res.first << " " << res.second << "\n";

//     return 0;
// }
