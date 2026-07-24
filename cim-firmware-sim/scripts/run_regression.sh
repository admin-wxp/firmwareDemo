#!/bin/bash
echo "Building firmware..."
cd firmware && make clean && make && cd ..

echo "Running regression..."
python3 tests/regression.py

echo "Done."