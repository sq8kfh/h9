# h9

## Build

### MacOS
```
sudo port install spdlog
sudo port install nlohmann-json
CMAKE_PREFIX_PATH=/opt/local//lib/libfmt10/cmake/fmt/ cmake .
make
```

### Enable core dump
```
sudo chmod 1777 /cores
ulimit -c unlimited
cat <<EOF > tmp.entitlements
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
	<key>com.apple.security.get-task-allow</key>
	<true/>
</dict>
</plist>
EOF
/usr/libexec/PlistBuddy -c "Add :com.apple.security.get-task-allow bool true" tmp.entitlements
codesign -s - -f --entitlements tmp.entitlements CrashSelf ./src/h9d
./src/h9d -c conf/h9d.conf -vvvD
```

### Raspberry Pi OS
```
sudo apt-get install python3.9-dev
sudo apt-get install libspdlog-dev
sudo apt-get install nlohmann-json3-dev
cmake .
make
```
