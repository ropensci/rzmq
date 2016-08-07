dnl Process this file with autoconf to produce a configure script
dnl configure is made by ./autoconf; ./cleanup

AC_INIT(DESCRIPTION)

dnl Determine R_HOME.
: ${R_HOME=‘R RHOME‘}
if test -z "${R_HOME}"; then
  echo "could not determine R_HOME"
  exit 1
fi
R_SCMD="${R_HOME}/bin/Rscript -e"

dnl Get ZMQ flags
ZMQ_CPPFLAGS=`${R_SCMD} "pbdZMQ:::get.zmq.cppflags()"`
ZMQ_LDFLAGS=`${R_SCMD} "pbdZMQ:::get.zmq.ldflags()"`

dnl Report
echo " "
echo "****************** Results of rzmq package configure *******************"
echo " "
echo ">> ZMQ_CPPFLAGS = ${ZMQ_CPPFLAGS}"
echo ">> ZMQ_LDFLAGS = ${ZMQ_LDFLAGS}"
echo " "
echo "************************************************************************"
echo " "

dnl Start to substitute templates
AC_SUBST(ZMQ_CPPFLAGS)
AC_SUBST(ZMQ_LDFLAGS)
AC_OUTPUT(src/Makevars)
