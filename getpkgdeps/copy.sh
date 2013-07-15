#!/bin/sh

PREF=./mysys
VERBOSE=
PKGLIST=./pkglist.txt
NEEDFILES=./pkgneeds.txt
MAINPKGS=./mainpkg.txt


if [ "x$1"="x-v" ]; then
    VERBOSE="-v"
fi

rm $VERBOSE $PKGLIST
rm $VERBOSE $NEEDFILES

copyfiles()
{
    for i in `cat $1` ; do
	NEWFILE=${PREF}/$i
	if [ -h "$i" ]; then
	    if [ ! -h "$NEWFILE" ]; then
		echo "link name $i to `readlink $i`"
		if [ -e "$NEWFILE" ]; then
		    rm -r $VERBOSE "$NEWFILE"
		fi
		ln -sf $VERBOSE `readlink "$i"` "$NEWFILE"
	    fi
	elif [ -d "$i" ] ; then
	    if [ ! -d "$NEWFILE" ]; then
		mkdir "$NEWFILE"
	    fi
	elif [ -r "$i" ]; then
	    if [ ! -r "$NEWFILE" ]; then
		cp $VERBOSE "$i" "$NEWFILE"
	    fi
	else
	    echo "another type of file: ${i}!" >&2 
	fi
    done
}

./getpkgdeps `cat $MAINPKGS` > $PKGLIST

for i in `cat $PKGLIST`; do
    cat "/var/lib/dpkg/info/$i.list" >> $NEEDFILES
done

copyfiles pkgneeds.txt
copyfiles otherneeds.txt
