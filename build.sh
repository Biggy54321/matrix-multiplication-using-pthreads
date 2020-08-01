#! /bin/bash

# Print the compilation message
echo "Compiling the source code ..."

# Compile the source code
gcc main.c matrix.c -DDO_THREADING -lpthread -o mat_mul

# Print the compilation over message
echo "Compilation over"
