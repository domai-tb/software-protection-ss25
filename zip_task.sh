#!/bin/bash

# Check if a directory was provided
if [ -z "$1" ]; then
  echo "Usage: $0 /path/to/directory"
  exit 1
fi

DIR="$1"

# Check if the provided path is a directory
if [ ! -d "$DIR" ]; then
  echo "Error: '$DIR' is not a valid directory."
  exit 1
fi

# Create a zip file name based on the directory name
ZIP_NAME="$(basename "$DIR").zip"

# Change to the target directory and zip all files
cd "$DIR" || exit
zip -r "../$ZIP_NAME" ./*

echo "All files in '$DIR' have been zipped into '$ZIP_NAME'."
