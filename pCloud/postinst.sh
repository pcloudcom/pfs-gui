#!/bin/sh

if ! defaults  read loginwindow AutoLaunchedApplicationDictionary 2>/dev/null | grep pCloud >/dev/null 2>/dev/null ; then
	defaults write loginwindow AutoLaunchedApplicationDictionary -array-add '{ "Path" = "/Applications/pCloud.app/Contents/MacOS/pCloud"; "Hide" = 0; }'; 
fi

mkdir -p ~/Library/LaunchAgents
chown $USER ~/Library/LaunchAgents
chown $USER ~/Library

cat > ~/Library/LaunchAgents/com.pcloud.launch.plist <<EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple Computer//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>Label</key>
	<string>com.pcloud.launch</string>
	<key>ProgramArguments</key>
	<array>
		<string>/Applications/pCloud.app/Contents/MacOS/pCloud</string>
	</array>
	<key>RunAtLoad</key>
	<true/>
	<key>KeepAlive</key>
	<true/>
</dict>
</plist>
EOF

chown $USER ~/Library/LaunchAgents/com.pcloud.launch.plist

su - $USER -c "launchctl load ~/Library/LaunchAgents/com.pcloud.launch.plist"
defaults write com.apple.loginwindow LoginHook 'launchctl start com.pcloud.launch'
defaults write com.apple.loginwindow LogoutHook 'launchctl stop com.pcloud.launch'

exit 0
if !  ps auxw | grep -v grep| grep pCloud.app > /dev/null; then
	su - $USER -c "launchctl start com.pcloud.launch"
fi
