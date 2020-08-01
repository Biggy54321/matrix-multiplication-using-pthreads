import sys
from random import randint

# Check the command line arguments
if (len(sys.argv) != 5):
    print("Enter the dimensions of the two input matrices")
    exit()

# Get the number of rows and columns for the first matrix
nb_rows = int(sys.argv[1])
nb_cols = int(sys.argv[2])

# Print the first matrix number of rows
print(nb_rows)
# Print the first matrix number of cols
print(nb_cols)
# Print the matrix elements
for i in range(nb_rows):
    for j in range(nb_cols):
        print(randint(1, 100), end = " ")
    print()

# Get the number of rows and columns for the first matrix
nb_rows = int(sys.argv[3])
nb_cols = int(sys.argv[4])

# Print the first matrix number of rows
print(nb_rows)
# Print the first matrix number of cols
print(nb_cols)
# Print the matrix elements
for i in range(nb_rows):
    for j in range(nb_cols):
        print(randint(1, 100), end = " ")
    print()
