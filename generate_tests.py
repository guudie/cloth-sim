r = 15
c = 21
dist = 12
locked_points = [[0, 0], [20, 0]]
with open("data.txt", "w") as fout:
    fout.write(f"{r} {c}\n")
    for y in range(0, r):
        for x in range(0, c):
            fout.write(f"{136 + x * dist} {50 + y * dist} {1 if [x, y] in locked_points else 0}\n")