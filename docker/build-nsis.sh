!# /bin/bash

if [ -z $1 ] || [ -z $2 ] ;then
    echo "Usage: build-nsis <base-directory> <installer-file-name>"
    exit 1
fi

BASE_DIR=$1
INSTALLER_FILE=$2
cd $BASE_DIR
makensis $INSTALLER_FILE
chown 1000:1000 installers/*.exe
