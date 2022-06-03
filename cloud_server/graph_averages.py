import matplotlib.pyplot as plt

def graph_averages():
    average_times = []

    with open("averages.txt", "r") as averages_file:
        for row in averages_file:
            average_times.append(int(row.strip()))
        
    session_counts = [i for i in range(1, len(average_times) + 1)]
    plt.bar(session_counts, average_times, color = "g", label="Average Reaction Speed")
    plt.xlabel("Session Count", fontsize = 12)
    plt.ylabel("Average Reaction Speed (ms)", fontsize = 12)
    plt.title("Average Reaction Speed Data", fontsize = 20)
    plt.savefig("graph_averages.png")


if __name__ == "__main__":
    graph_averages()
