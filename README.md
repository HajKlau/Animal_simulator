
# Animal_simulator

Animal Simulator is a program that allows you to add an animal of a selected type to your family and train its features.

## Requirements

To run this program, you need the following:
- GCC
- SQLite

On Debian/Ubuntu you can install it using
```
sudo apt-get update &&
sudo apt-get install g++ sqlite3 libsqlite3-dev
```
## Building
Clone the repository from GitHub:
	`git clone git@github.com:HajKlau/Animal_simulator.git`
Navigate to the project directory:
	`cd Animal_simulator`
Create build directory:
	`mkdir build`
Compile the project:
	`g++ -o build/animalSimulator *.cpp -lsqlite3`
Run the program:
	`./build/animalSimulator`

## Additional Notes
- The app will create a sqlite file game_state.db in the directory where the app is being run from.
