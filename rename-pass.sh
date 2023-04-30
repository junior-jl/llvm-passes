#!/bin/bash

if [ $# -ne 2 ]; then
  echo "Usage: $0 <old_pass> <new_pass>"
  echo "Example: $0 OldPass NewPass"
  exit 1
fi

# Get the old pass name and new pass name
old_pass=$1
new_pass=$2

# Convert the pass names to underscore format
underscored_oldpass=$(echo "$old_pass" | sed 's/\([A-Z]\)/_\L\1/g' | sed 's/^_//')
underscored_newpass=$(echo "$new_pass" | sed 's/\([A-Z]\)/_\L\1/g' | sed 's/^_//')

# Rename the source file
mv "passes/$underscored_oldpass.cpp" "passes/$underscored_newpass.cpp"

# Replace the old pass name with the new pass name in CMakeLists.txt
sed -i "s/${old_pass}/${new_pass}/g" passes/CMakeLists.txt
sed -i "s/${underscored_oldpass}/${underscored_newpass}/g" passes/CMakeLists.txt

echo "Renamed ${old_pass} to ${new_pass}"

