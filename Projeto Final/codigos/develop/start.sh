#!/bin/bash

export OMPI_ALLOW_RUN_AS_ROOT=1
export OMPI_ALLOW_RUN_AS_ROOT_CONFIRM=1
echo "10.244.0.149    kafka-broker " >> /etc/hosts
echo "10.245.217.227  quickstart-es-http" >> /etc/hosts

python3 executor.py