#!/bin/bash
# Script to upload firmware to both Node A and Node B

echo "=========================================="
echo "  Upload Script for Node A and Node B"
echo "=========================================="
echo ""

echo "Step 1: Uploading to Node A..."
echo "Please connect Node A ESP32 board to USB"
read -p "Press Enter when Node A is connected..."
pio run -e NodeA --target upload

if [ $? -eq 0 ]; then
    echo "✓ Node A uploaded successfully!"
else
    echo "✗ Node A upload failed!"
    exit 1
fi

echo ""
echo "Step 2: Uploading to Node B..."
echo "Please disconnect Node A and connect Node B ESP32 board to USB"
read -p "Press Enter when Node B is connected..."
pio run -e NodeB --target upload

if [ $? -eq 0 ]; then
    echo "✓ Node B uploaded successfully!"
else
    echo "✗ Node B upload failed!"
    exit 1
fi

echo ""
echo "=========================================="
echo "  Both nodes uploaded successfully!"
echo "=========================================="
