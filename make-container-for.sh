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


for::print_flags()
{
   echo "   -f             : force operation"
   echo "   --callback     : force to use callback code"
   echo "   --no-callback  : force to not use callback code"
   echo "   --compare      : force to use compare code"
   echo "   --no-compare   : force to not use compare code"
   echo "   --key-value    : force to use key value enumerator"
   echo "   --no-flexible  : force to use single item enumerator"
   echo "   --reverse      : force to use reverse enumerator code"
   echo "   --no-reverse   : force to not use reverse enumerator code"

   options_technical_flags_usage \
           "          : "
}


for::usage()
{
   [ $# -ne 0 ] && log_error "$*"


   cat <<EOF >&2
Usage:
   make-container-for [flags] <filename>

   make-container-for will try to discern from the filename, if the container
   utilizes callbacks (no pointer in name) and needs an external callback
   (-- in name).

Flags:
EOF
   for::print_flags | LC_ALL=C sort >&2

   exit 1
}

# AI generated
for::add_backslashes()
{
   log_entry "for::add_backslashes" "$@"

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


for::emit_for_macro_definition()
{
   log_entry "for::emit_for_macro_definition" "$@"

   local name="$1"
   local callback="$2"
   local keyvalue="$3"
   local reverse="$4"

   local callback_text

   if [ "${callback}" = 'YES' ]
   then
      callback_text=", callback"
   fi

   local keyvalue_text
   local assert_text

   keyvalue_text=", item"
   assert_text="\
   assert( sizeof( item) == sizeof( void *));"

   if [ "${keyvalue}" = 'YES' ]
   then
      keyvalue_text=", key, value"
      assert_text="\
   assert( sizeof( key) == sizeof( void *));
   assert( sizeof( value) == sizeof( void *));"

   fi

   local reverse_text

   if [ "${reverse}" = 'YES' ]
   then
      reverse_text="_reverse"
   fi

   cat <<EOF
#define ${name}_for${reverse_text}( name${callback_text}${keyvalue_text})
${assert_text}
EOF
}


# MEMO: why is _for using a pointer and _do using an identifier ?
#
# mulle_array_do( foo)
# {
#    mulle_array_for( foo)
#    {
#       // that's why
#    }
# }



for::emit_for_loop()
{
   log_entry "for::emit_for_loop" "$@"

   local name="$1"
   local callback="$2"
   local keyvalue="$3"
   local reverse="$4"

   local callback_text

   if [ "${callback}" = 'YES' ]
   then
      callback_text=", callback"
   fi

   local reverse_text

   if [ "${reverse}" = 'YES' ]
   then
      reverse_text="reverse"
   fi

   if [ "${keyvalue}" = 'YES' ]
   then
      cat <<EOF
   for( struct ${name}${reverse_text}enumerator
           rover__ ## key ## __ ## value = ${name}_${reverse_text}enumerate( name${callback_text}),
           *rover__  ## key ## __ ## value ## __i = (void *) 0;
        ! rover__  ## key ## __ ## value ## __i;
        rover__ ## key ## __ ## value ## __i = (_${name}${reverse_text}enumerator_done( &rover__ ## key ## __ ## value),
                                              (void *) 1))

      while( _${name}enumerator_next( &rover__ ## key ## __ ## value,
                                      (void **) &key,
                                      (void **) &value))
EOF
   else
      cat <<EOF
   for( struct ${name}${reverse_text}enumerator
           rover__ ## item = ${name}_${reverse_text}enumerate( name${callback_text}),
           *rover__  ## item ## __i = (void *) 0;
        ! rover__  ## item ## __i;
        rover__ ## item ## __i = (_${name}${reverse_text}enumerator_done( &rover__ ## item),
                                   (void *) 1))

      while( _${name}${reverse_text}enumerator_next( &rover__ ## item, (void **) &item))
EOF
   fi
}


for::main()
{
   local args="$*"

   #
   # simple option/flag handling
   #
   local OPTION_CALLBACK='DEFAULT'
   local OPTION_KEY_VALUE='DEFAULT'
   local OPTION_REVERSE='NO'

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
            for::usage
         ;;

         --callback)
            OPTION_CALLBACK='YES'
         ;;

         --no-callback)
            OPTION_CALLBACK='NO'
         ;;

         --key-value)
            OPTION_KEY_VALUE='YES'
         ;;

         --no-key-value)
            OPTION_KEY_VALUE='NO'
         ;;

         --reverse)
            OPTION_REVERSE='YES'
         ;;

         --no-reverse)
            OPTION_REVERSE='NO'
         ;;


         --version)
            printf "%s\n" "${MULLE_EXECUTABLE_VERSION}"
            exit 0
         ;;


         ##
         ## ADD YOUR FLAGS HERE
         ##

         -*)
            for::usage "Unknown flag \"$1\""
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

   local uses_key_value="${OPTION_KEY_VALUE}"

   if [ "${uses_key_value}" = 'DEFAULT' ]
   then
      uses_key_value='NO'
      case "${name}" in
         *[-_]map*|*[-_]assoc*)
            log_verbose "Name contains 'map' or 'assoc' so assumed to be key/value enumerator"
            uses_key_value='YES'
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
            *struct*)
               log_verbose "Name contains 'struct' so assumed to not use callbacks"
               uses_callback='NO'
            ;;

            *pointer*)
               log_verbose "Name contains 'pointer' so assumed to not use callbacks"
               uses_callback='NO'
            ;;
         esac
      fi
   fi

   log_setting "name           : ${name}"
   log_setting "uses_callback  : ${uses_callback}"
   log_setting "uses_key_value : ${uses_key_value}"
   log_setting "reverse        : ${OPTION_REVERSE}"

   printf "%s\n" "// created by ${MULLE_EXECUTABLE_NAME} ${args}"
   echo


   (
      for::emit_for_macro_definition "${name}" "${uses_callback}" "${uses_key_value}" "${OPTION_REVERSE}"
      for::emit_for_loop "${name}" "${uses_callback}" "${uses_key_value}" "${OPTION_REVERSE}"

   ) | for::add_backslashes

   echo
}

#
# You can also use the function `call_with_flags`, which has been defined
# during mulle-boot. It lets you call 'for::main'
# with MAKE_DO_FLAGS interposed.
#
# call_with_flags "for::main" "${MAKE_DO_FLAGS}" "$@"

for::main "$@"
