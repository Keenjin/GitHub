from sys import argv

script, filename = argv

txt = open(filename)

print(f"Here's your file {filename}:")
method = input("""
choose one method:
1.read
2.readline
3.readlines
""")

if method == "1":
    print(f"{txt.read()}")
elif method == "2":
    print(f"{txt.readline()}")
    print(f"{txt.readline()}")
    print(f"{txt.readline()}")
elif method == "3":
    print(f"{txt.readlines()}")
