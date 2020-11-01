#!/bin/sh

# source guard
if [ -z "${LIB_SH}" ]; then
    echo "This is a library. Don't execute it directly."
    exit 1
fi

normal="$(tput sgr0)"
red="$(tput setaf 1)"
green="$(tput setaf 2)"
yellow="$(tput setaf 3)"

warning() {
    echo "${N}: ${yellow}$*${normal}" 1>&2
}

say() {
    echo "${N}: $*${normal}" 1>&2
}

quote() {
    for s in "$@"; do
	printf "${sep}%q" "${s}"
	sep=" "
    done
}

quoteline() {
    local sep=""
    while read -r line; do
	echo -n "${sep}"
	quote "${line}"
    done
}

nquote() {
    local sep=""
    for s in "$@"; do
	local e="$(quote "${s}")"
	if [ "${s}" != "${e}" ]; then
	    echo -n "${sep}'${s}'"
	else
	    echo -n "${sep}${s}"
	fi
	sep=" "
    done
}

nquoteline() {
    local sep=" "
    while read -r line; do
	echo -n "${sep}"
	nquote "${line}"
	sep=" "
    done
}

run_editor() {
    local file="$1"
    local we=""
    for e in sensible-editor editor ${EDITOR} vim vi nano pico; do
	we="$(which "${e}")"
	if [ -n "${we}" ]; then
	    "${we}" "${file}"
	fi
        return 0
    done
    echo "Could not find an editor"
    exit 1
}

alltests() {
    basename -s .base.cmd *.base.cmd | nquoteline
}

alltestvariants() {
    basename -s .cmd *.cmd | nquoteline
}
