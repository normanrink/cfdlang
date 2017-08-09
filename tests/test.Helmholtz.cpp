
#include <stdlib.h>
#include <stdio.h>

#include <string>


#include "Runtime/TensorContext.h"
#include "test.utilities.h"


int main(int argc, char** argv) {
  if (argc != 2)
    exit(1);

  const int m = atoi(argv[1]);
  const std::string m_str(argv[1]);

  double A[m*m], B[m*m], C[m*m];
  double D[m*m*m], u[m*m*m], v[m*m*m];

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  const std::string Source = std::string(
    "type matrix  : [" + m_str + " " + m_str + "]\n"
    "type tensor  : [" + m_str + " " + m_str + " " + m_str + "]\n"
    "                                              \n"
    "var input A : matrix                          \n"
    "var input B : matrix                          \n"
    "var input C : matrix                          \n"
    "                                              \n"
    "var input D : tensor                          \n"
    "                                              \n"
    "var input u  : tensor                         \n"
    "var output v : tensor                         \n"
    "                                              \n"
    "var t : tensor                                \n"
    "                                              \n"
    "t = D * (A # B # C # u . [[0 6] [2 7] [4 8]]) \n"
    "v = A # B # C # t . [[1 6] [3 7] [5 8]]       \n");

  TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true,
                                       /* restrictPointer */true,
                                       /* graphCodeGen */true);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);
  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "B", &B[0]);
  TC.addArgument(&eh, "C", &C[0]);
  TC.addArgument(&eh, "D", &D[0]);
  TC.addArgument(&eh, "u", &u[0]);
  TC.addArgument(&eh, "v", &v[0]);

  // print source code:
  printf("SOURCE CODE:\n");
  printf("%s\n", Source.c_str());
  
  // initialize input tensors:
  {
    for (int ri = 0; ri < m; ri++) {
      for (int ci = 0; ci < m; ci++) {
        A[ci+m*(ri)] = (ci == ri) ? 1.0 : 0.0;
      }
    }

    for (int ri = 0; ri < m; ri++) {
      for (int ci = 0; ci < m; ci++) {
        B[ci+m*(ri)] = (ci == ri) ? 2.0 : 0.0;
      }
    }

    for (int ri = 0; ri < m; ri++) {
      for (int ci = 0; ci < m; ci++) {
        C[ci+m*(ri)] = (ci == ri) ? 3.0 : 0.0;
      }
    }

    for (int i0 = 0; i0 < m; i0++) {
      for (int i1 = 0; i1 < m; i1++) {
        for (int i2 = 0; i2 < m; i2++) {
          D[i2 + m*(i1 + m*(i0))] = 0.01;
        }
      }
    }

    for (int i0 = 0; i0 < m; i0++) {
      for (int i1 = 0; i1 < m; i1++) {
        for (int i2 = 0; i2 < m; i2++) {
          u[i2 + m*(i1 + m*(i0))] = (double)(i0+i1+i2);
        }
      }
    }
  }

  // print input tensors:
  {
    printf("INPUT \'u\' BEFORE:\n");
    print_tensor_3(u, m);

    printf("INPUT \'A\' BEFORE:\n");
    print_matrix_mxn(A, m, m);

    printf("INPUT \'B\' BEFORE:\n");
    print_matrix_mxn(B, m, m);

    printf("INPUT \'C\' BEFORE:\n");
    print_matrix_mxn(C, m, m);
  }

  // execute kernel:
  TC.execute(&eh);

  // print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_tensor_3(u, m);

    printf("OUTPUT \'v\':\n");
    print_tensor_3(v, m);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
