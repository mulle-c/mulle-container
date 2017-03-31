#! /bin/sh



main()
{
   if [ -f "cityhash/config.h" ]
   then
      return
   fi

   if [ ! -d "cityhash" ]
   then
      [ -z "${MULLE_BOOTSTRAP_LOGGING_SH}" ] && . mulle-bootstrap-logging.sh

      fail "Cityhash not present in \"$PWD\" as expected"
   fi

   (
      cd "cityhash"
      ./configure
   )
}

main "$@"
