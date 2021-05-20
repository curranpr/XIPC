#!/bin/bash

xipcinit
xipcstart -n demo1
java -jar Collector3Program.jar -i @localhost:demo1 -t 5