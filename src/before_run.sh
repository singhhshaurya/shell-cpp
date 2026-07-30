#!/bin/sh

sed -Ei 's|^([[:space:]]*)(//[[:space:]]*)?get_all_executables\(shell\);|\1// get_all_executables(shell);|' src/main.cpp