#!/bin/bash
# It does not work, because Bash doesn't ignore BOM.
set -ue

hoge="fuga"
if [[ $hoge == fuga ]]; then
  echo "Hello, Bash!"
fi
