
## CFDlang: Tensor kernels for high-order simulations in Computational Fluid Dynamics (CFD)

CFDlang is a domain-specific tensor language designed with the aim of generating efficient parallel code for tensor kernels 
that are at the heart of high-order simulations in Computational Fluid Dynamics (CFD).
Being a high-level language, CFDlang supports a number of abstract tensor operations, the most important of which are tensor contraction and outer product.
To implement these operations, CFDlang generates C code with efficiently vectorizable loop nests.
Moreover, in generating thread-parallel code, CFDlang exploits the fact that tensor kernels in CFD simulations are typically executed in an embarrassingly parallel fashion.

Full details on CFDlang and its implementation in the present repository can be found in the article
[CFDlang: High-level code generation for high-order methods in fluid dynamics](https://cfaed.tu-dresden.de/files/Images/people/chair-cc/publications/1802_Rink_RWDSL.pdf).


### Design summary

CFDlang is a stand-alone domain-specific language (DSL), i.e. an _external_ DSL, with the following key language facilities:
(i) declarations of tensor variables,
(ii) nested tensor expressions, and
(iii) assignments of expressions to tensor variables.
From this, the CFDlang compiler generates C functions that implement a sequence of tensor assignments and take as parameters the tensor variables declared in CFDlang.
The generated C functions can be lowered into executable code with any C compiler that supports [OpenMP](https://www.openmp.org/).
This allows users of CFDlang to select whichever compiler is the preferred choice for their target platform.

While being a stand-alone language, CFDlang is also designed to be integrated into existing simulation codes at simulation runtime.
For this purpose, CFDlang relies on the _separate source paradigm_ (similar to [OpenGL](https://www.opengl.org/) and [OpenCL](https://www.khronos.org/opencl/)),
meaning that CFDlang source code is stored either in a separate file or appears as a string inside a larger simulation code, typically written in C/C++ or Fortran.
At simulation runtime, the string of CFDlang source code is then passed to the CFDlang runtime library for compilation and execution.
This has the added advantage that the generated code can be specialized to tensor dimensions that are only known at simulation runtime,
which in turn usually leads to the generation of more efficient code.


### Getting started

The CFDlang stand-alone compiler and runtime library are built with `CMake` and `make`.
The only dependencies are
the scanner generator [flex](https://github.com/westes/flex) and
the parser generator [bison](https://www.gnu.org/software/bison/).

After successfully running `make`, the build directory is populated with the following subdirectories:
* `bin` contains the stand-alone CFDlang `compiler` and a number of executable end-to-end tests.
  (In addition, the executables `lex`, `parse`, and `check` will have been generated for running individual subphases of the CFDlang compiler frontend.)
  
* `lib` contains the CFDlang runtime library (`libCFDlang.so` on a Linux system).
  `lib` also contains the Fortran source file `Tensor_Wrappers.f` which defines subroutine interfaces that are bound to corresponding C functions in the CFDlang runtime library.
  
The stand-alone compiler can then be run, for example, on one of the test inputs from the `<cfdlang>/tests` folder:
  ```
  <build>/bin/compiler <cfdlang>/tests/valid.in
  ```

For a larger and more interesting example:
  ```
  <build>/bin/compiler <cfdlang>/tests/big.20.io.in
  ```

From the CFDlang program in the input file the compiler generates C source code that is dumped to the console.


### Integration with numerical simulation code

The common use case of CFDlang is to generate efficient and parallel code for tensor kernels inside a larger simulation project.
In this context, CFDlang kernels are typically compiled at simulation runtime.
This requires linking against the CFDlang runtime library and managing a `TensorContext` and various handles that are involved in compiling and executing CFDlang kernels.

Simple examples of CFDlang kernels and their integration into C++ applications can be found in the `tests` subdirectory.
While the file `test.1.cpp` executes a very simple CFDlang kernel, the file `test.AxAxA.cpp` implements the so-called _interpolation_ kernel from high-order CFD simulations.
Executable binaries for both tests are part of the default `make` target.
Note that when running the binaries for these tests, command line arguments must be given that specify the dimensions of the tensors involved in the respective CFDlang kernel.


### The operator language

The `operators` branch implements an _embbeded_ DSL in C++ that allows for a very natural notation of CFD tensor operators.
For example, the interpolation kernel from `test.AxAxA.cpp` (see the previous section) can be written as C++ code as follows:

  ```C++
  auto MA = Matrix("A", m, n);
  auto tu = Tensor<3>("u", n, n, n);

  auto Interpolation = (MA*MA*MA)(tu);
  ```

Section 5 of the article [TeIL: a type-safe imperative Tensor Intermediate Language](https://cfaed.tu-dresden.de/files/Images/people/chair-cc/publications/1906_Rink_Array.pdf)
refers to this embedded DSL as the _operator language_.
Embedding the operator language into C++ relies heavily on C++ operator overloading and templates.
The operator language is implemented on top of a variant of CFDlang by lowering expressions in the operator language to CFDlang ASTs.

