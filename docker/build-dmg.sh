!# /bin/bash

if [ -z $1 ] || [ -z $2 ] ;then
    echo "Usage: build-dmg <base-directory> <script-file-name>"
    exit 1
fi

BASE_DIR=$1
SCRIPT_FILE_NAME=$2

cd $BASE_DIR
$( ./$SCRIPT_FILE_NAME )
chown -Rv 1000:1000 *.dmg
