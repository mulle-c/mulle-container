#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
bench-chart - turn scaling-bench CSV into SVG line charts and a markdown table.

WHAT IT DOES
------------
`test/bench/scaling-bench.c` prints one machine-readable row per measurement:

    container,op,n,ns_op

This tool reads that stream and renders one log-log SVG per operation
(`insert`, `get`, `lookup`, `member`, `search`, `find`, `remove`), with one
line per container, so you can see which data structure scales and which one
degrades. Lines that flatten are O(1); lines that rise by one decade per
decade are O(n); the slope tells you which is which.

The operation names are the container's real verbs per mulle-c11's SYNTAX.md,
so `get` (by index) and `lookup` (by key) are not conflated.

The charts are plain, hand-written SVG: no matplotlib, no gnuplot, no
dependencies. The markdown mode prints the same numbers as tables, for pasting
into docs when a picture is overkill.

USAGE
-----
    # collect (release build recommended), then chart
    mulle-sde test --release craft
    ./test/bench/scaling-bench.exe 2> pix/bench-scaling.csv
    tools/bench-chart.py pix/bench-scaling.csv

    tools/bench-chart.py pix/bench-scaling.csv --outdir pix
    tools/bench-chart.py pix/bench-scaling.csv --markdown
    command-that-produces-csv | tools/bench-chart.py -

Options:
    --outdir DIR   where the .svg files go              (default: pix)
    --prefix NAME  filename prefix                      (default: bench)
    --markdown     print markdown tables to stdout instead of writing files
    --quiet        do not print the files written

CSV format: comment lines start with '#', the header row is skipped, and the
remaining rows are `container,op,n,ns_op`. Rows with a non-positive ns_op are
ignored.
"""

from __future__ import annotations

import argparse
import math
import os
import sys
from collections import OrderedDict
from xml.sax.saxutils import escape

# tab10/tab20-ish palette, readable on white and distinct from each other
PALETTE = [
    "#1f77b4", "#ff7f0e", "#2ca02c", "#d62728", "#9467bd", "#8c564b",
    "#e377c2", "#7f7f7f", "#bcbd22", "#17becf", "#aec7e8", "#ffbb78",
    "#98df8a", "#c5b0d5", "#c49c94", "#f7b6d2",
]

CANVAS_W = 1120
CANVAS_H = 640
MARGIN_LEFT = 78
MARGIN_RIGHT = 300
MARGIN_TOP = 56
MARGIN_BOTTOM = 74
PLOT_W = CANVAS_W - MARGIN_LEFT - MARGIN_RIGHT
PLOT_H = CANVAS_H - MARGIN_TOP - MARGIN_BOTTOM

OP_ORDER = ["insert", "get", "lookup", "member", "search", "find", "remove"]
OP_TITLE = {
    "insert": "insert / add",
    "get": "get (objectAtIndex:)",
    "lookup": "lookup (objectForKey:)",
    "member": "member (presence)",
    "search": "search (binary)",
    "find": "find (linear)",
    "remove": "pop / remove",
}


def parse_csv(stream):
    """-> OrderedDict[(container, op)] = [(n, ns_op), ...] (sorted by n)."""
    data = OrderedDict()
    for line in stream:
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        parts = line.split(",")
        if len(parts) != 4:
            continue
        container, op, n, ns = parts
        if container == "container" and op == "op":     # header
            continue
        try:
            n = int(n)
            ns = float(ns)
        except ValueError:
            continue
        if ns <= 0.0 or n <= 0:
            continue
        data.setdefault((container, op), []).append((n, ns))
    for key in data:
        data[key].sort()
    return data


def human(value):
    """Compact number for axis/tick labels."""
    if value >= 1e6:
        return "%.0fM" % (value / 1e6)
    if value >= 1e3:
        return "%.0fk" % (value / 1e3)
    if value >= 10:
        return "%.0f" % value
    return "%.2g" % value


def cell(value):
    """Number for markdown tables: keep resolution where it matters."""
    if value >= 100:
        return "%.0f" % value
    if value >= 10:
        return "%.1f" % value
    return "%.3g" % value


def log2_ticks(lo, hi):
    """Every exponent present in the data, in order."""
    return list(range(lo, hi + 1))


class Scale:
    """Maps data values onto pixel coordinates linearly (caller pre-logs)."""

    def __init__(self, lo, hi, p0, p1):
        self.lo = lo
        self.hi = hi
        self.p0 = p0
        self.p1 = p1

    def __call__(self, value):
        if self.hi == self.lo:
            return (self.p0 + self.p1) / 2.0
        t = (value - self.lo) / (self.hi - self.lo)
        return self.p0 + t * (self.p1 - self.p0)


def chart_svg(op, series, missing, dotted, colors):
    points = [p for pts in series.values() for p in pts]
    exps = [int(round(math.log2(n))) for n, _ in points]
    lo_exp, hi_exp = min(exps), max(exps)

    ns_values = [ns for _, ns in points]
    lo_lgy = math.floor(math.log10(min(ns_values)))
    hi_lgy = math.ceil(math.log10(max(ns_values)))
    if hi_lgy == lo_lgy:
        hi_lgy = lo_lgy + 1

    xof = Scale(lo_exp, hi_exp, MARGIN_LEFT, MARGIN_LEFT + PLOT_W)
    yof = Scale(hi_lgy, lo_lgy, MARGIN_TOP, MARGIN_TOP + PLOT_H)  # y inverted

    out = []
    a = out.append
    a('<svg xmlns="http://www.w3.org/2000/svg" width="%d" height="%d" '
      'viewBox="0 0 %d %d" font-family="Helvetica, Arial, sans-serif">'
      % (CANVAS_W, CANVAS_H, CANVAS_W, CANVAS_H))
    a('<rect x="0" y="0" width="%d" height="%d" fill="white"/>' % (CANVAS_W, CANVAS_H))

    title = "mulle-container %s cost vs. container size" % OP_TITLE.get(op, op)
    a('<text x="%d" y="30" font-size="18" font-weight="bold" fill="#1a1a1a">%s</text>'
      % (MARGIN_LEFT, escape(title)))
    a('<text x="%d" y="46" font-size="12" fill="#555555">'
      'ns/op, lower is better &#183; log-log &#183; flat = O(1) &#183; '
      'rising = O(log n) / O(n)%s</text>'
      % (MARGIN_LEFT, " &#183; &#215; = not applicable" if missing else ""))

    # ---- y grid + labels (decades) ----
    for lg in range(lo_lgy, hi_lgy + 1):
        y = yof(lg)
        a('<line x1="%d" y1="%.1f" x2="%d" y2="%.1f" stroke="#e6e6e6" stroke-width="1"/>'
          % (MARGIN_LEFT, y, MARGIN_LEFT + PLOT_W, y))
        a('<text x="%d" y="%.1f" font-size="11" fill="#555555" text-anchor="end">%s</text>'
          % (MARGIN_LEFT - 8, y + 4, escape(human(10.0 ** lg))))

    # ---- x grid + labels (powers of two) ----
    for e in log2_ticks(lo_exp, hi_exp):
        x = xof(e)
        a('<line x1="%.1f" y1="%d" x2="%.1f" y2="%d" stroke="#f0f0f0" stroke-width="1"/>'
          % (x, MARGIN_TOP, x, MARGIN_TOP + PLOT_H))
        a('<text x="%.1f" y="%d" font-size="10" fill="#555555" text-anchor="end" '
          'transform="rotate(-45 %.1f %d)">%s</text>'
          % (x, MARGIN_TOP + PLOT_H + 18, x, MARGIN_TOP + PLOT_H + 18,
             escape(human(2 ** e))))

    # ---- axes ----
    a('<line x1="%d" y1="%d" x2="%d" y2="%d" stroke="#333333" stroke-width="1.5"/>'
      % (MARGIN_LEFT, MARGIN_TOP + PLOT_H, MARGIN_LEFT + PLOT_W, MARGIN_TOP + PLOT_H))
    a('<line x1="%d" y1="%d" x2="%d" y2="%d" stroke="#333333" stroke-width="1.5"/>'
      % (MARGIN_LEFT, MARGIN_TOP, MARGIN_LEFT, MARGIN_TOP + PLOT_H))
    a('<text x="%d" y="%d" font-size="12" fill="#333333" text-anchor="middle">'
      'container size (entries)</text>'
      % (MARGIN_LEFT + PLOT_W // 2, CANVAS_H - 8))
    a('<text x="18" y="%d" font-size="12" fill="#333333" text-anchor="middle" '
      'transform="rotate(-90 18 %d)">ns/op</text>'
      % (MARGIN_TOP + PLOT_H // 2, MARGIN_TOP + PLOT_H // 2))

    # ---- series ----
    for name, pts in series.items():
        color = colors[name]
        dash = ' stroke-dasharray="6,4"' if name in dotted else ""
        d = " ".join("%.1f,%.1f" % (xof(int(round(math.log2(n)))), yof(math.log10(ns)))
                     for n, ns in pts)
        a('<polyline points="%s" fill="none" stroke="%s" stroke-width="2"%s '
          'stroke-linejoin="round"/>' % (d, color, dash))
        for n, ns in pts:
            a('<circle cx="%.1f" cy="%.1f" r="2.6" fill="%s"/>'
              % (xof(int(round(math.log2(n)))), yof(math.log10(ns)), color))

    # ---- legend ----
    lx = MARGIN_LEFT + PLOT_W + 24
    ly = MARGIN_TOP + 4
    a('<text x="%d" y="%d" font-size="13" font-weight="bold" fill="#1a1a1a">'
      'containers</text>' % (lx, ly))
    ly += 22
    for name in series:
        dash = ' stroke-dasharray="5,3"' if name in dotted else ""
        a('<line x1="%d" y1="%d" x2="%d" y2="%d" stroke="%s" stroke-width="3"%s/>'
          % (lx, ly - 4, lx + 22, ly - 4, colors[name], dash))
        a('<circle cx="%d" cy="%d" r="2.6" fill="%s"/>'
          % (lx + 11, ly - 4, colors[name]))
        a('<text x="%d" y="%d" font-size="12" fill="#222222">%s</text>'
          % (lx + 30, ly, escape(name)))
        ly += 20

    # containers that have no data for this operation (e.g. a queue has no
    # keyed lookup): show them with an x so every chart lists the same set
    for name in missing:
        cx, cy, s = lx + 10, ly - 5, 7
        a('<line x1="%d" y1="%d" x2="%d" y2="%d" stroke="#999999" stroke-width="2"/>'
          % (cx - s, cy - s, cx + s, cy + s))
        a('<line x1="%d" y1="%d" x2="%d" y2="%d" stroke="#999999" stroke-width="2"/>'
          % (cx - s, cy + s, cx + s, cy - s))
        a('<text x="%d" y="%d" font-size="12" fill="#888888">%s</text>'
          % (lx + 30, ly, escape(name)))
        ly += 20

    a('</svg>')
    return "\n".join(out) + "\n"


def markdown(data, containers, sizes, dotted):
    """Tables grouped by op: rows are sizes, columns are containers."""
    out = []
    for op in OP_ORDER:
        series = OrderedDict((c, dict(data[(c, op)]) if (c, op) in data else None)
                             for c in containers)
        if not any(pts is not None for pts in series.values()):
            continue
        out.append("### %s (ns/op, lower is better)\n" % OP_TITLE.get(op, op))
        names = [c + ("\\*" if (op == "remove" and c in dotted) else "")
                 for c in series]
        out.append("| n | " + " | ".join(names) + " |")
        out.append("|---|" + "|".join("---:" for _ in series) + "|")
        for n in sizes:
            if not any(pts is not None and n in pts for pts in series.values()):
                continue
            cells = []
            for c in series:
                pts = series[c]
                if pts is None:
                    cells.append("x")           # operation not applicable
                elif n in pts:
                    cells.append(cell(pts[n]))
                else:
                    cells.append("-")           # not measured at this size
            out.append("| %d | %s |" % (n, " | ".join(cells)))
        if op == "remove" and dotted:
            out.append("\n\\* = `pop` (remove from the end), others are `remove`.")
        out.append("")
    return "\n".join(out)


def main(argv):
    parser = argparse.ArgumentParser(
        description="Render SVG charts from scaling-bench CSV (stderr stream).")
    parser.add_argument("csv", nargs="?", default="pix/bench-scaling.csv",
                        help="CSV file, or - for stdin (default: pix/bench-scaling.csv)")
    parser.add_argument("--outdir", default="pix", help="output directory")
    parser.add_argument("--prefix", default="bench", help="output filename prefix")
    parser.add_argument("--markdown", action="store_true",
                        help="print markdown tables to stdout instead of writing SVG")
    parser.add_argument("--quiet", action="store_true", help="do not list written files")
    args = parser.parse_args(argv)

    if args.csv == "-":
        stream = sys.stdin
    else:
        if not os.path.exists(args.csv):
            parser.error("no such file: %s" % args.csv)
        stream = open(args.csv, "r", encoding="utf-8")

    try:
        data = parse_csv(stream)
    finally:
        if stream is not sys.stdin:
            stream.close()

    if not data:
        parser.error("no benchmark rows found")

    # `pop` (remove from the end) and `remove` (by key/range) share one chart,
    # with the pop lines dashed.
    dotted = set()
    merged = OrderedDict()
    for (container, op), pts in data.items():
        if op == "pop":
            dotted.add(container)
            op = "remove"
        merged.setdefault((container, op), []).extend(pts)
    for pts in merged.values():
        pts.sort()
    data = merged

    containers = OrderedDict()
    ops = OrderedDict()
    sizes = set()
    for (container, op), pts in data.items():
        containers.setdefault(container, None)
        ops.setdefault(op, None)
        for n, _ in pts:
            sizes.add(n)
    containers = list(containers)
    sizes = sorted(sizes)
    colors = {name: PALETTE[i % len(PALETTE)] for i, name in enumerate(containers)}

    if args.markdown:
        sys.stdout.write(markdown(data, containers, sizes, dotted))
        return 0

    if not os.path.isdir(args.outdir):
        os.makedirs(args.outdir)

    for op in OP_ORDER:
        series = OrderedDict((c, data[(c, op)]) for c in containers if (c, op) in data)
        missing = [c for c in containers if (c, op) not in data]
        if not series:
            continue
        path = os.path.join(args.outdir, "%s-%s.svg" % (args.prefix, op))
        with open(path, "w", encoding="utf-8") as fp:
            fp.write(chart_svg(op, series, missing, dotted, colors))
        if not args.quiet:
            print(path)

    return 0


if __name__ == "__main__":
    sys.exit(main(sys.argv[1:]))
