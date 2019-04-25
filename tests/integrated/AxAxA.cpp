
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include <time.h>
#include <unistd.h>

#include "Runtime/TensorContext.h"
#include "test.utilities.h"


#define PRINT_TENSORS 0


int main(int argc, char** argv) {
  if (argc != 3)
    exit(1);

  // Dimensions of the matrix 'A' are taken from
  // the first two command-line parameters:
  const int m = atoi(argv[1]); const std::string m_str(argv[1]);
  const int n = atoi(argv[2]); const std::string n_str(argv[2]);

  // Input tensors:
  double *A = (double*)malloc(sizeof(double)*(m*n));
  double *u = (double*)malloc(sizeof(double)*(n*n*n));
  double *v = (double*)malloc(sizeof(double)*(m*m*m));

  // Initialize input tensors:
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

#if (PRINT_TENSORS)
  // Print input tensors:
  {
    printf("INPUT \'u\' BEFORE:\n");
    print_tensor_3(u, n);

    printf("INPUT \'A\' BEFORE:\n");
    print_matrix_mxn(A, m, n);
  }
#endif

  // Handles for the CFDlang library:
  TensorContext TC;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  // Interfacing with the CFDlang library:
  // (code generation and kernel execution)
  {
    TensorContext::CodeGenHandle cgh;
    void (*kernel)(double *A, double *u, double *v);

    // Kernel source code:
    const std::string Source = std::string(
      "var input  A : [" + m_str + " " + n_str + "]               \n"
      "var input  u : [" + n_str + " " + n_str + " " + n_str + "] \n"
      "var output v : [" + m_str + " " + m_str + " " + m_str + "] \n"
      "                                                           \n"
      "v = (A # A # A # u) . [[1 6] [3 7] [5 8]]                  \n");

    // Print kernel source code:
    printf("KERNEL SOURCE CODE:\n");
    printf("%s\n", Source.c_str());


    // Configuration of CFDlang's code generation:
    // (aka "compiler flags")
    TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true);
    
    // TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true,
    //                                      /* restrictPointer */true,
    //                                      /* iccPragmas */false,
    //                                      /* ompPragmas */false);

    TC.generateCCode(&cgh);

    TC.initKernel(&kh, &cgh);
    TC.buildAndReturnKernel(&kh, (void**)&kernel);
    TC.finalCodeGen(&cgh);

    TC.initExecution(&eh, &kh);

    const clock_t start_t = clock();
    // Execute the kernel:
    kernel(&A[0], &u[0], &v[0]);
    const clock_t end_t = clock();
    printf("Kernel execution time: %f ms\n",
           (double)(end_t - start_t)/CLOCKS_PER_SEC*1000);
  }

#if (PRINT_TENSORS)
  // Print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_tensor_3(u, n);

    printf("INPUT \'A\' AFTER:\n");
    print_matrix_mxn(A, m, n);

    printf("OUTPUT \'v\':\n");
    print_tensor_3(v, m);
  }
#endif

  // Interfacing with the CFDlang library:
  // (cleaning up)
  {
    TC.finalExecution(&eh);
    TC.finalKernel(&kh);
  }

  free(v); free(u); free(A);

  return 0;
}
