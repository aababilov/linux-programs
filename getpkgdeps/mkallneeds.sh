#!/bin/sh

PKGLIST=./allpacks.txt
NEEDFILES=./pkgneeds.txt
MAINPKGS=./mainpkg.txt
DEST_DIR=./mysys
VERBOSE=

if [ "x$1" = "x-v" ]; then
    VERBOSE="-v"
fi

copyfiles()
{
    for i in `cat $1` ; do        
	NEWFILE=${DEST_DIR}/$i
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
        elif [ -e "$i" ]; then
	    echo "another type of file: ${i}!" >&2 
	else
	    echo "file doesn't exist: ${i}!" >&2 
	fi
    done
}

rm -f $PKGLIST
rm -f $NEEDFILES

./getpkgdeps `cat $MAINPKGS` > $PKGLIST

if [ ! -d "$DEST_DIR" ]; then
    mkdir "$DEST_DIR" || { echo "no dir \`$DEST_DIR'! die" ; exit 1; }
fi

for i in `cat $PKGLIST`; do    
    cat "/var/lib/dpkg/info/$i.list" >> $NEEDFILES
done

copyfiles pkgneeds.txt

if [ -r otherneeds.txt ]; then
    copyfiles otherneeds.txt
fi
