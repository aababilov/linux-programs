#!/usr/bin/python2

import re
import os

def norm(s):
    ret = ""
    for c in s:
        if c in ("[", "("):
            break
        if c not in ("_", " ", ".", ",", "'", "\""):
            ret += c
    return ret.lower()

def common(s1, s2):
    l = min(len(s1), len(s2))
    for i in xrange(l):
        if s1[i] != s2[i]:
            return i if i > l / 2 else 0
    return l

def main():
    songs = open("albums", "rt").read().split("\n")
    files = open("files", "rt").read().split("\n")
    
    song_re = re.compile("([^/]*)/([0-9]*) - (.*)\.[a-zA-Z0-9]*")
    song_data = []
    for song in songs:
        components = song_re.match(song)
        if not components:
            continue
        n1 = norm(components.group(3))
        try:
            os.mkdir(components.group(1))
        except OSError:
            pass
        song_data.append((song, components.group(3), n1))
    used_songs = set()
    problem_files = set()
    plan = []
    for f in files:
        if not f:
            continue
        best_common = 0
        best_song = "!!NOT FOUND!!"
        n2 = norm(f)
        for (song, song_short, n1) in song_data:
            curr_common = common(n1, n2)
            if curr_common > best_common:
                best_common = curr_common
                best_song = song
        print "%s - %s" % (f, best_song)
        plan.append((f, best_song))
        if best_song:
            if best_song in used_songs:
                print "problem: %s - %s" % (f, best_song)
                problem_files.add(f)
            else:
                used_songs.add(best_song)
    print problem_files
    for f, song in plan:
        if f not in problem_files:
            os.rename(f, song)



main()
