#!/bin/sh

directory="$(dirname $0)"

"$directory"/test.sh terran "$directory"/fullterrantechtree.txt
"$directory"/test.sh protoss "$directory"/fullprotosstechtree.txt
