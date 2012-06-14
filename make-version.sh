#!/bin/sh
echo Subversion: `svnversion`
echo Date: `date +%F`
(
  svnversion
  date +%F
) >version.txt
