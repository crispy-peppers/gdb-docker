#!/bin/bash
cd /tmp
python3 -m http.server 80 &> /dev/null &
/vuln-echo-server
tail -f /dev/null
