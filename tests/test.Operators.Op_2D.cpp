
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
  if (argc != 2)
    exit(1);

  const int n = atoi(argv[1]);

  auto I = Identity("I", n);
  auto D = Diagonal("D", n);
  auto S = Symmetric("S", n);
  auto u = Tensor<2>("u", n, n);

  double II[n*n], DD[n*n];
  double uu[n*n], vv[n*n];

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;
  void (*kernel)(double *D, double *I,
                 double *u, double *v);

 //auto Op_2D  = (D*I)(D*I)(I*D);
 //auto Op_2D  = (D*I)(D*I)(I*D)(u);
 auto Op_2D  = (D*I)((D*I)((I*D)(u)));
 const CFDlang::Program *Prog = Emitter.lowerExpr(Op_2D, "v");
 Prog->print();
 printf("--------------------------------------------------\n");
 Prog = ProductRightAssoc().run(Prog);
 Prog->print();

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
    for (int ri = 0; ri < n; ri++) {
      for (int ci = 0; ci < n; ci++) {
        II[ci+n*(ri)] = (ci == ri) ? 1.0 : 0.0;
      }
    }

    for (int ri = 0; ri < n; ri++) {
      for (int ci = 0; ci < n; ci++) {
        DD[ci+n*(ri)] = (ci == ri) ? 2.0 : 0.0;
      }
    }

    for (int i0 = 0; i0 < n; i0++) {
      for (int i1 = 0; i1 < n; i1++) {
          uu[i1 + n*(i0)] = (i0 == i1) ? 1.0 : 0.0;
      }
    }
  }


  // print input tensors:
  {
    printf("INPUT \'u\' BEFORE:\n");
    print_matrix_mxn(uu, n, n);

    printf("INPUT \'I\' BEFORE:\n");
    print_matrix_mxn(II, n, n);

    printf("INPUT \'D\' BEFORE:\n");
    print_matrix_mxn(DD, n, n);
  }

  // execute kernel:
  kernel(&DD[0], &II[0], /*&SS[0],*/ &uu[0], &vv[0]);

  // print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_matrix_mxn(uu, n, n);

    printf("OUTPUT \'v\':\n");
    print_matrix_mxn(vv, n, n);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
