#! /usr/bin/env mulle-bash
#! MULLE_BASHFUNCTIONS_VERSION=<|MULLE_BASHFUNCTIONS_VERSION|>
# shellcheck shell=bash
#
#
#  make-do.sh
#  mulle-container
#
#  Copyright (c) 2023 Nat! - Mulle kybernetiK.
#  All rights reserved.
#
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions are met:
#
#  Redistributions of source code must retain the above copyright notice, this
#  list of conditions and the following disclaimer.
#
#  Redistributions in binary form must reproduce the above copyright notice,
#  this list of conditions and the following disclaimer in the documentation
#  and/or other materials provided with the distribution.
#
#  Neither the name of Mulle kybernetiK nor the names of its contributors
#  may be used to endorse or promote products derived from this software
#  without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
#  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
#  POSSIBILITY OF SUCH DAMAGE.
#


[ "${TRACE}" = 'YES' -o "${MAKE_CONTAINER_DO_TRACE}" = 'YES' ] \
&& set -x  \
&& : "$0" "$@"

#
# Versioning of this script
#
MULLE_EXECUTABLE_VERSION="0.0.0"


do::print_flags()
{
   echo "   -f             : force operation"
   echo "   --allocator    : force to use allocator code"
   echo "   --no-allocator : force to not use allocator code"
   echo "   --callback     : force to use callback code"
   echo "   --no-callback  : force to not use callback code"
   echo "   --compare      : force to use compare code"
   echo "   --no-compare   : force to not use compare code"
   echo "   --flexible     : force to use flexible code"
   echo "   --no-flexible  : force to not use flexible code"
   echo "   --type <type>  : use type instead of void * for elements"


   options_technical_flags_usage \
           "          : "
}


do::usage()
{
   [ $# -ne 0 ] && log_error "$*"


   cat <<EOF >&2
Usage:
   make-container-do [flags] <filename>

   You can only use --flexible with containers that have an "initial_storage"
   pointer.

   make-container-do will try to discern from the filename, that a container
   embeds an allocator (no double underscores) and that it utilizes callbacks
   (no pointer in name).

Flags:
EOF
   do::print_flags | LC_ALL=C sort >&2

   exit 1
}

# AI generated
do::add_backslashes()
{
   log_entry "do::add_backslashes" "$@"

  local lines=()
  local max_length=0

  # Split the macro into lines and find the maximum length
  IFS=$'\n' read -d '' -ra lines
  for line in "${lines[@]}"; do
    local length=${#line}
    if [[ $length -gt $max_length ]]; then
      max_length=$length
    fi
  done

  # Add a continuation character '\' to each line except the last one
  local last_index=$(( ${#lines[@]} - 1 ))
  for i in "${!lines[@]}"; do
    local line="${lines[$i]}"
    local length=${#line}
    local num_spaces=$(( max_length - length + 1))

    if [[ $i -eq $last_index ]]; then
      printf "%s\n" "$line"
    else
      printf "%s\n" "$line$(printf '%*s' $num_spaces)\\"
    fi
  done
}


do::emit_macro_definition()
{
   log_entry "do::emit_macro_definition" "$@"

   local name="$1"
   local allocator="$2"
   local callback="$3"
   local flexible="$4"
   local compare="$5"

   local callback_text

   if [ "${callback}" = 'YES' ]
   then
      callback_text=", callback"
   fi

   local compare_text

   if [ "${compare}" = 'YES' ]
   then
      compare_text=", compare"
   fi

   cat <<EOF
#define ${name}_do( name${callback_text}${compare_text})
EOF
}

do::emit_flexible_macro_definition()
{
   log_entry "do::emit_flexible_macro_definition" "$@"

   local name="$1"
   local allocator="$2"
   local callback="$3"

   local callback_text

   if [ "${callback}" = 'YES' ]
   then
      callback_text=", callback"
   fi

   cat <<EOF
#define ${name}_do_flexible( name, stackcount${callback_text})
EOF
}


do::r_initialize_text()
{
   log_entry "do::r_initialize_text" "$@"

   local name="$1"
   local allocator="$2"
   local callback="$3"
   local flexible="$4"
   local compare="$5"
   local flexible_text="$6"
   local is_zeros="$7"

   include "case"

   local macro_prefix

   r_smart_upcase_identifier "${name}"
   macro_prefix="${RVAL}"

   local text

   if [ "${flexible}" = 'YES' ]
   then
      text="${flexible_text}"
   fi

   if [ "${callback}" = 'YES' ]
   then
      r_concat "${text}" "callback" ", "
      text="${RVAL}"
      is_zeros='NO'
   fi

   if [ "${compare}" = 'YES' ]
   then
      r_concat "${text}" "compare" ", "
      text="${RVAL}"
      is_zeros='NO'
   fi

   if [ "${allocator}" = 'YES' ]
   then
      r_concat "${text}" "NULL" ", "
      text="${RVAL}"
   fi

   if [ -z "${text}" -o "${is_zeros}" = 'YES' ]
   then
      RVAL=
      return 1
   fi

   RVAL="${macro_prefix}_INIT( ${text})"
   return 0
}


do::emit_loop_start()
{
   log_entry "do::emit_loop_start" "$@"

   local name="$1"
   local allocator="$2"
   local callback="$3"
   local flexible="$4"
   local compare="$5"

   local initializer="{ 0 }"

   if [ "${allocator}" = 'YES' -o "${callback}" = 'YES' ]
   then
      if do::r_initialize_text "${name}" \
                               "${allocator}" \
                               "${callback}" \
                               "${flexible}" \
                               "${compare}" \
                               "NULL, 0" \
                               "YES"
      then
         initializer="${RVAL}"
      fi
   fi

   cat <<EOF
   for( struct ${name}
           name ## __container = ${initializer},
           *name = &name ## __container,
           *name ## __i = NULL;
EOF
}


do::emit_flexible_loop_start()
{
   log_entry "do::emit_flexible_loop_start" "$@"

   local name="$1"
   local allocator="$2"
   local callback="$3"
   local flexible="$4"

   do::r_initialize_text "${name}" \
                         "${allocator}" \
                         "${callback}" \
                         "${flexible}" \
                         "" \
                         "name ## __storage, stackcount" \
                         "NO"
   initializer="${RVAL}"

   cat <<EOF
   ${OPTION_TYPE}name ## __storage[ stackcount];
   for( struct ${name}
           name ## __container =
              ${initializer},
           *name = &name ## __container,
           *name ## __i = NULL;
EOF
}



do::emit_loop_test()
{
   log_entry "do::emit_loop_test" "$@"

   local name="$1"

   local text

   text="        ! name ## __i;"
   printf "%s\n" "${text}"
}


do::emit_loop_end()
{
   log_entry "do::emit_loop_end" "$@"

   local name="$1"
   local allocator="$2"
   local callback="$3"

   cat <<EOF
        name ## __i =
        (
EOF

   local text="           _${name}_done( &name ## __container"

   if [ "${callback}" = 'YES' ]
   then
      text="${text}, callback"
   fi

   if [ "${allocator}" = 'YES' ]
   then
      text="${text}, NULL"
   fi
   text="${text}),"

   printf "%s\n" "${text}"

   cat <<EOF
           (void *) 0x1
        )
      )
EOF
}


do::emit_break_protection()
{
   log_entry "do::emit_break_protection" "$@"

   local name="$1"

   cat <<EOF
      for( int  name ## __j = 0;    /* break protection */
           name ## __j < 1;
           name ## __j++)
EOF
}


do::main()
{
   local args="$*"

   #
   # simple option/flag handling
   #
   local OPTION_TYPE='void   *'
   local OPTION_FLEXIBLE='DEFAULT'
   local OPTION_ALLOCATOR='DEFAULT'
   local OPTION_CALLBACK='DEFAULT'
   local OPTION_COMPARE='NO'

   while [ $# -ne 0 ]
   do
      if options_technical_flags "$1"
      then
         shift
         continue
      fi

      case "$1" in
         -f|--force)
            MULLE_FLAG_MAGNUM_FORCE='YES'
         ;;

         -h*|--help|help)
            do::usage
         ;;

         --allocator)
            OPTION_ALLOCATOR='YES'
         ;;

         --no-allocator)
            OPTION_ALLOCATOR='NO'
         ;;

         --compare)
            OPTION_COMPARE='YES'
         ;;

         --no-compare)
            OPTION_COMPARE='NO'
         ;;

         --flexible)
            OPTION_FLEXIBLE='YES'
         ;;

         --no-flexible)
            OPTION_FLEXIBLE='NO'
         ;;

         --callback)
            OPTION_CALLBACK='YES'
         ;;

         --no-callback)
            OPTION_CALLBACK='NO'
         ;;

         --type)
            [ $# -eq 1 ] && match_list_usage "missing argument to $1"
            shift

            OPTION_TYPE="$1   "
         ;;

         --version)
            printf "%s\n" "${MULLE_EXECUTABLE_VERSION}"
            exit 0
         ;;


         ##
         ## ADD YOUR FLAGS HERE
         ##

         -*)
            do::usage "Unknown flag \"$1\""
         ;;

         *)
            break
         ;;
      esac

      shift
   done

   options_setup_trace "${MULLE_TRACE}" && set -x

   local name="${1:-unknown}"

   r_extensionless_basename "${name}"
   r_identifier "${RVAL}"
   name="${RVAL}"

   local flexible="${OPTION_FLEXIBLE}"

   if [ "${OPTION_FLEXIBLE}" = 'DEFAULT' ]
   then
      flexible='NO'
      log_verbose "Type assumed to be inflexible"
   fi

   local uses_allocator="${OPTION_ALLOCATOR}"

   if [ "${uses_allocator}" = 'DEFAULT' ]
   then
      uses_allocator='NO'
      case "${name}" in
         *[a-zA-Z0-9]__[a-zA-Z0-9]*)
            log_verbose "Name contains '__' so assumed to use an allocator for done"
            uses_allocator='YES'
         ;;
      esac
   fi

   local embeds_allocator="${OPTION_ALLOCATOR}"

   if [ "${embeds_allocator}" = 'DEFAULT' ]
   then
      case "${name}" in
         *[a-zA-Z0-9]__[a-zA-Z0-9]*)
            log_verbose "Name contains '__' so assumed to not embed an allocator"
            embeds_allocator='NO'
         ;;

         *)
            log_verbose "Name contains no '__' or 'pointer' so assumed to embed an allocator"
            embeds_allocator='YES'
         ;;
      esac
   fi

   #
   #
   #
   local embeds_callback="${OPTION_CALLBACK}"

   if [ "${embeds_callback}" = 'DEFAULT' ]
   then
      case "${name}" in
         *pointer*)
            log_verbose "Name contains 'pointer' so assumed to not embed callbacks"
            embeds_callback='NO'
         ;;

         *[a-zA-Z0-9]__[a-zA-Z0-9]*)
            log_verbose "Name contains '__' so assumed to not embed callbacks"
            embeds_callback='NO'
         ;;

         *)
            embeds_callback='YES'
         ;;
      esac
   fi

   local uses_callback="${OPTION_CALLBACK}"

   if [ "${uses_callback}" = 'DEFAULT' ]
   then
      uses_callback='YES'
      if [ "${embeds_callback}" = 'YES' ]
      then
         uses_callback='NO'
      else
         case "${name}" in
            *pointer*)
               log_verbose "Name contains 'pointer' so assumed to not use callbacks"
               uses_callback='NO'
            ;;
         esac
      fi
   fi

   local compare="${OPTION_COMPARE}"

   log_setting "name             : ${name}"
   log_setting "flexible         : ${flexible}"
   log_setting "compare          : ${compare}"
   log_setting "embeds_allocator : ${embeds_allocator}"
   log_setting "uses_allocator   : ${uses_allocator}"
   log_setting "embeds_callback  : ${embeds_callback}"
   log_setting "uses_callback    : ${uses_callback}"

   printf "%s\n" "// created by ${MULLE_EXECUTABLE_NAME} ${args}"
   echo

   local needs_callback

   if [ "${embeds_callback}" = 'YES' -o "${uses_callback}" = 'YES' ]
   then
      needs_callback='YES'
   fi

   if [ "${embeds_callback}" = 'NO' -a "${embeds_allocator}" = 'NO' ]
   then
      log_verbose "As neither allocator nor callback is embedded, compare won't be too"
      compare='NO'
   fi

   (
      do::emit_macro_definition "${name}" "${embeds_allocator}" "${needs_callback}" "${flexible}" "${compare}"
      do::emit_loop_start "${name}" "${embeds_allocator}" "${embeds_callback}" "${flexible}" "${compare}"
      do::emit_loop_test "${name}" "${uses_allocator}" "${uses_callback}"
      do::emit_loop_end "${name}" "${uses_allocator}" "${uses_callback}"

      echo

      do::emit_break_protection
   ) | do::add_backslashes


   echo

   if [ "${flexible}" = 'YES' ]
   then
      (
         do::emit_flexible_macro_definition "${name}" "${embeds_allocator}" "${needs_callback}" "${flexible}" "${compare}"
         do::emit_flexible_loop_start  "${name}" "${embeds_allocator}" "${embeds_callback}" "${flexible}" "${compare}"
         do::emit_loop_test "${name}" "${uses_allocator}" "${uses_callback}"
         do::emit_loop_end "${name}" "${uses_allocator}" "${uses_callback}"

         echo

         do::emit_break_protection

      ) | do::add_backslashes

      echo
   fi
}

#
# You can also use the function `call_with_flags`, which has been defined
# during mulle-boot. It lets you call 'do::main'
# with MAKE_DO_FLAGS interposed.
#
# call_with_flags "do::main" "${MAKE_DO_FLAGS}" "$@"

do::main "$@"
