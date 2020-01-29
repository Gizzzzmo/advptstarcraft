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
json_output="$(mktemp)"
server_answer="$(mktemp)"

"$dir_name"/../forwardSim.sh "$race" "$file_name" > "$json_output"
curl -X POST --data-urlencode forwardSimulationLog@"$json_output" https://www10.cs.fau.de/advptSC2/forwardSimulation/validation --header "Content-Type:application/x-www-form-urlencoded" 2>/dev/null >"$server_answer"

if grep '<div class="bs-callout bs-callout-danger">' "$server_answer" >/dev/null || grep 'Internal Server Error' "$server_answer" >/dev/null
then
	echo "Doesn't seem to work, see" "$dir_name"/error.html "for the error message and "$dir_name"/error.json for our output."
    mv "$json_output" "$dir_name"/error.json
    mv "$server_answer" "$dir_name"/error.html
	exit 1

else
    rm "$json_output"
    rm "$server_answer"
	exit 0

fi

