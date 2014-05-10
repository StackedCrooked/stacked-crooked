#!/bin/bash
#
# Cache.sh enables you to cache the result
# of commands so that you can reuse them
# the next time.
#
# Usage:
#   Cache.sh command
#
# Example:
#   Cache.sh svn up # slow 
#   Cache.sh svn up # much quicker now!
#
encoded_command="$(echo "$@" | base64)"
cache_dir="${HOME}/.Cache.sh"
mkdir -p "${cache_dir}"
cache_file="${cache_dir}/${encoded_command}"
[ -f "${cache_file}" ] || {
    echo "$($@)" >"$cache_file"
}
cat "${cache_file}"
