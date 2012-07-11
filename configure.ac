AC_INIT(EPerF, version-0.5.1)

AC_ARG_ENABLE(
    [fortran-api],
    AS_HELP_STRING(
        [--disable-fortran-api],
        [Disables the FORTRAN API]
    )
)

AS_IF(
    [test "x$enable_c_api" != "xno"],
    AC_SUBST(WITH_CAPI, YES),
    [
        AC_SUBST(WITH_CAPI, NO),
        AS_IF(
            [test "x$enable_fortran_api" != "xno"],
            AC_MSG_ERROR([You tried to disable the C API with enabled FORTRAN API])
        )
    ]
)

AS_IF(
    [test "x$enable_fortran_api" != "xno"],
    AC_SUBST(WITH_FAPI, YES),
    AC_SUBST(WITH_FAPI, NO)
)

# Check for C++ compiler
AC_PROG_CXX

# Check for C compiler
AC_PROG_CC

# Check for FORTRAN compiler
AC_PROG_FC

# Check whether FORTRAN / C interface enabled + compiler exists
AS_IF(
    [test "x$ac_cv_c_compiler_gnu" = "xno"],
    AS_IF(
        [test "x$enable_c_api" = "xno"],
        AC_MSG_ERROR([You enabled the C API but there is no valid C compiler])
    )
)

AS_IF(
    [test "x$ac_cv_fc_compiler_gnu" = "xno"],
    AS_IF(
        [test "x$enable_fortran_api" != "xno"],
        AC_MSG_ERROR([You enabled the FORTRAN API but there is no valid FORTRAN compiler])
    )
)

# Check whether Berkeley DB > 4.x is installed
AX_PATH_BDB(
    [4],
    [],
    AC_MSG_ERROR([BerkeleyDB not found.])
)

AX_CHECK_OPENSSL(
    AC_SUBST(LSSL, -lssl),
    AC_MSG_ERROR([OpenSSL not found.])
)

AC_LANG(C)

AC_CHECK_LIB(
    rt,
    clock_gettime,
    AC_SUBST(LRT, -lrt)
)

AC_CHECK_LIB(
    crypto,
    SHA256_Init,
    AC_SUBST(LCRYPTO, -lcrypto)
)

AC_LANG(C++)

# Check whether the C++ header for BDB is also installed
AC_CHECK_HEADER(
    [db_cxx.h],
    [],
    AC_MSG_ERROR([Berkeley DB C++ header not found.])
)

# C / FORTRAN API
AC_ARG_ENABLE(
    [c-api],
    AS_HELP_STRING(
        [--disable-c-api],
        [Disables the C API]
    )
)

AC_OUTPUT(Makefile)
AC_OUTPUT(Makefile.inc)

[echo]
[echo ======= SUMMARY =======]
[echo]


[echo EPerF can be build.]
[echo]

AS_IF(
    [test "x$enable_c_api" != "xno"],
    [echo C API is enabled.],
    [echo C API is disabled.]
)

AS_IF(
    [test "x$enable_fortran_api" != "xno"],
    [echo FORTRAN API is enabled.],
    [echo FORTRAN API is disabled.]
)

[echo]
[echo ===== SUMMARY END =====]
[echo]
