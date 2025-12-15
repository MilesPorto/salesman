Starter code and data for traveling salesman problem


INSTRUCTIONS:
The simulated annealing code is in datareader.cpp. To run it first run the makefile if you haven't, then run datareader and enter parameters. The first parameter is the data file you are reading from, then the rest are optional but recommended to specify. They are the number of temperatures iterated over, the number of iterations per temperature, the initial temperature (recommended to leave at 0 so the program calculates it automatically), the cooling coefficient (after every temperature iteration, the temperature is multiplied by the cooling coefficient, coefficient of 0.99 corresponds to 1% decrease in temperature per iteration), and the name of the file that you want to write to.

The sol.dat files have the minimized distance at the top. I still haven't made the plots of distance vs temperature but I will. Then you can see the cities pdfs that have the tour that my code found. The cities random pdfs are the tours that the cities data started with just reading them in order.

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
