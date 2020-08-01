#include <stdio.h>
#include "matrix.h"

int main() {

    matrix_t mats1, mats2, matd;
    matrix_err_t ret_val;

    /* Get the first matrix from standard input */
    if (matrix_get_input(&mats1) == MATRIX_NO_MEM) {

        /* Print the error */
        printf("Not enough memory to initialize matrix 1\n");
        return -1;
    }

    /* Get the second matrix from standard input */
    if (matrix_get_input(&mats2) == MATRIX_NO_MEM) {

        /* Print the error */
        printf("Not enough memory to initialize matrix 2\n");
        return -1;
    }

    /* Multiply the two matrices */
    if ((ret_val = matrix_multiply(&matd, &mats1, &mats2)) == MATRIX_DIM_INVD) {

        /* Print the error */
        printf("Matrix muliplication dimension invalid\n");
        return -1;
    }
    else if (ret_val == MATRIX_NO_MEM) {

        /* Print the erro */
        printf("Not enough memory to initialize matrix 3\n");
        return -1;
    }

    /* Print the result matrix */
    matrix_print(&matd);

    return 0;
}
