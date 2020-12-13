#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <errno.h>

typedef struct item
{
    double value;
    double weight;
} Item;

typedef struct itemset
{
    int number;
    Item *item;
    unsigned char *flag;
} Itemset;

Itemset *init_itemset(int number, int seed);
void free_itemset(Itemset *list);
Itemset *load_itemset(char *filename);
void print_itemset(const Itemset *list);
void save_itemset(char *filename);
double solve(Itemset *list, double capacity);
double search(int index, Itemset *list, double capacity, double sum_v, double sum_w);
int load_int(const char *argvalue);
double load_double(const char *argvalue);

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

    double total = solve(items, W);

    printf("----\nbest solution:\n");
    printf("value: %4.1f\n", total);
    for (int i = 0; i < n; ++i)
        if (items->flag[i] == 1)
            printf("item: %d  weight %5.1f  value %5.1f\n", i, items->item[i].weight, items->item[i].value);

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
        item[i].value = 0.1 * (rand() % 200);
        item[i].weight = 0.1 * (rand() % 200 + 1);
    }
    *list = (Itemset){.number = number, .item = item};
    return list;
}

void free_itemset(Itemset *list)
{
    free(list->item);
    free(list->flag);
    free(list);
}

void print_itemset(const Itemset *list)
{
    int n = list->number;
    const char *format = "v[%d] = %4.1f, v[%d] = %4.1f\n";
    for (int i = 0; i < n; i++)
    {
        printf(format, i, list->item[i].value, i, list->item[i].weight);
    }
    printf("----\n");
}

double solve(Itemset *list, double capacity)
{
    list->flag = (unsigned char *)calloc(list->number, sizeof(unsigned char));
    double max_value = search(0, list, capacity, 0.0, 0.0);

    return max_value;
}

double search(int index, Itemset *list, double capacity, double sum_v, double sum_w)
{
    int max_index = list->number;
    unsigned char *flags = list->flag;
    assert(index >= 0 && sum_v >= 0 && sum_w >= 0);
    if (index == max_index)
    {
        const char *format_ok = ", total_value = %5.1f, total_weight = %5.1f\n";
        const char *format_ng = ", total_value = %5.1f, total_weight = %5.1f NG\n";
        for (int i = 0; i < max_index; i++)
        {
            printf("%d", flags[i]);
        }
        if (sum_w < capacity)
        {
            printf(format_ok, sum_v, sum_w);
            return sum_v;
        }
        else
        {
            printf(format_ng, sum_v, sum_w);
            return 0;
        }
    }

    flags[index] = 0;
    const double v0 = search(index + 1, list, capacity, sum_v, sum_w);

    flags[index] = 1;
    double v1;
    if (sum_w + list->item[index].weight > capacity)
        v1 = 0;
    else
        v1 = search(index + 1, list, capacity, sum_v + list->item[index].value, sum_w + list->item[index].weight);

    if (v0 > v1)
    {
        flags[index] = 0;
        return v0;
    }
    else
    {
        flags[index] = 1;
        return v1;
    }
}
