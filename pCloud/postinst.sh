#!/bin/sh


osascript -e 'tell application "System Events" to delete login item "pCloud"' 2>/dev/null || true
osascript -e 'tell application "System Events" to make new login item at end with properties {path:"/Applications/pCloud.app", name:"pCloud", hidden:true}'

if !  ps auxw | grep -v grep| grep pCloud.app > /dev/null; then
	/Applications/pCloud.app/Contents/MacOS/pCloud &
fi
