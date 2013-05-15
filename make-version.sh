#!/bin/sh
## Run this after any 'svn commit' but before compiling/building the software.
## It creates a version.txt file which is included as a Qt resource
## and ultimately shown on the titlebar.

if ! svnversion
then
  ## Abort if svnversion fails (e.g. wrong version of Subversion)
  exit 1
fi
if ! date +%F
then
  ## Abort if date fails (e.g. no date program)
  exit 1
fi
(
  svnversion
  date +%F
) >version.txt
