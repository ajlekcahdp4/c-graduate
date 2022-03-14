#include "hashtable.h"
#include "../dump/dump.h"
#include <string.h>
#include <assert.h>

struct node {
    char* word;
    struct node *next;
};

struct Hashtable {
    struct node **lists_ar;
    struct node  *list_head;
    struct node  *list_tail;
    unsigned long long size;
    unsigned long long inserts;
    unsigned long long (*hash_func)(const char*);
};

struct node *ListInit (void);
size_t ListCount (struct node *top, struct node *aim_node);
int nodecmp (struct node* node1, struct node* node2);
struct node *ListInsert (struct node *last, size_t str_len, char* word);
void DeleteNodeAft (struct Hashtable *HashT, struct node* last);
void DeleteList (struct node* top);
struct Hashtable *FillHashtable (struct Hashtable *HashT, char **buf, size_t N);



//==================================================================================
unsigned long long Hash (const char* str)
{
    unsigned long long hash = 0;
    unsigned long long c = 0;
    while ((c = (unsigned long long)(*str++)) != 0)
        hash = (hash << 5) + (hash << 16) - hash  + c;
    return hash;
}
//==================================================================================
struct node *ListInit (void)
{
    struct node *top = calloc (1, sizeof (struct node));
    return top;
}
//==================================================================================
size_t ListCount (struct node *top, struct node *aim_node)
{
    assert (top);
    assert (aim_node);

    size_t cnt = 0;
    struct node *cur = top;
    while (cur)
    {
        if (cur != top && nodecmp (cur, aim_node) == 0)
            cnt += 1;
        cur = cur->next;
    }
    return cnt;
}
//==================================================================================
int nodecmp (struct node* node1, struct node* node2)
{
    return strcmp (node1->word, node2->word);
}
//==================================================================================

struct Hashtable* HashTableInit (size_t size, unsigned long long (*hash_f)(const char*))
{
    assert (size);
    struct Hashtable* HashT = calloc (1, sizeof(struct Hashtable));
    HashT->list_head        = calloc (1, sizeof (struct node));
    HashT->lists_ar         = calloc (size, sizeof(struct node*));
    assert (HashT);
    assert (HashT->lists_ar);
    assert (HashT->list_head);
    HashT->size      = size;
    HashT->hash_func = hash_f;
    HashT->list_tail = HashT->list_head;

    return HashT;
}
//==================================================================================
struct node *ListInsert (struct node *last, size_t str_len, char* word)
{
    struct node *cur = last;
    if (cur->next == 0)
    {
        cur->next       = calloc (1, sizeof (struct node));
        cur->next->word = calloc (str_len + 1, sizeof (char));

        assert (cur->next);
        assert (cur->next->word);

        memcpy (cur->next->word, word, str_len * sizeof(char));
    }
    else
    {
        struct node *new_node = calloc (1, sizeof (struct node));
        new_node->word        = calloc (str_len + 1, sizeof (char));

        assert (new_node);
        assert (new_node->word);

        memcpy (new_node->word, word, str_len * sizeof (char));
        new_node->next = cur->next;
        cur->next = new_node;
    }
    
    return cur->next;
}
//==================================================================================

void DeleteNodeAft (struct Hashtable *HashT, struct node* last)
{
    assert (last->next);

    if (HashT->list_tail == last->next)
        HashT->list_tail = last;

    struct node* cur = last->next;
    last->next       = cur->next;

    free (cur->word);
    free (cur); 
}

//==================================================================================

void DeleteList (struct node* top)
{
    struct node* cur_node = top;
    struct node* next_node = top;
    while (cur_node->next != 0)
    {
        next_node = cur_node->next;
        free(cur_node->word);
        free(cur_node);
        cur_node = next_node;
    }
    free(cur_node->word);
    free(cur_node);
}

//==================================================================================



struct Hashtable* HashtableInsert (struct Hashtable* HashT, char* word)
{
    assert (HashT);
    assert (word);
    if ((float)HashT->inserts / (float)HashT->size >= 0.7)
    {
        HashT = HashTableResize (HashT);
    }


    size_t str_len = strlen (word);
    unsigned long long hash = HashT->hash_func (word) % HashT->size;

    if (HashT->lists_ar[hash] == 0)
    {
        HashT->lists_ar[hash] = calloc (1, sizeof(struct node));
        assert (HashT->lists_ar[hash]);
        
        HashT->lists_ar[hash]->next = ListInsert (HashT->list_tail, str_len, word);
        HashT->list_tail            = HashT->list_tail->next;
    }
    else
    {
        struct node* cur = HashT->lists_ar[hash];

        while (cur != HashT->list_tail && HashT->hash_func(cur->next->word) % HashT->size == hash)
            cur = cur->next;
        if (cur == HashT->list_tail)
        {
            ListInsert (HashT->list_tail, str_len, word);
            HashT->list_tail = HashT->list_tail->next;
        }
        else
        {
            ListInsert (cur, str_len, word);
        }
    }

    HashT->inserts += 1;
    return HashT;
}

//==================================================================================



struct Hashtable* HashTableResize (struct Hashtable* HashT)
{
    for (unsigned long long int i = 0; i < HashT->size; i++)
        if(HashT->lists_ar[i] != 0)
            free (HashT->lists_ar[i]);
    free (HashT->lists_ar);
    
    
    HashT-> size *= 2;
    HashT->lists_ar = calloc (HashT->size, sizeof (struct node*));

    char* temp_str = 0;
    size_t str_len = 0;
    struct node *last = HashT->list_tail;
    unsigned long long old_inserts = HashT->inserts;
    struct node* cur = HashT->list_head;
    //+++++++++++++++++++++++++++++++++++++++++
    while (cur->next != last)
    {
        HashT->inserts = 0;
        str_len = strlen (cur->next->word);
        temp_str = calloc (str_len + 1, sizeof (char));
        memcpy (temp_str, cur->next->word, str_len);
        DeleteNodeAft (HashT, cur);
        HashtableInsert (HashT, temp_str);
        free (temp_str);
    }
    HashT->inserts = 0;
    str_len = strlen (cur->next->word);
    temp_str = calloc (str_len + 1, sizeof (char));
    memcpy (temp_str, cur->next->word, str_len);
    DeleteNodeAft (HashT, cur);
    HashtableInsert (HashT, temp_str);
    free (temp_str);
    //+++++++++++++++++++++++++++++++++++++++++
    HashT->inserts = old_inserts;
    
    return HashT;
}

//==================================================================================

size_t NumOfWord (struct Hashtable* HashT, char* word)//cringe
{

    size_t N  = 0;
    unsigned long long word_hash = HashT->hash_func (word) % HashT->size;
    struct node* cur_node        = HashT->lists_ar[word_hash];

    if (cur_node == 0)
        return 0;

    cur_node = cur_node->next;
    while (cur_node != HashT->list_tail && HashT->hash_func (cur_node->word) % HashT->size == word_hash)
    {

        if (strcmp(word, cur_node->word) == 0)
            N += 1;
        cur_node = cur_node->next;
    }

    if (strcmp(word, cur_node->word) == 0)
            N += 1;

    return N;
}

size_t NumberOfFour (struct Hashtable *HashT)//идём по хэштаблице и проверяем коллизии
{
    unsigned long long words_hash = 0;
    size_t numb_of_four   = 0;
    size_t cnt            = 0;
    struct node *cur      = 0;
    struct node *cmp_node = 0;
    struct node *top      = 0;
    struct node *last     = 0;

    top = ListInit ();
    last = top;
    for (size_t i = 0; i < HashT->size; i++)
    {
        if (HashT->lists_ar[i])
        {
            printf("---------------\n[%lu]\n", i);
            cur = HashT->lists_ar[i]->next;
            cmp_node = cur;
            words_hash = i;
            while (cmp_node && words_hash == HashT->hash_func (cmp_node->word) % HashT->size)
            {
                if (ListCount (top, cmp_node) == 0)
                {
                    last = ListInsert (last, strlen (cmp_node->word), cmp_node->word);
                    cnt = ListCount (HashT->lists_ar[i], cmp_node);
                    if (cnt > 1)
                    {
                        numb_of_four += cnt;
                        printf("[%lu]+\n", i);
                    }
                    printf ("%lu\n", cnt);
                }           
                cmp_node = cmp_node->next;
            }
        }
    }

    DeleteList (top);


    return numb_of_four;
}


//==================================================================================
void HashTDump (struct Hashtable *HashT, char *name)
{
    FILE *dotfile  = fopen ("dump.dot", "w");

    DtStart (dotfile);
    DtSetTitle (dotfile, HashT);
    struct node *cur = HashT->list_head->next;
    DtSetBuf (dotfile, HashT);
    while (cur != 0)
    {
        DtSetNodes (dotfile, cur, HashT);
        cur = cur->next;
    }
    fprintf (dotfile, "\n\n\n");

    DtSetDependence (dotfile, HashT);

    DtEnd (dotfile);
    fclose (dotfile);
    
    char *command = calloc (100, sizeof(char));
    memcpy (command, "dot dump.dot -T png -o ", 23 * sizeof(char));
    strcat (command, name);
    system (command);
    system ("rm dump.dot");
    free (command);
}
//==================================================================================
struct Hashtable *FillHashtable (struct Hashtable *HashT, char **buf, size_t N)
{
    char   *temp_str = calloc (100, sizeof(char));
    size_t i         = 0;

    for (size_t first = 0; first < N; first++)
    {
        for (size_t second = 0; second < N; second++)
        {
            i = 0;
            while (temp_str[i] != 0)
            {
                temp_str[i] = 0;
                i++;
            }
            if (second != first)
            {
                strcat (temp_str, buf[first]);
                strcat (temp_str, buf[second]);
                HashtableInsert (HashT, temp_str);
            }
        }
    }
    free (temp_str);
    return HashT;
}




//==================================================================================
void DeleteHastable (struct Hashtable* HashT)
{
    DeleteList (HashT->list_head);
    for (unsigned long long int i = 0; i < HashT->size; i++)
        if(HashT->lists_ar[i] != 0)
            free (HashT->lists_ar[i]);
    free (HashT->lists_ar);
    free (HashT);
}