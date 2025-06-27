#!/bin/bash

# === Setup script for fast_matrix_rns project ===

echo "Giving execution permission to test scripts..."
chmod +x run_all_tests.sh
chmod +x check_tests.sh

# === Define aliases ===
echo "Creating aliases in ~/.bashrc..."

BASHRC="$HOME/.bashrc"
PROJECT_PATH=$(pwd)

# Add only if not already present
if ! grep -q "alias run_tests=" "$BASHRC"; then
    echo "alias run_tests='$PROJECT_PATH/run_all_tests.sh'" >> "$BASHRC"
    echo "Alias 'run_tests' added."
else
    echo "ℹAlias 'run_tests' already exists."
fi

if ! grep -q "alias check_tests=" "$BASHRC"; then
    echo "alias check_tests='$PROJECT_PATH/check_tests.sh'" >> "$BASHRC"
    echo "Alias 'check_tests' added."
else
    echo "ℹAlias 'check_tests' already exists."
fi

# Reload bashrc
echo "Reloading ~/.bashrc..."
source "$BASHRC"

echo ""
echo "Setup complete. You can now use:"
echo "   ▶ run_tests"
echo "   ▶ check_tests"
echo ""
