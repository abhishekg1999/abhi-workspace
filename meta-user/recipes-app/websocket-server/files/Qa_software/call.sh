#!/bin/bash

SERVER="ws://localhost:9000"

{
  while true; do
    echo "CANIn_final_status"
    sleep 0.1
  done
} | websocat -t "$SERVER" | jq >> /tmp/out.txt
