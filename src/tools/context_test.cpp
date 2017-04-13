

#include <fstream>
#include <iomanip>
#include <iostream>


#include "Compile/Context.h"
#include "Runtime/TensorContext.h"


int main(int argc, char* argv[]) {
  //Context Ctx;

  //Context::Handle handle;

  const int n = 10;
  const std::string n_str = std::to_string(n);
  const std::string Source =
    std::string("type matrix  : [" + n_str + " " + n_str + "]\n"
                "type vector  : [" + n_str + "]\n"
                "                      \n"
                "var input A : matrix  \n"
                "                      \n"
                "var input s  : vector \n"
                "var input t  : vector \n"
                "                      \n"
                "var input U  : matrix \n"
                "var output v : matrix \n"
                "                      \n"
                "v = (A + s # t) * U   \n");

  //Ctx.compile(&handle, Source.c_str());

  double A[n*n],
         U[n*n],
         v[n*n],
         s[n];

  for (int i = 0; i < n*n; i++) {
    A[i] = 0.0;
    U[i] = 2.0;
  }
  for (int i = 0; i < n; i++) {
    s[i] = (double)i;
  }

  //Ctx.addArgument(&handle, "A", &A[0]);
  //Ctx.addArgument(&handle, "U", &U[0]);
  //Ctx.addArgument(&handle, "s", &s[0]);
  //Ctx.addArgument(&handle, "t", &s[0]);
  //Ctx.addArgument(&handle, "v", &v[0]);

  //Ctx.execute(&handle);

  for (int i = 0; i < (n*n); i++) {
    if (i && (i % n == 0))
      std::cout << "\n";

    printf("%6.2f ", v[i]);
    //std::cout << std::setprecision(2) << v[i] << " ";
  }
  std::cout << "\n\n";

  for (int i = 0; i < (n*n); i++)
    A[i] = 2.0*(double)n*n;

  //Ctx.execute(&handle);

  for (int i = 0; i < (n*n); i++) {
    if (i && (i % n == 0))
      std::cout << "\n";

    printf("%6.2f ", v[i]);
    //std::cout << std::setprecision(2) << v[i] << " ";
  }
  std::cout << "\n";

  TensorContext TC;

  TensorContext::CodeGenHandle cgh;
  TensorContext::KernelHandle kh;
  TensorContext::ExecutionHandle eh;

  TC.initCodeGen(&cgh, Source.c_str(), true, true);
  TC.generateCCode(&cgh);

  TC.initKernel(&kh, &cgh, true);
  TC.buildKernel(&kh);
  TC.finalCodeGen(&cgh);

  TC.initExecution(&eh, &kh);

  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "U", &U[0]);
  TC.addArgument(&eh, "s", &s[0]);
  TC.addArgument(&eh, "t", &s[0]);
  TC.addArgument(&eh, "v", &v[0]);

  TC.execute(&eh);

  for (int i = 0; i < (n*n); i++) {
    if (i && (i % n == 0))
      std::cout << "\n";

    printf("%6.2f ", v[i]);
    //std::cout << std::setprecision(2) << v[i] << " ";
  }
  std::cout << "\n\n";

  TC.clearArguments(&eh);
  TC.addArgument(&eh, "A", &A[0]);
  TC.addArgument(&eh, "U", &U[0]);
  TC.addArgument(&eh, "s", &s[0]);
  TC.addArgument(&eh, "t", &s[0]);
  TC.addArgument(&eh, "v", &v[0]);

  TC.execute(&eh);
  TC.finalExecution(&eh);

  return 0;
}
