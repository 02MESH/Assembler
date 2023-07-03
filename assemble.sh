#!/bin/bash
if [ "$1" != "" ]
then
    assembly2binary $*
else
    echo 'Missing file argument.'
fi
