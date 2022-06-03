from flask import Flask
from flask import request

app = Flask(__name__)

@app.route("/")
def get_reaction():
    with open("data.txt", "a") as data_file:
        data_file.write(request.args.get("var") + "\n")
    return "Reaction Speed: " + request.args.get("var")

@app.route("/average")
def get_average():
    sum = 0
    with open("data.txt", "r") as data_file:
        lines = data_file.readlines()[-3:]
        for i in lines:
            sum += int(i.strip())
    average = sum // 3
    with open("averages.txt", "a") as averages_file:
        averages_file.write(str(average) + "\n")
    return "Average Reaction Speed: " + str(average)
