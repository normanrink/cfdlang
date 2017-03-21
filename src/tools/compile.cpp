
#include <fstream>
#include <iostream>


#include "CodeGen/DirectCodeGen.h"
#include "CodeGen/GraphCodeGen.h"
#include "CodeGen/PythonCodeGen.h"
#include "Parse/Parser.h"
#include "Sema/Sema.h"


int main(int argc, char* argv[]) {
  std::ifstream ifs;
  std::filebuf *buffer;
  std::size_t size;
  char *input;

  if (argc != 2) {
    return 1;
  } else {
    ifs.open(argv[1]);
    if (!ifs.is_open())
      return 2;
  }

  buffer = ifs.rdbuf();
  size = buffer->pubseekoff(0, ifs.end, ifs.in);
  buffer->pubseekpos(0, ifs.in);

  input = new char [size + 1];
  buffer->sgetn(input, size);
  input[size] = 0;

  ifs.close();

  Parser parser(input);
  if (parser.parse()) {
    return 3;
  }

  delete [] input;

  Sema sema;
  sema.visitProgram(parser.getAST());

  //DirectCodeGen DCG(&sema);
  GraphCodeGen GCG(&sema);
  TheanoEmitter emitter(&GCG);
  emitter.codeGen(parser.getAST());
  std::cout << emitter.getCode();

  Program::destroy(parser.getAST());

  return 0;
}

