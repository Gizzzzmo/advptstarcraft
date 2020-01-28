#!/bin/sh

dir_name=$(dirname "$0")

if [ -z "$1" ] || [ -z "$2" ]; then
    echo "Usage:" "$0" "race" "filename"
    exit 1
fi

if ! [ -f "$2" ]; then
    echo "Not a file"
    exit 1
fi

if ! [ -x "$dir_name""/../build/forward" ]; then
    echo "Build first"
    exit 1
fi

race=$1 
file_name="$2" 
temp_file="$(mktemp)"

"$dir_name"/../forwardSim.sh "$race" "$file_name" | curl -X POST --data-urlencode forwardSimulationLog@- https://www10.cs.fau.de/advptSC2/forwardSimulation/validation --header "Content-Type:application/x-www-form-urlencoded" 2>/dev/null >"$temp_file"

if grep '<div class="bs-callout bs-callout-danger">' "$temp_file" >/dev/null; then

	echo "Doesn't seem to work, see" "$dir_name"/error.html "for the error message."
    mv "$temp_file" "$dir_name"/error.html
	exit 1

else

	rm "$temp_file"
	exit 0

fi

