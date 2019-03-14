
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
  auto u = Tensor<3>("u", n, n, n);

  double II[n*n], DD[n*n], SS[n*n];
  double uu[n*n*n], vv[n*n*n];

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;
  void (*kernel)(double *I, double *D, //double *S,
                 double *u, double *v);


 //auto Op_3D  = (D*I*I)(I*D*I)(I*I*D)(u);
 auto Op_3D  = (D*I*I)((I*D*I)((D*I*I)(u)));
 const CFDlang::Program *Prog = Emitter.lowerExpr(Op_3D, "v");
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

    for (int ri = 0; ri < n; ri++) {
      for (int ci = 0; ci < n; ci++) {
        SS[ci+n*(ri)] = 1.0;
      }
    }

    for (int i0 = 0; i0 < n; i0++) {
      for (int i1 = 0; i1 < n; i1++) {
        for (int i2 = 0; i2 < n; i2++) {
          uu[i2 + n*(i1 + n*(i0))] = (i0 == i1 && i1 == i2) ? 1.0 : 0.0;
        }
      }
    }
  }


  // print input tensors:
  {
    printf("INPUT \'u\' BEFORE:\n");
    print_tensor_3(uu, n);

    printf("INPUT \'I\' BEFORE:\n");
    print_matrix_mxn(II, n, n);

    printf("INPUT \'D\' BEFORE:\n");
    print_matrix_mxn(DD, n, n);

    printf("INPUT \'S\' BEFORE:\n");
    print_matrix_mxn(SS, n, n);
    }

  // execute kernel:
  kernel(&DD[0], &II[0], /*&SS[0],*/ &uu[0], &vv[0]);

  // print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_tensor_3(uu, n);

    printf("OUTPUT \'v\':\n");
    print_tensor_3(vv, n);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
