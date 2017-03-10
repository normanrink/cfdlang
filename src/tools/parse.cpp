
#include <fstream>
#include <iostream>


#include "Parse/Parser.h"


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

  parser.getAST()->print();
  Program::destroy(parser.getAST());

  delete [] input;

  return 0;
}

