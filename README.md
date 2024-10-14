
# Animal_simulator

Animal Simulator is a program that allows you to add an animal of a selected type to your family and train its traits.

## Building
1. Clone the repository from GitHub:

	```
	git clone git@github.com:HajKlau/Animal_simulator.git
	```

2. Navigate to the project directory:
 
	```
	cd Animal_simulator
	```

3. Build container using docker

	```
	docker build -t animal-simulator .
	```

## Running the app

```
docker run -it animal-simulator
```

## Additional Notes
- We need to run the container with -it flags as it requires access to `STDIN`
