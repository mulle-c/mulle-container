#! /bin/sh

MULLE_TREE_H="/home/src/srcM/mulle-tree-h"

if [ ! -x "${MULLE_TREE_H}" ]
then
   MULLE_TREE_H=
fi

if [ ! -d pix ]
then
   echo "run this in project root" >&2
   exit 1
fi

for filename in mulle-flexarray.h mulle-pointerarray.h mulle-structarray.h \
mulle-map.h mulle-array.h mulle-set.h mulle-pointerpairarray.h \
mulle-uniquepointerarray.h mulle-pointerqueue.h
do
   ${MULLE_TREE_H:-mulle-tree-h} "${filename}" | dot -Tsvg -o "pix/${filename%\.h}-includes.svg"
done
