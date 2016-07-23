#!/bin/sh -m
# (bash really)
#
#  run-test.sh
#  MulleObjC
#
#  Created by Nat! on 01.11.13.
#  Copyright (c) 2013 Mulle kybernetiK. All rights reserved.
#  (was run-mulle-scion-test)

SOURCE_EXTENSION=".c"

LIBRARY_SHORTNAME="mulle_container"
SHLIB_PREFIX="lib"
SHLIB_EXTENSION=".so"
STANDALONE_SUFFIX="_standalone"

case `uname` in
   Darwin)
      SHLIB_EXTENSION=".dylib"
      ;;

   Linux)
     LDFLAGS="-ldl -lpthread"
esac

LIBRARY_FILENAME="${SHLIB_PREFIX}${LIBRARY_SHORTNAME}${STANDALONE_SUFFIX}${SHLIB_EXTENSION}"


if [ -z "${DEBUGGER}" ]
then
   DEBUGGER=lldb
fi

DEBUGGER="`which "${DEBUGGER}"`"

if [ -z "${DEBUGGER_LIBRARY_PATH}" ]
then
   DEBUGGER_LIBRARY_PATH="`dirname "${DEBUGGER}"`/../lib"
fi


# check if running a single test or all
DEFAULTCFLAGS="-w -O0 -g"

executable=`basename "$0"`
executable=`basename "$executable" .sh`

if [ "`basename "$executable"`" = "run-all-tests" ]
then
   TEST=""
   VERBOSE=yes
   if [ "$1" = "-q" ]
   then
      VERBOSE=no
      shift
   fi
else
   TEST="$1"
   [ -z $# ] || shift
fi

#
# this is system wide, not so great
# and also not trapped...
#
suppress_crashdumping()
{
   local restore

   case `uname` in
      Darwin)
         restore="`defaults read com.apple.CrashReporter DialogType 2> /dev/null`"
         defaults write com.apple.CrashReporter DialogType none
         ;;
      Linux)
         ;;
   esac

   echo "${restore}"
}


restore_crashdumping()
{
   local restore

   restore="$1"

   case `uname` in
      Darwin)
         if [ -z "${restore}" ]
         then
            defaults delete com.apple.CrashReporter DialogType
         else
            defaults write com.apple.CrashReporter DialogType "${restore}"
         fi
         ;;
      Linux)
         ;;
   esac
}


trace_ignore()
{
   restore_crashdumping "$1"
   return 0
}


RESTORE_CRASHDUMP=`suppress_crashdumping`
trap 'trace_ignore "${RESTORE_CRASHDUMP}"' 0 5 6

if [ -z "${CFLAGS}" ]
then
   CFLAGS="${DEFAULTCFLAGS}"
fi

# find runtime and headers
#
# this is more or less an ugly hack, that should work
# if
#    a) you used mulle-clang-install
# or
#    b) have a setup like mine
#
#        ./mulle-clang-install/tests
#        ./mulle-objc-runtime
#

# cmake
lib="`ls -1 "../lib/${LIBRARY_FILENAME}" 2> /dev/null | tail -1`"
DEPENDENCIES_INCLUDE="../dependencies/include"

if [ ! -f "${lib}" ]
then
   # xcode
   lib="`ls -1 "../build/Products/Debug/${LIBRARY_FILENAME}" | tail -1 2> /dev/null`"
fi

LIBRARY="${1:-${lib}}"
[ -z $# ] || shift

if [ -z "${LIBRARY}" ]
then
   echo "${LIBRARY_FILENAME} can not be found" >&2
   exit 1
fi


LIBRARY_INCLUDE="`dirname "${LIBRARY}"`"
if [ -d "${LIBRARY_INCLUDE}/usr/local/include" ]
then
   # xcode
   LIBRARY_INCLUDE="${LIBRARY_INCLUDE}/usr/local/include"
else
   if [ -d "${LIBRARY_INCLUDE}/include" ]
   then
      # xcode2
      LIBRARY_INCLUDE="${LIBRARY_INCLUDE}/include"
   else
      # cmake
      LIBRARY_INCLUDE="`dirname "${LIBRARY_INCLUDE}"`/include"
   fi
fi


DIR=${1:-`pwd`}
shift

HAVE_WARNED="no"
RUNS=0

search_plist()
{
   local plist
   local root

   dir=`dirname "$1"`
   plist=`basename "$1"`
   root="$2"

   while :
   do
      if [ -f "$dir"/"$plist" ]
      then
         echo "$dir/$plist"
         break
      fi

      if [ "$dir" = "$root" ]
      then
         break
      fi

      next=`dirname "$dir"`
      if [ "$next" = "$dir" ]
      then
         break
      fi
      dir="$next"
   done
}


relpath()
{
   python -c "import os.path; print os.path.relpath('$1', '$2')"
}



absolute_path_if_relative()
{
   case "$1" in
      .*)  echo "`pwd`/$1"
      ;;

      *) echo "$1"
      ;;
   esac
}


maybe_show_diagnostics()
{
   local errput

   errput="$1"

   local contents
   contents="`head -2 "$errput"`" 2> /dev/null
   if [ "${contents}" != "" ]
   then
      echo "DIAGNOSTICS:" >&2
      cat  "$errput"
   fi
}


maybe_show_output()
{
   local output

   output="$1"

   local contents
   contents="`head -2 "$output"`" 2> /dev/null
   if [ "${contents}" != "" ]
   then
      echo "--------------------------------------------------------------" >&2
      echo "OUTPUT:" >&2
      cat  "$output"
   fi
}


search_for_strings()
{
   local errput
   local strings
   local banner

   banner="$1"
   errput="$2"
   strings="$3"

   local fail
   local expect
   local match

   fail=0
   while read expect
   do
      if [ ! -z "$expect" ]
      then
         match=`grep "$expect" "$errput"`
         if [ "$match" = "" ]
         then
            if [ $fail -eq 0 ]
            then
               echo "${banner}" >&2
               fail=1
            fi
            echo "   $expect" >&2
         fi
      fi
   done < "$strings"

   [ $fail -eq 1 ]
}


fail_test()
{
   local m_source
   local a_out
   local stdin

   m_source="$1"
   a_out="$2"
   stdin="$3"

   echo "DEBUG: " >&2
   echo "rebuilding with -O0 and debug symbols..." >&2
   $MULLE_CLANG -O0 -g -o "${a_out}.debug" \
      -fobjc-runtime=mulle \
      "-I${LIBRARY_INCLUDE}" \
      "-I${DEPENDENCIES_INCLUDE}" \
      ${LDFLAGS} \
      "${LIBRARY}" \
      "${m_source}" > "$errput" 2>&1

   echo "MULLE_OBJC_AUTORELEASEPOOL_TRACE=15 \
MULLE_OBJC_TEST_ALLOCATOR=1 \
MULLE_TEST_ALLOCATOR_TRACE=2 \
MallocStackLogging=1 \
MALLOC_FILL_SPACE=1 \
DYLD_INSERT_LIBRARIES=/usr/lib/libgmalloc.dylib \
DYLD_FALLBACK_LIBRARY_PATH=\"${DYLD_FALLBACK_LIBRARY_PATH}\" \
LD_LIBRARY_PATH=\"${LD_LIBRARY_PATH}:${DEBUGGER_LIBRARY_PATH}\" ${DEBUGGER} ${a_out}.debug" >&2
   if [ "${stdin}" != "/dev/null" ]
   then
      echo "run < ${stdin}" >&2
   fi
   exit 1
}



run()
{
   local m_source
   local root
   local stdin
   local stdout
   local stderr
   local ccdiag

   m_source="$1"
   root="$2"
   stdin="$3"
   stdout="$4"
   stderr="$5"
   ccdiag="$6"

   local output
   local errput
   local random
   local fail
   local match

   random=`mktemp -t "${LIBRARY_SHORTNAME}.XXXX"`
   output="$random.stdout"
   errput="$random.stderr"
   errors=`basename $m_source ${SOURCE_EXTENSION}`.errors

   local owd

   owd=`pwd`
   pretty_source=`relpath "$owd"/"$m_source" "$root"`

   if [ "$VERBOSE" = "yes" ]
   then
      echo "$pretty_source" >&2
   fi

   a_out="${owd}/`basename "$m_source" ${SOURCE_EXTENSION}`.exe"

   RUNS=`expr "$RUNS" + 1`

   # plz2shutthefuckup bash
   set +m
   set +b
   set +v
   # denied, will always print TRACE/BPT

   local rval

   if [ -z "${CC}" ]
   then
      $MULLE_CLANG ${CFLAGS} -o "${a_out}" \
      -fobjc-runtime=mulle \
      "-I${LIBRARY_INCLUDE}" \
      "-I${DEPENDENCIES_INCLUDE}" \
      "${LIBRARY}" \
      ${LDFLAGS} \
      "${m_source}" > "$errput" 2>&1
      rval=$?
   else
      "${CC}" ${CFLAGS} -o "${a_out}" ${LDFLAGS} -framework Foundation "${m_source}" > "$errput" 2>&1
      rval=$?
   fi

   if [ $rval -ne 0 ]
   then
      if [ "$ccdiag" = "-" ]
      then
         echo "COMPILER ERRORS: \"$pretty_source\"" >&2
         maybe_show_diagnostics "$errput"
         exit 1
      else
         search_for_strings "COMPILER FAILED TO PRODUCE ERRORS: \"$pretty_source\" ($errput)" \
                            "$errput" "$ccdiag"
         if [ $? -ne 0 ]
         then
            return 0
         fi
         maybe_show_diagnostics "$errput" >&2
         exit 1
      fi
   fi

   MULLE_OBJC_TEST_ALLOCATOR=1 \
MallocStackLogging=1 \
MallocScribble=1 \
MallocPreScribble=1 \
MallocGuardEdges=1 \
MallocCheckHeapEach=1 \
\
   "${a_out}" < "$stdin" > "$output" 2> "$errput"
   rval=$?

   if [ $rval -ne 0 ]
   then
      if [ ! -f "$errors" ]
      then
         echo "TEST CRASHED: \"$pretty_source\" (${a_out}, ${errput})" >&2
         maybe_show_diagnostics "$errput" >&2

         fail_test "${m_source}" "${a_out}" "${stdin}"
      else
         search_for_strings "TEST FAILED TO PRODUCE ERRORS: \"$pretty_source\" ($errput)" \
                            "$errput" "$errors"
         if [ $? -ne 0 ]
         then
            return 0
         fi
         maybe_show_diagnostics "$errput" >&2
         fail_test "${m_source}" "${a_out}" "${stdin}"
      fi
   else
      if [ -f "$errors" ]
      then
         echo "TEST FAILED TO CRASH: \"$pretty_source\" (${a_out})" >&2
         maybe_show_diagnostics "$errput" >&2
         fail_test "${m_source}" "${a_out}" "${stdin}"
      fi
   fi

   if [ "$stdout" != "-" ]
   then
      result=`diff -q "$stdout" "$output"`
      if [ "$result" != "" ]
      then
         white=`diff -q -w "$stdout" "$output"`
         if [ "$white" != "" ]
         then
            echo "FAILED: \"$pretty_source\" produced unexpected output" >&2
            echo "DIFF: ($output vs. $stdout)" >&2
            diff -y "$output" "$stdout" >&2
         else
            echo "FAILED: \"$pretty_source\" produced different whitespace output" >&2
            echo "DIFF: ($stdout vs. $output)" >&2
            od -a "$output" > "$output.actual.hex"
            od -a "$stdout" > "$output.expect.hex"
            diff -y "$output.expect.hex" "$output.actual.hex" >&2
         fi

         maybe_show_diagnostics "$errput" >&2
         maybe_show_output "$output"

         fail_test "${m_source}" "${a_out}" "${stdin}"
      fi
   else
      contents="`head -2 "$output"`" 2> /dev/null
      if [ "${contents}" != "" ]
      then
         echo "WARNING: \"$pretty_source\" produced unexpected output ($output)" >&2

         maybe_show_diagnostics "$errput" >&2
         maybe_show_output "$output"

         fail_test "${m_source}" "${a_out}" "${stdin}"
      fi
   fi

   if [ "$stderr" != "-" ]
   then
      result=`diff "$stderr" "$errput"`
      if [ "$result" != "" ]
      then
         echo "WARNING: \"$pretty_source\" produced unexpected diagnostics ($errput)" >&2
         echo "" >&2
         diff "$stderr" "$errput" >&2

         maybe_show_diagnostics "$errput"
         fail_test "${m_source}" "${a_out}" "${stdin}"
      fi
   fi
}


run_test()
{
   local m_source
   local root

   m_source="$1${SOURCE_EXTENSION}"
   root="$2"

   local stdin
   local stdout
   local stderr
   local plist

   stdin="$1.stdin"
   if [ ! -f "$stdin" ]
   then
      stdin="provide/$1.stdin"
   fi
   if [ ! -f "$stdin" ]
   then
      stdin="default.stdin"
   fi
   if [ ! -f "$stdin" ]
   then
      stdin="/dev/null"
   fi

   stdout="$1.stdout"
   if [ ! -f "$stdout" ]
   then
      stdout="expect/$1.stdout"
   fi
   if [ ! -f "$stdout" ]
   then
      stdout="default.stdout"
   fi
   if [ ! -f "$stdout" ]
   then
      stdout="-"
   fi

   stderr="$1.stderr"
   if [ ! -f "$stderr" ]
   then
      stderr="expect/$1.stderr"
   fi
   if [ ! -f "$stderr" ]
   then
      stderr="default.stderr"
   fi
   if [ ! -f "$stderr" ]
   then
      stderr="-"
   fi

   ccdiag="$1.ccdiag"
   if [ ! -f "$ccdiag" ]
   then
      ccdiag="expect/$1.ccdiag"
   fi
   if [ ! -f "$ccdiag" ]
   then
      ccdiag="default.ccdiag"
   fi
   if [ ! -f "$ccdiag" ]
   then
      ccdiag="-"
   fi

   run "$m_source" "$root" "$stdin" "$stdout" "$stderr" "$ccdiag"
}


scan_current_directory()
{
   local root

   root="$1"

   local i
   local filename
   local dir

   for i in [^_]*
   do
      if [ -d "$i" ]
      then
         dir=`pwd`
         cd "$i"
         scan_current_directory "$root"
         cd "$dir"
      else
         filename=`basename "$i" ${SOURCE_EXTENSION}`
         if [ "$filename" != "$i" ]
         then
            run_test "$filename" "$root"
         fi
      fi
   done
}


test_binary()
{
   "$MULLE_CLANG" > /dev/null 2>&1
   code=$?

   if [ $code -eq 127 ]
   then
      echo "mulle-clang can not be found" >&2
      exit 1
   fi

   echo "using ${MULLE_CLANG} to test" >&2
}


LIBRARY="`absolute_path_if_relative "$LIBRARY"`"
LIBRARY_INCLUDE="`absolute_path_if_relative "$LIBRARY_INCLUDE"`"
DEPENDENCIES_INCLUDE="`absolute_path_if_relative "$DEPENDENCIES_INCLUDE"`"

# OS X
DYLD_FALLBACK_LIBRARY_PATH="`dirname "${LIBRARY}"`" ; export DYLD_FALLBACK_LIBRARY_PATH
# Linux
LD_LIBRARY_PATH="`dirname "${LIBRARY}"`" ; export LD_LIBRARY_PATH


if [ -z "${CC}" ]
then
   MULLE_CLANG="`absolute_path_if_relative "mulle-clang"`"
   test_binary "$MULLE_CLANG"
fi


if [ "$TEST" = "" ]
then
   cd "${DIR}"
   scan_current_directory "`pwd -P`"

   if [ "$RUNS" -ne 0 ]
   then
      echo "All tests ($RUNS) passed successfully"
   else
      echo "no tests found" >&2
      exit 1
   fi
else
    dirname=`dirname "$TEST"`
    if [ "$dirname" = "" ]
    then
       dirname="."
    fi
    file=`basename "$TEST"`
    filename=`basename "$file" "${SOURCE_EXTENSION}"`

    if [ "$file" = "$filename" ]
    then
       echo "error: source file must have ${SOURCE_EXTENSION} extension" >&2
       exit 1
    fi

    if [ ! -f "$TEST" ]
    then
       echo "error: source file not found" >&2
       exit 1
    fi

    old="`pwd -P`"
    cd "${dirname}" || exit 1
    run_test "$filename" "${old}"
    rval=$?
    cd "${old}" || exit 1
    exit $rval
fi
