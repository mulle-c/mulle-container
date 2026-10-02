#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
cmake-graph - render this project's CMake system as graphviz.

WHAT IT SHOWS
-------------
Every `.cmake` file (plus the root `CMakeLists.txt`) becomes one node. The node
is an HTML table that reads top to bottom:

    +-------------------------------------------+
    | Library.cmake                    (header) |   <- include() arrows land here
    | in  - phase:       LINK_PHASE             |   <- ports "in_<group>"
    | in  - identity:    LIBRARY_NAME ...       |
    | includes: 1 PreLibrary  2 InstallRpath... |   <- include order, in order
    | out - targets:     LIBRARY_COMPILE_TARGET |   <- ports "out_<group>"
    | out - identity:    LIBRARY_IDENTIFIER ... |
    +-------------------------------------------+

So values arrive at the top, values leave at the bottom. Rows are colored by
*group* - all path-ish values share one color, all phase values another, etc.
Solid colored edges are values handed from the file that produces them (out
port) to every file that consumes them (in port); the edge is colored by group.
Dashed grey edges are `include()` and carry the include order number.

Four graphs are produced:

  * `pix/cmake.dot`           - the value-flow view described above
  * `pix/cmake-order.dot`     - the include-order view (compact nodes, order labels)
  * `pix/cmake-commands.dot`  - the flat, pre-processed view: every effectful
                                cmake command (add_library, install, target_*...)
                                beside the file that issues it, in the order cmake
                                reaches that file, labeled with the enclosing
                                if()/foreach() that has to be true for it to run
  * `pix/cmake-state.dot`     - the global-state view: only the writes that
                                escape the file making them, grouped by blast
                                radius - process/directory scope, cache keys,
                                the link+compile contract of the library, the
                                install tree. Answers "what does running this
                                config change outside itself?"

The parser is deliberately approximate: it is a regex pass over comment-stripped
source, not a CMake interpreter. It understands `include/set/unset/option/list/
foreach/function/macro` and `${...}` / `$ENV{...}` / `$CACHE{...}` references,
and the control flow around effectful commands.

USAGE
-----
    tools/cmake-graph.py                      # both graphs into pix/
    tools/cmake-graph.py --focus Library      # only Library.cmake +- 1 hop
    tools/cmake-graph.py --all --no-hooks     # every cmake file, no Aux hooks
    tools/cmake-graph.py --show-hidden        # also show TMP_*/guards/loop vars
    tools/cmake-graph.py --layout value       # rank by value flow instead of include()
    tools/cmake-graph.py --mode commands      # only the flat commands view
    tools/cmake-graph.py --mode state         # only the global-state view
    tools/cmake-graph.py --show-private       # state view: also PRIVATE target writes
    tools/cmake-graph.py --render             # write .svg next to the .dot

Requires graphviz (`dot`) only when --render is given.
"""

from __future__ import annotations

import argparse
import html as _html
import os
import re
import shutil
import subprocess
import sys
from collections import defaultdict, OrderedDict

REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

# Mirrors the CMAKE_MODULE_PATH order built in CMakeLists.txt.
# in CMAKE_MODULE_PATH order - CMakeLists.txt inserts share, then reflect, then
# cmake at the front, so the first hit wins in this order
MODULE_DIRS = ["cmake", "cmake/reflect", "cmake/share"]

# Groups that are noise for a human reading the picture. --show-hidden shows them.
HIDDEN_BY_DEFAULT = {"guard", "internal", "debug"}

# Available to every cmake file everywhere; drawing them adds edges, not insight.
NOISE_NAMES = {
    "CMAKE_CURRENT_LIST_FILE", "CMAKE_CURRENT_LIST_DIR", "CMAKE_CURRENT_LIST_LINE",
    "CMAKE_CURRENT_SOURCE_DIR", "CMAKE_CURRENT_BINARY_DIR", "CMAKE_SOURCE_DIR",
    "CMAKE_BINARY_DIR", "CMAKE_VERSION", "CMAKE_COMMAND", "CMAKE_ARGC",
}

# group -> (fill color for table rows, stroke color for edges)
GROUP_STYLE = OrderedDict([
    ("phase",    ("#f9d98c", "#b8860b")),
    ("identity", ("#f7b3c2", "#c2185b")),
    ("build",    ("#e6b3f0", "#8e24aa")),
    ("install",  ("#f8c8a0", "#e65100")),
    ("files",    ("#a8e6a3", "#2e7d32")),
    ("deps",     ("#c7b3f7", "#4527a0")),
    ("compiler", ("#9fe3e3", "#00695c")),
    ("targets",  ("#7fd4a0", "#00695c")),
    ("paths",    ("#a9cdf5", "#1565c0")),
    ("cmake",    ("#d9d9d9", "#616161")),
    ("platform", ("#c8d9c8", "#4e6b4e")),
    ("env",      ("#f2e6a6", "#9a8000")),
    ("function", ("#bcbcf0", "#303f9f")),
    ("other",    ("#e8e8e8", "#757575")),
    ("internal", ("#f4f4f4", "#bdbdbd")),
    ("debug",    ("#f4f4f4", "#bdbdbd")),
    ("guard",    ("#f4f4f4", "#bdbdbd")),
])
GROUP_ORDER = [g for g in GROUP_STYLE if g not in HIDDEN_BY_DEFAULT]

# Where a file lives decides how much you are allowed to care about editing it:
# cmake/ is yours, cmake/share/ is rewritten by `mulle-sde upgrade`, and
# cmake/reflect/ is generated. `header` is the dark variant used behind white
# text in the flow graph, `fill`/`stroke` the pastel used in the order graph.
FOLDER_STYLE = OrderedDict([
    ("root",    ("#fff6d6", "#b8860b", "#2b2b2b",
                 "root  CMakeLists.txt")),
    ("cmake",   ("#dcf5dc", "#2e7d32", "#1f5f24",
                 "cmake/  your overrides, safe to edit")),
    ("share",   ("#dbe8f7", "#1565c0", "#1f3a5f",
                 "cmake/share/  managed, clobbered on upgrade")),
    ("reflect", ("#ecdff7", "#6a1b9a", "#452a63",
                 "cmake/reflect/  generated, never edit")),
])


FOLDER_SHORT = {
    "root": "root /",
    "cmake": "cmake /",
    "share": "cmake/share /",
    "reflect": "cmake/reflect /",
}


def origin(f):
    """Which directory a cmake file came from."""
    rel = f.relpath.replace("\\", "/")
    if not rel.startswith("cmake/"):
        return "root"
    rest = rel[len("cmake/"):]
    for d in ("share", "reflect"):
        if rest.startswith(d + "/"):
            return d
    return "cmake"

# Ordered: first match wins. Deliberately over-specific names before the
# catch-all regexes, because e.g. LIBRARY_SOURCES is a file list, not a library.
GROUP_RULES = [
    ("guard",    r"^__[A-Za-z0-9_]+__CMAKE__$"),
    ("debug",    r"^MULLE_TRACE_INCLUDE$"),
    ("phase",    r"^[A-Z_]*PHASE$"),
    ("identity", r"^(PROJECT|LIBRARY|STANDALONE|EXECUTABLE|FRAMEWORK|CONFIG)_"
                 r"(IDENTIFIER|UPCASE_IDENTIFIER|DOWNCASE_IDENTIFIER|NAME|VERSION)$"
                 r"|^(PROJECT_NAME|LIBRARY_NAME|EXECUTABLE_NAME|STANDALONE_LIBRARY_NAME)$"),
    ("build",    r"^(BUILD_SHARED_LIBS|CMAKE_BUILD_TYPE|DLL_EXPORT_ALL|STANDALONE|"
                 r"SHARED_UNRESOLVED_SYMBOLS|MULLE_TEST|IDE_SUPPORT|"
                 r"RESOLVE_INSTALLABLE_HEADER_SYMLINKS|INHERIT_DEPENDENCY_INCLUDES|"
                 r"INHERIT_DEPENDENCY_DEFINITIONS|MULLE_TRANSITIVE_LINK_LIBRARIES|"
                 r"MULLE_TRANSITIVE_SOURCE_INCLUDE|MULLE_NO_CMAKE_INSTALL_RPATH|"
                 r"MUSL_STATIC_ONLY|COSMOPOLITAN|CMAKE_POSITION_INDEPENDENT_CODE|"
                 r"CMAKE_SKIP_RPATH|CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS|"
                 r"CMAKE_DEBUG_POSTFIX|CMAKE_FIND_LIBRARY_SUFFIXES|"
                 r"WANTED_WARNINGS|UNWANTED_WARNINGS)$"),
    ("install",  r"^(INSTALL|ASSET_DIR|CONFIG_CMAKE_IN_FILENAME|HEADER_CMAKE_IN_FILENAME)"),
    ("internal", r"^_(?:[A-Z][A-Z0-9]*_|[a-z])"),
    ("files",    r"^(SOURCES|LIBRARY_SOURCES|LIBRARY_RESOURCES|STAGE2_SOURCES|STAGE2_HEADERS|"
                 r"PROJECT_FILES|PROJCT_CMAKE_EDITABLE_FILES|INCLUDE_DIRS|TMP_HEADERS|"
                 r"HEADER_OUTPUTS|OTHER_[A-Z_]*OBJECT_FILES|OBJC_DEPS_INC|RESOURCES|"
                 r"STAGE2_HEADERS)$"
                 r"|^(PUBLIC|PRIVATE)_(GENERIC_|GENERATED_)?HEADERS$"),
    ("deps",     r"(DEPENDEN|ALL_LOAD|OPTIONAL_DEPENDENCY|OS_SPECIFIC|STARTUP_|"
                 r"HEADER_ONLY_LIBRARIES|SHARED_LIBRARY_LIST|INHERITED_|FORCE_|"
                 r"LIBRARIES|FRAMEWORK)"),
    ("compiler", r"(COMPILER_ID|_FLAGS$|_WARNINGS$|^MULLE_LANGUAGE$|^MULLE_C_|^MULLE_CXX_|"
                 r"^CMAKE_C_FLAGS|^CMAKE_CXX_FLAGS|^CMAKE_(EXE|SHARED|STATIC)_LINKER_FLAGS)"),
    ("targets",  r"^(LIBRARY_(COMPILE|LINK|STAGE2)_TARGET|INSTALL_LIBRARY_TARGETS|"
                 r"ALL_OBJECT_FILES|MULLE_HEADER_STAGING_TARGETS)$|_TARGETS$"),
    ("platform", r"^(WIN32|APPLE|UNIX|MSVC|MINGW|MINGW32|MSYS|CYGWIN|ANDROID|EMSCRIPTEN|"
                 r"WASI|WATCHOS|IOS|MACOS|TVOS|VISIONOS|MULLE_UNAME|MULLE_TARGET_TRIPLE)$"),
    ("env",      r"^ENV:"),
    ("cmake",    r"^CMAKE_"),
    ("paths",    r"((^|_)(DIR|DIRS|PATH|PATHS|ROOT|FILENAME|FILE)$|"
                 r"MULLE_VIRTUAL_ROOT|MULLE_SDK_)"),
    ("internal", r"^(TMP_|_[a-z]|_[A-Z]|__)"),
]

# Lower case names are cmake locals / loop vars -> internal.
LOWER_LOCAL = re.compile(r"^[a-z][a-z0-9_]*$")

KEYWORDS = {
    "NOT", "AND", "OR", "DEFINED", "EXISTS", "COMMAND", "TARGET", "TEST",
    "IS_DIRECTORY", "IS_ABSOLUTE", "IS_NEWER_THAN", "MATCHES", "STREQUAL",
    "STRLESS", "STRGREATER", "STRLESS_EQUAL", "STRGREATER_EQUAL", "LESS",
    "GREATER", "EQUAL", "LESS_EQUAL", "GREATER_EQUAL", "VERSION_LESS",
    "VERSION_GREATER", "VERSION_EQUAL", "VERSION_LESS_EQUAL",
    "VERSION_GREATER_EQUAL", "IN_LIST", "POLICY", "IF", "ELSE", "ENDIF",
    "TRUE", "FALSE", "ON", "OFF", "YES", "NO", "Y", "N", "IGNORE", "NOTFOUND",
    "REQUIRED", "OPTIONAL", "PRIVATE", "PUBLIC", "INTERFACE", "SYSTEM",
    "BEFORE", "AFTER", "PROPERTY", "PROPERTIES", "APPEND", "STRING", "FILEPATH",
    "PATH", "BOOL", "INTERNAL", "PARENT_SCOPE", "RELEASE", "DEBUG",
    "RELWITHDEBINFO", "MINSIZEREL", "NONE", "STATIC", "SHARED", "MODULE",
    "OBJECT", "EXCLUDE_FROM_ALL", "IMPORTED", "GLOBAL", "ALIAS", "VERBATIM",
    "COMMAND", "COMMENT", "OUTPUT", "DEPENDS", "WORKING_DIRECTORY", "ALL",
    "CACHE", "FORCE", "ENV", "CMAKE_CURRENT_LIST_FILE",
}

# --------------------------------------------------------------------------- #
# effectful cmake commands (the ones that actually build something)
# --------------------------------------------------------------------------- #

# command -> family, used for grouping and coloring in the commands view
EFFECT_COMMANDS = {
    "add_library": "create",
    "add_executable": "create",
    "add_custom_target": "create",
    "add_subdirectory": "create",
    "target_link_libraries": "link",
    "target_link_options": "link",
    "add_dependencies": "link",
    "target_compile_definitions": "compile",
    "target_compile_options": "compile",
    "target_include_directories": "compile",
    "add_definitions": "compile",
    "add_compile_definitions": "compile",
    "add_compile_options": "compile",
    "include_directories": "compile",
    "set_target_properties": "props",
    "set_property": "props",
    "set_source_files_properties": "props",
    "install": "install",
    "export": "install",
    "configure_file": "install",
    "configure_package_config_file": "install",
    "write_basic_package_version_file": "install",
    "add_custom_command": "custom",
    "cmake_policy": "config",
    "find_library": "probe",
    "find_program": "probe",
    "execute_process": "probe",
}

# family -> (fill, stroke)
FAMILY_STYLE = OrderedDict([
    ("create",  ("#a8e6a3", "#2e7d32")),
    ("link",    ("#c7b3f7", "#4527a0")),
    ("compile", ("#9fe3e3", "#00695c")),
    ("props",   ("#a9cdf5", "#1565c0")),
    ("install", ("#f8c8a0", "#e65100")),
    ("custom",  ("#f7b3c2", "#c2185b")),
    ("config",  ("#f9d98c", "#b8860b")),
    ("probe",   ("#d9d9d9", "#616161")),
])

BLOCK_OPEN = ("if", "elseif", "foreach", "while", "function", "macro", "block")
BLOCK_CLOSE = ("endif", "endforeach", "endwhile", "endfunction", "endmacro", "endblock")

RE_CMDTOK = re.compile(r"(?<![A-Za-z0-9_$])([a-z_][a-z0-9_]*)\s*\(")

RE_VARREF = re.compile(r"\$(?:CACHE|ENV)?\{([A-Za-z_][A-Za-z0-9_]*)\}")
RE_ENVREF = re.compile(r"\$ENV\{([A-Za-z_][A-Za-z0-9_]*)\}")
RE_INCLUDE = re.compile(r"\binclude\s*\(\s*([A-Za-z0-9_./+-]+)\s*(OPTIONAL)?\s*\)")
RE_SET = re.compile(r"\bset\s*\(\s*(?:CACHE\{)?\s*([A-Za-z_][A-Za-z0-9_]*)")
RE_UNSET = re.compile(r"\bunset\s*\(\s*(?:CACHE\{)?\s*([A-Za-z_][A-Za-z0-9_]*)")
RE_OPTION = re.compile(r"\boption\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)")
RE_LIST_MOD = re.compile(r"\blist\s*\(\s*(?:APPEND|PREPEND|INSERT|REMOVE_ITEM|"
                         r"REMOVE_DUPLICATES|REVERSE|SORT|FILTER|TRANSFORM|POP_BACK|"
                         r"POP_FRONT)\s+([A-Za-z_][A-Za-z0-9_]*)")
RE_LIST_READ = re.compile(r"\blist\s*\(\s*(?:LENGTH|GET)\s+([A-Za-z_][A-Za-z0-9_]*)\s+"
                          r"([A-Za-z_][A-Za-z0-9_]*)")
RE_FOREACH = re.compile(r"\bforeach\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)")
RE_OUTPUT_VAR = re.compile(r"\bOUTPUT_VARIABLE\s+([A-Za-z_][A-Za-z0-9_]*)")
RE_PROJECT = re.compile(r"\bproject\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)")
# mulle convention: these helpers take the name of the variable they fill as
# their last argument instead of using set()
FUNC_OUT_LAST = {"snakeCaseString", "ResolveFileSymlinksIfNeeded"}
RE_FUNC = re.compile(r"\b(function|macro)\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)")
RE_COND = re.compile(r"\b(?:if|elseif|while)\s*\(([^()]*)\)", re.S)
RE_WORD = re.compile(r"(?<![\w${])([A-Za-z_][A-Za-z0-9_]*)(?![\w}])")
RE_UPPER = re.compile(r"^[A-Z][A-Z0-9_]*$")


# --------------------------------------------------------------------------- #
# parsing
# --------------------------------------------------------------------------- #

def strip_comments(text: str) -> str:
    """Drop `# ...` to end of line, but not inside a double quoted string."""
    out = []
    for line in text.splitlines():
        res, in_str, i = [], False, 0
        while i < len(line):
            c = line[i]
            if c == "\\" and in_str:
                res.append(line[i:i + 2])
                i += 2
                continue
            if c == '"':
                in_str = not in_str
            elif c == "#" and not in_str:
                break
            res.append(c)
            i += 1
        out.append("".join(res))
    return "\n".join(out)


def match_paren(text, open_idx):
    """Index of the ')' closing the '(' at open_idx, string-aware."""
    depth, i, in_str = 0, open_idx, False
    while i < len(text):
        c = text[i]
        if in_str:
            if c == "\\":
                i += 2
                continue
            if c == '"':
                in_str = False
        elif c == '"':
            in_str = True
        elif c == "(":
            depth += 1
        elif c == ")":
            depth -= 1
            if depth == 0:
                return i
        i += 1
    return len(text) - 1


def split_args(args):
    """Whitespace split that keeps quoted runs together."""
    out, cur, in_str = [], "", False
    for c in args:
        if c == '"':
            in_str = not in_str
            cur += c
        elif c.isspace() and not in_str:
            if cur:
                out.append(cur)
                cur = ""
        else:
            cur += c
    if cur:
        out.append(cur)
    return out


def unquote(tok):
    return tok.strip('"')


class CmakeFile:
    def __init__(self, name: str, path: str, relpath: str):
        self.name = name
        self.path = path
        self.relpath = relpath
        self.title = relpath if name == "CMakeLists" else name + ".cmake"
        self.includes = []          # [(order, target_name, optional)]
        self.inputs = set()
        self.outputs = set()
        self.functions = set()      # defined here
        self.calls = set()          # functions called here
        self.envs = set()
        self.text = ""              # comment-stripped source, cached for later passes

    def __repr__(self):
        return "<%s>" % self.relpath


def scan(root: str, want_all: bool):
    """Return (files, index) where index maps include-name -> CmakeFile."""
    files = OrderedDict()

    def add(name, path, relpath):
        files[name] = CmakeFile(name, path, relpath)

    cl = os.path.join(root, "CMakeLists.txt")
    if os.path.exists(cl):
        add("CMakeLists", cl, "CMakeLists.txt")

    for d in MODULE_DIRS:
        full = os.path.join(root, d)
        if not os.path.isdir(full):
            continue
        for fn in sorted(os.listdir(full)):
            if not fn.endswith(".cmake"):
                continue
            stem = fn[:-len(".cmake")]
            rel = os.path.join(d, fn) if d else fn
            if stem not in files:
                add(stem, os.path.join(full, fn), rel)

    return files


def _static_name(text, match):
    """A set/list name that is computed, e.g. `INSTALL_${X}_RESOURCES`, is not a
    static variable name - drop it."""
    return text[match.end():match.end() + 1] != "$"


def parse_file(f: CmakeFile):
    with open(f.path, "r", encoding="utf-8", errors="replace") as fh:
        raw = fh.read()
    text = strip_comments(raw)
    f.text = text

    for m in RE_INCLUDE.finditer(text):
        target, optional = m.group(1), bool(m.group(2))
        f.includes.append((len(f.includes) + 1, target, optional))

    for m in RE_SET.finditer(text):
        if _static_name(text, m):
            f.outputs.add(m.group(1))
    for m in RE_UNSET.finditer(text):
        if _static_name(text, m):
            f.outputs.add(m.group(1))
    for m in RE_OPTION.finditer(text):
        f.outputs.add(m.group(1))
    for m in RE_LIST_MOD.finditer(text):
        if _static_name(text, m):
            f.outputs.add(m.group(1))
    for m in RE_LIST_READ.finditer(text):
        f.inputs.add(m.group(1))
        f.outputs.add(m.group(2))
    for m in RE_FOREACH.finditer(text):
        f.outputs.add(m.group(1))
    for m in RE_OUTPUT_VAR.finditer(text):
        f.outputs.add(m.group(1))

    # project( <name> ...) makes CMake define PROJECT_NAME and friends
    for m in RE_PROJECT.finditer(text):
        f.outputs.update(("PROJECT_NAME", "PROJECT_VERSION", "PROJECT_VERSION_MAJOR"))

    for fn in FUNC_OUT_LAST:
        for m in re.finditer(r"\b%s\s*\(([^()]*)\)" % re.escape(fn), text):
            args = m.group(1).split()
            if args and re.fullmatch(r"[A-Za-z_][A-Za-z0-9_]*", args[-1]):
                f.outputs.add(args[-1])
    for m in RE_FUNC.finditer(text):
        f.functions.add(m.group(2))

    for m in RE_VARREF.finditer(text):
        f.inputs.add(m.group(1))
    for m in RE_ENVREF.finditer(text):
        f.envs.add(m.group(1))
        f.inputs.add("ENV:" + m.group(1))

    # Conditions: bare identifiers without ${} are variable reads too.
    for m in RE_COND.finditer(text):
        condition = re.sub(r'"[^"]*"', " ", m.group(1))   # quoted literals aren't vars
        condition = re.sub(r"\$\{[^}]*\}", " ", condition)  # nor are ${refs}
        for w in RE_WORD.finditer(condition):
            word = w.group(1)
            if word in KEYWORDS or LOWER_LOCAL.match(word):
                continue
            f.inputs.add(word)

    f.inputs -= f.functions           # a def is not a read of itself
    return f


def subject_for(cmd, args):
    """The thing a command acts on, so repeated calls collapse into one node.

    `install(FILES a)` and `install(FILES b)` are both `install(FILES)`;
    `add_library(_1_x OBJECT)` and `add_library(x ...)` are two nodes.
    """
    toks = split_args(args)
    if not toks:
        return ""
    t = [unquote(x) for x in toks]
    up = [x.upper() for x in t]

    def scope(i=1):
        if len(up) > i and up[i] in ("PRIVATE", "PUBLIC", "INTERFACE"):
            return "%s %s" % (t[0], up[i])
        return t[0]

    if cmd in ("add_library", "add_executable", "add_custom_target"):
        for i in (1, 2):
            if len(up) > i and up[i] in ("STATIC", "SHARED", "MODULE", "OBJECT",
                                         "INTERFACE", "ALIAS", "IMPORTED"):
                return "%s %s" % (t[0], up[i])
        return t[0]
    if cmd in ("target_link_libraries", "target_link_options",
               "target_compile_definitions", "target_compile_options",
               "target_include_directories", "set_target_properties",
               "add_dependencies", "set_source_files_properties"):
        return scope()
    if cmd == "set_property":
        obj = t[1] if len(t) > 1 and up[1] not in ("APPEND", "PROPERTY") else ""
        return (up[0] + " " + obj).strip()
    if cmd == "install":
        return up[0] if up[0] in ("TARGETS", "EXPORT", "FILES", "DIRECTORY",
                                  "PROGRAMS", "CODE", "SCRIPT") else t[0]
    if cmd == "add_custom_command":
        for i, u in enumerate(up[:-1]):
            if u in ("OUTPUT", "TARGET"):
                return t[i + 1]
        return ""
    if cmd in ("configure_package_config_file",):
        return t[1] if len(t) > 1 else t[0]
    if cmd in ("configure_file", "write_basic_package_version_file"):
        return t[-1]
    if cmd == "include_directories":
        return "SYSTEM" if up[0] == "SYSTEM" else "global"
    if cmd in ("add_definitions", "add_compile_definitions", "add_compile_options"):
        return "global"
    if cmd in ("find_library", "find_program", "cmake_policy", "export"):
        return t[0]
    if cmd == "execute_process":
        return "probe"
    return t[0]


# --------------------------------------------------------------------------- #
# global state: the writes that escape the file that makes them
# --------------------------------------------------------------------------- #

# bucket -> (fill, stroke, title, what it means). The bucket is the blast
# radius of a write, which is the question this view exists to answer: does it
# stop at the target being built, or does it leak to everything configured
# afterwards / to whoever links the library / to disk?
STATE_STYLE = OrderedDict([
    ("global",  ("#a9cdf5", "#1565c0", "PROCESS / DIRECTORY",
                 "visible to every target configured after this point, in this "
                 "directory and everything below it")),
    ("cache",   ("#f9d98c", "#b8860b", "CACHE",
                 "persisted between reconfigures, shows up in ccmake, and is "
                 "inherited by a project that embeds this one")),
    ("export",  ("#a8e6a3", "#2e7d32", "EXPORTED",
                 "what a target that links this library compiles and links "
                 "against - the public contract, also what gets installed")),
    ("install", ("#f8c8a0", "#e65100", "ON DISK",
                 "the files and the cmake package written into the install tree")),
    ("private", ("#e8e8e8", "#757575", "PRIVATE",
                 "this target in this build only - never leaves the file")),
])

# Everything scan_commands has to look at for this view: the effectful ones plus
# the variable-ish commands, because `option()` and `set(... CACHE)` are global
# state without building anything.
STATE_COMMANDS = set(EFFECT_COMMANDS) | {
    "set", "unset", "option", "list",
    "link_directories", "add_link_options", "target_link_directories",
}

# CMAKE_* variables that are pure process-wide state, most specific first. This
# is the "does it move my search paths and flags" list.
CMAKE_STATE_RULES = [
    (r"^CMAKE_MODULE_PATH$", "module search path"),
    (r"^CMAKE_(INCLUDE|FRAMEWORK)_PATH$", "include search path"),
    (r"^CMAKE_(LIBRARY|PROGRAM|PREFIX|SYSTEM_PREFIX)_PATH$", "link / library search path"),
    (r"^CMAKE_(INSTALL_RPATH|SKIP_RPATH|BUILD_WITH_INSTALL_RPATH|"
     r"INSTALL_RPATH_USE_LINK_PATH)", "rpath of built binaries"),
    (r"^CMAKE_(EXE|SHARED|STATIC|MODULE)_LINKER_FLAGS", "linker flags"),
    (r"^CMAKE_[A-Z0-9_]*_FLAGS|^CMAKE_INCLUDE_SYSTEM_FLAG_", "compile flags"),
    (r"^CMAKE_(OSX|APPLE|SYSTEM|SYSROOT|FIND_ROOT_PATH|CROSSCOMPILING)", "sdk / platform"),
    (r"^CMAKE_INSTALL_(PREFIX|BINDIR|LIBDIR|INCLUDEDIR|DATADIR|MANDIR)", "install layout"),
    (r"^CMAKE_[A-Z0-9_]*COMPILER|^CMAKE_[A-Z0-9_]*LAUNCHER|^CMAKE_LINKER",
     "compiler / tools"),
    (r"^CMAKE_(BUILD_TYPE|POSITION_INDEPENDENT_CODE|DEBUG_POSTFIX|"
     r"WINDOWS_EXPORT_ALL_SYMBOLS|FIND_LIBRARY_SUFFIXES|[A-Z]+_LIBRARY_SUFFIX|"
     r"EXPORT_COMPILE_COMMANDS|VERBOSE_MAKEFILE|C_STANDARD|CXX_STANDARD)",
     "build setting"),
    (r"^CMAKE_", "other CMAKE_ variable"),
]

# install(...) kind -> what it puts on disk
INSTALL_KINDS = {
    "TARGETS": "installed targets",
    "EXPORT": "installed export set",
    "FILES": "installed files",
    "DIRECTORY": "installed directory",
    "PROGRAMS": "installed programs",
    "CODE": "install script",
    "SCRIPT": "install script",
}

# command -> the exported interface slot it fills
TARGET_WRITES = {
    "target_include_directories": "INTERFACE include dirs",
    "target_compile_definitions": "INTERFACE compile definitions",
    "target_compile_options": "INTERFACE compile options",
    "target_link_libraries": "INTERFACE link libraries",
    "target_link_options": "INTERFACE link options",
    "target_link_directories": "INTERFACE link search path",
}


def _scope_of(tokens, stop=4):
    """Index and name of the PRIVATE/PUBLIC/INTERFACE keyword, if any."""
    for i in range(1, min(len(tokens), stop)):
        up = tokens[i].upper()
        if up in ("PRIVATE", "PUBLIC", "INTERFACE"):
            return up, i
    return "", -1


def _cmake_channel(name):
    for pattern, channel in CMAKE_STATE_RULES:
        if re.search(pattern, name):
            return channel
    return "other CMAKE_ variable"


def _up_to_cache(tokens):
    out = []
    for t in tokens:
        if t.upper() in ("CACHE", "PARENT_SCOPE"):
            break
        out.append(t)
    return " ".join(out)


def state_write(cmd, args):
    """Classify one command as a write to state outside its own file.

    Returns `(bucket, channel, value)` for the writes that leave the file, and
    None for commands that only read (find_*, execute_process) or only touch the
    target being built in this very build (so `private` is the floor).
    """
    tokens = [unquote(t) for t in split_args(args)]
    if not tokens:
        return None
    up = [t.upper() for t in tokens]

    if cmd in ("find_library", "find_program", "execute_process", "add_subdirectory",
               "add_custom_command", "add_custom_target", "add_dependencies"):
        return None

    if cmd in ("include_directories", "link_directories", "add_definitions",
               "add_compile_definitions", "add_compile_options", "add_link_options"):
        channel = {
            "include_directories": "include search path (include_directories)",
            "link_directories": "link search path (link_directories)",
            "add_definitions": "compile definitions (global)",
            "add_compile_definitions": "compile definitions (global)",
            "add_compile_options": "compile options (global)",
            "add_link_options": "link options (global)",
        }[cmd]
        rest = tokens[1:] if up[0] == "SYSTEM" else tokens
        return ("global", channel, " ".join(rest))

    if cmd in TARGET_WRITES:
        scope, idx = _scope_of(tokens)
        rest = " ".join(t for i, t in enumerate(tokens)
                        if i and i != idx and up[i] != "SYSTEM")
        if not rest:                       # target_x(T INTERFACE) declares the slot
            rest = "(scope only)"
        if scope in ("PUBLIC", "INTERFACE"):
            return ("export", TARGET_WRITES[cmd], rest)
        return ("private", "private target flags (%s)" % cmd, rest)

    if cmd == "set_target_properties":
        props = tokens[up.index("PROPERTIES") + 1:] if "PROPERTIES" in up else []
        if any(p.upper().startswith(("INTERFACE_", "IMPORTED_")) for p in props):
            return ("export", "INTERFACE target properties", " ".join(props))
        return ("private", "private target properties", " ".join(props))

    if cmd == "set_property":
        kind = up[0]
        prop = ""
        if "PROPERTY" in up:
            j = up.index("PROPERTY")
            prop = tokens[j + 1] if len(tokens) > j + 1 else ""
        if kind in ("GLOBAL", "DIRECTORY"):
            return ("global", "%s property (set_property)" % kind.lower(),
                    " ".join(tokens[1:]))
        if kind == "INSTALL":
            return ("install", "INSTALL property", " ".join(tokens[1:]))
        if kind == "CACHE":
            return ("cache", "cached value (set_property CACHE)", " ".join(tokens[1:]))
        if kind == "TARGET":
            if prop.upper().startswith(("INTERFACE_", "IMPORTED_")):
                return ("export", "INTERFACE target properties", prop)
            return ("private", "private target properties", prop)
        return None

    if cmd == "install":
        kind = up[0]
        if kind not in INSTALL_KINDS:
            return None
        channel = INSTALL_KINDS[kind]
        if kind == "TARGETS" and "EXPORT" in up[1:]:
            channel += " (+ EXPORT)"
        return ("install", channel, " ".join(tokens[1:]))

    if cmd == "export":
        return ("export", "export set written", " ".join(tokens))

    if cmd in ("configure_file", "configure_package_config_file",
               "write_basic_package_version_file"):
        return ("install", "generated cmake package file", " ".join(tokens))

    if cmd in ("add_library", "add_executable"):
        return ("global", "target name registered", " ".join(tokens))

    if cmd == "cmake_policy":
        return ("global", "cmake policy", " ".join(tokens))

    if cmd == "option":
        default = tokens[-1] if len(tokens) > 1 else ""
        return ("cache", "option() knob", "%s = %s" % (tokens[0], default))

    if cmd in ("set", "unset", "list"):
        return _set_state(cmd, tokens, up)

    return None


def _set_state(cmd, tokens, up):
    """A set()/unset()/list() write: is it global state, or a local variable?"""
    if cmd == "list":
        # list(APPEND CMAKE_MODULE_PATH ...) / list(INSERT CMAKE_MODULE_PATH 0 ...)
        if len(tokens) < 2:
            return None
        name = tokens[1]
        skip = 3 if up[0] in ("INSERT", "GET", "REMOVE_AT") else 2
        rest = tokens[skip:]
    elif cmd == "unset":
        name, rest = tokens[0], []
    else:
        name, rest = tokens[0], tokens[1:]

    named = not name.startswith("${")          # generic helpers fill ${...}
    if "CACHE" in up:
        if name.startswith("CMAKE_"):
            return ("global", _cmake_channel(name), _up_to_cache(rest))
        if "INTERNAL" in up:
            return ("cache", "internal memoization (CACHE INTERNAL)",
                    name if named else "${caller-named}")
        return ("cache", "cached value (set ... CACHE ...)",
                "%s = %s" % (name if named else "${caller-named}", _up_to_cache(rest)))

    if name.startswith("CMAKE_"):
        return ("global", _cmake_channel(name), " ".join(rest))

    if "PARENT_SCOPE" in up:
        return ("global", "variable set in the caller's scope (PARENT_SCOPE)",
                "%s = %s" % (name if named else "${caller-named}",
                             short(" ".join(rest), 40)))
    return None


def scan_state(f: CmakeFile):
    """[(Effect, (bucket, channel, value))] for one file, in source order."""
    out = []
    for eff in scan_commands(f, STATE_COMMANDS):
        write = state_write(eff.cmd, eff.args)
        if write:
            out.append((eff, write))
    return out


def short(text, width=34):
    text = " ".join(text.split())
    return text if len(text) <= width else text[:width - 1] + "\u2026"


def group_of(name: str) -> str:
    if name.startswith("ENV:"):
        return "env"
    for group, pattern in GROUP_RULES:
        if re.search(pattern, name):
            return group
    if LOWER_LOCAL.match(name):
        return "internal"
    return "other"


def is_hook_name(name: str) -> bool:
    return bool(re.search(r"(Aux|AuxC)$", name))


class Effect:
    """One effectful cmake command, with the control flow that guards it."""

    def __init__(self, cmd, subject, line, guards, in_function, args=""):
        self.cmd = cmd
        self.subject = subject
        self.line = line
        self.guards = tuple(guards)
        self.in_function = in_function
        self.args = args

    @property
    def node(self):
        return "%s(%s)" % (self.cmd, self.subject)

    def why(self):
        """Human readable `what has to be true for this to run`."""
        return " and ".join(self.guards)


def scan_commands(f: CmakeFile, allow=EFFECT_COMMANDS):
    """Walk a file and collect the effectful commands, with their guard stack.

    `if( LINK_PHASE)` matters a lot here: without it the commands view would
    claim add_library(${LIBRARY_NAME}) always runs. Commands inside a
    function()/macro() body are collected too, but flagged, because they only
    run when something calls the helper.
    """
    text = f.text
    stack, out, skip_until = [], [], 0

    for m in RE_CMDTOK.finditer(text):
        if m.start() < skip_until:
            continue
        name = m.group(1)
        if name in BLOCK_OPEN or name in BLOCK_CLOSE or name == "else":
            close = match_paren(text, m.end() - 1)
            inner = text[m.end():close]
            if name in ("if", "elseif"):
                # Keep the whole condition: each view shortens or wraps it for
                # display, but the tooltip should not inherit a truncated guard.
                cond = " ".join(inner.split())
                if name == "if":
                    stack.append(["if", cond, False])
                else:
                    for i in range(len(stack) - 1, -1, -1):
                        if stack[i][0] == "if":
                            stack[i] = ["if", cond, False]
                            break
            elif name == "else":
                for i in range(len(stack) - 1, -1, -1):
                    if stack[i][0] == "if":
                        stack[i][2] = True
                        break
            elif name == "foreach":
                stack.append(["foreach", " ".join(inner.split()), False])
            elif name == "while":
                stack.append(["while", " ".join(inner.split()), False])
            elif name in ("function", "macro"):
                stack.append(["function", unquote(split_args(inner)[0]) if inner.strip() else "?", False])
            elif name == "block":
                stack.append(["block", "", False])
            else:
                if stack:
                    stack.pop()
            skip_until = close + 1
            continue

        if name not in allow:
            continue

        close = match_paren(text, m.end() - 1)
        args = text[m.end():close]
        guards = []
        for kind, what, is_else in stack:
            if kind == "if":
                guards.append("if " + what + (" [else]" if is_else else ""))
            elif kind == "foreach":
                guards.append("∀ " + what)
            elif kind == "while":
                guards.append("while " + what)
            elif kind == "function":
                guards.append("in " + what + "()")
        in_fn = any(k == "function" for k, _, _ in stack)
        out.append(Effect(name, subject_for(name, args),
                          text.count("\n", 0, m.start()) + 1, guards, in_fn,
                          args))
        skip_until = close + 1

    return out


def flatten_order(files, root="CMakeLists"):
    """Depth first include order - the order cmake reaches each file in.

    With a partial tree (--focus) there is no single root, so every file that
    nothing inside the subset includes becomes a starting point.
    """
    if root in files:
        roots = [root]
    else:
        has_parent = set()
        for f in files.values():
            for _n, target, _opt in f.includes:
                if target in files:
                    has_parent.add(target)
        roots = [n for n in files if n not in has_parent] or list(files)

    order, seen = [], set()

    def visit(name):
        if name in seen or name not in files:
            return
        seen.add(name)
        order.append(name)
        for _n, target, _opt in files[name].includes:
            visit(target)

    for r in roots:
        visit(r)
    for name in files:                      # anything unreachable, keep visible
        if name not in seen:
            seen.add(name)
            order.append(name)
    return order


# --------------------------------------------------------------------------- #
# model
# --------------------------------------------------------------------------- #

class Model:
    def __init__(self, files, hidden):
        self.files = files
        self.hidden = hidden
        self.hooks = set()
        self.externals = {}            # name -> group (consumed, never produced)
        self.external_names = set()    # names that come in from outside
        self.flow = {}                 # (producer, consumer, group) -> {vars}
        self.group_of = {}
        self.produced_by = defaultdict(set)
        self.consumed_by = defaultdict(set)

    def visible(self, name):
        return name not in NOISE_NAMES and group_of(name) not in self.hidden

    def group(self, name):
        return self.group_of.get(name) or group_of(name)


def build_model(files, hidden, keep_hooks=True):
    m = Model(files, hidden)

    # ---- resolve include targets to real files / hooks / externals ----------
    resolved = {}
    for f in files.values():
        for order, target, optional in f.includes:
            if target in files:
                resolved[target] = files[target]
            elif target == "CMakeLists":
                resolved[target] = files["CMakeLists"]
            else:
                resolved[target] = None
                if is_hook_name(target):
                    if keep_hooks:
                        m.hooks.add(target)
                else:
                    m.externals.setdefault(target, "cmake")

    # ---- variable flow -----------------------------------------------------
    for f in files.values():
        for name in f.inputs:
            if m.visible(name):
                m.consumed_by[name].add(f.name)
        for name in f.outputs:
            if m.visible(name):
                m.produced_by[name].add(f.name)
        for fn in f.functions:
            m.produced_by[fn].add(f.name)
            m.group_of[fn] = "function"

    # function calls are reads of the defining file's output
    for f in files.values():
        for fn in m.produced_by:
            if fn in f.functions:
                continue
            if re.search(r"\b%s\s*\(" % re.escape(fn), f.text):
                f.inputs.add(fn)
                m.consumed_by[fn].add(f.name)
                m.group_of[fn] = "function"

    # ---- edges -------------------------------------------------------------
    for name, producers in m.produced_by.items():
        consumers = m.consumed_by.get(name)
        if not consumers:
            continue
        group = m.group(name)
        for p in producers:
            for c in consumers:
                if p == c:
                    continue
                m.flow.setdefault((p, c, group), set()).add(name)

    # consumed but never produced anywhere -> comes in from outside
    for name in m.consumed_by:
        if name not in m.produced_by and m.visible(name):
            m.externals.setdefault(name, m.group(name))
            m.external_names.add(name)

    return m


def reachable(files, root="CMakeLists", keep_hooks=True):
    """Transitive closure of include() starting at root."""
    seen, stack = set(), [root]
    while stack:
        cur = stack.pop()
        if cur in seen or cur not in files:
            continue
        seen.add(cur)
        for _, target, _ in files[cur].includes:
            if target in files:
                stack.append(target)
    return seen


def neighborhood(files, focus, depth):
    if focus not in files:
        raise SystemExit("unknown cmake file: %s (try --all -h)" % focus)
    forward, backward = {focus}, {focus}
    layer = {focus}
    for _ in range(depth):
        nxt = set()
        for n in layer:
            for _, t, _ in files[n].includes:
                if t in files and t not in forward:
                    nxt.add(t)
        forward |= nxt
        layer = nxt
    layer = {focus}
    for _ in range(depth):
        nxt = set()
        for f in files.values():
            if f.name in forward:
                continue
            for _, t, _ in f.includes:
                if t in layer and f.name not in backward:
                    nxt.add(f.name)
        backward |= nxt
        layer = nxt
    return forward | backward


# --------------------------------------------------------------------------- #
# dot helpers
# --------------------------------------------------------------------------- #

def esc(s):
    return _html.escape(str(s), quote=False)


def ttip(s):
    return str(s).replace("\\", "/").replace('"', "'").replace("\n", "&#10;")


def wrap_names(names, width=48, cap=None):
    names = list(names)
    extra = 0
    if cap and len(names) > cap:
        extra = len(names) - cap
        names = names[:cap]
    lines, cur = [], ""
    for n in names:
        piece = n if not cur else ", " + n
        if len(cur) + len(piece) > width and cur:
            lines.append(cur)
            cur = n
        else:
            cur += piece
    if cur:
        lines.append(cur)
    if extra:
        lines[-1] += ", +%d more" % extra
    return "<BR ALIGN=\"LEFT\"/>".join(lines)


def wrap_tokens(tokens, width=34):
    lines, cur = [], ""
    for t in tokens:
        piece = t if not cur else "  " + t
        if len(cur) + len(piece) > width and cur:
            lines.append(cur)
            cur = t
        else:
            cur += piece
    if cur:
        lines.append(cur)
    return "<BR ALIGN=\"LEFT\"/>".join(lines)


def wrap_label(text, width=28, max_lines=3):
    """`<...>` HTML label for an edge, wrapped instead of truncated."""
    return "<%s>" % wrap_value(text, width, max_lines)


def wrap_value(value, width=46, max_lines=4):
    """One state-view row: wrapped over lines instead of truncated.

    Breaks on whitespace. A single token longer than the width goes on a line of
    its own rather than being cut, so only a value that needs more than
    `max_lines` lines loses anything at all.
    """
    words = str(value).split()
    if not words:
        return esc("(no values)")
    lines, cur = [], ""
    for word in words:
        if not cur:
            cur = word
        elif len(cur) + 1 + len(word) > width:
            lines.append(cur)
            cur = word
        else:
            cur += " " + word
    lines.append(cur)
    if len(lines) > max_lines:
        lines = lines[:max_lines]
        lines[-1] = short(lines[-1] + " \u2026", width)
    return "<BR ALIGN=\"LEFT\"/>".join(esc(l) for l in lines)


def node_id(name):
    return '"%s"' % name


def file_table(f, m, opts):
    def mark(n):
        return n + ("*" if n in m.external_names else "")

    def local(n):
        """Set and read only here - a helper variable, not a hand-off."""
        return (n in f.outputs and n not in m.external_names
                and m.produced_by.get(n, set()) <= {f.name})

    in_groups, out_groups = OrderedDict(), OrderedDict()
    for n in sorted(f.inputs):
        if m.visible(n) and n not in f.functions and not local(n):
            in_groups.setdefault(m.group(n), []).append(mark(n))
    for n in sorted(f.outputs):
        if m.visible(n):
            out_groups.setdefault(m.group(n), []).append(n)
    for n in sorted(f.functions):
        if m.visible(n):
            out_groups.setdefault("function", []).append(n)

    def sort_key(g):
        return (GROUP_ORDER.index(g) if g in GROUP_ORDER else 99, g)

    rows = []
    header = FOLDER_STYLE[origin(f)][2]
    rows.append('<TR><TD PORT="inc" BGCOLOR="%s" ALIGN="LEFT">'
                '<FONT COLOR="white"><B>%s</B></FONT></TD></TR>'
                % (header, esc(f.title)))

    for g in sorted(in_groups, key=sort_key):
        fill = GROUP_STYLE.get(g, GROUP_STYLE["other"])[0]
        rows.append('<TR><TD PORT="in_%s" BGCOLOR="%s" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="8">in&#160;&#183;&#160;%s</FONT><BR ALIGN="LEFT"/>'
                    '%s</TD></TR>' % (esc(g), fill, esc(g),
                                       wrap_names(in_groups[g], cap=opts.max_names)))

    if f.includes:
        tokens = ["%d %s%s" % (o, t, "?" if opt else "") for o, t, opt in f.includes]
        rows.append('<TR><TD BGCOLOR="#f8f8f8" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="7" COLOR="#666666">includes:<BR ALIGN="LEFT"/>'
                    '%s</FONT></TD></TR>' % wrap_tokens(tokens))

    for g in sorted(out_groups, key=sort_key):
        fill = GROUP_STYLE.get(g, GROUP_STYLE["other"])[0]
        rows.append('<TR><TD PORT="out_%s" BGCOLOR="%s" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="8">out&#160;&#183;&#160;%s</FONT><BR ALIGN="LEFT"/>'
                    '%s</TD></TR>' % (esc(g), fill, esc(g),
                                       wrap_names(out_groups[g], cap=opts.max_names)))

    label = ('<<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3" '
             'BGCOLOR="white">%s</TABLE>>' % "".join(rows))

    tip = "%s\nin:  %s\nout: %s\ninc: %s" % (
        f.relpath,
        ", ".join(sorted(n for n in f.inputs if m.visible(n))) or "-",
        ", ".join(sorted(n for n in f.outputs if m.visible(n))) or "-",
        ", ".join("%d %s" % (o, t) for o, t, _ in f.includes) or "-")
    return label, tip


def legend_node(groups):
    rows = ['<TR><TD BGCOLOR="#2b2b2b" ALIGN="LEFT"><FONT COLOR="white">'
            '<B>legend</B></FONT></TD></TR>']
    for g in groups:
        fill, stroke = GROUP_STYLE.get(g, GROUP_STYLE["other"])
        rows.append('<TR><TD BGCOLOR="%s" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="9">%s</FONT></TD></TR>' % (fill, esc(g)))
    rows.append('<TR><TD BGCOLOR="white" ALIGN="LEFT">'
                '<FONT POINT-SIZE="8" COLOR="#666666">dashed grey = include()'
                '</FONT></TD></TR>')
    rows.append('<TR><TD BGCOLOR="white" ALIGN="LEFT">'
                '<FONT POINT-SIZE="8" COLOR="#666666">solid = value handed on'
                '</FONT></TD></TR>')
    rows.append('<TR><TD BGCOLOR="white" ALIGN="LEFT">'
                '<FONT POINT-SIZE="8" COLOR="#666666">name* = comes from outside'
                '</FONT></TD></TR>')
    rows.append('<TR><TD BGCOLOR="white" ALIGN="LEFT">'
                '<FONT POINT-SIZE="8" COLOR="#666666">header = file folder:'
                '</FONT></TD></TR>')
    for folder, (_f, _s, header, _label) in FOLDER_STYLE.items():
        rows.append('<TR><TD BGCOLOR="%s" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="8" COLOR="white">%s</FONT></TD></TR>'
                    % (header, esc(FOLDER_SHORT[folder])))
    return ('<<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3" '
            'BGCOLOR="white">%s</TABLE>>' % "".join(rows))


def external_table(groups, external_names):
    """A legend for the values that are read but never produced in this tree.

    Deliberately has no edges: an edge per consumer turns the picture into a
    hairball, and a single `constraint=false` edge into a node that also has an
    include() edge can make dot abort with "trouble in init_rank". The values are
    marked with a * where they are consumed instead.
    """
    rows = ['<TR><TD BGCOLOR="#2b2b2b" ALIGN="LEFT">'
            '<FONT COLOR="white"><B>outside world</B></FONT>'
            '<BR ALIGN="LEFT"/><FONT POINT-SIZE="7">read here, never set here:'
            ' cmake command line, environment, builtins, editor/project hooks'
            '</FONT></TD></TR>']
    names = sorted(external_names)
    for g in groups:
        fill, _ = GROUP_STYLE.get(g, GROUP_STYLE["other"])
        mine = [n for n in names if group_of(n) == g]
        if not mine:
            continue
        rows.append('<TR><TD BGCOLOR="%s" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="8">%s</FONT><BR ALIGN="LEFT"/>'
                    '<FONT POINT-SIZE="8">%s</FONT></TD></TR>'
                    % (fill, esc(g), wrap_names(mine, cap=8)))
    return ('<<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3" '
            'BGCOLOR="white">%s</TABLE>>' % "".join(rows))


# --------------------------------------------------------------------------- #
# renderers
# --------------------------------------------------------------------------- #

def render_flow(m, opts):
    files = m.files
    by_include = opts.layout == "include"
    vconstraint = "constraint=false" if by_include else "weight=3"
    iconstraint = "weight=10" if by_include else "constraint=false, weight=1"
    groups = [g for g in GROUP_ORDER if g in
              {m.group(n) for n in list(m.produced_by) + list(m.consumed_by)}]

    out = []
    a = out.append
    a("// generated by tools/cmake-graph.py - do not edit")
    a("digraph cmake_flow {")
    a('   graph [rankdir=TB, bgcolor="white", fontname="Helvetica", fontsize=11,')
    a('          label=<<B>mulle-container &#8212; CMake include order &amp; value flow</B>'
      '<BR/><FONT POINT-SIZE="9">each node is one .cmake file &#183; values enter at the top, '
      'leave at the bottom &#183; row color = value group &#183; header color = file folder '
      '&#183; dashed grey = include()</FONT>>,')
    # splines=line: the curved/polyline routers bail out with "routesplines,
    # illegal values" on the --all graph, straight segments never do.
    a('          labelloc=t, nodesep=0.30, ranksep=1.10, splines=line,'
      ' outputorder=edgesfirst];')
    a('   node [shape=plaintext, fontname="Helvetica", fontsize=10];')
    a('   edge [fontname="Helvetica", fontsize=8];')
    a("")
    a("   // ---- legend ----")
    a('   legend [label=%s];' % legend_node(groups))
    a("")

    # ---- external inputs -------------------------------------------------
    ext_groups = sorted({g for g in m.externals.values() if g not in m.hidden},
                        key=lambda g: (GROUP_ORDER.index(g) if g in GROUP_ORDER else 99, g))
    if ext_groups and not opts.no_external:
        a("   // ---- values that arrive from outside (marked * in the nodes) ----")
        a('   "__external__" [label=%s, tooltip="cmake command line, environment,'
          ' builtin cmake variables"];' % external_table(ext_groups, m.external_names))
        a("")

    # ---- value edges -----------------------------------------------------
    a("   // ---- values handed from file to file ----")
    for (p, c, g), names in sorted(m.flow.items()):
        stroke = GROUP_STYLE.get(g, GROUP_STYLE["other"])[1]
        lbl = ", ".join(sorted(names)[:2])
        if len(names) > 2:
            lbl += " +%d" % (len(names) - 2)
        a('   %s:out_%s -> %s:in_%s [color="%s", penwidth=1.4, label="%s", '
          '%s, tooltip="%s"];'
          % (node_id(p), esc(g), node_id(c), esc(g), stroke, ttip(lbl),
             vconstraint, ttip(", ".join(sorted(names)))))
    a("")

    # ---- include edges ---------------------------------------------------
    a("   // ---- include order ----")
    for f in files.values():
        for order, target, optional in f.includes:
            if target in files:
                a('   %s -> %s:inc [color="#999999", style=dashed, label="%d", '
                  '%s, tooltip="include( %s )"];'
                  % (node_id(f.name), node_id(target), order, iconstraint, target))
    a("")

    # ---- hook stubs ------------------------------------------------------
    for h in sorted(m.hooks):
        a('   %s [label="%s", shape=box, style="dashed,rounded", color="#bbbbbb", '
          'fontcolor="#888888", fontsize=9, tooltip="extension hook - drop a '
          '%s.cmake next to CMakeLists.txt to take part"];' % (node_id(h), esc(h), esc(h)))
    for h in sorted(m.hooks):
        for f in files.values():
            for order, target, optional in f.includes:
                if target == h:
                    a('   %s -> %s [color="#cccccc", style=dashed, label="%d"];'
                      % (node_id(f.name), node_id(h), order))
    a("")

    # ---- file nodes ------------------------------------------------------
    a("   // ---- cmake files ----")
    for f in files.values():
        label, tip = file_table(f, m, opts)
        a('   %s [label=%s, tooltip="%s"];' % (node_id(f.name), label, ttip(tip)))
    a("}")
    return "\n".join(out) + "\n"


def folder_legend():
    rows = ['<TR><TD COLSPAN="2" BGCOLOR="#2b2b2b" ALIGN="LEFT">'
            '<FONT COLOR="white"><B>where the file lives</B></FONT></TD></TR>']
    for fill, stroke, _h, label in FOLDER_STYLE.values():
        rows.append('<TR><TD BGCOLOR="%s" WIDTH="16" BORDER="1" COLOR="%s">'
                    '&#160;</TD><TD ALIGN="LEFT">'
                    '<FONT POINT-SIZE="9">%s</FONT></TD></TR>'
                    % (fill, stroke, esc(label)))
    rows.append('<TR><TD BGCOLOR="white" BORDER="1" STYLE="dashed">&#160;</TD>'
                '<TD ALIGN="LEFT"><FONT POINT-SIZE="9" COLOR="#888888">'
                '*Aux  extension hook, not in this repo</FONT></TD></TR>')
    return ('<<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3" '
            'BGCOLOR="white">%s</TABLE>>' % "".join(rows))


def command_legend(families):
    rows = ['<TR><TD COLSPAN="2" BGCOLOR="#2b2b2b" ALIGN="LEFT">'
            '<FONT COLOR="white"><B>command families</B></FONT></TD></TR>']
    for fam in families:
        fill, stroke = FAMILY_STYLE.get(fam, ("#f2f2f2", "#999999"))
        rows.append('<TR><TD BGCOLOR="%s" WIDTH="16" BORDER="1" COLOR="%s">'
                    '&#160;</TD><TD ALIGN="LEFT"><FONT POINT-SIZE="9">%s</FONT>'
                    '</TD></TR>' % (fill, stroke, esc(fam)))
    for line in ("left column: each file, in the order cmake reaches it",
                 "right column: the commands that file issues",
                 "edge label: the innermost if() / foreach() guard",
                 "+N: N further enclosing guards (hover for the chain)",
                 "&#215;N: the call happens N times",
                 "dashed: built by a function()/macro() body"):
        rows.append('<TR><TD COLSPAN="2" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="8" COLOR="#666666">%s</FONT></TD></TR>' % line)
    return ('<<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3" '
            'BGCOLOR="white">%s</TABLE>>' % "".join(rows))


def render_commands(m, opts):
    """Flat, pre-processed view: which commands each file actually creates."""
    files, order = m.files, flatten_order(m.files)
    rank = {name: i + 1 for i, name in enumerate(order)}

    per_file = OrderedDict((name, scan_commands(files[name])) for name in order)
    if not any(per_file.values()):
        raise SystemExit("no effectful cmake commands found in %d files" % len(order))

    # (file, node) -> aggregated call info; node order follows first appearance
    edges, node_order, node_fam, node_dashed = OrderedDict(), [], {}, set()
    longest = {}                      # node -> (cmd, subject) with the longest subject
    for name in order:
        for eff in per_file[name]:
            node = (eff.cmd, eff.subject)
            node_fam.setdefault(node, EFFECT_COMMANDS[eff.cmd])
            if len(eff.subject) > len(longest.get(node, ("", ""))[1]):
                longest[node] = (eff.cmd, eff.subject)
            if eff.in_function:
                node_dashed.add(node)
            if node not in node_order:
                node_order.append(node)
            key = (name, node)
            info = edges.setdefault(key, {"n": 0, "why": [], "line": eff.line})
            info["n"] += 1
            if eff.why() not in info["why"]:
                info["why"].append(eff.why())

    out = []
    a = out.append
    a("// generated by tools/cmake-graph.py - do not edit")
    a("digraph cmake_commands {")
    a('   graph [rankdir=LR, bgcolor="white", fontname="Helvetica", fontsize=11,')
    a('          label=<<B>mulle-container &#8212; cmake commands created by each file</B>'
      '<BR/><FONT POINT-SIZE="9">the include tree flattened into the order cmake '
      'processes it &#183; every command edge is labeled with what has to be true for it '
      'to run</FONT>>,')
    a('          labelloc=t, nodesep=0.07, ranksep=0.9, splines=line];')
    a('   node [shape=box, style="rounded,filled", fontname="Helvetica", fontsize=9, '
      'color="#888888", fillcolor="#f2f2f2"];')
    a('   edge [fontname="Helvetica", fontsize=8, color="#555555"];')
    a("")

    fams = [f for f in FAMILY_STYLE if f in set(node_fam.values())]
    a('   __legend__ [label=%s, shape=plaintext];' % command_legend(fams))
    a("")

    a("   // ---- files, in the order cmake reaches them ----")
    a("   { rank=same;")
    for name in order:
        f = files[name]
        fill, stroke = FOLDER_STYLE[origin(f)][:2]
        a('     %s [label="%d  %s", fillcolor="%s", color="%s", tooltip="%s"];'
          % (node_id("f_" + name), rank[name], esc(f.relpath), fill, stroke,
             ttip("%s\nreached after %d file(s)" % (f.relpath, rank[name] - 1))))
    a("     " + " -> ".join(node_id("f_" + n) for n in order) + " [style=invis];")
    a("   }")
    a("")

    a("   // ---- commands ----")
    a("   { rank=same;")
    for node in node_order:
        cmd, subject = longest[node]
        fam = node_fam[node]
        fill, stroke = FAMILY_STYLE[fam]
        style = "rounded,filled" + (",dashed" if node in node_dashed else "")
        # `cmd(subject)` on one line while it fits; the long ones (mostly
        # `${LIBRARY_COMPILE_TARGET}`-ish arguments) move the subject to its own
        # wrapped lines rather than being cut off.
        if len(subject) <= 26:
            label = ('<B>%s</B>(<FONT POINT-SIZE="8">%s</FONT>)'
                     % (esc(cmd), esc(subject)))
        else:
            label = ('<B>%s</B><BR ALIGN="LEFT"/>'
                     '<FONT POINT-SIZE="8">%s</FONT>'
                     % (esc(cmd), wrap_value(subject, 30, 3)))
        a('     %s [label=<%s>, '
          'fillcolor="%s", color="%s", style="%s", tooltip="%s"];'
          % (node_id("c_%s_%s" % (cmd, subject)), label,
             fill, stroke, style, ttip("%s(%s)" % (cmd, subject))))
    a("     " + " -> ".join(node_id("c_%s_%s" % n) for n in node_order) + " [style=invis];")
    a("   }")
    a("")

    a("   // ---- file -> command ----")
    for (name, node), info in edges.items():
        _fill, stroke = FAMILY_STYLE[node_fam[node]]
        whys = [w for w in info["why"] if w]
        # A deeply nested call site has a 190 character guard chain. The inner
        # guard is the interesting one, so label with that and keep the whole
        # chain in the tooltip - edge labels cost layout width, tooltips do not.
        bits = []
        if info["n"] > 1:
            bits.append("\u00d7%d" % info["n"])
        if len(whys) == 1:
            parts = whys[0].split(" and ")
            bits.append(parts[-1] if len(parts) == 1
                        else "+%d %s" % (len(parts) - 1, parts[-1]))
        elif len(whys) > 1:
            bits.append("%d paths" % len(whys))
        tip = "%s line %d\n%s" % (files[name].relpath, info["line"],
                                  "\n".join(whys) or "unconditional")
        a('   %s -> %s [color="%s", label=%s, tooltip="%s"];'
          % (node_id("f_" + name), node_id("c_%s_%s" % node), stroke,
             wrap_label(" \u00b7 ".join(bits).strip(), 30, 4), ttip(tip)))
    a("}")
    return "\n".join(out) + "\n"


def state_legend(counts, show_private):
    rows = ['<TR><TD COLSPAN="2" BGCOLOR="#2b2b2b" ALIGN="LEFT">'
            '<FONT COLOR="white"><B>blast radius of a write</B></FONT></TD></TR>']
    for bucket, (fill, stroke, title, why) in STATE_STYLE.items():
        if bucket == "private" and not show_private:
            continue
        rows.append('<TR><TD BGCOLOR="%s" WIDTH="14" BORDER="1" COLOR="%s">'
                    '&#160;</TD><TD ALIGN="LEFT"><FONT POINT-SIZE="9"><B>%s</B>'
                    '<FONT COLOR="#666666">  %d writes</FONT></FONT></TD></TR>'
                    % (fill, stroke, esc(title), counts.get(bucket, 0)))
    for line in ("left column: the file doing the writing, in execution order",
                 "right column: the piece of state it writes to",
                 "a write is shown only if it outlives the file that makes it",
                 "PRIVATE target flags are hidden unless --show-private",
                 "read-only commands (find_*, execute_process) are not state",
                 "dashed edge: issued from inside a function()/macro()"):
        rows.append('<TR><TD COLSPAN="2" ALIGN="LEFT">'
                    '<FONT POINT-SIZE="8" COLOR="#666666">%s</FONT></TD></TR>' % line)
    return ('<<TABLE BORDER="1" CELLBORDER="1" CELLSPACING="0" CELLPADDING="3" '
            'BGCOLOR="white">%s</TABLE>>' % "".join(rows))


def render_state(m, opts):
    """Global state view: only the writes that escape the file making them.

    The commands view answers "what does this build do". This one answers "what
    does it change outside itself" - which is what bites you when the library is
    embedded in another project (parent scope search paths, cache keys) or linked
    by a consumer (the INTERFACE_* contract) or installed.
    """
    files, order = m.files, flatten_order(m.files)
    rank = {name: i + 1 for i, name in enumerate(order)}

    channels = OrderedDict()        # (bucket, channel) -> {"writes": [...]}
    file_use = OrderedDict()        # file -> [(bucket, channel)]
    edge_info = OrderedDict()       # (file, channel key) -> aggregation
    for name in order:
        for eff, (bucket, channel, value) in scan_state(files[name]):
            if bucket == "private" and not opts.show_private:
                continue
            key = (bucket, channel)
            ch = channels.setdefault(key, {"seen": OrderedDict(), "writes": []})
            wkey = (name, value, eff.line)
            if wkey in ch["seen"]:
                ch["seen"][wkey]["n"] += 1
            else:
                row = {"file": name, "value": value, "line": eff.line, "n": 1,
                       "cmd": eff.cmd}
                ch["seen"][wkey] = row
                ch["writes"].append(row)

            used = file_use.setdefault(name, [])
            if key not in used:
                used.append(key)
            ef = edge_info.setdefault((name, key),
                                      {"n": 0, "why": [], "line": eff.line,
                                       "cmds": [], "fn": True})
            ef["n"] += 1
            ef["fn"] = ef["fn"] and eff.in_function
            if eff.why() not in ef["why"]:
                ef["why"].append(eff.why())
            if eff.cmd not in ef["cmds"]:
                ef["cmds"].append(eff.cmd)

    if not channels:
        raise SystemExit("no state-changing cmake commands found")

    counts = defaultdict(int)
    for (bucket, _c), ch in channels.items():
        counts[bucket] += len(ch["writes"])
    buckets = [b for b in STATE_STYLE if b in counts]
    chan_id = {key: "ch_%d" % (i + 1) for i, key in enumerate(channels)}

    out = []
    a = out.append
    a("// generated by tools/cmake-graph.py - do not edit")
    a("digraph cmake_state {")
    a('   graph [rankdir=LR, bgcolor="white", fontname="Helvetica", fontsize=11,')
    a('          label=<<B>mulle-container &#8212; state this cmake config changes '
      'outside itself</B><BR/><FONT POINT-SIZE="9">every write that outlives the file '
      'making it &#183; search paths, flags, cache keys, the link+compile contract, the '
      'install tree</FONT>>,')
    a('          labelloc=t, nodesep=0.08, ranksep=0.7, splines=line];')
    a('   node [shape=box, style="rounded,filled", fontname="Helvetica", fontsize=9, '
      'color="#888888", fillcolor="#f2f2f2"];')
    a('   edge [fontname="Helvetica", fontsize=8, color="#555555"];')
    a("")
    a('   __legend__ [label=%s, shape=plaintext];' % state_legend(counts, opts.show_private))
    a("")

    a("   // ---- files, in the order cmake reaches them ----")
    a("   { rank=same;")
    for name in order:
        if name not in file_use:
            continue
        f = files[name]
        fill, stroke = FOLDER_STYLE[origin(f)][:2]
        a('     %s [label="%d  %s", fillcolor="%s", color="%s", tooltip="%s"];'
          % (node_id("f_" + name), rank[name], esc(f.relpath), fill, stroke,
             ttip("%s\nwrites state in %d place(s)" % (f.relpath, len(file_use[name])))))
    writers = [n for n in order if n in file_use]
    a("     " + " -> ".join(node_id("f_" + n) for n in writers) + " [style=invis];")
    a("   }")
    a("")

    # A cluster per blast radius, so the picture reads as a handful of vertical
    # groups rather than one undifferentiated column of channels.
    for bucket in buckets:
        fill, stroke, title, why = STATE_STYLE[bucket]
        a('   subgraph cluster_state_%s {' % bucket)
        a('      label=<<B>%s</B><BR/><FONT POINT-SIZE="8">%s</FONT>>;'
          % (esc(title), esc(why)))
        a('      labelloc=b; fontsize=10; fontcolor="%s"; color="%s"; '
          'style="rounded,dashed"; penwidth=1.1;' % (stroke, stroke))
        for key, ch in channels.items():
            bucket_of, channel = key
            if bucket_of != bucket:
                continue
            rows = ['<TR><TD BGCOLOR="%s" ALIGN="LEFT">'
                    '<FONT COLOR="white" POINT-SIZE="9"><B>%s</B></FONT>'
                    '<FONT COLOR="white" POINT-SIZE="8">  %d</FONT></TD></TR>'
                    % (stroke, esc(channel), sum(w["n"] for w in ch["writes"]))]
            shown = ch["writes"][:opts.max_names]
            for w in shown:
                cell = wrap_value(w["value"])
                if w["n"] > 1:
                    cell += " &#215;%d" % w["n"]
                cell += ('<FONT COLOR="#666666">  &#183; %s:%d</FONT>'
                         % (esc(files[w["file"]].name), w["line"]))
                rows.append('<TR><TD ALIGN="LEFT"><FONT POINT-SIZE="8">%s'
                            '</FONT></TD></TR>' % cell)
            hidden = len(ch["writes"]) - len(shown)
            if hidden:
                rows.append('<TR><TD ALIGN="LEFT"><FONT POINT-SIZE="8" '
                            'COLOR="#666666">+%d more (hover)</FONT></TD></TR>' % hidden)
            tip = "%s\n%s\n\n" % (channel, why)
            tip += "\n".join("%s  %s:%d%s" % (w["value"] or "", files[w["file"]].relpath,
                                              w["line"],
                                              "  x%d" % w["n"] if w["n"] > 1 else "")
                             for w in ch["writes"])
            a('      %s [label=<<TABLE BORDER="0" CELLBORDER="1" CELLSPACING="0" '
              'CELLPADDING="3" BGCOLOR="white">%s</TABLE>>, penwidth=1.2, tooltip="%s"];'
              % (node_id(chan_id[key]), "".join(rows), ttip(tip)))
        a("   }")
        a("")

    a("   // ---- file -> state ----")
    for (name, key), ef in edge_info.items():
        fill, stroke, _t, _w = STATE_STYLE[key[0]]
        bits = [ef["cmds"][0] if len(ef["cmds"]) == 1
                else "%d calls" % len(ef["cmds"])]
        if ef["n"] > 1:
            bits.append("\u00d7%d" % ef["n"])
        whys = [w for w in ef["why"] if w]
        if len(whys) == 1:
            parts = whys[0].split(" and ")
            bits.append(parts[-1] if len(parts) == 1
                        else "+%d %s" % (len(parts) - 1, parts[-1]))
        elif len(whys) > 1:
            bits.append("%d paths" % len(whys))
        tip = "%s line %d\n%s" % (files[name].relpath, ef["line"],
                                  "\n".join(whys) or "unconditional")
        a('   %s -> %s [color="%s", label=%s, tooltip="%s"%s];'
          % (node_id("f_" + name), node_id(chan_id[key]), stroke,
             wrap_label(" \u00b7 ".join(bits), 28, 4), ttip(tip),
             ", style=dashed" if ef["fn"] else ""))
    a("}")
    return "\n".join(out) + "\n"


def render_order(m, opts):
    files = m.files
    out = []
    a = out.append
    a("// generated by tools/cmake-graph.py - do not edit")
    a("digraph cmake_order {")
    a('   graph [rankdir=TB, bgcolor="white", fontname="Helvetica", fontsize=11,')
    a('          label=<<B>mulle-container &#8212; CMake include order</B>'
      '<BR/><FONT POINT-SIZE="9">edge label = position of the include() call in the '
      'including file &#183; ? = include(... OPTIONAL) which may not exist</FONT>>,')
    a('          labelloc=t, nodesep=0.25, ranksep=0.55];')
    a('   node [shape=box, style="rounded,filled", fillcolor="#f2f2f2", '
      'fontname="Helvetica", fontsize=10, color="#888888"];')
    a('   edge [fontname="Helvetica", fontsize=9, color="#666666"];')
    a("")
    a('   __legend__ [shape=plaintext, label=%s];' % folder_legend())
    a("")

    for h in sorted(m.hooks):
        a('   %s [label="%s*", style="rounded,dashed", fillcolor="white", '
          'fontcolor="#888888", shape=box, tooltip="extension hook - not in this '
          'repository, drop cmake/%s.cmake next to CMakeLists.txt"];'
          % (node_id(h), esc(h), esc(h)))

    for f in files.values():
        fill, stroke, _h, label = FOLDER_STYLE[origin(f)]
        a('   %s [label="%s", fillcolor="%s", color="%s", tooltip="%s (%s)"];'
          % (node_id(f.name), esc(f.title), fill, stroke, ttip(f.relpath), label))

    a("")
    for f in files.values():
        prev = None
        for order, target, optional in f.includes:
            if target in files or target in m.hooks:
                a('   %s -> %s [label="%d%s"];'
                  % (node_id(f.name), node_id(target), order, "?" if optional else ""))
                if prev is not None:
                    a('   %s -> %s [style=invis, weight=5];' % (node_id(prev), node_id(target)))
                prev = target
    a("}")
    return "\n".join(out) + "\n"


# --------------------------------------------------------------------------- #
# main
# --------------------------------------------------------------------------- #

def main(argv=None):
    p = argparse.ArgumentParser(
        description="Render the project's cmake include/value system as graphviz.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__.split("USAGE")[-1])
    p.add_argument("--root", default=REPO_ROOT, help="project root (default: repo root)")
    p.add_argument("--out", default=None, help="output .dot (default: pix/cmake.dot)")
    p.add_argument("--mode", choices=["flow", "order", "commands", "state",
                                      "both", "all"],
                   default="all",
                   help="which graph to write (both = flow+order, all = everything)")
    p.add_argument("--all", action="store_true",
                   help="graph every .cmake file, not just those included by CMakeLists.txt")
    p.add_argument("--focus", metavar="NAME",
                   help="restrict to NAME.cmake and its include neighbours")
    p.add_argument("--depth", type=int, default=1, help="hops for --focus (default 1)")
    p.add_argument("--show-hidden", action="store_true",
                   help="also show TMP_*/loop vars, include guards and MULLE_TRACE_INCLUDE")
    p.add_argument("--no-hooks", action="store_true", help="hide the *Aux extension hooks")
    p.add_argument("--no-external", action="store_true", help="hide the outside-world node")
    p.add_argument("--show-private", action="store_true",
                   help="state view: also show writes that do not leave the file")
    p.add_argument("--max-names", type=int, default=6,
                   help="variables listed per group and row (default 6, full list is the tooltip)")
    p.add_argument("--layout", default="include", choices=["include", "value"],
                   help="lay out by include hierarchy (default) or by value flow")
    p.add_argument("--render", action="store_true", help="also write .svg via dot(1)")
    opts = p.parse_args(argv)

    root = os.path.abspath(opts.root)
    if not os.path.isdir(os.path.join(root, "cmake")):
        raise SystemExit("not a cmake project root: %s" % root)

    out = opts.out or os.path.join(root, "pix", "cmake.dot")
    hidden = set() if opts.show_hidden else set(HIDDEN_BY_DEFAULT)

    all_files = scan(root, opts.all)
    for f in all_files.values():
        parse_file(f)

    if opts.all or opts.focus:
        keep = set(all_files)
    else:
        keep = reachable(all_files, "CMakeLists", keep_hooks=True)
    if opts.focus:
        keep = neighborhood(all_files, opts.focus, opts.depth)

    files = OrderedDict((k, v) for k, v in all_files.items() if k in keep)
    m = build_model(files, hidden, keep_hooks=not opts.no_hooks)

    os.makedirs(os.path.dirname(out), exist_ok=True)
    written = []
    if opts.mode in ("flow", "both", "all"):
        flow_out = out if opts.mode != "both" else out
        with open(flow_out, "w", encoding="utf-8") as fh:
            fh.write(render_flow(m, opts))
        written.append(flow_out)
    def extra(suffix):
        if opts.out:
            return os.path.join(os.path.dirname(out),
                                os.path.basename(out)[:-4] + suffix + ".dot")
        return os.path.join(os.path.dirname(out), "cmake" + suffix + ".dot")

    if opts.mode in ("order", "both", "all"):
        order_out = out if opts.mode == "order" and opts.out else extra("-order")
        with open(order_out, "w", encoding="utf-8") as fh:
            fh.write(render_order(m, opts))
        written.append(order_out)
    if opts.mode in ("commands", "all"):
        cmd_out = out if opts.mode == "commands" and opts.out else extra("-commands")
        with open(cmd_out, "w", encoding="utf-8") as fh:
            fh.write(render_commands(m, opts))
        written.append(cmd_out)
    if opts.mode in ("state", "all"):
        state_out = out if opts.mode == "state" and opts.out else extra("-state")
        with open(state_out, "w", encoding="utf-8") as fh:
            fh.write(render_state(m, opts))
        written.append(state_out)

    print("files: %d   value edges: %d   hooks: %d   external values: %d"
          % (len(files), len(m.flow), len(m.hooks), len(m.external_names)))
    for w in written:
        print("wrote %s" % os.path.relpath(w, root))

    if opts.render:
        if not shutil.which("dot"):
            raise SystemExit("graphviz 'dot' not found")
        for w in written:
            svg = w[:-4] + ".svg"
            subprocess.run(["dot", "-Tsvg", "-o", svg, w], check=True)
            print("wrote %s" % os.path.relpath(svg, root))
    return 0


if __name__ == "__main__":
    sys.exit(main())
