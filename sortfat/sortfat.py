#!/usr/bin/python
import os
import sys

def processdir(dirname):
    print "process: %s" % dirname
    dirsuff = ".new"
    while True:
        try:
            os.mkdir(dirname + dirsuff, 0777)
            break
        except:
            dirsuff += "#"
    try:
        flist = os.listdir(dirname)
        flist.sort()
        for shortname in flist:
            os.rename(dirname + "/" + shortname,
                      dirname + dirsuff + "/" + shortname)
        os.rmdir(dirname)
        os.rename(dirname + dirsuff, dirname)
        for shortname in flist:
            fullname = dirname + "/" + shortname
            if os.path.isdir(fullname):
                processdir(fullname)
    except Exception as e:
        print "%s" % e

for arg in sys.argv[1:]:
    processdir(os.path.normpath(arg))
