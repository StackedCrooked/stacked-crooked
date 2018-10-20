#!/bin/bash

PORT() {
	printf "CreatePort "
	for i in "$@"; do
		printf " \--${i}" 
	done | xargs
}


FLOW_TEMPLATE() {
	printf "CreateFlowTemplate "
	for i in "$@"; do 
		printf " \--${i}"
	done | xargs
}

FLOW() {
	printf "CreateFlow "
	for i in "$@"; do 
		printf " \--${i}"
	done | xargs
}

SCENARIO() {
	printf "CreateScenario "
	for i in "$@"; do 
		printf " \--${i}"
	done | xargs
}


cat config  | while read line ; do
	name=$(echo "$line" | cut -d':' -f1)
	value=$(echo $line | cut -d':' -f2)

	# invoke $name as a function call
	$name $value
done

