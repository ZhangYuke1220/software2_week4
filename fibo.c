#include <stdio.h>
#include <stdlib.h>

typedef struct
{
    int m00;
    int m01;
    int m10;
    int m11;
} Matrix_two;

Matrix_two matrix_mul(Matrix_two mat1, Matrix_two mat2)
{
    Matrix_two mat;

    mat.m00 = mat1.m00 * mat2.m00 + mat1.m01 * mat2.m10;
    mat.m01 = mat1.m00 * mat2.m01 + mat1.m01 * mat2.m11;
    mat.m10 = mat1.m10 * mat2.m00 + mat1.m11 * mat2.m10;
    mat.m11 = mat1.m10 * mat2.m01 + mat1.m11 * mat2.m11;

    return mat;
}

Matrix_two matrix_pow(int n)
{
    Matrix_two mat;
    Matrix_two fibonacci = { .m00 = 1, .m01 = 1, .m10 = 1, .m11 = 0 };
    if (n == 1)
        mat = (Matrix_two){ .m00 = 1, .m01 = 1, .m10 = 1, .m11 = 0 };
    else if (n % 2 == 0)
    {
        mat = matrix_pow(n / 2);
        mat = matrix_mul(mat, mat);
    }
    else if (n % 2 == 1)
    {
        mat = matrix_pow((n-1) / 2);
        mat = matrix_mul(mat, mat);
        mat = matrix_mul(mat, fibonacci);
    }
    
    return mat;
}

int fibo(int n)
{
    if (n == 0 || n == 1)
        return 1;

    Matrix_two matrix = matrix_pow(n - 1);
    return matrix.m00;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr,"usage: %s index of fibonacci sequence.\n", argv[1]);
        return EXIT_FAILURE;
    }

    int n = atoi(argv[1]);
    if (n < 0)
    {
        fprintf(stderr,"usage: %s number must above zero.\n", argv[1]);
        return EXIT_FAILURE;
    }

    int fibonacci = fibo(n);
    printf("index: %d  fibonacci number: %d\n", n, fibonacci);

    return EXIT_SUCCESS;
}
