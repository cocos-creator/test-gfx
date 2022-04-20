#!/bin/bash

set -e

PROGRAM_NAME=$0

build () {
    if ! [ -x "$(command -v tsc)" ]; then
        echo 'Error: tsc is not installed.' >&2
        exit 1
    fi
    echo "compiling..."
    tsc
}

clean () {
    echo "cleaning..."
    rm -rf web-desktop/cocos-js/cocos.js
}

copy () {
    echo "copying..."
    cp dist/* web-desktop/cocos-js
}

update () {
    build
    copy
}

SERVERPID="server.pid"
SERVERLOG="server.log"
SERVER="live-server"
OPENFOLDER="test-gfx/engine/web-desktop"
WATCHFOLDER="${OPENFOLDER}"
ROOTPATH="../../"

serverrun () {
    if ! [ -x "$(command -v ${SERVER})" ]; then
        echo "Error: ${SERVER} is not installed." >&2
        echo "      please install ${SERVER} with npm install (-g) ${SERVER} " >&2
        exit 1
    fi

    if [ -f "${SERVERPID}" ]; then
        echo "server already running on $(cat ${SERVERPID})"
        exit 1
    fi

    ${SERVER} "${ROOTPATH}" --open="${OPENFOLDER}" --watch="${WATCHFOLDER}" > "${SERVERLOG}" 2>&1 &

    echo "$!" > "${SERVERPID}"

    echo "${SERVER} is running, see log in ${SERVERLOG}"
}

serverstop () {
    taskid="$(cat ${SERVERPID})"
    if [ -f "${SERVERPID}" ]; then
        if ps ax | grep ${taskid} >/dev/null; then
            kill -9 "${taskid}"
        fi
        rm ${SERVERPID}
        echo "${SERVER} has been stopped"
    else
        echo "${SERVER} is not running"
    fi
}

serverusage () {
    echo "Usage: ${PROGRAM_NAME} server [start|stop]"
    exit 1
}

server () {
    if [ $# -eq 0 ]; then
        serverusage
    fi
    if [ $# -gt 0 ]; then
        case "$1" in
            start) serverrun;;

            stop) serverstop;;

            *) serverusage;;
        esac
        shift
    fi
}

all () {
    clear
    update
}

usage () {
    echo "Usage:"
    echo "    ${PROGRAM_NAME} [all|build|server|clean|copy|update|help] ..."
    exit 1
}

if [ $# -eq 0 ]; then
    all
    exit 0
fi

while [ $# -gt 0 ]; do
    case "$1" in
        all) all;;

        build) build;;

        server)
            shift
        server $@;;

        clean) clean;;

        copy) copy;;

        update) update;;

        help) usage;;

        *) usage;;
    esac
    shift
done
