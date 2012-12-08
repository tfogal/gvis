#!/bin/sh
# creates a .srcs file, which is a makefile fragment that defines the SRC
# variable to be all of the source files in a project.
# `all the source files' comes from just searching for them.

find=$(find . \( -iname \*.cpp -or -iname \*.c \) -print | grep -v backup)

echo "SRC:=\\" > .src
n_files=0
for file in ${find} ; do
  n_files+=1
done
# we do it twice so we know when the last file is.
i=0
for file in ${find} ; do
  i+=1
  if test ${i} == ${n_files} ; then
    printf "  ${file}" >> .src
  else
    printf "  ${file} \\" >> .src
  fi
  echo "" >> .src # newline.
done
