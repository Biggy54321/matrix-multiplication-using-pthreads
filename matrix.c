#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <pthread.h>
#include "matrix.h"

/* Number of threads used for multiplying the matrices */
#define MATRIX_NB_OF_THREADS (3)

#ifdef DO_THREADING

/* Structure to be passed as an arg to the thread */
typedef struct __matrix_thread_arg_t {

    matrix_t *p_matd;           /* Destination matrix */
    matrix_t *p_mats1;          /* Source matrix 1 */
    matrix_t *p_mats2;          /* Source matrix 2 */
    int row_start;              /* Starting row index */
    int row_end;                /* Ending row index */

} matrix_thread_arg_t;

#endif

/**
 * @brief Allocates memory for the matrix and sets all the elements to zero
 * @param[out] p_mat Pointer to the matrix instance
 * @param[in] nb_rows Number of rows
 * @param[in] nb_cols Number of columns
 * @return Matrix error code
 */
matrix_err_t matrix_init(
        matrix_t *p_mat,
        int nb_rows,
        int nb_cols) {

    int row_i;

    /* Check if pointer to matrix instance is not null */
    assert(p_mat);
    /* Check if the dimensions are non zero */
    assert(nb_rows);
    assert(nb_cols);

    /* Allocate pointers equal to the number of rows */
    p_mat->data = (int **)malloc(nb_rows * sizeof(int *));

    /* Return error is memory is not allocated */
    if (!p_mat->data) {
        return MATRIX_NO_MEM;
    }

    /* For every row */
    for (row_i = 0; row_i < nb_rows; row_i++) {

        /* Allocate integers equal to the number of columns */
        p_mat->data[row_i] = (int *)malloc(nb_cols * sizeof(int));

        /* Return error is memory is not allocated */
        if (!p_mat->data[row_i]) {
            return MATRIX_NO_MEM;
        }

        /* Set the entire row to zero */
        memset(p_mat->data[row_i], 0, nb_cols * sizeof(int));
    }

    /* Initialize the number of rows and columns */
    p_mat->nb_rows = nb_rows;
    p_mat->nb_cols = nb_cols;

    /* Return success */
    return MATRIX_SUCC;
}

#ifdef DO_THREADING

/**
 * @brief Threaded matrix muliplication
 * @param[in] arg Points to the thread argument instance
 */
static void *_thread_matrix_multiply(void *arg) {

    /* Get the thread arguments */
    matrix_thread_arg_t thr_arg = *(matrix_thread_arg_t *)arg;
    /* Destination matrix */
    matrix_t *p_matd = thr_arg.p_matd;
    /* First source matrix */
    matrix_t *p_mats1 = thr_arg.p_mats1;
    /* Second source matrix */
    matrix_t *p_mats2 = thr_arg.p_mats2;
    /* Starting row index */
    int row_start = thr_arg.row_start;
    /* Ending row index */
    int row_end = thr_arg.row_end;
    /* Matrix indices */
    int i;
    int j;
    int k;

    /* Check if the row starting and ending index are not same */
    if (row_end <= row_start) {

        /* Exit from the thread */
        pthread_exit(NULL);
    }

    /* Starting from the first row to the last row given as an argument */
    for (i = row_start; i < row_end; i++) {

        /* Multiply the given number of rows with the entire second matrix */
        for (j = 0; j < p_mats1->nb_cols; j++) {

            for (k = 0; k < p_matd->nb_cols; k++) {

                /* Multiply in a cache friendly way */
                p_matd->data[i][k] += p_mats1->data[i][j] * p_mats2->data[j][k];
            }
        }
    }

    /* Return from thread */
    pthread_exit(NULL);
}

#endif

#ifdef DO_THREADING
/**
 * @brief Multiplies the matrices pointed by #p_mats1 and #p_mats2 and stores
 *        the result in matrix pointed by #p_matd, using threading
 * @param[out] p_matd Pointer to the destination matrix instance
 * @param[in] p_mats1 Pointer to the first source matrix
 * @param[in] p_mats2 Pointer to the second source matrix
 * @return Matrix error code
 */
matrix_err_t matrix_multiply(
        matrix_t *p_matd,
        matrix_t *p_mats1,
        matrix_t *p_mats2) {

    /* Thread argument */
    matrix_thread_arg_t thr_arg1, thr_arg2, thr_arg3;
    matrix_thread_arg_t thr_arg[MATRIX_NB_OF_THREADS];
    /* Thread structures */
    pthread_t thr1, thr2, thr3;
    pthread_t thr_id[MATRIX_NB_OF_THREADS];
    /* Number of rows of the first source matrix */
    int nb_rows;
    /* Index for traversing the threads */
    int i;

    /* Check if the matrices instance pointer do exists */
    assert(p_matd);
    assert(p_mats1);
    assert(p_mats2);

    /* Check if the dimensions of the source matrices are valid */
    if (p_mats1->nb_cols != p_mats2->nb_rows) {

        /* Return the dimension invalid error */
        return MATRIX_DIM_INVD;
    }

    /* Initialize the destination matrix */
    if (matrix_init(p_matd, p_mats1->nb_rows, p_mats2->nb_cols)) {

        /* Return the no memory error */
        return MATRIX_NO_MEM;
    }

    /* Get the number of rows in the first source matrix */
    nb_rows = p_mats1->nb_rows;

    /* Spawn the threads */
    for (i = 0; i < MATRIX_NB_OF_THREADS; i++) {

        /* If the current thread is not the last one */
        if (i < (MATRIX_NB_OF_THREADS - 1)) {

            /* Include one by number of rows in current thread as argument */
            thr_arg[i] = (matrix_thread_arg_t)
                    {p_matd, p_mats1, p_mats2,
                     i * nb_rows / MATRIX_NB_OF_THREADS,
                     (i + 1) * nb_rows / MATRIX_NB_OF_THREADS};
        }
        /* If the current thread is the last one */
        else {

            /* Include all the rows till the last one as argument */
            thr_arg[i] = (matrix_thread_arg_t)
                    {p_matd, p_mats1, p_mats2,
                     i * nb_rows / MATRIX_NB_OF_THREADS,
                     nb_rows};
        }

        /* Create the thread */
        pthread_create(&thr_id[i], NULL, _thread_matrix_multiply, &thr_arg[i]);
    }

    /* For each thread */
    for (i = 0; i < MATRIX_NB_OF_THREADS; i++) {

        /* Wait for the thread to join */
        pthread_join(thr_id[i], NULL);
    }

    /* Return success */
    return MATRIX_SUCC;
}

#else

/**
 * @brief Multiplies the matrices pointed by #p_mats1 and #p_mats2 and stores
 *        the result in matrix pointed by #p_matd, without using threading
 * @param[out] p_matd Pointer to the destination matrix instance
 * @param[in] p_mats1 Pointer to the first source matrix
 * @param[in] p_mats2 Pointer to the second source matrix
 * @return Matrix error code
 */
matrix_err_t matrix_multiply(
        matrix_t *p_matd,
        matrix_t *p_mats1,
        matrix_t *p_mats2) {

    int i;
    int j;
    int k;

    /* Check if the matrices instance pointer do exists */
    assert(p_matd);
    assert(p_mats1);
    assert(p_mats2);

    /* Check if the dimensions of the source matrices are valid */
    if (p_mats1->nb_cols != p_mats2->nb_rows) {
        return MATRIX_DIM_INVD;
    }

    /* Initialize the destination matrix */
    if (matrix_init(p_matd, p_mats1->nb_rows, p_mats2->nb_cols)) {

        /* Return the no memory error */
        return MATRIX_NO_MEM;
    }

    /* For every row in the first matrix */
    for (i = 0; i < p_mats1->nb_rows; i++) {

        /* FOr every row in the second matrix */
        for (j = 0; j < p_mats1->nb_cols; j++) {

            for (k = 0; k < p_matd->nb_cols; k++) {

                /* Multiply the two matrices in cache friendly way */
                p_matd->data[i][k] += p_mats1->data[i][j] * p_mats2->data[j][k];
            }
        }
    }

    /* Return success */
    return MATRIX_SUCC;
}

#endif

/**
 * @brief Prints the elements of the matrix to the standard input
 * @param[in] p_mat Pointer to the matrix instance
 */
void matrix_print(matrix_t *p_mat) {

    int row_i;
    int col_i;

    /* Check if the pointer to the matrix instance exists */
    assert(p_mat);

    /* For every row of the matrix */
    for (row_i = 0; row_i < p_mat->nb_rows; row_i++) {

        /* For every column of the matrix */
        for (col_i = 0; col_i < p_mat->nb_cols; col_i++) {

            /* Print the element */
            printf("%d ", p_mat->data[row_i][col_i]);
        }

        printf("\n");
    }
}

/**
 * @brief Gets the matrix dimensions and elements from the standard input
 * @param[in] p_mat Pointer to the matrix instance
 * @return Matrix error code
 */
matrix_err_t matrix_get_input(matrix_t *p_mat) {

    int row_i;
    int col_i;
    int nb_rows;
    int nb_cols;

    /* Check if the pointer to the matrix instance exists */
    assert(p_mat);

    /* Get the number of rows */
    scanf("%d", &nb_rows);
    /* Get the number of cols */
    scanf("%d", &nb_cols);

    /* Initialize the matrix */
    if (matrix_init(p_mat, nb_rows, nb_cols)) {

        /* Return the possible error */
        return MATRIX_NO_MEM;
    }

    /* For every row of the matrix */
    for (row_i = 0; row_i < nb_rows; row_i++) {

        /* For every column of the matrix */
        for (col_i = 0; col_i < nb_cols; col_i++) {

            /* Get the element from the standard input */
            scanf("%d", &p_mat->data[row_i][col_i]);
        }
    }

    /* Return success */
    return MATRIX_SUCC;
}
