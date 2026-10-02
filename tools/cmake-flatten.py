#!/usr/bin/env python3
"""Expand a cmake include tree into one flat, readable file.

`cmake-graph.py` answers "how do these files connect" (order view) and "what
state do they write" (state view). This answers a third question: **what does
the whole thing look like once the include()s are gone**.

cmake runs include() in the *current* scope, so walking the tree depth first and
splicing each body in at the point of its include() call reproduces the order of
effects. Everything that resolves below the project's cmake/ directory is
expanded; everything else - builtin cmake modules, the missing *Aux hooks - is
left alone as an include() call and reported as such.

Truthful caveats, all of them conditions the tool cannot evaluate:

  * Conditions are NOT evaluated. An include() inside `if( LINK_PHASE)` is
    expanded as if the branch were taken, so the flat file is the union of all
    branches. Each expansion says which if() it sits under.
  * `CMAKE_CURRENT_LIST_DIR` / `_FILE` refer to the flat file now, not to the
    file the line came from. In this project they only appear in the
    MULLE_TRACE_INCLUDE message(), so it only changes trace output.
  * `return()` in an included file returns to its includer; in the flat file it
    returns out of everything. Reported as a warning when it occurs.

USAGE
    tools/cmake-flatten.py                       # CMakeLists.txt -> pix/cmake-flat.cmake
    tools/cmake-flatten.py --stdout              # pipe it instead of writing
    tools/cmake-flatten.py --entry Library       # expand a single module
    tools/cmake-flatten.py --verify              # prove nothing was lost
    tools/cmake-flatten.py --entry Library --out /tmp/library.cmake

Markers in the output, all at column 0 so they grep:

    # >> INLINE  <file>      <-  include( <name>)  [<site>:<line>]   body follows
    # << END     <file>                                              body ends
    # -- KEEP    include( <name>)  [<reason>]                        call kept
    # -- SKIP    include( <name>)  [<reason>]                        dropped, it was a no-op
"""

from __future__ import annotations

import argparse
import collections
import importlib.util
import os
import re
import sys


REPO_ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
GRAPH_TOOL = os.path.join(REPO_ROOT, "tools", "cmake-graph.py")

# cmake searches CMAKE_MODULE_PATH in order and takes the first hit. The
# project's CMakeLists.txt inserts cmake/share, then cmake/reflect, then cmake
# at position 0 - which leaves the opposite order behind.
DEFAULT_MODULE_DIRS = ("cmake", "cmake/reflect", "cmake/share")

# `list(INSERT <list> <index> <element>...)` takes any number of elements. The
# whole argument run is captured and split afterwards, because the elements
# have to stay grouped: cmake inserts each statement's elements at `index 0`
# *in order*, so what gets reversed is the statement order, not the element
# order inside a statement.
RE_MODULE_PATH = re.compile(
    r"list\s*\(\s*INSERT\s+CMAKE_MODULE_PATH\s+0\s+([^)]*)\)", re.I)
RE_TOKEN = re.compile(r"(?<![A-Za-z0-9_$])([A-Za-z_][A-Za-z0-9_]*)\s*\(")
RE_INCLUDE_CALL = re.compile(r"(?<![A-Za-z0-9_$])include\s*\(", re.I)
RE_GUARD_TEST = re.compile(
    r"^[ \t]*if[ \t]*\([ \t]*NOT[ \t]+(__[A-Za-z0-9_]*CMAKE__)[ \t]*\)", re.M | re.I)
RE_GUARD_SET = re.compile(
    r"^[ \t]*set[ \t]*\([ \t]*(__[A-Za-z0-9_]*CMAKE__)[ \t]+(?:ON|TRUE|1)[ \t]*\)",
    re.M | re.I)

MARK_IN = "# >> INLINE  "
MARK_END = "# << END     "
MARK_KEEP = "# -- KEEP    "
MARK_SKIP = "# -- SKIP    "
RULE = "# " + "-" * 76


def load_graph_module():
    """`cmake-graph.py` has a dash in its name, so it needs a path import.

    The parser in there (comment stripping, paren matching, block tracking,
    command scanning) is the project's single source of truth for what a cmake
    file says - re-implementing it here would let the two drift apart.
    """
    spec = importlib.util.spec_from_file_location("cmake_graph", GRAPH_TOOL)
    mod = importlib.util.module_from_spec(spec)
    spec.loader.exec_module(mod)
    return mod


mod = load_graph_module()


# --------------------------------------------------------------------------- #
# locating modules
# --------------------------------------------------------------------------- #

def split_cmake_elements(text: str):
    """The elements of an argument run, in order.

    A quoted run stays one element and loses its quotes; an unquoted element
    ends at the next whitespace. This is the split the port calls
    +componentsSeparatedByWhitespaceKeepingQuotes:.
    """
    return [quoted if quoted else bare
            for quoted, bare in re.findall(r'"([^"]*)"|(\S+)', text)]


def module_dirs(root: str):
    """The module search dirs, in the order cmake will actually search them."""
    entry = os.path.join(root, "CMakeLists.txt")
    found = []
    if os.path.exists(entry):
        with open(entry, "r", encoding="utf-8", errors="replace") as fh:
            text = mod.strip_comments(fh.read())
        statements = [split_cmake_elements(m.group(1))
                      for m in RE_MODULE_PATH.finditer(text)]
        # every statement was inserted at index 0, so it lands in front of the
        # one before it. The elements of a statement keep their own order.
        found = [d for statement in reversed(statements) for d in statement]
    if not found:
        found = list(DEFAULT_MODULE_DIRS)
    dirs, seen = [], set()
    for d in found:
        d = d.strip().strip("/")
        if d and d not in seen and os.path.isdir(os.path.join(root, d)):
            seen.add(d)
            dirs.append(d)
    for d in DEFAULT_MODULE_DIRS:           # never lose a dir that exists
        if d not in seen and os.path.isdir(os.path.join(root, d)):
            seen.add(d)
            dirs.append(d)
    return dirs


class Layout:
    def __init__(self, root, dirs):
        self.root = root
        self.dirs = dirs
        self.sources = {}

    def source(self, relpath):
        src = self.sources.get(relpath)
        if src is None:
            src = Source(self.root, relpath)
            self.sources[relpath] = src
        return src

    def resolve(self, name):
        """Include-target -> relpath below the root, or None if not ours.

        None means "do not expand": a builtin module, or a *Aux hook that this
        project simply does not have.
        """
        if name in ("CMakeLists", "CMakeLists.txt"):
            if os.path.exists(os.path.join(self.root, "CMakeLists.txt")):
                return "CMakeLists.txt"
            return None
        base = name if name.endswith(".cmake") else name + ".cmake"
        if "/" in base:
            if os.path.exists(os.path.join(self.root, base)):
                return base
            return None
        for d in self.dirs:
            if os.path.exists(os.path.join(self.root, d, base)):
                return os.path.join(d, base)
        return None

    def is_ours(self, relpath):
        return relpath == "CMakeLists.txt" or relpath.split("/")[0] == "cmake"


# --------------------------------------------------------------------------- #
# source access
# --------------------------------------------------------------------------- #

class Source:
    def __init__(self, root, relpath):
        self.relpath = relpath
        self.path = os.path.join(root, relpath)
        self.name = ("CMakeLists" if relpath == "CMakeLists.txt"
                     else os.path.basename(relpath)[:-len(".cmake")])
        with open(self.path, "r", encoding="utf-8", errors="replace") as fh:
            self.raw = fh.read()
        self.lines = self.raw.splitlines()
        self.text = mod.strip_comments(self.raw)        # same line count
        self.stripped = self.text.split("\n")
        self.line_start = []                            # raw offset of each line
        at = 0
        for line in self.lines:
            self.line_start.append(at)
            at += len(line) + 1                         # + the newline
        self._includes = None
        self._guard = None

    def raw_offset(self, line, col):
        """(line, col) computed on stripped text -> offset into the raw text.

        Safe because stripping comments only ever truncates a line, so every
        column of the stripped line is the same column of the raw line.
        """
        return self.line_start[line] + col

    def line_col(self, offset):
        line = self.text.count("\n", 0, offset)
        return line, offset - (self.text.rfind("\n", 0, offset) + 1)

    # ---- include() sites ---------------------------------------------------

    def includes(self):
        """Every include() call, in file order, with its raw span and guards.

        Matching happens on the comment stripped text, but the spans are mapped
        back to raw offsets so the raw source can be spliced directly.
        """
        if self._includes is not None:
            return self._includes

        out = []
        for m in RE_INCLUDE_CALL.finditer(self.text):
            open_idx = m.end() - 1
            close = mod.match_paren(self.text, open_idx)
            args = " ".join(self.text[open_idx + 1:close].split())
            tokens = mod.split_args(args)
            if not tokens:
                continue
            name = mod.unquote(tokens[0])
            optional = tokens[-1].upper() == "OPTIONAL"
            line, col = self.line_col(m.start())
            end_line, end_col = self.line_col(close)
            out.append({
                "name": name,
                "args": args,
                "optional": optional,
                "line": line + 1,
                "col": col,
                "start": self.raw_offset(line, col),
                "end": self.raw_offset(end_line, end_col) + 1,
                "conds": conditions_at(self.text, m.start()),
            })
        self._includes = out
        return out

    # ---- the __X__CMAKE__ once-guard ---------------------------------------

    def guard(self):
        """(test_name, set_name) for the file's once-guard, or (None, None).

        A guard is only a guard when it follows the `__FOO__CMAKE__` naming -
        `if( NOT LIBRARY_NAME)` at the top of Library.cmake is an ordinary
        default, not a re-run protection.
        """
        if self._guard is not None:
            return self._guard
        t = RE_GUARD_TEST.search(self.text)
        s = RE_GUARD_SET.search(self.text)
        test = t.group(1).upper() if t else None
        sett = s.group(1).upper() if s else None
        self._guard = (test, sett)
        return self._guard


def conditions_at(text, offset):
    """The block context at `offset`, innermost last.

    Walks like cmake-graph.py's command scanner: `elseif` *replaces* the
    innermost if condition and `else` marks that frame as the else branch -
    neither opens a new block, so a naive counter would drift.
    """
    frames = []
    for m in RE_TOKEN.finditer(text):
        if m.start() >= offset:
            break
        name = m.group(1).lower()
        if name not in mod.BLOCK_OPEN and name not in mod.BLOCK_CLOSE \
                and name != "else":
            continue
        close = mod.match_paren(text, m.end() - 1)
        inner = " ".join(text[m.end():close].split())
        if name in ("if", "elseif"):
            if name == "if":
                frames.append({"kind": "if", "chain": [inner], "else": False})
            else:
                for f in reversed(frames):
                    if f["kind"] == "if":
                        f["chain"].append(inner)
                        f["else"] = False
                        break
        elif name == "else":
            for f in reversed(frames):
                if f["kind"] == "if":
                    f["else"] = True
                    break
        elif name in ("function", "macro"):
            args = mod.split_args(inner)
            label = mod.unquote(args[0]) if args else "?"
            frames.append({"kind": name, "chain": [label], "else": False})
        elif name == "block":
            frames.append({"kind": "block", "chain": [""], "else": False})
        elif name in ("foreach", "while"):
            frames.append({"kind": name, "chain": [inner], "else": False})
        elif frames:
            frames.pop()
    return frames


def describe_condition(frame):
    """One frame as something readable in a banner comment."""
    chain = frame["chain"]
    if len(chain) == 1 and not frame["else"]:
        return "%s( %s)" % (frame["kind"], chain[0])
    parts = ["%s( %s)" % ("if" if i == 0 else "elseif", c)
             for i, c in enumerate(chain)]
    if frame["else"]:
        parts.append("else")
    return " / ".join(parts)


# --------------------------------------------------------------------------- #
# compiling
# --------------------------------------------------------------------------- #

class Compiler:
    def __init__(self, layout):
        self.layout = layout
        self.seen = set()           # expanded so far - the guard, made global
        self.expanded = []          # relpaths, with multiplicity
        self.skips = []             # include records that were a no-op
        self.keeps = []             # include records left as include() calls
        self.warnings = []

    def compile(self, entry):
        head = [
            "# " + "=" * 76,
            "# FLAT EXPANSION of %s" % entry,
            "#",
            "# Generated by tools/cmake-flatten.py - do not edit.",
            "# Every include() below the cmake/ directory is spliced in at the",
            "# position of its call, which is where cmake would have run it.",
            "# Conditions are not evaluated: an include() under if( X) is shown",
            "# as taken. Each expansion names the if() it sits under.",
            "# " + "=" * 76,
            "#",
        ]
        bottom = ["#", RULE]
        body = self.expand(self.layout.source(entry), None)

        summary = []
        summary.append("# %d file(s) expanded, %d include(s) left as calls, "
                       "%d no-op include(s)"
                       % (len(self.expanded), len(self.keeps), len(self.skips)))
        if self.warnings:
            summary.append("# %d warning(s) - see the summary on stderr"
                           % len(self.warnings))
        else:
            summary.append("# no warnings")
        summary.append(RULE)
        return "\n".join(head + body + bottom + summary) + "\n", summary

    def expand(self, src, origin):
        """The body of `src`, with its own include()s replaced in place."""
        out = []
        self.seen.add(src.relpath)
        self.expanded.append(src.relpath)
        out.append(self._banner(src, origin))

        pos = 0
        for inc in src.includes():
            out.append(trim_pending_indent(src.raw[pos:inc["start"]]))
            pos = self._include(inc, src, out)
            # the include call itself is gone; the rest of its line is already
            # accounted for by _include
        out.append(src.raw[pos:])
        out.append(MARK_END + src.relpath)
        return out

    def _banner(self, src, origin):
        if origin is None:
            return MARK_IN + "%-34s <- entry point" % src.relpath
        from_src, inc = origin
        site = "%s:%d" % (from_src.relpath, inc["line"])
        conds = [describe_condition(c) for c in inc["conds"]
                 if not is_noise_condition(c)]
        if conds:
            site += " under %s" % " and ".join(conds)
        return MARK_IN + "%-34s <- %s" % (src.relpath, site)

    def _include(self, inc, src, out):
        """Handle one include() site, return the offset to resume from."""
        name = inc["name"]
        relpath = self.layout.resolve(name)
        head = "include( %s)" % inc["args"]

        if relpath is None or not self.layout.is_ours(relpath):
            if relpath is not None:
                reason = "outside cmake/: " + relpath
            elif is_hook_name(name):
                reason = "project hook, not present"
            else:
                reason = "builtin / supplied elsewhere"
            self.keeps.append((src, inc, reason))
            out.append(MARK_KEEP + "%s  [%s]" % (head, reason) + "\n")
            # The call itself stays: a hook that fires, or a builtin module,
            # still has to be included for the flat file to keep working.
            out.append(" " * inc["col"] + src.raw[inc["start"]:inc["end"]] + "\n")
            return inc["end"]

        target = self.layout.source(relpath)
        test, sett = target.guard()
        if test and test == sett and relpath in self.seen:
            self.skips.append((src, inc, test))
            out.append(MARK_SKIP + "%s  [already expanded, guard %s]"
                       % (head, test) + "\n")
            return inc["end"]

        if test and sett and test != sett:
            self.warnings.append(
                "%s:%d guard tests %s but sets %s - it never latches, so a "
                "second include( %s) would re-run the file"
                % (relpath, guard_line(target, test), test, sett, name))

        if not test and relpath in self.seen:
            self.warnings.append(
                "%s has no __CMAKE__ guard and is expanded %d times - it really "
                "does run more than once"
                % (relpath, self.expanded.count(relpath) + 1))

        if len(self.seen) > 200:
            raise SystemExit("include recursion exploded at %s" % relpath)

        # Keep anything that trailed the include() call (usually a comment) in
        # front of the body instead of stranding it after it.
        nl = src.raw.find("\n", inc["end"])
        tail = src.raw[inc["end"]:nl if nl != -1 else len(src.raw)]
        if tail.strip():
            out.append("   " + tail.strip() + "\n")
        resume = len(src.raw) if nl == -1 else nl + 1

        out.extend(self.expand(target, (src, inc)))
        return resume

    # ---- reporting ---------------------------------------------------------

    def notes(self):
        lines = []
        if self.keeps:
            lines.append("%d include() left as calls:" % len(self.keeps))
            by_reason = collections.OrderedDict()
            for src, inc, reason in self.keeps:
                by_reason.setdefault(reason, []).append((src, inc))
            for reason, group in sorted(by_reason.items()):
                lines.append("   %d x %s" % (len(group), reason))
                for src, inc in group:
                    lines.append("        %-42s  include( %s)"
                                 % ("%s:%d" % (src.relpath, inc["line"]),
                                    inc["args"]))
        if self.skips:
            lines.append("%d no-op include() (guarded, already expanded):"
                         % len(self.skips))
            for src, inc, guard in self.skips:
                lines.append("        %-42s  include( %s)  [%s]"
                             % ("%s:%d" % (src.relpath, inc["line"]),
                                inc["args"], guard))
        if self.warnings:
            lines.append("%d warning(s):" % len(self.warnings))
            lines.extend("   " + w for w in self.warnings)
        return lines


def trim_pending_indent(chunk):
    """Drop indentation that belonged to an include() we are about to replace.

    The text before a call ends with the call's own leading whitespace, which
    would otherwise be left stranded on a line of its own.
    """
    head, nl, tail = chunk.rpartition("\n")
    if not tail.strip() and tail != chunk:
        return head + nl
    if not nl and not chunk.strip():
        return ""
    return chunk


def say(text=""):
    """Everything except the flat file itself goes to stderr, so --stdout pipes."""
    sys.stderr.write(text + "\n")


def guard_line(src, name):
    for i, line in enumerate(src.stripped, 1):
        if name.lower() in line.lower():
            return i
    return 1


def is_hook_name(name):
    """A project extension point, by the naming convention in the cmake tree."""
    return "Aux" in name or name.startswith(("Pre", "Post"))


RE_RERUN_GUARD = re.compile(r"^NOT\s+__\w*CMAKE__$", re.I)


def is_noise_condition(frame):
    """`if( NOT __FOO__CMAKE__)` is always true on the pass being modelled.

    Every expansion in the flat file *is* the first inclusion, so sitting
    inside a re-run guard says nothing about whether the include happens.
    """
    return (frame["kind"] == "if" and not frame["else"]
            and len(frame["chain"]) == 1
            and bool(RE_RERUN_GUARD.match(frame["chain"][0].strip())))


# --------------------------------------------------------------------------- #
# verifying
# --------------------------------------------------------------------------- #

def effect_counts(raw):
    f = mod.CmakeFile("flat", "<memory>", "flat")
    f.text = mod.strip_comments(raw)
    counts = collections.Counter()
    for e in mod.scan_commands(f):
        counts[(e.cmd, " ".join(e.args.split()))] += 1
    return counts


def block_balance(raw):
    """(open, close) block counts, to catch a bad splice."""
    text = mod.strip_comments(raw)
    opens = closes = 0
    for m in RE_TOKEN.finditer(text):
        name = m.group(1).lower()
        if name == "elseif":        # replaces the innermost if, opens nothing
            continue
        if name in mod.BLOCK_OPEN:
            opens += 1
        elif name in mod.BLOCK_CLOSE:
            closes += 1
    return opens, closes


def verify(compiler, flat_text):
    """Did the splice keep every effectful command, exactly as written?

    Both sides go through the same scanner in cmake-graph.py, and the flat file
    is compared as a multiset of (command, arguments) - a duplicate really did
    run twice, so a lost or invented call is a real defect.
    """
    expected = collections.Counter()
    for relpath in compiler.expanded:
        expected += effect_counts(compiler.layout.source(relpath).raw)
    actual = effect_counts(flat_text)

    missing = expected - actual
    extra = actual - expected
    opens, closes = block_balance(flat_text)

    say("verify: %d effectful commands in the %d expanded sources, %d in the flat file"
        % (sum(expected.values()), len(compiler.expanded), sum(actual.values())))
    for label, diff in (("lost", missing), ("invented", extra)):
        for (cmd, args), n in sorted(diff.items()):
            say("   %s x%d: %s( %s)" % (label, n, cmd, args))
    if not missing and not extra:
        say("   ok: identical (command, arguments) multisets")
    say("   blocks: %d open / %d close %s"
        % (opens, closes, "ok" if opens == closes else "MISMATCH"))
    return not missing and not extra and opens == closes


# --------------------------------------------------------------------------- #
# main
# --------------------------------------------------------------------------- #

def main(argv=None):
    p = argparse.ArgumentParser(
        description="Expand the cmake include tree into one flat file.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__.split("USAGE")[-1])
    p.add_argument("--root", default=REPO_ROOT, help="project root (default: repo root)")
    p.add_argument("--entry", default="CMakeLists.txt",
                   help="file to expand: CMakeLists.txt or a module name (default: CMakeLists.txt)")
    p.add_argument("--out", default=None,
                   help="output file (default: pix/cmake-flat.cmake)")
    p.add_argument("--stdout", action="store_true", help="write to stdout, no file")
    p.add_argument("--quiet", action="store_true", help="no summary on stderr")
    p.add_argument("--verify", action="store_true",
                   help="check that no effectful command was lost or invented")
    opts = p.parse_args(argv)

    root = os.path.abspath(opts.root)
    dirs = module_dirs(root)
    layout = Layout(root, dirs)

    entry = opts.entry
    relpath = layout.resolve(entry)
    if relpath is None and os.path.exists(os.path.join(root, entry)):
        relpath = entry
    if relpath is None:
        raise SystemExit("cannot find entry %r below %s (searched: %s)"
                         % (entry, root, ", ".join(dirs)))

    compiler = Compiler(layout)
    flat_text, summary = compiler.compile(relpath)

    report = ["", "entry      %s" % relpath,
              "search     %s" % ", ".join(dirs),
              "expanded   %d file(s)" % len(compiler.expanded)]
    report.extend("           " + n for n in compiler.expanded)
    report.extend(compiler.notes())

    if opts.stdout:
        sys.stdout.write(flat_text)
    else:
        out = opts.out or os.path.join(root, "pix", "cmake-flat.cmake")
        os.makedirs(os.path.dirname(out), exist_ok=True)
        with open(out, "w", encoding="utf-8") as fh:
            fh.write(flat_text)
        report.insert(1, "")
        report.insert(1, "wrote      %s (%d lines)"
                      % (os.path.relpath(out, root), flat_text.count("\n")))

    if not opts.quiet:
        say("\n".join(report))

    if opts.verify:
        say("")
        if not verify(compiler, flat_text):
            return 1
    return 0


if __name__ == "__main__":
    sys.exit(main())
