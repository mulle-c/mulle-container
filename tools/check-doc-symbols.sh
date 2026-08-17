#!/bin/bash
#
# check-doc-symbols.sh - verify that symbols mentioned in dox/*.md
# actually exist in src/**/*.h
#
# Usage: ./tools/check-doc-symbols.sh [dox-dir] [src-dir]
#
# Exits 0 if clean, 1 if phantom symbols found.
#

DOX_DIR="${1:-dox}"
SRC_DIR="${2:-src}"

if [ ! -d "$DOX_DIR" ] || [ ! -d "$SRC_DIR" ]; then
   echo "Usage: $0 [dox-dir] [src-dir]" >&2
   exit 2
fi

# Extract all backtick-quoted mulle_ symbols from markdown
doc_symbols=$(grep -rohP '`(_?mulle_[A-Za-z0-9_]+)`' "$DOX_DIR"/*.md \
   | tr -d '`' \
   | LC_ALL=C sort -u)

# Extract all mulle_ identifiers from headers
src_symbols=$(find "$SRC_DIR" -name "*.h" -o -name "*.c" \
   | xargs grep -ohP '\b_?mulle_[A-Za-z0-9_]+\b' \
   | LC_ALL=C sort -u)

# Also check dependency headers if available
DEP_DIR=""
for d in $(find ~/.mulle/var/cache/sde -maxdepth 1 -type d -name "*container*" 2>/dev/null); do
   dep="$d/dependency"
   if [ -d "$dep" ]; then
      DEP_DIR="$dep"
      break
   fi
done

dep_symbols=""
if [ -n "$DEP_DIR" ]; then
   dep_symbols=$(find "$DEP_DIR" -name "*.h" \
      -exec grep -ohP '\b_?mulle_[A-Za-z0-9_]+\b' {} + 2>/dev/null \
      | LC_ALL=C sort -u)
fi

all_src=$(printf '%s\n%s' "$src_symbols" "$dep_symbols" | LC_ALL=C sort -u)

# Find doc symbols not in source
phantoms=$(LC_ALL=C comm -23 <(echo "$doc_symbols") <(echo "$all_src"))

# Filter out known non-function symbols (type names used as prefixes, etc.)
# that won't match because they're struct names or appear only as part of
# longer identifiers
filtered=""
while IFS= read -r sym; do
   [ -z "$sym" ] && continue
   # Skip pure container type names (mulle_map, mulle_set, etc.) that are
   # struct tags and appear as "struct mulle_map" not bare identifiers
   # Only flag things that look like function calls (have a verb)
   case "$sym" in
      mulle_map|mulle_set|mulle_array|mulle_pointerarray|mulle_pointerpairarray|\
      mulle_pointerqueue|mulle_structarray|mulle_structqueue|mulle_assoc|\
      mulle_pointerset|mulle__rangeset|mulle_allocator|mulle_hash)
         continue ;;
   esac
   filtered="$filtered$sym"$'\n'
done <<< "$phantoms"

if [ -z "$filtered" ]; then
   echo "OK: All documented symbols found in source."
   exit 0
fi

echo "PHANTOM SYMBOLS (in docs but not in source):"
echo "$filtered" | while IFS= read -r sym; do
   [ -z "$sym" ] && continue
   # Show which doc file references it
   files=$(grep -l "\`$sym\`" "$DOX_DIR"/*.md 2>/dev/null | xargs -I{} basename {})
   printf "  %-50s  %s\n" "$sym" "$files"
done
exit 1
