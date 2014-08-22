#!/bin/sh
mkdir -p m4
mkdir -p config
autoreconf --install -I config -I m4

