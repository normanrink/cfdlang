
#include <stdlib.h>
#include <stdio.h>

#include <string>


#include "Runtime/TensorContext.h"
#include "test.utilities.h"


int main(int argc, char** argv) {
  if (argc != 3)
    exit(1);

  const int m = atoi(argv[1]);
  const std::string m_str(argv[1]);

  const int n = atoi(argv[2]);
  const std::string n_str(argv[2]);

  double A[m*n];
  double u[n*n*n];
  double v[m*m*m];

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

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

  TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true,
                                       /* restrictPointer */true,
                                       /* iccPragmas */false,
                                       /* ompPragmas */false,
                                       /* graphCodeGen */false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);
  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "u", &u[0]);
  TC.addArgument(&eh, "v", &v[0]);

  // print source code:
  printf("SOURCE CODE:\n");
  printf("%s\n", Source.c_str());
  
  // initialize input tensors:
  {
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
  }

  // print input tensors:
  {
    printf("INPUT \'u\' BEFORE:\n");
    print_tensor_3(u, n);

    printf("INPUT \'A\' BEFORE:\n");
    print_matrix_mxn(A, m, n);
  }

  // execute kernel:
  TC.execute(&eh);

  // print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_tensor_3(u, n);

    printf("INPUT \'A\' AFTER:\n");
    print_matrix_mxn(A, m, n);

    printf("OUTPUT \'v\':\n");
    print_tensor_3(v, m);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
