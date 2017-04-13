
#include <stdlib.h>
#include <stdio.h>

#include <string>


#include "Runtime/TensorContext.h"


static void print_sqmatrix(const double *m, int rows) {
  for (int i = 0; i < (rows*rows); i++) {
    if (i && (i % rows == 0)) printf("\n");

    printf("%6.2f ", m[i]);
  }
  printf("\n");
}

int main(int argc, char** argv) {
  if (argc != 2)
    exit(1);

  const int n = atoi(argv[1]);
  const std::string n_str(argv[1]);

  const std::string Source =
    std::string("type matrix  : [" + n_str + " " + n_str + "]\n"
                "type vector  : [" + n_str + "]\n"
                "                      \n"
                "var input A  : matrix \n"
                "                      \n"
                "var input s  : vector \n"
                "var input t  : vector \n"
                "                      \n"
                "var input U  : matrix \n"
                "var output v : matrix \n"
                "                      \n"
                "v = (A + s # t) / U   \n");

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  TC.initCodeGen(&cgh, Source.c_str(), true, false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh, true);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  double A[n*n],
         U[n*n],
         v[n*n],
         s[n];

  for (int i = 0; i < n*n; i++) {
    A[i] = 0.0;
    U[i] = 2.0;
  }  for (int i = 0; i < n; i++) {
    s[i] = (double)i;
  }

  TC.initExecution(&eh, &kh);
  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "U", &U[0]);
  TC.addArgument(&eh, "s", &s[0]);
  TC.addArgument(&eh, "t", &s[0]);
  TC.addArgument(&eh, "v", &v[0]);

  TC.execute(&eh);
  print_sqmatrix(v, n);
  printf("\n");

  for (int i = 0; i < (n*n); i++)
    A[i] = 2.0*(double)n*n;

  TC.execute(&eh);
  print_sqmatrix(v, n);

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
