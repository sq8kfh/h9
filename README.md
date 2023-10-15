# h9

```
sudo chmod 1777 /cores
ulimit -c unlimited
/usr/libexec/PlistBuddy -c "Add :com.apple.security.get-task-allow bool true" tmp.entitlements
codesign -s - -f --entitlements tmp.entitlements CrashSelf ./src/h9d
./src/h9d -c conf/h9d.conf -vvvD
```