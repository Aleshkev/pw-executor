#!/usr/bin/env bash

cd ~/PW_testy/unix || exit
python3 ./test.py ~/pw-executor/ $*
