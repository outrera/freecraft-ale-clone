AC_INIT([Aleonas Tales], [0.0.0], [stratagus-devel@nongnu.org], [aleona])
AC_CONFIG_SRCDIR(buildfcmp.sh)
dnl AC_CONFIG_HEADERS([config.h])
dnl AM_INIT_AUTOMAKE(dist-bzip2)

dnl -- Try to locate the Stratagus Directory --

dnl Possible Stratagus locations to check
STRATAGUS_DIRS="\
../stratagus:\
../../stratagus/stratagus:\
$HOME/projects/stratagus/cvs:\
$HOME/Projects/stratagus/cvs:\
$HOME/Projects/stratagus:\
$HOME/projects/stratagus:\
$HOME/Projects/open/stratagus:\
"
AC_MSG_CHECKING([for Stratagus directory])
AC_ARG_WITH([stratagus], 
  AC_HELP_STRING([--with-stratagus=DIR], [Set the Stratagus directory to DIR]),
  [STRATAGUS_DIR="$withval"; AC_MSG_RESULT($STRATAGUS_DIR); 
  if test ! -e $STRATAGUS_DIR/data/ccl/stratagus.ccl; then
     AC_MSG_ERROR([
    Couldn't find $STRATAGUS_DIR/data/ccl/stratagus.ccl, which means that
    the directory you set with --with-stratagus is incorrect, try again.])
  fi],
  [
  oldifs=$IFS
  IFS=":"
  for i in $STRATAGUS_DIRS; do
    if test -e "$i/data/ccl/stratagus.ccl"; then
       STRATAGUS_DIR="$i"
    fi
  done
  IFS=$oldifs

  if test -z "$STRATAGUS_DIR"; then
     AC_MSG_RESULT(not found)
     AC_MSG_ERROR([
Error, couldn't locate Stratagus directory, please set it
manually with:

  ./configure --with-stratagus=YOU_STRATAGUS_DIR"
])
  fi
  ])
AC_MSG_RESULT($STRATAGUS_DIR)
AC_SUBST(STRATAGUS_DIR)

AC_OUTPUT( Makefile )

dnl EOF dnl
