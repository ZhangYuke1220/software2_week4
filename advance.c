#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

typedef struct item
{
    int arrange;
    double value;
    double weight;
} Item;

typedef struct itemset
{
    int number;
    Item *item;
} Itemset;

typedef struct ans
{
    double value;
    unsigned char *flags;
} Answer;

Itemset *init_itemset(int number, int seed);
void free_itemset(Itemset *list);
Itemset *load_itemset(char *filename);
void print_itemset(Itemset *list);
void save_itemset(char *filename);

Answer solve(Itemset *list, double capacity);
Answer greedy_search(int index, Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w);

int load_int(const char *argvalue);
double load_double(const char *argvalue);

void quick_sort(Itemset *list, int low, int high);
void swap(Item *i, Item *j);

int load_int(const char *argvalue)
{
    long nl;
    char *e;
    errno = 0;
    nl = strtol(argvalue, &e, 10);
    if (errno == ERANGE)
    {
        fprintf(stderr, "%s: %s\n", argvalue, strerror(errno));
        exit(1);
    }
    if (*e != '\0')
    {
        fprintf(stderr, "%s: an irregular character '%c' is detected.\n", argvalue, *e);
        exit(1);
    }
    return (int)nl;
}
double load_double(const char *argvalue)
{
    double ret;
    char *e;
    errno = 0;
    ret = strtod(argvalue, &e);
    if (errno == ERANGE)
    {
        fprintf(stderr, "%s: %s\n", argvalue, strerror(errno));
        exit(1);
    }
    if (*e != '\0')
    {
        fprintf(stderr, "%s: an irregular character '%c' is detected.\n", argvalue, *e);
        exit(1);
    }
    return ret;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <the number of items (int)> <max capacity (double)>\n", argv[0]);
        exit(1);
    }

    const int max_items = 100;

    const int n = load_int(argv[1]);
    assert(n <= max_items);

    const double W = load_double(argv[2]);
    assert(W >= 0.0);

    printf("max capacity: W = %.f, # of items: %d\n", W, n);

    int seed = 1;
    Itemset *items = init_itemset(n, seed);
    print_itemset(items);

    Answer a = solve(items, W);

    printf("----\nbest solution:\n");
    printf("value: %4.1f\n", a.value);
    for (int i = 0; i < items->number; i++)
    {
        printf("%d", a.flags[i]);
    }
    printf("\n");
    free(a.flags);
    a.flags = NULL;

    free_itemset(items);
    return 0;
}

Itemset *init_itemset(int number, int seed)
{
    Itemset *list = (Itemset *)malloc(sizeof(Itemset));

    Item *item = (Item *)malloc(sizeof(Item) * number);

    srand(seed);
    for (int i = 0; i < number; i++)
    {
        item[i].arrange = i+1;
        item[i].value = 0.1 * (rand() % 200);
        item[i].weight = 0.1 * (rand() % 200 + 1);
    }
    *list = (Itemset){.number = number, .item = item};
    return list;
}

void free_itemset(Itemset *list)
{
    free(list->item);
    free(list);
}

void print_itemset(Itemset *list)
{
    int n = list->number;
    const char *format = "No.%d v[%d] = %4.1f, w[%d] = %4.1f\n";
    for (int i = 0; i < n; i++)
    {
        printf(format, list->item[i].arrange, i, list->item[i].value, i, list->item[i].weight);
    }
    printf("----\n");
}

Answer solve(Itemset *list, double capacity)
{
    unsigned char *flags = (unsigned char *)malloc(sizeof(unsigned char) * list->number);
    Answer max_value = greedy_search(0, list, capacity, flags, 0.0, 0.0);
    return max_value;
}

Answer greedy_search(int index, Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w)
{
    int choosed[list->number];
    for (int i=0; i<list->number; ++i)
    {
        choosed[i] = 0;
        flags[i] = 0;
    }
    quick_sort(list, 0, list->number - 1);
    print_itemset(list);
    for (int i = list->number-1; i>=0; --i)
    {
        if (list->item[i].weight < capacity)
        {
            choosed[i] = list->item[i].arrange;
            capacity -= list->item[i].weight;
            sum_w += list->item[i].weight;
            sum_v += list->item[i].value;
        }
        else
            continue;
    }

    for (int i=0; i<list->number; ++i)
        if (choosed[i] != 0)
            flags[choosed[i]-1] = 1;

    return (Answer){.flags = flags, .value = sum_v};
}

void swap(Item *i, Item *j)
{
    Item trans;

    trans = *i;
    *i = *j;
    *j = trans;
}

void quick_sort(Itemset *itemset, int low, int high)
{
    Item pivot = itemset->item[low];
    int i = low, j = high;

    if (low >= high)
        return;
    while (low < high)
    {
        while (low < high && pivot.value <= itemset->item[high].value)
            --high;
        if (pivot.value > itemset->item[high].value)
        {
            swap(&itemset->item[low], &itemset->item[high]);
            ++low;
        }
        while (low < high && pivot.value >= itemset->item[low].value)
            ++low;
        if (pivot.value < itemset->item[low].value)
        {
            swap(&itemset->item[low], &itemset->item[high]);
            --high;
        }
    }
    quick_sort(itemset, i, low-1);
    quick_sort(itemset, low+1, j);
}