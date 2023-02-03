#! /bin/sh
#
#   Copyright (c) 2015-2017 Nat! - Mulle kybernetiK
#   All rights reserved.
#
#   Redistribution and use in source and binary forms, with or without
#   modification, are permitted provided that the following conditions are met:
#
#   Redistributions of source code must retain the above copyright notice, this
#   list of conditions and the following disclaimer.
#
#   Redistributions in binary form must reproduce the above copyright notice,
#   this list of conditions and the following disclaimer in the documentation
#   and/or other materials provided with the distribution.
#
#   Neither the name of Mulle kybernetiK nor the names of its contributors
#   may be used to endorse or promote products derived from this software
#   without specific prior written permission.
#
#   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#   POSSIBILITY OF SUCH DAMAGE.
#
[ "${TRACE:-}" = 'YES' -o "${MULLE_SOURCETREE_TRACE:-}" = 'YES' ] && set -x && : "$0" "$@"


MULLE_EXECUTABLE_VERSION="1.1.0"
##########################################################################
###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###
###########################################################################
# #
# # PRELUDE - Standard startup code for loading mulle-bashfunctions
# #
###

#
# Figure out MULLE_BASHFUNCTIONS_LIBEXEC_DIR on this machine if needed.
#
if ! [ ${MULLE_BASHFUNCTIONS_LIBEXEC_DIR+x} ]
then
   MULLE_BASHFUNCTIONS_LIBEXEC_DIR="`mulle-bashfunctions libexec-dir`" || exit 1
   export MULLE_BASHFUNCTIONS_LIBEXEC_DIR
fi

#
# The booter reruns the executable either with bash or zsh, whatever the
# system provides. On the second run --no-auto-shell will be set first.
#
if [ "${1:-}" != --no-auto-shell ]
then
   . "${MULLE_BASHFUNCTIONS_LIBEXEC_DIR}/mulle-boot.sh" || exit 1
else
   shift    # get rid of --no-auto-shell
fi


#
# Include mulle-bashfunctions.sh         : default functions
#         mulle-bashfunctions-none.sh    : almost no functions
#         mulle-bashfunctions-minimal.sh : few functions for basic scripts
#         mulle-bashfunctions-all.sh     : everything, even obscure functions
#
# Do this first to get aliases
#
MULLE_EXECUTABLE="${BASH_SOURCE[0]:-${(%):-%x}}"
. "${MULLE_BASHFUNCTIONS_LIBEXEC_DIR}/mulle-bashfunctions.sh" || exit 1

###
# # Now we are in bash or zsh with mulle-bashfunctions loaded
# #
###########################################################################
###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###   ###
###########################################################################


strip_leading_comment()
(
   log_entry "strip_leading_comment" "$@"

   local line
   local state

   IFS=$'\n'
   while read -r line
   do
      case "${line}" in
         "")
         ;;

         //*)
         ;;

         /\**)
            state=c-start
         ;;

         \ \*\ *)
            case "${state}" in
               c-start|c-cont)
                  continue
               ;;
            esac
            break
         ;;

         \ \*/*)
            state=c-end
         ;;

         *)
            break
         ;;
      esac
   done

   if [ ! -z "${line}" ]
   then
      printf "%s\n" "${line}"
   fi

   while read -r line
   do
      printf "%s\n" "${line}"
   done
)


strip_leading_includes()
(
   log_entry "strip_leading_includes" "$@"

   local line
   local state

   IFS=$'\n'
   while read -r line
   do
      case "${line}" in
         \#*)
         ;;

         "")
         ;;

         //*)
         ;;

         /\**)
            state=c-start
         ;;

         \ \*\ *)
            case "${state}" in
               c-start|c-cont)
                  continue
               ;;
            esac
            break
         ;;

         \ \*/*)
            state=c-end
         ;;

         *)
            break
         ;;
      esac
   done

   if [ ! -z "${line}" ]
   then
      printf "%s\n" "${line}"
   fi

   while read -r line
   do
      printf "%s\n" "${line}"
   done
)


amalgamate_sources()
{
   log_entry "amalgamate_sources" "$@"

   local source

   .foreachline source in $1
   .do
       cat "${source}" | strip_leading_comment | strip_leading_includes
   .done
}


main()
{
   local sources

   log_entry "main" "$@"

   while [ $# -ne 0 ]
   do
      if options_technical_flags "$1"
      then
         shift
         continue
      fi

      case "$1" in
         -h*|--help|help)
            usage
         ;;

         --version)
            printf "%s\n" "${MULLE_EXECUTABLE_VERSION}"
            return 0
         ;;

         -*)
            log_error "Unknown option \"$1\""
            usage
         ;;

         *)
            break
         ;;
      esac

      shift
   done

   options_setup_trace "${MULLE_TRACE}" && set -x


   local lines
   local line

   if [ $# -ne 0 ]
   then
      for line in "$@"
      do
         r_add_line "${lines}" "${line}"
         lines="${RVAL}"
      done
   else
      lines="`mulle-match -v list -f "%f\\n" --category-filter sources`" || exit 1
   fi

   amalgamate_sources "${lines}"
}


call_with_flags "main" "${AMALGAMATE_SH_FLAGS}" "$@"
