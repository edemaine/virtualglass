#!/bin/sh
(
  #svnversion
  head -4 .svn/entries | tail -1
  date +%F
) >version.txt
