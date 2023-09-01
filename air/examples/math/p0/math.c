/*
 * Copyright (C) 2008-2014  GMVIS Skysoft S.A.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * air/LICENSE
 */

#include <math.h>
//#include <printf.h>

/*
 * Print Matrix
 */
void mat_print(char *name, float *A, int m, int n)
{
    int i, j;

    (void)printf(" %s = \n", name);
    for (i = 0; i < m; ++i)
    {
        (void)printf("    ");
        for (j = 0; j < n; ++j)
        {
            (void)printf("%6f ", A[i * n + j]);
        }
        (void)printf("\n");
    }
}

/*
 * Matrix multiplication
 *    C = A * B
 */
void mat_mul(float *A, float *B, int m, int n, int p, float *C)
{
    static int i, j, k;
    for (i = 0; i < m; ++i)
    {
        for (k = 0; k < p; ++k)
        {
            C[i * p + k] = 0.0;
            for (j = 0; j < n; ++j)
            {
                C[i * p + k] += A[i * n + j] * B[j * p + k];
            }
        }
    }
}

/*
 * Matrix trace
 */
float mat_trace(float *A, int p)
{
    int i;
    float trace = 0.0;
    for (i = 0; i < p; ++i)
    {
        trace += A[i * p + i];
    }
    return trace;
}

void entry_point()
{

    (void)printf(" = Partition Math tests ==================================\n");

    (void)printf("\n # Printing numbers\n");
    (void)printf("   %2f %F (expected  1.50000)\n", 1.5, 1.5);
    (void)printf("   %2f %F (expected -1.50000)\n", -1.5, -1.5);
    (void)printf("   %2f %F (expected  0.25000)\n", 0.25, 0.25);
    (void)printf("   %2f %F (expected  4.15000)\n", 4.15, 4.15);
    (void)printf("   %2f %F (expected  4.00159)\n", 4.00159, 4.00159);
    (void)printf("   %2f %F (expected  9.79000)\n", 9.7900, 9.7900);
    (void)printf("   %f %f %f\n", 1.4, 3.2, 9.5);

    volatile float val1 = 1.0;
    volatile float val2 = 2.0;
    volatile float val3 = 4.0;
    volatile float val4 = 2.0;
    volatile float res1 = val1 + val2;
    volatile float res2 = val2 - val1;
    volatile float res3 = val4 * val3;
    volatile float res4 = val3 / val4;

    (void)printf("\n # Addition and Subtraction\n");
    (void)printf("    1 + 2 = %f (expected 3) (int: %i)\n", res1, (int)res1);
    (void)printf("    2 - 1 = %f (expected 1) (int: %i)\n", res2, (int)res2);

    (void)printf("\n # Multiplication and Division\n");
    (void)printf("    2 * 4 = %f (expected 8) (int: %i)\n", res3, (int)res3);
    (void)printf("    4 / 2 = %f (expected 2) (int: %i)\n", res4, (int)res4);

    (void)printf("\n # Matrix playground\n");
    float A[3 * 3] = {1.0, 2.0, 0.0, 0.0, 1.0, 3.0, 5.0, 0.0, 1.0};
    mat_print("A", A, 3, 3);

    float B[3 * 3] = {1, 0, 2, 0, 3, 0, 4, 0, 1};
    mat_print("B", B, 3, 3);

    float C[3 * 3];
    mat_mul(A, B, 3, 3, 3, C);
    mat_print("C = A * B", C, 3, 3);

    (void)printf("trace(C) = %5f\n", mat_trace(C, 3));

    (void)printf(" = Partition Lib Math tests ==============================\n");
    volatile float a, b, c, d, e;
    b = 1.6;
    a = cos(5 + b);
    c = acos(a);
    d = 2;
    e = pow(d, 3);
    printf("a = cos(1.6 + 5) = %f\n"
           "b = 1.6 = %f (runtime)\n"
           "c = acos(a) = 6.6 = %f\n"
           "d = 2 = %f (runtime)\n"
           "e = pow(d, 3) = 8 = %f\n",
           a, b, c, d, e);

    air_syscall_shutdown_module();
}
