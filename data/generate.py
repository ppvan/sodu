import csv
import math
from pathlib import Path

# WARN: don't expect this to works, it may destroy all test files.

# Get current script location 's parrent.
DATA_DIR = Path(__file__).parent.resolve()
csv_file = DATA_DIR / "sudoku.csv"

# Open the CSV file and read its contents
with open(csv_file, "r") as file:
    csv_reader = csv.reader(file)

    # Skip the header row (assuming it's present)
    header = next(csv_reader, None)

    # Read the data and split it into quizzes and solutions
    with open(DATA_DIR / "test-9x9.txt", "w") as f:
        lines = []
        size = None

        for index, row in enumerate(csv_reader):
            quiz, _ = row
            size = len(quiz)
            lines.append(" ".join(quiz) + "\n")

        rows = len(lines)
        size = int(math.sqrt(size))
        lines.insert(0, f"{rows} {size}\n")
        f.writelines(lines)
