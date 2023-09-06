r = 10
c = 10
dist = 25
with open("data.txt", "w") as fout:
    fout.write(f"{r} {c}\n")
    for y in range(0, r):
        for x in range(0, c):
            fout.write(f"{50 + x * dist} {50 + y * dist}\n")