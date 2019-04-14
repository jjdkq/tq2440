#!/bin/sh

/usr/bin/servfox -s 640x480 -w 192.168.1.6:7070 

echo "Content-type: text/html; charset=gb2312"
echo
/bin/cat webcam-result.template

exit 0
