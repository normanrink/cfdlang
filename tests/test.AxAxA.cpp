
#include <stdlib.h>
#include <stdio.h>

#include <string>


#include "Runtime/TensorContext.h"


static void print_matrix_mxn(const double *matrix, const int m, const int n) {
  for (int i0 = 0; i0 < m; i0++) {
    for (int i1 = 0; i1 < n; i1++) {
      printf("%6.2f ", matrix[i1 + n*(i0)]);
    }
    printf("\n");
  }
  printf("\n");
}

static void print_tensor_3(const double *tensor, const int d) {
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

int main(int argc, char** argv) {
  if (argc != 3)
    exit(1);

  const int m = atoi(argv[1]);
  const std::string m_str(argv[1]);

  const int n = atoi(argv[2]);
  const std::string n_str(argv[2]);

  const std::string Source = std::string(
    "type matrix     : [" + m_str + " " + n_str + "]\n"
    "type tensorIN   : [" + n_str + " " + n_str + " " + n_str + "]\n"
    "type tensorOUT  : [" + m_str + " " + m_str + " " + m_str + "]\n"
    "                                        \n"
    "var input A : matrix                    \n"
    "var input u  : tensorIN                 \n"
    "var output v : tensorOUT                \n"
    "                                        \n"
    "v = A # A # A # u . [[1 6] [3 7] [5 8]] \n");

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true,
                                       /* graphCodeGen */false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  double A[m*n],
         u[n*n*n],
         v[m*m*m];

  for (int ri = 0; ri < m; ri++) {
    for (int ci = 0; ci < n; ci++) {
      A[ci+n*(ri)] = (ci == ri) ? 1.0 : 0.0;
    }
  }

  for (int i0 = 0; i0 < n; i0++) {
    for (int i1 = 0; i1 < n; i1++) {
      for (int i2 = 0; i2 < n; i2++) {
        u[i2 + n*(i1 + n*(i0))] = (double)(i0+i1+i2);
      }
    }
  }

  TC.initExecution(&eh, &kh);
  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "u", &u[0]);
  TC.addArgument(&eh, "v", &v[0]);

  printf("INPUT \'u\' BEFORE:\n");
  print_tensor_3(u, n);

  printf("INPUT \'A\' BEFORE:\n");
  print_matrix_mxn(A, m, n);

  TC.execute(&eh);

  printf("INPUT \'u\' AFTER:\n");
  print_tensor_3(u, n);

  printf("INPUT \'A\' AFTER:\n");
  print_matrix_mxn(A, m, n);

  printf("OUTPUT \'v\':\n");
  print_tensor_3(v, m);

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
