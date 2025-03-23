#!/bin/bash

# Define ports
HUD_PORT=6005
SRC_PORT=6002
DST_PORT=7003
DST_IP="127.0.0.1"

# Define log files
LOG_HUD="hud_traffic.log"
LOG_SRC="src_traffic.log"

# Start tcpdump to capture traffic on HUD port and SRC port
echo "[INFO] Starting packet capture..."
sudo tcpdump -i any udp port $HUD_PORT -X > "$LOG_HUD" 2>&1 &
TCPDUMP_HUD_PID=$!
sudo tcpdump -i any udp port $SRC_PORT -X > "$LOG_SRC" 2>&1 &
TCPDUMP_SRC_PID=$!

# Give tcpdump a moment to start
sleep 2

# JSON message to send
JSON_MESSAGE='{"command": "send", "src_port": 6002, "dst_ip": "127.0.0.1", "dst_port": 7003, "message": "Hello CNL_HUD_DCE from HUD_CNL_DTE"}'

# Send JSON command to HUD using netcat (nc)
echo "[INFO] Sending command to HUD on port $HUD_PORT..."
echo "$JSON_MESSAGE" | nc -u -w1 127.0.0.1 $HUD_PORT

# Wait for the packets to be captured
sleep 3

# Stop tcpdump
echo "[INFO] Stopping packet capture..."
sudo kill $TCPDUMP_HUD_PID
sudo kill $TCPDUMP_SRC_PID

# Show the results
echo "[INFO] Captured traffic on HUD port ($HUD_PORT):"
cat "$LOG_HUD"

echo "[INFO] Captured traffic on SRC port ($SRC_PORT):"
cat "$LOG_SRC"
