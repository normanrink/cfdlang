
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
                                       /* graphCodeGen */false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);
  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "U", &U[0]);
  TC.addArgument(&eh, "s", &s[0]);
  TC.addArgument(&eh, "t", &s[0]);
  TC.addArgument(&eh, "v", &v[0]);

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
  TC.execute(&eh);
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
  TC.execute(&eh);
  // print results:
  {
    printf("OUTPUT \'v\':\n");
    print_sqmatrix(v, n);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
