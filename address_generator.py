import random

# Generate a list of 1000 random integers
random_integers = [random.randint(0, 65535) for _ in range(1000)]

# Open the file in write mode
with open("addresses.txt", "w") as file:
    # Write each integer to a new line in the file
    for num in random_integers:
        file.write(str(num) + "\n")
