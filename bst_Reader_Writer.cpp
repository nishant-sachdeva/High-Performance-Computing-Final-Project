#include <bits/stdc++.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>

using namespace std;

struct Slice
{
    uint8_t size;
    char *data;
};

class kvStore
{
public:
    kvStore(uint64_t max_entries)
    {
        root = NULL;
        is_overwritten = false;
        is_found_flag = true;
        num_nodes = 0;
        overwrit = 0;

        pthread_rwlock_init(&lock,NULL);
    };

    int slicecmp(Slice &a, Slice &b)
    {
        int minsize = min(a.size, b.size);
        int strcmp_result = strncmp(a.data, b.data, minsize);
        if (strcmp_result != 0)
            return strcmp_result;
        else
        {
            return a.size - b.size;
        }
    }

    struct Node
    {
        struct Node *l;
        struct Node *r;
        int left_children;
        Slice key, value;
    };

    struct Node *root;
    bool is_overwritten;
    bool is_found_flag;

    int overwrit;

    int num_nodes;

    pthread_rwlock_t lock;

    struct Node *makeNode(Slice &key, Slice &value)
    {
        num_nodes++;
        struct Node *tmp = (struct Node *)malloc(sizeof(struct Node));
        tmp->left_children = 0;
        tmp->l = tmp->r = 0;
        tmp->key.size = key.size;
        tmp->key.data = (char *)malloc(key.size);
        strncpy(tmp->key.data, key.data, key.size);
        // tmp->key.data[key.size] = '\0';
        tmp->value.size = value.size;

        tmp->value.data = (char *)malloc(value.size);
        strncpy(tmp->value.data, value.data, value.size);
        // tmp->value.data[value.size] = '\0';

        return tmp;
    }

    struct Node *insert(struct Node *rt, Slice &key, Slice &value, bool should_increment)
    {

        if (!rt)
        {
            return makeNode(key, value);
        }
        int strcmp_result = slicecmp(key, rt->key);

        if (strcmp_result < 0)
        {
            if (should_increment)
                rt->left_children++;
            rt->l = insert(rt->l, key, value, should_increment);
        }
        else if (strcmp_result > 0)
            rt->r = insert(rt->r, key, value, should_increment);
        else
        {
            overwrit++;
            rt->value.size = value.size;
            free(rt->value.data);
            rt->value.data = (char *)malloc(value.size);
            strncpy(rt->value.data, value.data, value.size);
            // rt->value.data[value.size] = '\0';
            is_overwritten = true;
        }

        return rt;
    }

    Slice *search(struct Node *rt, Slice &key)
    {
        if (!rt)
        {
            is_found_flag = false;
            return NULL;
        }
        int strcmp_result = slicecmp(key, rt->key);
        if (strcmp_result == 0)
            return &rt->value;
        else if (strcmp_result < 0)
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

    struct Node *del(struct Node *rt, Slice &key)
    {
        if (!rt)
            return 0;

        int strcmp_result = slicecmp(rt->key, key);

        if (strcmp_result > 0)
        {
            rt->left_children--;
            rt->l = del(rt->l, key);
        }
        else if (strcmp_result < 0)
            rt->r = del(rt->r, key);
        else
        {
            if (!rt->l && !rt->r)
            {
                free(rt->key.data);
                free(rt->value.data);
                free(rt);
                return 0;
            }

            if (!rt->r)
            {
                struct Node *tmp = rt->l;
                free(rt->key.data);
                free(rt->value.data);
                free(rt);
                return tmp;
            }

            if (!rt->l)
            {
                struct Node *tmp = rt->r;
                free(rt->key.data);
                free(rt->value.data);
                free(rt);
                return tmp;
            }

            struct Node *succ = delHelper(rt);

            rt->key.size = succ->key.size;
            free(rt->key.data);
            rt->key.data = (char *)malloc(rt->key.size);
            strncpy(rt->key.data, succ->key.data, rt->key.size);

            rt->value.size = succ->value.size;
            free(rt->value.data);
            rt->value.data = (char *)malloc(rt->value.size);
            strncpy(rt->value.data, succ->value.data, rt->value.size);

            rt->r = del(rt->r, rt->key);
        }

        return rt;
    }

    bool get(Slice &key, Slice &value)
    {
        pthread_rwlock_rdlock(&lock);
        Slice *res = search(root, key);
        if (res)
        {
            value.size = res->size;
            value.data = (char *)malloc(value.size);
            strncpy(value.data, res->data, value.size);
            //value is valid
            pthread_rwlock_unlock(&lock);
            return true;
        }
        is_found_flag = true;
        pthread_rwlock_unlock(&lock);
        return false;
    }

    bool put(Slice &key, Slice &value)
    {
        pthread_rwlock_wrlock(&lock);
        bool should_increment = !(search(root, key));
        root = insert(root, key, value, should_increment);
        bool retval = is_overwritten;
        if (!is_overwritten)
        {
            // num_nodes++;
        }
        is_overwritten = false;
        pthread_rwlock_unlock(&lock);
        return retval;
    }

    bool del(Slice &key)
    {
        pthread_rwlock_wrlock(&lock);
        bool key_exists = search(root, key);
        if (!key_exists)
        {
            num_nodes--;
            pthread_rwlock_unlock(&lock);
            return false;
        }
        root = del(root, key);
        pthread_rwlock_unlock(&lock);
        return true;
    }

    bool get(int N, Slice &key, Slice &value)
    {
        pthread_rwlock_rdlock(&lock);
        struct Node *nth = getnth(root, N);
        if (nth == NULL)
        {
            pthread_rwlock_unlock(&lock);
            return false;
        }

        key.size = nth->key.size;
        key.data = (char *)malloc(nth->key.size);
        strncpy(key.data, nth->key.data, key.size);

        value.size = nth->value.size;
        value.data = (char *)malloc(nth->value.size);
        strncpy(value.data, nth->value.data, value.size);

        pthread_rwlock_unlock(&lock);
        return true;
    }

    bool del(int N)
    {
        pthread_rwlock_wrlock(&lock);
        struct Node* nth = getnth(root, N);
        if (!nth)
        {
            pthread_rwlock_unlock(&lock);
            return false;
        }
        root = del(root, nth->key);
        pthread_rwlock_unlock(&lock);
        return true;
    }
};
