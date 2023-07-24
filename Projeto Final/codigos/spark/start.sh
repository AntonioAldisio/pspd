#!/bin/bash

echo "10.244.0.149    kafka-broker " > /etc/hosts
echo "10.245.217.227  quickstart-es-http" > /etc/hosts

python3 executor.py