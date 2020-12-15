#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

typedef struct
{
    int x;
    int y;
} City;

typedef struct
{
    int width;
    int height;
    char **dot;
} Map;

typedef struct
{
    int *route;
    double distance;
} Answer;


int max(const int a, const int b)
{
    return (a > b) ? a : b;
}

void draw_line(Map map, City a, City b);
void draw_route(Map map, City *city, int n, const int *route);
void plot_cities(FILE *fp, Map map, City *city, int n, const int *route);
double distance(City a, City b);


Map init_map(const int width, const int height);
void free_map_dot(Map m);
City *load_cities(const char *filename, int *n);

Answer solve(City *city, int n, int m, int *route);
double total_distance(City *city, int *route, int n);
Answer hillclimb(City *city, int n, int *route);
void init_random_route(int *route, int n);
void copy_list(int *list1, int *list2, int n);

Map init_map(const int width, const int height)
{
    char **dot = (char **)malloc(width * sizeof(char *));
    char *tmp = (char *)malloc(width * height * sizeof(char));
    for (int i = 0; i < width; i++)
        dot[i] = tmp + i * height;
    return (Map){.width = width, .height = height, .dot = dot};
}

void free_map_dot(Map m)
{
    free(m.dot[0]);
    free(m.dot);
}

City *load_cities(const char *filename, int *n)
{
    City *city;
    FILE *fp;
    if ((fp = fopen(filename, "rb")) == NULL)
    {
        fprintf(stderr, "%s: cannot open file.\n", filename);
        exit(1);
    }
    fread(n, sizeof(int), 1, fp);
    city = (City *)malloc(sizeof(City) * *n);
    for (int i = 0; i < *n; i++)
    {
        fread(&city[i].x, sizeof(int), 1, fp);
        fread(&city[i].y, sizeof(int), 1, fp);
    }
    fclose(fp);
    return city;
}

int main(int argc, char **argv)
{
    const int width = 70;
    const int height = 40;
    const int max_cities = 100;
    Map map = init_map(width, height);

    FILE *fp = stdout;
    if (argc != 3)
    {
        fprintf(stderr, "Usage: %s <city file><number of random solutions>\n", argv[0]);
        exit(1);
    }

    int n;
    City *city = load_cities(argv[1], &n);
    assert(n > 1 && n <= max_cities);

    const int random_route_number = atoi(argv[2]);
    assert(random_route_number > 0);

    plot_cities(fp, map, city, n, NULL);

    int route[n];

    Answer ans = solve(city, n, random_route_number, route);
    plot_cities(fp, map, city, n, ans.route);
    printf("total distance = %f\n", ans.distance);
    for (int i = 0; i < n; i++)
    {
        printf("%d -> ", ans.route[i]);
    }
    printf("0\n");

    free(city);

    return 0;
}

void draw_line(Map map, City a, City b)
{
    const int n = max(abs(a.x - b.x), abs(a.y - b.y));
    for (int i = 1; i <= n; i++)
    {
        const int x = a.x + i * (b.x - a.x) / n;
        const int y = a.y + i * (b.y - a.y) / n;
        if (map.dot[x][y] == ' ')
            map.dot[x][y] = '*';
    }
}

void draw_route(Map map, City *city, int n, const int *route)
{
    if (route == NULL)
        return;

    for (int i = 0; i < n; i++)
    {
        const int c0 = route[i];
        const int c1 = route[(i + 1) % n];
        draw_line(map, city[c0], city[c1]);
    }
}

void plot_cities(FILE *fp, Map map, City *city, int n, const int *route)
{
    fprintf(fp, "----------\n");

    memset(map.dot[0], ' ', map.width * map.height);

    for (int i = 0; i < n; i++)
    {
        char buf[100];
        sprintf(buf, "C_%d", i);
        for (int j = 0; j < strlen(buf); j++)
        {
            const int x = city[i].x + j;
            const int y = city[i].y;
            map.dot[x][y] = buf[j];
        }
    }

    draw_route(map, city, n, route);

    for (int y = 0; y < map.height; y++)
    {
        for (int x = 0; x < map.width; x++)
        {
            const char c = map.dot[x][y];
            fputc(c, fp);
        }
        fputc('\n', fp);
    }
    fflush(fp);
}

double distance(City a, City b)
{
    const double dx = a.x - b.x;
    const double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

Answer solve(City *city, int n, int m, int *route)
{
    srand(time(NULL));
    Answer ans_dis = {.distance = 1.0E10, .route = NULL};
    Answer pos_dis = {.distance = 1.0E10, .route = NULL};

    for (int i=0; i<m; ++i)
    {
        init_random_route(route, n);
        pos_dis = hillclimb(city, n, route);
        if (pos_dis.distance < ans_dis.distance)
            ans_dis = pos_dis;
    }
    return ans_dis;
}

void init_random_route(int *route, int n)
{
    int count = 1;
    int random;
    route[0] = 0;
    while (count < n)
    {
        label :
        random = rand() % n;
        for (int i=0; i<count; ++i)
            if (random == route[i])
                goto label;
        route[count] = random;
        count++;
    }
}

void copy_list(int *list1, int *list2, int n)
{
    for (int i=0; i<n; i++)
        list1[i] = list2[i];
}

double total_distance(City *city, int *route, int n)
{
    double total = 0.0;

    for(int i=1; i<n; ++i)
        total += distance(city[route[i]], city[route[i-1]]);
    total += distance(city[route[0]], city[route[n-1]]);

    return total;
}

Answer hillclimb(City *city, int n, int *route)
{
    double distance = total_distance(city, route, n);
    Answer *ans = malloc(sizeof(int) * n + sizeof(double));
    int temp;
    ans->distance = 1.0E5;
    ans->route = (int *)malloc(sizeof(int) * n);
    for (int i=1; i<n-1; ++i)
        for (int j=i+1; j<n; ++j)
        {
            temp = route[i];
            route[i] = route[j];
            route[j] = temp;
            distance = total_distance(city, route, n);
            if (distance < ans->distance)
            {
                ans->distance = distance;
                copy_list(ans->route, route, n);
            }
        }

    return *ans;
}