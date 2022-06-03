import matplotlib.pyplot as plt

def graph_data():
    times = []

    with open("data.txt", "r") as data_file:
        for row in data_file:
            times.append(int(row.strip()))

    numbers = [i for i in range(1, len(times) + 1)]
    plt.bar(numbers, times, color = "g", label="Reaction Speed")
    plt.xlabel("Count", fontsize = 12)
    plt.ylabel("Reaction Speed (ms)", fontsize = 12)
    plt.title("Reaction Speed Data", fontsize = 20)
    plt.savefig("graph.png")


if __name__ == "__main__":
    graph_data()
