from sys import argv


def parse_data(path):
    lengths = []
    i = 0
    f = open(path, "r", encoding="utf-8")
    for line in f.read().split("\n"):
        for word in line.split(" "):
            lengths.append((len(word), word, i))
        i += 1
    return lengths

if __name__ == "__main__":
    if len(argv) != 2:
        print("Usage: ./length_checker.py <file>")
    else:
        data = parse_data(argv[1])
        print(max(data))
