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
      TYPE(C_PTR) :: eperfKernelConf
      integer :: i, err, fNum,f
      character*256 :: s

      ! Call some C
      eperf = EPerfInit()

      ! Add kernels
      print '(a)', "Adding kernel."
      err = EPerfAddKernel(eperf, 1_C_INT, "Fibonacci" // C_NULL_CHAR )

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
! Fib number to generate
        f = i  * 10
! Initialize Kernel Configuration
        eperfKernelConf = EPerfInitKernelConf()
! Write Configuration String
        write(s,'(I10)')f
        err=EPerfInsertKernelConfPair(eperfKernelConf, "number"// C_NULL_CHAR, s// C_NULL_CHAR)
! Set KD    
        err = EPerfAddKernelDataVolumes(eperf, 1, 0, 8_8, 8_8)
! Start Timer      
        err = EPerfStartTimerWithConfig(eperf, 1, 0, eperfKernelConf)
! Run Kernel
        fNum = fib(f)  
! Stop Timer
        err = EPerfStopTimer(eperf, 1, 0)

        write(*,'(A,I2,A,I12)')'Fibonacci',f,': ',fNum
      end do

      print '(a)', "Printing content:"
      call EPerfPrintResults(eperf)
      print '(a)', "Commiting to DB:"
      call EPerfCommitToDB(eperf)
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

