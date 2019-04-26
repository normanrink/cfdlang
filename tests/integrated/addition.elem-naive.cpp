
#include <stdlib.h>
#include <stdio.h>

#include <string>

#include <time.h>
#include <unistd.h>

#include "Runtime/TensorContext.h"
#include "test.utilities.h"


int main(int argc, char** argv) {
  if (argc != 6)
    exit(1);

  // Dimensions of the tensors are taken
  // from the command-line parameters:
  const int elems = atoi(argv[1]); const std::string e_str(argv[1]);
  const int m0 = atoi(argv[2]); const std::string m0_str(argv[2]);
  const int m1 = atoi(argv[3]); const std::string m1_str(argv[3]);
  const int m2 = atoi(argv[4]); const std::string m2_str(argv[4]);
  const int m3 = atoi(argv[5]); const std::string m3_str(argv[5]);

  // Input tensors:
  double *A = (double*)malloc(sizeof(double)*(elems*m0*m1*m2*m3));
  double *B = (double*)malloc(sizeof(double)*(elems*m0*m1*m2*m3));
  double *C = (double*)malloc(sizeof(double)*(elems*m0*m1*m2*m3));

  // Initialize input tensors:
  {
    for (int e = 0; e < elems; e++) {
      for (int i0 = 0; i0 < m0; i0++) {
	for (int i1 = 0; i1 < m1; i1++) {
	  for (int i2 = 0; i2 < m2; i2++) {
	    for (int i3 = 0; i3 < m3; i3++) {
	      A[i3 + m3*(i2 + m2*(i1 + m1*(i0 + m0*e)))] = 1.0;
	      B[i3 + m3*(i2 + m2*(i1 + m1*(i0 + m0*e)))] = 1.0;
	      C[i3 + m3*(i2 + m2*(i1 + m1*(i0 + m0*e)))] = 0.0;
	    }
	  }
        }
      }
    }
  }

  // Handles for the CFDlang library:
  TensorContext TC;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  // Interfacing with the CFDlang library:
  // (code generation and kernel execution)
  {
    TensorContext::CodeGenHandle cgh;
    void (*kernel)(double*, double*, double*);

    const std::string Dimensions =
      "[" + e_str  + " " + m0_str + " " + m1_str + " " + m2_str + " " + m3_str + "]";
    // Kernel source code:
    const std::string Source = std::string(
      "var input  A  : " + Dimensions + "\n"
      "var input  B  : " + Dimensions + "\n"
      "var output C  : " + Dimensions + "\n"
      "                                  \n"
      "C = A + B                         \n");

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
    kernel(&A[0], &B[0], &C[0]);
    
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &stop);
    // Kernel execution time in micro-seconds:
    const double elapsed_us =
      (stop.tv_sec - start.tv_sec) * 1.0e6
      + (stop.tv_nsec - start.tv_nsec) / 1.0e3; 
    printf("Kernel execution time: %fus\n\n", elapsed_us);
  }

  // Print a few entries in the resulting tensor 'C':
  {
    printf("C(0,0,0,0,0)=%f \t C(0,0,0,0,%d)=%f\n",
           C[0],
           (m3-1),
           C[(m3-1) + m3*(0 + m2*(0 + m1*(0)))]);

    printf("C(%d,%d,0,0,0)=%f \t C(%d,%d,0,0,%d)=%f\n\n",
           (elems-1),
	   (m0-1),
           C[0 + m3*(0 + m2*(0 + m1*((m0-1) + m0*(elems-1))))],
           (elems-1),
	   (m0-1),
           (m3-1),
           C[(m3-1) + m3*(0 + m2*(0 + m1*((m0-1) + m0*(elems-1))))]);
  }

  // Interfacing with the CFDlang library:
  // (cleaning up)
  {
    TC.finalExecution(&eh);
    TC.finalKernel(&kh);
  }

  free(C); free(A); free(B);

  return 0;
}
