!
! The fibonacci number generator for ePerF
!
program fibonacci

      !
      ! This is a FORTRAN test for ePerF
      !


      ! Use the ePerF module
      USE m_ePerF
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

      ! Add kernels
      print '(a)', "Adding kernel."
      err = EPerfAddKernel(eperf, 1_C_INT, "Fibonacci 10" // C_NULL_CHAR )
      err = EPerfAddKernel(eperf, 2_C_INT, "Fibonacci 20" // C_NULL_CHAR )
      err = EPerfAddKernel(eperf, 3_C_INT, "Fibonacci 30" // C_NULL_CHAR )
      err = EPerfAddKernel(eperf, 4_C_INT, "Fibonacci 40" // C_NULL_CHAR )

      if ( err /= E_OK ) then
         print '(a,a)', "Error", err
         stop 'Error in EPerfAddKernel'
      end if

      ! Add device
      print '(a)', "Adding device"
      err = EPerfAddDevice(eperf, 0, "CPU" // C_NULL_CHAR );

      ! Generate some numbers
      print '(a)', "Generating timings and datavolumes"
      do i = 1,4
        err = EPerfAddKernelDataVolumes(eperf, i, 0, 1024_8, 512_8)
        err = EPerfStartTimer(eperf, i, 0)
        fNum = fib(i * 10)  
        err = EPerfStopTimer(eperf, i, 0)

        write(*,'("Fibonacci ",i2,": ",i12)') i * 10, fNum
      end do

      print '(a)', "Printing content:"
      call EPerfPrintResults(eperf)

      stop
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

