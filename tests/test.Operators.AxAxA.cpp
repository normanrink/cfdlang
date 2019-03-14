
#include <stdlib.h>
#include <stdio.h>


#include "AST/AST.h"
#include "AST/ProductRightAssoc.h"
#include "Runtime/TensorContext.h"
#include "Operators/ExprLowering.h"
#include "Operators/Operators.h"
#include "test.utilities.h"


using namespace CFDlang;
using namespace Operators;


ExprLowering Emitter;

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
  void (*kernel)(double *A, double *u, double *v);

  auto MA = Matrix("A", m, n);
  auto tu = Tensor<3>("u", n, n, n);

  auto Interpolation = (MA*MA*MA)(tu);
  const CFDlang::Program *Prog = Emitter.lowerExpr(Interpolation, "v");
  Prog = ProductRightAssoc().run(Prog);
  Prog->print();
  printf("--------------------------------------------------\n");


  TC.initCodeGen(&cgh, Prog, /* rowMajor */true,
                             /* restrictPointer */true,
                             /* iccPragmas */false,
                             /* ompPragmas */false,
                             /* graphCodeGen */false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildAndReturnKernel(&kh, (void**)&kernel);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);

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
  kernel(&A[0], &u[0], &v[0]);

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
