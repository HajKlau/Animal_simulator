# Animal_simulator

	Animal Simulator is a program that allows you to add an animal of a selected type to your family and train its features.

	Requirements

To run this program, you need the following:

- Compiler: GCC (tested on Linux, but should work on other systems with GCC as well).
- SQLite: The program uses SQLite for database management. Ensure that SQLite is installed before proceeding.

	Installation

Linux (Ubuntu/Debian):
1. Install the required packages (GCC and SQLite):
	sudo apt-get update
	sudo apt-get install g++ sqlite3 libsqlite3-dev
2. Clone the repository from GitHub:
	git clone <repository_link>
3. Navigate to the project directory:
	cd Animal_simulator
4. Compile the project:
	g++ -o animalSimulator *.cpp -lsqlite3
5. Run the program:
	./animalSimulator

	
Windows:

1. Install a GCC-compatible environment (e.g., MinGW (https://sourceforge.net/projects/mingw/) or Cygwin (https://www.cygwin.com/)).

2. Install SQLite:
        - Download and install SQLite from SQLite official website (https://www.sqlite.org/download.html).
        - Ensure that sqlite3 and libsqlite3 development libraries are available in your environment.

3. Clone the repository from GitHub:
	git clone <repository_link>

4. Open a terminal (Git Bash, Command Prompt, or Cygwin), and navigate to the project directory:

	cd Animal_simulator

5. Compile the project using the MinGW GCC compiler:

	g++ -o animalSimulator *.cpp -lsqlite3

6. Run the program:

	./animalSimulator
	

	Additional Notes
- The program uses a file named game_state.db for database operations, which is included in the project.
- Ensure that you have write access to the directory where the program is run to avoid issues with database file creation and modifications.
