
#include <stdlib.h>
#include <stdio.h>

#include <string>


#include "Runtime/TensorContext.h"
#include "test.utilities.h"


int main(int argc, char** argv) {
  if (argc != 2)
    exit(1);

  const int n = atoi(argv[1]);
  const std::string n_str(argv[1]);

  double A[n*n], U[n*n], v[n*n];
  double s[n];

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;
  void (*kernel)(double *A, double *s, double *t, double *U, double *v);

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

  TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true,
                                       /* restrictPointer */true,
                                       /* iccPragmas */false,
                                       /* ompPragmas */false,
                                       /* graphCodeGen */false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildAndReturnKernel(&kh, (void**)&kernel);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);

  // print source code:
  printf("SOURCE CODE:\n");
  printf("%s\n", Source.c_str());

  // initialize input tensors:
  {
    for (int i = 0; i < n*n; i++) {
      A[i] = 0.0;
      U[i] = 2.0;
    }  for (int i = 0; i < n; i++) {
      s[i] = (double)i;
    }
  }

  // execute kernel:
  kernel(&A[0], &s[0], &s[0], &U[0], &v[0]);
  // print results:
  {
    printf("OUTPUT \'v\':\n");
    print_sqmatrix(v, n);
  }

  // re-initialize input:
  {
    for (int i = 0; i < (n*n); i++)
      A[i] = 2.0*(double)n*n;
  }

  // execute kernel with new input:
  kernel(&A[0], &s[0], &s[0], &U[0], &v[0]);
  // print results:
  {
    printf("OUTPUT \'v\':\n");
    print_sqmatrix(v, n);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
