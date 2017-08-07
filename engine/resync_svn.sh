#!/bin/sh
#
# Creates a patch in the current directory to resync a branched
#   working copy to a newer revision of a main Subversion Repository.
#
# Created to resync standalone games based on ioquake3 (ioq3).
#     (Such as Turtle Arena, Smokin' Guns, and many more)
#
# ~ZTurtleMan
#

#
# Most of the variables that are used are listed below.
#

# Show usage; 1 = show full usage, 2 = show mini usage
USAGE=0

# For command line parsing
NEXT_ARG=""

# Revisions
HEAD_REV=""
CURRENT_REV=""
DEFAULT_RESYNC_REV="HEAD"

# Name of the main branch
DEFAULT_MAINNAME="Main"

# Default SVN path to create the patch/log from
DEFAULT_SVN_PATH="svn://svn.example.com/trunk"

# Magic tokens in $CFGFILE
#  "MAIN_REVISION = current-revision"
DEFAULT_MAINREVISION="MAIN_REVISION"
#  "MAIN_SVN = svn path"
DEFAULT_MAINSVN="MAIN_SVN"

# True if config file was used to get current revision
USED_CFGFILE=0

# Name of local file where "$MAINREVISION = ..." is stored.
CFGFILE="Makefile"

# Patch/log prefix, followed by '${CURRENT_REV}to$RESYNC_REV.ext'
DEFAULT_OUTPREFIX="resync_"

APPLY_PATCH=0
SAVE_LOG=1


#
# ioquake3 resync specfic
#
DEFAULT_MAINNAME="ioquake3"

# Default SVN path
DEFAULT_SVN_PATH="https://github.com/ioquake/ioq3/trunk/"

# Magic tokens
DEFAULT_MAINREVISION="IOQ3_REVISION"
# For consistency...
DEFAULT_MAINSVN="IOQ3_SVN"



# Set to the default value
MAINREVISION=$DEFAULT_MAINREVISION
MAINSVN=$DEFAULT_MAINSVN
RESYNC_REV=$DEFAULT_RESYNC_REV
OUTPREFIX=$DEFAULT_OUTPREFIX
MAINNAME=$DEFAULT_MAINNAME


#
# Read command line arguments
#
for ARG in `echo $*`
do

	#
	# Single argument options
	#

	if [ "$ARG" = "--help" ] || [ "$ARG" = "-help" ] || [ "$ARG" = "-h" ]
	then
		USAGE=1
		break
	fi

	if [ "$ARG" = "--apply-patch" ]
	then
		APPLY_PATCH=1
		continue
	fi

	if [ "$ARG" = "--no-log" ]
	then
		SAVE_LOG=0
		continue
	fi


	#
	# Arguments that have a token after it
	#

	if [ "$ARG" = "--svn" ] || [ "$ARG" = "--current-rev" ] || [ "$ARG" = "--resync-rev" ] || [ "$ARG" = "--cfgfile" ] || [ "$ARG" = "--svn-token" ] || [ "$ARG" = "--rev-token" ] || [ "$ARG" = "--prefix" ] || [ "$ARG" = "--mainname" ]
	then
		NEXT_ARG="$ARG"
		continue
	fi

	case "$NEXT_ARG" in
		--svn)
			SVN_PATH="$ARG"
			;;
		--current-rev)
			CURRENT_REV="$ARG"
			;;
		--resync-rev)
			RESYNC_REV="$ARG"
			;;
		--cfgfile)
			CFGFILE="$ARG"
			;;
		--svn-token)
			MAINSVN="$ARG"
			;;
		--rev-token)
			MAINREVISION="$ARG"
			;;
		--prefix)
			OUTPREFIX="$ARG"
			;;
		--mainname)
			MAINNAME="$ARG"
			;;
		*)
			echo "Unknown argument '$ARG'"
			USAGE=2
			;;
	esac
	NEXT_ARG=""

done

# Check for missing ARG!
if [ "$NEXT_ARG" != "" ] && [ ! "$USAGE" -eq 1 ]
then
	echo "Error: Missing ARG for $NEXT_ARG!"
	USAGE=2
fi

# Check for missing current revision!
if [ "$CURRENT_REV" = "" ] && [ "$CFGFILE" = "" ] && [ ! "$USAGE" -eq 1 ]
then
	echo "$0: you have to specify the current revision"
	USAGE=2
fi

# Check for missing config file!
if [ ! "$CFGFILE" = "" ] && [ ! -f "$CFGFILE" ] && [ ! "$USAGE" -eq 1 ]
then
	echo "$0: you have to specify a existing readable/writable file."
	USAGE=2
fi

#
# Show usage, the user asked to see it.
#
if [ $USAGE -eq 1 ]
then
	echo "Usage: $0 OPTIONS... CURRENTREV..."
	echo "  Creates a patch in the current directory to resync a branched"
	echo "    working copy to a newer revision of a main Subversion Repository."
	echo ""
	echo "  CURRENTREV"
	echo "    You must pass ether --current-rev [rev] or --cfgfile [file]"
	echo ""
	echo "  OPTIONS"
	echo "    --help         Show this help"
	echo "           -help"
	echo "           -h"
	echo "    --apply-patch       apply patch, otherwise just download"
	echo "    --no-log            don't save svn log file"
	echo "    --svn [text]        path to main Subversion Repository"
	echo "                          (default: $DEFAULT_SVN_PATH)"
	echo "    --current-rev [rev] branched off revision from main svn"
	echo "    --resync-rev [rev]  resync to main svn at revision (default: $DEFAULT_RESYNC_REV)"
	echo "    --cfgfile [file]    file to get config from"
	echo "                          '$DEFAULT_MAINSVN = svn path'"
	echo "                          '$DEFAULT_MAINREVISION = current-revision'"
	echo "                          $DEFAULT_MAINREVISION is updated when --apply-patch is used"
	echo "    --svn-token [text]  set token for --cfgfile to find svn path"
	echo "                          (default: $DEFAULT_MAINSVN)"
	echo "    --rev-token [text]  set token for --cfgfile to find current-revision"
	echo "                          (default: $DEFAULT_MAINREVISION)"
	echo "    --prefix [text]     prefix for output patch and log files"
	echo "                          (default: $DEFAULT_OUTPREFIX)"
	echo "    --mainname [text]   name of the main branch, for log"
	echo "                          (default: $DEFAULT_MAINNAME)"
	echo "  [rev] requires integer or 'HEAD'"
	exit 1
fi

#
# Mini usage message for miss formated command.
#
if [ $USAGE -eq 2 ]
then
	echo "Try \`$0 --help' for more information."
	exit 1
fi


#
# Set SVN_PATH (if not passed on command line)
#
if [ "$SVN_PATH" = "" ]
then

	# Get svn path from $CFGFILE
	if [ -f "$CFGFILE" ]
	then
		SVN_PATH=`grep "$MAINSVN =" "$CFGFILE" | cut -f3 -d' '`
	fi

	# Check if found the svn path
	if [ "$SVN_PATH" = "" ]
	then
		SVN_PATH=$DEFAULT_SVN_PATH
	fi
fi

#
# Set current revision (if not passed on command line)
#
if [ "$CURRENT_REV" = "" ]
then
	# Get current revision from $CFGFILE
	if [ -f "$CFGFILE" ]
	then
		CURRENT_REV=`grep "$MAINREVISION =" "$CFGFILE" | cut -f3 -d' '`
	fi

	# Check if found the current revision
	if [ "$CURRENT_REV" != "" ]
	then
		USED_CFGFILE=1
	else
		echo "$0 error: failed to find $MAINREVISION in $CFGFILE."
		exit 1
	fi
fi


# Change "HEAD" to integer!
#     Get the HEAD svn log (Which has the revision number),
# ------------------------------------------------------------------------
# r1754 | tjw | 2009-12-08 16:36:48 -0600 (Tue, 08 Dec 2009) | 2 lines
# 
# * build the dedicated server with voip support (benmachine)
# 
# ------------------------------------------------------------------------
#     We only want the line with the revision,
# r1754 | tjw | 2009-12-08 16:36:48 -0600 (Tue, 08 Dec 2009) | 2 lines
#     The revision nubmer is in the first field, ...
# r1754
#     Cut characters after 'r' through end!
# 1754

HEAD_REV=`svn log $SVN_PATH -rHEAD | grep "^r[0-9]" | cut -f1 -d' ' | cut -c 2-`



# There are some places we can't use "HEAD" so just use the integer.
if [ "$CURRENT_REV" = "HEAD" ]
then
	CURRENT_REV=$HEAD_REV
fi

if [ "$CURRENT_REV" -eq "$HEAD_REV" ]
then
	# Were already up to date!
	exit 0
fi

# CURRENT_REV can't be more than the HEAD_REV
if [ "$CURRENT_REV" -gt "$HEAD_REV" ]
then
	echo "$0 error: current revision can't be greater than the HEAD revision."
	exit 1
fi


if [ "$RESYNC_REV" = "HEAD" ]
then
	RESYNC_REV=$HEAD_REV
fi

# RESYNC_REV must be more than the CURRENT_REV
if [ ! "$RESYNC_REV" -gt "$CURRENT_REV" ]
then
	echo "$0 error: resync revision must be more then the current revision."
	exit 1
fi

if [ "$RESYNC_REV" -gt "$HEAD_REV" ]
then
	echo "$0 error: resync revision can't be greater than the HEAD revision."
	exit 1
fi


#
# Create patch
#

PATCH="$OUTPREFIX${CURRENT_REV}to$RESYNC_REV.patch"
PATCH_NOBINARY="$OUTPREFIX${CURRENT_REV}to$RESYNC_REV-nobinary.patch"

# Get patch from CURRENT_REV to RESYNC_REV in $SVN_PATH
if [ ! -f "$PATCH" ] ; then
	svn diff $SVN_PATH@$CURRENT_REV $SVN_PATH@$RESYNC_REV > $PATCH
fi


binaryChanges=`grep -e "Index: .*\.a" -e "Index: .*\.dll" -e "Index: .*\.dylib" $PATCH`
if [ "x$binaryChanges" != "x" ]
then
	binaryChanges=`echo "$binaryChanges" | sed 's/Index: //g'`
	echo "Patch has binary files which need to be manually updated."
	echo "You need to manually remove their diff sections from patch too."
	echo
	echo "$binaryChanges"
	echo
	# filter diff creates a broken patch
	#if [ ! -f "$PATCH_NOBINARY" ] ; then
	#	filterdiff -x "*.a" -x "*.dll" -x "*.dylib" $PATCH > $PATCH_NOBINARY
	#fi
fi


#
# Output svn log messages
#

if [ "$SAVE_LOG" -eq 1 ]
then
	LOG="$OUTPREFIX${CURRENT_REV}to$RESYNC_REV.log"

	if [ ! -f "$LOG" ] ; then
		echo "$MAINNAME resync to revision $RESYNC_REV from $CURRENT_REV.\n" > $LOG
		svn log $SVN_PATH -r "`expr $CURRENT_REV + 1`:$RESYNC_REV" | grep -v "^$" \
			| grep -v "^------*$" | grep -v "| [0-9]* lines$" \
			| grep -v "| 1 line$" >> $LOG

		echo "\n\nLog Messages: (Full)\n" >> $LOG
		svn log $SVN_PATH -r "`expr $CURRENT_REV + 1`:$RESYNC_REV" >> $LOG
	fi
fi


#
# Apply patch
#

if [ "$APPLY_PATCH" -eq 1 ]
then

	if [ -f "$PATCH_NOBINARY" ]
	then
		APPLY_PATCH_FILE=$PATCH_NOBINARY
	else
		APPLY_PATCH_FILE=$PATCH
	fi

	#
	# Apply patch
	#
	# TODO: Can I check the return value to see if patching failed? (So I can tell user to fix it manually)
	#
	# Only shows warnings/errors about failed patching [a existing file].
	patch -p0 -i $APPLY_PATCH_FILE | grep -v "patching file" | grep -v "Hunk #"

	#
	# Remove patch?
	# We shouldn't need it (The failed patch sections are saved as "file-to-patch-with-ext.rej",
	#   unless there is no source to patch.
	#
	#rm $PATCH
	#rm $PATCH_NOBINARY


	if [ "$USED_CFGFILE" -eq 1 ]
	then

		#
		# Update '$MAINREVISION =' in $CFGFILE
		#

		sed "s/$MAINREVISION = $CURRENT_REV/$MAINREVISION = $RESYNC_REV/" $CFGFILE > $CFGFILE.temp
		rm $CFGFILE
		mv $CFGFILE.temp $CFGFILE

	fi

fi

# Finish!
exit 0

