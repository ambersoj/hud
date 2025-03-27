#!/bin/bash

# Constants
HUD_PORT=6005
DST_IP="127.0.0.1"

# Validate arguments
if [ "$#" -lt 3 ]; then
    echo "Usage: send <src_port> <dst_port> <message>"
    exit 1
fi

SRC_PORT=$1
DST_PORT=$2
MESSAGE=$3

# Construct JSON payload
JSON_MESSAGE=$(jq -n \
    --arg command "send" \
    --arg src_port "$SRC_PORT" \
    --arg dst_ip "$DST_IP" \
    --arg dst_port "$DST_PORT" \
    --arg message "$MESSAGE" \
    '{command: $command, src_port: ($src_port|tonumber), dst_ip: $dst_ip, dst_port: ($dst_port|tonumber), message: $message}')

# Send the JSON message via netcat
echo "$JSON_MESSAGE" | nc -u -w1 127.0.0.1 $HUD_PORT
