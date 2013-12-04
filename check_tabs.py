# just a simple script that checks all .cpp and .h
# files in the directory to see if the whitespace
# starting each line is tabs and not spaces.

import sys
import os

def checkFile(filename):
	f = open(filename, 'r')

	problems = False
	lineNum = 1
	for line in f:
		if line[0] == ' ':
			if not problems:
				problems = True
				print filename + ':'
			print 'Line ' + str(lineNum) + ': ' + line,	
		lineNum += 1

	f.close()

if __name__ == '__main__':
		
	for filename in os.listdir('.'):
		if filename == 'qrc_virtualglass.cpp':
			continue
		if filename[:4] == 'moc_':
			continue
		if (filename[-4:] == '.cpp' or filename[-2:] == '.h'):
			checkFile(filename)

