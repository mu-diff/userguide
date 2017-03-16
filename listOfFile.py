import os
import subprocess
import sys

#list files
os.system('find ../src/ -name \*.m -print > listFiles');

PWD = os.getcwd();
MUDIFF_DIR = PWD + "/../src"

FOLDER_TO_AVOID=['.git']
fidFromName=open('FunFromName.tex, 'w');
fidFromFolder=open('FunFromFolder.tex, 'w');

fidFromName

#read the file of list of files...
for dname, dirs, files in os.walk(MUDIFF_DIR):
    for fname in files:
        fpath = os.path.join(dname, fname)
        fileName, fileExtension = os.path.splitext(fpath)
        if any(x in dname for x in FOLDER_TO_AVOID) == False and fileExtension == '.m':
            #add to list
            #write on file

