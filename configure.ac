AC_INIT([CNonymizer], [0.1], [patrick.burkard@bsk-consulting.de])
AM_INIT_AUTOMAKE([foreign -Wall -Werror])

AC_PROG_CXX
AC_LANG(C++)

AC_CHECK_LIB(boost_filesystem, main, , [AC_MSG_ERROR(You need the boost_filesystem library.)])
AC_CHECK_LIB(boost_system, main, , [AC_MSG_ERROR(You need the boost_system library.)])
AC_CHECK_LIB(boost_program_options, main, , [AC_MSG_ERROR(You need the boost_program_options library.)])
AC_CHECK_LIB(boost_regex, main, , [AC_MSG_ERROR(You need the boost_regex library.)])

AC_CHECK_HEADERS([boost/filesystem.hpp boost/program_options.hpp boost/regex.hpp], , [AC_MSG_ERROR(You need the boost development libraries.)])

AC_CONFIG_HEADERS([config.h])
AC_CONFIG_FILES([Makefile src/Makefile])
AC_OUTPUT
