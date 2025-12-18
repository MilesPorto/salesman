traveling salesman problem


INSTRUCTIONS:
The simulated annealing code is in datareader.cpp. To run it first run the makefile if you haven't, then run datareader and enter parameters. The first parameter is the data file you are reading from, then the rest are optional but recommended to specify. They are the number of temperatures iterated over, the number of iterations per temperature, the initial temperature (recommended to leave at 0 so the program calculates it automatically), the cooling coefficient (after every temperature iteration, the temperature is multiplied by the cooling coefficient, coefficient of 0.99 corresponds to 1% decrease in temperature per iteration), the name of the file that you want to store the data of the optimized tour, and the name of the file that you want to store the distance vs temperature data.

statistics: - initial distance (km) - improved distance (km) - execution time (s)

cities150 - 317298.65 - 47647.63 - 105.659
cities1k - 732177.74 - 95378.61 - 79.19
cities2k - 10187617.64 - 280855.00 - 97.244

Increasing the number of iterations will increase the execution time but also makes it more likely to get a better solution.

To pick the parameters, I always let the program calculate the initial temperature. I found online a good algorithm for calculating the initial temperature that chooses an initial temperature that is about double of an average change in energy for a random move (taking the absolute value of the delta). That way, the average bad move has about a 60% chance of being selected, which is a good place to start. Then to pick the number of iterations and the cooling coefficient, I just tried different numbers until I had a balance of execution time and performance. I kept the cooling coefficient very high at 0.999 to ensure smooth annealing and I modified the number of temperature iterations to end the annealing at around 0.5 degrees. From there, I just increased or decreased the iterations per temp.

Files in this directory:

* datareader.cpp : example code to read in the data files (use Makefile)
* datareader.py  : example code to read in the data files
* cities23.dat : list of coordinates for 23 cities in North America
* cities150.dat : 150 cities in North America
* cities1k.dat : 1207 cities in North America
* cities2k.dat : 2063 cities around the world
* routeplot.py : code to plot the globe and salesman's path<br>
usage:<br>
python routeplot.py cities.dat [cities2.dat] -r [="NA"],"World"'<br>
NA = North America, World = Mercator projection of the whole earth
* earth.C : (just for fun) plotting the globe in ROOT
