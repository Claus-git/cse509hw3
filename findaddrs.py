import sys
import os

filename = sys.argv[1]

os.system(f"readelf -r {filename} | grep R_X86_64_RELATIVE > readelf.out")
os.system(f"objdump -D {filename} > objdump.out")

addrs = set()

with open("readelf.out") as file:
    for line in file:
        addrs.add(line[64:-1])

with open("objdump.out") as file:
    for line in file:
        if "lea " in line and "#" in line:
            id = line.index("# ")
            addrs.add(line[id+2:].split(" ")[0])

addrstr = " ".join(addrs)
print(addrstr)
executablename = filename.split("/")[-1]
with open(f"{executablename}addrs.out", "w") as file:
    file.write(addrstr)

