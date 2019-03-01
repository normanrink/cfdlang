
#include <iostream>


#include "Operators/ExprPrinter.h"
#include "Operators/Operators.h"


ExprPrinter Printer(std::cout);

int main() {
  int m = 3, n = 4;

  // General matrices (of dimensions 3x4):
  auto A = Matrix("A", m, n);
  auto B = Matrix("B", m, n);
  auto C = Matrix("C", m, n);

  // Special matrices (square, of dimensions 4x4):
  auto I = Identity("I", n);
  auto D = Diagonal("D", n);
  auto S = Symmetric("S", n);

  // Constant scalars:
  auto pi = SConst(3.14);
  auto s  = SConst(42.);

  // Variable scalars:
  auto x  = SVar("x");
  auto y  = SVar("y");

  // Interpolation operator:
  auto Interpolation = A*B*C;
  // Formatted printing to the console:
  std::cout << "1. "; Interpolation.visit(Printer); std::cout << "\n";

  // Adding up cyclic permuations of the interpolation operator:
  // (Not sure if this may be relevant):
  auto CycInterp = A*B*C + B*C*A + C*A*B;
  // Formatted printing to the console:
  std::cout << "2. "; CycInterp.visit(Printer); std::cout << "\n";


  /******************* IMPORTANT FOR PROPOSAL/PROJECT: *******************/
  /*                                                                     */
  /* A fully working version of the proposed "Operator language" would   */
  /* also offer code generation, e.g., through a call like this:         */
  /*                                                                     */
  /*    Interpolation.codeGen()                                          */
  /*                                                                     */
  /* One could also envision method calls such as                        */
  /*                                                                     */
  /*    Interpolation.optimize()                                         */
  /*    Interpolation.autoTune()                                         */
  /*                                                                     */
  /* for the generation of efficient code with different strategies,     */
  /* i.e. "standard optimization" or "auto-tuning".                      */
  /*                                                                     */
  /* Similarly, we could have methods such as                            */
  /*                                                                     */
  /*    Interpolation.parallel()                                         */
  /*    Interpolation.accelerate()                                       */
  /*    Interpolation.gpuCodeGen()                                       */
  /*                                                                     */
  /* for generating codes for different targets, e.g. parallel or        */
  /* accelerated OpenMP code, or GPU code.                               */
  /*                                                                     */
  /***********************************************************************/

  // For now, let's have a few more examples of operators
  // to get a feel for the proposed "Operator language:

  // Another operator in 3D:
  auto Op_3D   = S*D*D;
  // And another way of writing the previous operator:
  auto Op_3D_  = (S*I*I)(I*D*I)(I*I*D);
  // And yet another way of arranging things:
  auto Op_3D__ = (I*D*I)(S*I*I)(I*I*D);
  // Formatted printing to the console:
  std::cout << "3. "; Op_3D.visit(Printer); std::cout << "\n";
  std::cout << "4. "; Op_3D_.visit(Printer); std::cout << "\n";
  std::cout << "5. "; Op_3D__.visit(Printer); std::cout << "\n";

  // Something a bit more complex, not necessarily meaningful:
  auto Op = (pi+x*pi*s)*(A*B*C*A)
            + (s*x-s)*(A*B*C*B - pi*s*pi*(B*C*A*C) + C*A*B*A);
  // Formatted printing to the console:
  std::cout << "6. "; Op.visit(Printer); std::cout << "\n";

  // An operator in 1 dimension:
  auto Op_1D = (x*I + s*D)(D-I);
  // Formatted printing to the console:
  std::cout << "7. "; Op_1D.visit(Printer); std::cout << "\n";

  // And an operator in 2 dimension:
  auto Op_2D = (I*S + s*(S*D))(D*D-I*I);
  // Formatted printing to the console:
  std::cout << "8. "; Op_2D.visit(Printer); std::cout << "\n";

  // And, finally, an example in 5D:
  auto Op_5D   = (A*D*D*A*B + B*I*D*A*B - A*S*D*B*C)(D*I*I*S*D - S*I*I*D*D) ;
  std::cout << "9. "; Op_5D.visit(Printer); std::cout << "\n";

  return 0;
}
