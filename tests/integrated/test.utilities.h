
#ifndef __TEST_UTILITIES_H__
#define __TEST_UTILITIES_H__

#include <stdio.h>


static inline
void print_matrix_mxn(const double *matrix, const int m, const int n) {
  for (int i0 = 0; i0 < m; i0++) {
    for (int i1 = 0; i1 < n; i1++) {
      printf("%6.2f ", matrix[i1 + n*(i0)]);
    }
    printf("\n");
  }
  printf("\n");
}

static inline
void print_sqmatrix(const double *matrix, const int rows) {
  print_matrix_mxn(matrix, rows, rows);
}

static inline
void print_tensor_3(const double *tensor, const int d) {
  for (int i0 = 0; i0 < d; i0++) {
    for (int i1 = 0; i1 < d; i1++) {
      for (int i2 = 0; i2 < d; i2++) {
        printf("%6.2f ", tensor[i2 + d*(i1 + d*(i0))]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

static inline
void print_tensor_3d(const double *tensor, const int d0, 
		     const int d1, const int d2) {
  for (int i0 = 0; i0 < d0; i0++) {
    for (int i1 = 0; i1 < d1; i1++) {
      for (int i2 = 0; i2 < d2; i2++) {
        printf("%6.2f ", tensor[i2 + d2*(i1 + d1*(i0))]);
      }
      printf("\n");
    }
    printf("\n");
  }
}

#endif /* __TEST_UTILITIES_H__ */
