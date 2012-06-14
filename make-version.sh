#!/bin/sh
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
