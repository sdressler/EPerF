MODULE m_eperf
!
! #include "../include/EPerf/EPerfC.h"
!
    IMPLICIT NONE

    ENUM, BIND(C)
        ENUMERATOR ::   E_OK = 0,   & ! No error, success
&                       E_DUPK,     & ! Kernel already exists
&                       E_DUPD,     & ! Device already exists
&                       E_NOK,      & ! Kernel not found
&                       E_NOD,      & ! Device not found
&                       E_TMR,      & ! Timer error
&                       E_RES,      & ! Error in results
&                       E_EQU         ! Two ID's were equal
    END ENUM

    INTERFACE
        FUNCTION EPerfInit() &
&           BIND(C, NAME="EPerfInit")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR
            IMPLICIT NONE
            TYPE(C_PTR) :: EPerfInit
        END FUNCTION EPerfInit
       
        FUNCTION EPerfInitKernelConf() &
&           BIND(C, NAME="EPerfInitKernelConf")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR
            IMPLICIT NONE
            TYPE(C_PTR) :: EPerfInitKernelConf
        END FUNCTION EPerfInitKernelConf

        FUNCTION EPerfAddKernel(e, ID, kName) &
&           BIND(C, NAME="EPerfAddKernel")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT, C_CHAR
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: ID
            CHARACTER(LEN=1, KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: kName
            INTEGER(C_INT) :: EPerfAddKernel
        END FUNCTION EPerfAddKernel
        
        FUNCTION EPerfAddDevice(e, ID, dName) &
&           BIND(C, NAME="EPerfAddDevice")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT, C_CHAR
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: ID
            CHARACTER(LEN=1, KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: dName
            INTEGER(C_INT) :: EPerfAddDevice
        END FUNCTION EPerfAddDevice

        FUNCTION EPerfAddSubDeviceToDevice(e, ID, sID) &
&           BIND(C, NAME="EPerfAddSubDeviceToDevice")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: ID
            INTEGER(C_INT), VALUE :: sID
            INTEGER(C_INT) :: EPerfAddSubDeviceToDevice
        END FUNCTION EPerfAddSubDeviceToDevice

        FUNCTION EPerfStartTimer(e, KernelID, DeviceID) &
&           BIND(C, NAME="EPerfStartTimer")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: KernelID
            INTEGER(C_INT), VALUE :: DeviceID
            INTEGER(C_INT) :: EPerfStartTimer
        END FUNCTION EPerfStartTimer
        
        FUNCTION EPerfStopTimer(e, KernelID, DeviceID)&
&           BIND(C, NAME="EPerfStopTimer")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: KernelID
            INTEGER(C_INT), VALUE :: DeviceID
            INTEGER(C_INT) :: EPerfStopTimer
        END FUNCTION EPerfStopTimer

        FUNCTION EPerfAddKernelDataVolumes(e, KernelID, DeviceID, inBytes, outBytes) &
&           BIND(C, NAME="EPerfAddKernelDataVolumes")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT, C_LONG_LONG
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: KernelID
            INTEGER(C_INT), VALUE :: DeviceID
            INTEGER(C_LONG_LONG), VALUE :: inBytes
            INTEGER(C_LONG_LONG), VALUE :: outBytes
            INTEGER(C_INT) :: EPerfAddKernelDataVolumes
        END FUNCTION EPerfAddKernelDataVolumes

        FUNCTION EPerfInsertKernelConfPair(c, cKey, cValue) &
&           BIND(C, NAME="EPerfInsertKernelConfPair")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT, C_CHAR
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: c
            CHARACTER(LEN=1, KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: cKey
            CHARACTER(LEN=1, KIND=C_CHAR), DIMENSION(*), INTENT(IN) :: cValue
            INTEGER(C_INT) :: EPerfInsertKernelConfPair
        END FUNCTION EPerfInsertKernelConfPair

        FUNCTION EPerfSetKernelConf(e, KernelID, c) &
&           BIND(C, NAME="EPerfSetKernelConf")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR, C_INT
            TYPE(C_PTR), VALUE :: e
            INTEGER(C_INT), VALUE :: KernelID
            TYPE(C_PTR), VALUE :: c
            INTEGER(C_INT) :: EPerfSetKernelConf
        END FUNCTION EPerfSetKernelConf

        SUBROUTINE EPerfCommitToDB(e) &
&           BIND(C, NAME="EPerfCommitToDB")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
        END SUBROUTINE EPerfCommitToDB

        SUBROUTINE EPerfPrintResults(e) &
&           BIND(C, NAME="EPerfPrintResults")

            USE, INTRINSIC :: ISO_C_BINDING, ONLY: C_PTR
            IMPLICIT NONE
            TYPE(C_PTR), VALUE :: e
        END SUBROUTINE EPerfPrintResults
    END INTERFACE

END MODULE m_eperf
