module Tensor_Wrappers
  use iso_c_binding
  implicit none
  
  type Tensor_Handle
    integer(c_long) :: ctx
    integer(c_long) :: cg
    integer(c_long) :: k
    integer(c_long) :: ex
  end type Tensor_Handle

  interface
    subroutine Tensor_Init_Context(h_ctx) &
               bind(C, name="TensorInitContext")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx
    end subroutine Tensor_Init_Context

    subroutine Tensor_Final_Context(h_ctx) &
               bind(C, name="TensorFinalContext")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx
    end subroutine Tensor_Final_Context


    subroutine Tensor_Init_Code_Gen(h_ctx, h_cg,     &
                                    source,          &
                                    rowMajor,        &
                                    restrictPointer, &
                                    graphCodeGen)    &
               bind(C, name="TensorInitCodeGen")
      use iso_c_binding
      integer(c_long),    intent(inout) :: h_ctx, h_cg
      character(c_char), intent(in)    :: source(*)
      integer(c_int),    intent(in)    :: rowMajor,        &
                                          restrictPointer, &
                                          graphCodeGen
    end subroutine Tensor_Init_Code_Gen
      
    subroutine Tensor_Generate_C_Code(h_ctx, h_cg) &
               bind(C, name="TensorGenerateCCode")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_cg
    end subroutine Tensor_Generate_C_Code
    
    subroutine Tensor_Final_Code_Gen(h_ctx, h_cg) &
               bind(C, name="TensorFinalCodeGen")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_cg
    end subroutine Tensor_Final_Code_Gen
    
    
    subroutine Tensor_Init_Kernel(h_ctx, h_cg, h_k,   &
                                  cleanOnDestruction) &
               bind(C, name="TensorInitKernel")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_cg, h_k
      integer(c_int), intent(in)    :: cleanOnDestruction
    end subroutine Tensor_Init_Kernel
    
    subroutine Tensor_Build_Kernel(h_ctx, h_k) &
               bind(C, name="TensorBuildKernel")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_k 
    end subroutine Tensor_Build_Kernel
    
    subroutine Tensor_Final_Kernel(h_ctx, h_k) &
               bind(C, name="TensorFinalKernel")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_k 
    end subroutine Tensor_Final_Kernel
    
    subroutine Tensor_Init_Execution(h_ctx, h_k, h_ex) &
               bind(C, name="TensorInitExecution")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_k, h_ex
    end subroutine Tensor_Init_Execution
    
    subroutine Tensor_Add_Argument(h_ctx, h_ex, &
                                   name,        &
                                   arg)         &
               bind(C, name="TensorAddArgument")
      use iso_c_binding
      integer(c_long),    intent(inout)     :: h_ctx, h_ex
      character(c_char), intent(in)        :: name(*)
      type(c_ptr),       intent(in), value :: arg
    end subroutine Tensor_Add_Argument

    subroutine Tensor_Add_Arguments(h_ctx, h_ex, &
                                    names,       &
                                    args,        &
                                    num_args)    &
               bind(C, name="TensorAddArguments")
      use iso_c_binding
      integer(c_long),    intent(inout)      :: h_ctx, h_ex
      integer(c_int), intent(in)             :: num_args
      type(c_ptr), intent(in)                :: names(num_args)
      type(c_ptr), intent(in)                :: args(num_args)
    end subroutine Tensor_Add_Arguments
                         
    subroutine Tensor_Clear_Arguments(h_ctx, h_ex) &
               bind(C, name="TensorClearArguments")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_ex
    end subroutine Tensor_Clear_Arguments
    
    subroutine Tensor_Execute(h_ctx, h_ex) &
               bind(C, name="TensorExecute")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_ex
    end subroutine Tensor_Execute

    subroutine Tensor_Execute_With_Arguments(h_ctx, h_ex, &
                                             names,       &
                                             args,        &
                                             num_args)    &
               bind(C, name="TensorExecuteWithArguments")
      use iso_c_binding
      integer(c_long),    intent(inout)      :: h_ctx, h_ex
      integer(c_int), intent(in)             :: num_args
      type(c_ptr), intent(in)                :: names(num_args)
      type(c_ptr), intent(in)                :: args(num_args)
    end subroutine Tensor_Execute_With_Arguments

    subroutine Tensor_Execute_With_Positional_Args(h_ctx, h_ex, &
                                                    args,        &
                                                    num_args)    &
               bind(C, name="TensorExecuteWithPositionalArgs")
      use iso_c_binding
      integer(c_long),    intent(inout)      :: h_ctx, h_ex
      integer(c_int), intent(in)             :: num_args
      type(c_ptr), intent(in)                :: args(num_args)
    end subroutine Tensor_Execute_With_Positional_Args
    
    subroutine Tensor_Final_Execution(h_ctx, h_ex) &
               bind(C, name="TensorFinalExecution")
      use iso_c_binding
      integer(c_long), intent(inout) :: h_ctx, h_ex
    end subroutine Tensor_Final_Execution
  end interface
end module


