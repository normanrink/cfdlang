
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

  double D[3*m*m];
  double u[m*m*m];
  double v[3*m*m*m];

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  const std::string Source = std::string(
    "type T : [" + m_str + " " + m_str + "]\n"
    "var input  u : [" + m_str + " " + m_str + " " + m_str + "]  \n"
    "var output v : [3 " + m_str + " " + m_str + " " + m_str + "]\n"
    "                 \n"
    "var input Dx : T \n"
    "var input Dy : T \n"
    "var input Dz : T \n"
    "                 \n"
    "v = [ Dx#u.[[1 2]] Dy#u.[[1 3]] Dz#u.[[1 4]] ]\n");


  TC.initCodeGen(&cgh, Source.c_str(), /* rowMajor */true,
                                       /* graphCodeGen */false);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);
  TC.addArgument(&eh, "Dx", &D[0*m*m]);
  TC.addArgument(&eh, "Dy", &D[1*m*m]);
  TC.addArgument(&eh, "Dz", &D[2*m*m]);
  TC.addArgument(&eh, "u", &u[0]);
  TC.addArgument(&eh, "v", &v[0]);

  // initialize input tensors:
  {
    for (int i = 0; i < 3; i++) {
      for (int ri = 0; ri < m; ri++) {
        for (int ci = 0; ci < m; ci++) {
          D[ci+m*(ri+m*(i))] = (ci == ri) ? (float)(i+1) : 0.0;
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

    printf("INPUT \'D[0]\' BEFORE:\n");
    print_matrix_mxn(&D[0*m*m], m, m);
    printf("INPUT \'D[1]\' BEFORE:\n");
    print_matrix_mxn(&D[1*m*m], m, m);
    printf("INPUT \'D[2]\' BEFORE:\n");
    print_matrix_mxn(&D[2*m*m], m, m);
  }

  // execute kernel:
  TC.execute(&eh);

  // print results:
  {
    printf("INPUT \'u\' AFTER:\n");
    print_tensor_3(u, m);

    printf("OUTPUT \'v[0]\':\n");
    print_tensor_3(&v[0*m*m*m], m);
    printf("OUTPUT \'v[1]\':\n");
    print_tensor_3(&v[1*m*m*m], m);
    printf("OUTPUT \'v[2]\':\n");
    print_tensor_3(&v[2*m*m*m], m);
  }

  TC.finalExecution(&eh);
  TC.finalKernel(&kh);

  return 0;
}
