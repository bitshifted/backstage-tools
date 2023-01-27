!# /bin/bash

if [ -z $1 ] || [ -z $2 ] ;then
    echo "Usage: build-rpm <base-directory> <specfile-name>"
    exit 1
fi

BASE_DIR=$1
echo "Base directory: $BASE_DIR"
SPEC_FILE_NAME=$2
echo "SPECFILE name: $SPEC_FILE_NAME"

cd $BASE_DIR
rpmbuild --define "_topdir $BASE_DIR" -bb SPECS/$SPEC_FILE_NAME
chown -Rv 1000:1000 *
