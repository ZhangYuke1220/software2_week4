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
} Itemset;

Itemset *init_itemset(int number, int seed);
void free_itemset(Itemset *list);
Itemset *load_itemset(char *filename);
void print_itemset(const Itemset *list);

double solve(const Itemset *list, double capacity);
double search(int index, const Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w);
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

Itemset *load_itemset(char *filename)
{
    Itemset *itemset = NULL;
    Item *items = NULL;
    FILE *fp = NULL;
    int n = 0;
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "%s: cannot open file.\n", filename);
        exit(1);
    }

    fread(&n, sizeof(int), 1, fp);
    assert(n > 0);
    itemset = (Itemset *)malloc(sizeof(Itemset));
    items = (Item *)malloc(sizeof(Item) * n);

    for (int i = 0; i < n; i++)
    {
        fread(&items[i].value, sizeof(double), 1, fp);
        fread(&items[i].weight, sizeof(double), 1, fp);
    }
    *itemset = (Itemset){.number = n, .item = items};
    fclose(fp);

    return itemset;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "usage: %s <filename><number>\n", argv[0]);
        exit(1);
    }

    const int max_items = 100;
    Itemset *items = load_itemset(argv[1]);
    const double max_weight = atof(argv[2]);
    const int n = items->number;
    assert(n <= max_items); 
    assert(max_weight >= 0);

    double check_value = 0;
    double check_weight = 0;
    for (int i=0; i<n; ++i)
    {
        check_value = items->item[i].value;
        check_weight = items->item[i].weight;
        assert(check_value >= 0.0);
        assert(check_weight >= 0.0);
    }

    printf("max capacity: W = %.f, # of items: %d\n", max_weight, n);
    print_itemset(items);

    double total = solve(items, max_weight);

    printf("----\nbest solution:\n");
    printf("value: %4.1f\n", total);

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

double solve(const Itemset *list, double capacity)
{
    unsigned char *flags = (unsigned char *)calloc(list->number, sizeof(unsigned char));
    double max_value = search(0, list, capacity, flags, 0.0, 0.0);
    free(flags);
    return max_value;
}

double search(int index, const Itemset *list, double capacity, unsigned char *flags, double sum_v, double sum_w)
{
    int max_index = list->number;
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
    const double v0 = search(index + 1, list, capacity, flags, sum_v, sum_w);

    flags[index] = 1;
    double v1;
    if (sum_w + list->item[index].weight > capacity)
        v1 = 0;
    else
        v1 = search(index + 1, list, capacity, flags, sum_v + list->item[index].value, sum_w + list->item[index].weight);

    return (v0 > v1) ? v0 : v1;
}
