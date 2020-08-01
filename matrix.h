#ifndef _MATRIX_H_
#define _MATRIX_H_

/**
 * @brief Matrix structure
 */
typedef struct __matrix_t {

    /* Pointer to the 2-D array of the elements */
    int **data;
    /* Number of rows */
    int nb_rows;
    /* Number of columns */
    int nb_cols;

} matrix_t;

/**
 * @brief Error enumeration
 */
typedef enum __matrix_err_t {

    /* Successful execution */
    MATRIX_SUCC,
    /* Memory not available for the matrix */
    MATRIX_NO_MEM,
    /* Matrix muliplication operand dimensions invalid */
    MATRIX_DIM_INVD

} matrix_err_t;

matrix_err_t matrix_init(
        matrix_t *p_mat,
        int nb_rows,
        int nb_cols);

matrix_err_t matrix_multiply(
        matrix_t *p_matd,
        matrix_t *p_mats1,
        matrix_t *p_mats2);

void matrix_print(matrix_t *p_mat);

matrix_err_t matrix_get_input(matrix_t *p_mat);

#endif
