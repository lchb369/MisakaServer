#!/usr/bin/env python2.7
# -*- coding: utf-8 -*-

import os, sys
import sys, getopt
import re
import filecmp
import sys

class MisakaTool(object):
    """manager tool for debug"""
    def matchfile(self, reg, dstfile):
        p = re.compile(reg)
        for mfile in os.listdir('./'):
            if mfile == dstfile:
                    continue
            if None == p.match(mfile):
                    continue
            if not filecmp.cmp(mfile, dstfile):
                    print("%s->%s missmatch " % (mfile, dstfile) )
            else:
                    pass
                    #print("%s->%s match " % (mfile, dstfile) )

    def help(self):
        print("use [regex] [dstfile]")

if __name__ == '__main__':
    mtool = MisakaTool()
    srcfile = None
    dstfile = None
    opts, args = getopt.getopt(sys.argv[1:], "h:s:d:")
    for op, val in opts:
        if op == "-s":
            srcfile = val
        elif op == "-d":
            dstfile = val
        else:
            MisakaTool.help()

    mtool.matchfile(srcfile, dstfile)






