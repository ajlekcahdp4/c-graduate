#include "dump.h"
#include "../hashtable/hashtable.h"



struct node_t {
    struct node_t *next;
    unsigned ip1;
    unsigned ip2;
};

struct Hashtable_elem {
    struct node_t *next;
    unsigned capacity;
};

struct Hashtable {
    struct Hashtable_elem **lists_ar;
    struct node_t_t  *list_head;
    struct node_t_t  *list_tail;
    unsigned long long size;
    unsigned long long inserts;
    unsigned long long (*HashFunc)(struct buffer*, struct node_t*);
    int (*Cmp)(struct buffer*, struct node_t*, struct node_t*);
};


void DtStart (FILE* dotfile)
{
    fprintf (dotfile, "digraph G {\n");
    fprintf (dotfile, "rankdir = LR\n");
}

void DtSetTitle(FILE* dotfile, struct Hashtable* HashT)
{
    fprintf(dotfile ,"TITLE [shape=record, color=\"red\", label = \"DUMP of the hashTable\"];\n\n");
    fprintf (dotfile, "size [shape=record, style=rounded, label =\"size | %llu\"];\n", HashT->size);
    fprintf (dotfile, "inserts [shape=record, style=rounded, label = \"inserts | %llu\"];\n", HashT->inserts);
}

void DtSetBuf (FILE *dotfile, struct Hashtable* HashT)
{
    fprintf (dotfile, "\n");
    
    fprintf (dotfile, "subgraph clusterbuffer {\n");
    fprintf (dotfile, "node [style=filled, fillcolor=white];\n");
    fprintf (dotfile, "style=filled;\ncolor=lightgrey");

    for (size_t i = 0; i < HashT->size; i++)
    {
        if (HashT->lists_ar[i])
        {
            fprintf (dotfile, "    Buf%lu [shape=record, label=\"[%lu]\"];\n", i, i);
        }
    }
    fprintf (dotfile, "label=\"buffer\";\n}\n\n");

}

void DtSetNodes (FILE* dotfile, struct buffer *buf, struct node_t *node, struct Hashtable *HashT)
{
    fprintf (dotfile, "Node%p [shape=record, style=filled, fillcolor = \"palegreen\", label=\"adr\\n%p|%u, %u|Hash = %llu|next\\n%p\"];\n", node, node,  node->ip1, node->ip2, HashT->HashFunc(buf, node) % HashT->size, node->next);
}

void DtSetDependence (FILE* dotfile, struct Hashtable *HashT)
{
    struct node_t        *cur       = 0;


    fprintf (dotfile, "\n");
    for (size_t i = 0; i < HashT->size; i++)
    {
        if (HashT->lists_ar[i])
        {
            fprintf (dotfile, "Buf%lu -> Node%p[color=\"red\", constraint=true];\n", i, HashT->lists_ar[i]->next);
        }
    }

    //lists dependencies=======================================================
    for (size_t i = 0; i < HashT->size; i++)
    {
        if (HashT->lists_ar[i])
        {
            cur = HashT->lists_ar[i]->next;
            
            size_t j = 0;
            while (cur->next && j < HashT->lists_ar[i]->capacity - 1)
            {
                fprintf (dotfile, "Node%p->", cur);
                cur = cur->next;
                j++;
            }
            if (cur != HashT->lists_ar[i]->next)
            {
                fprintf (dotfile, "Node%p", cur);
                fprintf(dotfile, "[color=\"red\"];\n");
            }
        }
    }
    fprintf (dotfile, "\n");
}

void DtEnd (FILE* dotfile)
{
    fprintf (dotfile, "}\n");
} 