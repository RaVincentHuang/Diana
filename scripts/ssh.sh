#!/bin/expect -f
set timeout 30
spawn ssh pi@192.168.43.24
expect "*password:" {send "compiler\r"}
interact