#!/bin/sh
## Run this after any 'git push origin master' but before compiling/building the software.
## It creates a version.txt file which is included as a Qt resource
## and ultimately shown on the titlebar.

if ! git --version
then
  ## Abort if no git installation
  exit 1
fi
if ! date +%F
then
  ## Abort if date fails (e.g. no date program)
  exit 1
fi
(
  git show -s --pretty=format:%h
  echo ""
  date +%F
) >version.txt
