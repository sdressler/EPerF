!
! The fibonacci number generator for ePerF
!
program fibonacci

      !
      ! This is a FORTRAN test for ePerF
      !


      ! Use the ePerF module
      USE MODEPERF
      USE, INTRINSIC :: ISO_C_BINDING

      implicit none                     ! No implicit declarations

      ! Interface for the fibonacci function
      interface
          function fib(n)               ! Fibonacci function
              integer, intent(in) :: n  ! Input number
              integer :: fib            ! Fibonacci number
          end function fib
      end interface

      ! Integer for count
      TYPE(C_PTR) :: eperf
      integer :: i, err, fNum

      ! Call some C
      eperf = EPerfInit()

      ! Add device
      err = EPerfAddDevice(eperf, 0, "CPU");

      ! Generate some numbers
      do i = 1,4
        err = EPerfAddKernel(eperf, i, "Fibonacci")
        err = EPerfAddKernelDataVolumes(eperf, i, 0, 1024_8, 512_8)

        err = EPerfStartTimer(eperf, i, 0)
        fNum = fib(i * 10)  
        err = EPerfStopTimer(eperf, i, 0)

        write(*,*) 'Fibonacci', i * 10, ': ', fNum
      end do

      call EPerfPrintResults(eperf)

end program fibonacci

!
! Now we're defining the fibonacci function itself
!
recursive function fib (n) result (fNum)
    integer, intent(in) :: n
    integer :: fNum

    if (n < 2) then
        fNum = n
    else
        fNum = fib(n - 1) + fib(n - 2)
    endif
end function fib
