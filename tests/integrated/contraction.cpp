
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include <time.h>
#include <unistd.h>

#include "Runtime/TensorContext.h"
#include "test.utilities.h"


#define PRINT_TENSORS 1


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
  double *v = (double*)malloc(sizeof(double)*(m*n*n));

  // Initialize input tensors:
  {
    for (int ri = 0; ri < m; ri++) {
      for (int ci = 0; ci < n; ci++) {
        A[ci+n*(ri)] = (ci == ri) ? 2.0 : 0.0;
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
    void (*kernel)(double*, double*, double*);

    // Kernel source code:
    const std::string Source = std::string(
      "var input  A : [" + m_str + " " + n_str + "]               \n"
      "var input  u : [" + n_str + " " + n_str + " " + n_str + "] \n"
      "var output v : [" + m_str + " " + n_str + " " + n_str + "] \n"
      "                                                           \n"
      "v = (A # u) . [[1 2]]                                      \n");

    // Print kernel source code:
    printf("KERNEL SOURCE CODE:\n");
    printf("%s\n", Source.c_str());


    // Configuration of CFDlang's code generation:
    // (aka "compiler flags")
    TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true);
    
    TC.generateCCode(&cgh);

    TC.initKernel(&kh, &cgh);
    TC.buildAndReturnKernel(&kh, (void**)&kernel);
    TC.finalCodeGen(&cgh);

    TC.initExecution(&eh, &kh);

    struct timespec start, stop;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

    // Execute the kernel:
    kernel(&A[0], &u[0], &v[0]);

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    // Kernel execution time in micro-seconds:
    const double elapsed_us =
      (stop.tv_sec - start.tv_sec) * 1.0e6
      + (stop.tv_nsec - start.tv_nsec) / 1.0e3; 
    printf("Kernel execution time: %fus\n\n", elapsed_us);
  }

#if (PRINT_TENSORS)
  // Print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_tensor_3(u, n);

    printf("INPUT \'A\' AFTER:\n");
    print_matrix_mxn(A, m, n);

    printf("OUTPUT \'v\':\n");
    print_tensor_3d(v, m, n, n);
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
