#!/bin/bash

# Constants
HUD_PORT=6005
DST_IP="127.0.0.1"

# Validate arguments
if [ "$#" -lt 1 ]; then
    echo "Usage: recv <listener_port>"
    exit 1
fi

LISTENER_PORT=$1

# Construct JSON payload
JSON_MESSAGE=$(jq -n \
    --arg command "recv" \
    --arg listener_port "$LISTENER_PORT" \
    '{command: $command, listener_port: ($listener_port|tonumber)}')

# Send the JSON message via netcat
echo "$JSON_MESSAGE" | nc -u -w1 127.0.0.1 $HUD_PORT

# Start listening on the specified port
echo "Listening on port $LISTENER_PORT..."
nc -u -l -p "$LISTENER_PORT"
