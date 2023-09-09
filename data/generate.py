import csv
from pathlib import Path

# Get current script location 's parrent.
DATA_DIR = Path(__file__).parent.resolve()
csv_file = DATA_DIR / "sudoku.csv"

# Open the CSV file and read its contents
with open(csv_file, "r") as file:
    csv_reader = csv.reader(file)

    # Skip the header row (assuming it's present)
    next(csv_reader, None)

    # Read the data and split it into quizzes and solutions
    for index, row in enumerate(csv_reader):
        quiz, solution = row

        with open(DATA_DIR / f"test-{index:02}.txt", "w") as f:
            f.write("9\n")
            f.write(" ".join(quiz) + "\n")
            f.write(" ".join(solution) + "\n")
