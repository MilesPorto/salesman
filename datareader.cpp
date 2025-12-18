#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <ctime>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// simple structure to store city coordinates
// could also use std::pair<double> 
// or define a class
const int EARTHR = 6371;
const double DEG_TO_RAD = M_PI / 180.0;

typedef struct {
  double lon, lat;
} COORD;

// fill the array of city locations
int GetData(char* fname, COORD *cities){
  FILE* fp=fopen(fname,"r");
  const int bufsiz=1000;
  char line[bufsiz+1];
  int ncity=0;
  while(1){
    fgets(line,bufsiz,fp);
    if (line[0]=='#') continue;  // skip comments
    if (feof(fp)) break;
    // we only scan for two numbers at start of each line
    sscanf(line,"%lf %lf",&cities[ncity].lon,&cities[ncity].lat);    
    ncity++;
  }
  fclose(fp);
  return ncity;
}

double calcDistBetween2(COORD c1, COORD c2){
  double lat1 = c1.lat * DEG_TO_RAD;
  double lat2 = c2.lat * DEG_TO_RAD;
  double dlat = lat2 - lat1;
  double dlon = (c2.lon - c1.lon) * DEG_TO_RAD;
  double a = pow(sin(dlat/2), 2) + cos(lat1) * cos(lat2) * pow(sin(dlon/2), 2);
  return EARTHR * 2 * asin(sqrt(a));
}

double twoOptDelta(int ncity, COORD *cities, int i, int j){
  if (i > j) {
    int temp = i;
    i = j;
    j = temp;
  }
  int i_prev = i-1;
  int j_next = (j+1)%ncity;
  double oldDist = calcDistBetween2(cities[i_prev],cities[i])+calcDistBetween2(cities[j],cities[j_next]);
  double newDist = calcDistBetween2(cities[i_prev],cities[j])+calcDistBetween2(cities[i],cities[j_next]);
  return newDist-oldDist;
}

double calcDist(int ncity, COORD *cities){
  double dist = 0.0;
  for (int i=0; i<ncity; i++){
    dist+=calcDistBetween2(cities[i],cities[(i+1)%ncity]);
  }
  return dist;
}

int WriteData(char* fname, int ncity, COORD *cities, double execTime){
  FILE* fp = fopen(fname, "w");
  if (!fp) {
    printf("Error: Could not open output file %s\n", fname);
    return 0;
  }
  
  fprintf(fp, "# Optimized TSP tour with %d cities\n", ncity);
  fprintf(fp, "# Format: longitude latitude\n");
  fprintf(fp, "# Total distance: %.2f km\n", calcDist(ncity, cities));
  fprintf(fp, "# Execution time: %.3f seconds\n", execTime);
  
  for (int i = 0; i < ncity; i++) {
    fprintf(fp, "%.6f %.6f\n", cities[i].lon, cities[i].lat);
  }
  
  // Optionally write first city again to close the loop for plotting
  fprintf(fp, "%.6f %.6f\n", cities[0].lon, cities[0].lat);
  
  fclose(fp);
  printf("Wrote optimized tour to %s\n", fname);
  return 1;
}

void twoOptMove(int ncity, COORD *cities, int i, int j){
  // Ensure i < j
  if (i > j) {
    int temp = i;
    i = j;
    j = temp;
  }
  // Reverse the segment from i to j
  while (i < j) {
    COORD temp = cities[i];
    cities[i] = cities[j];
    cities[j] = temp;
    i++;
    j--;
  }
}

double estimateT0(int ncity, COORD *cities, int nSamples){
  double sumDelta = 0.0;
  int count = 0;
  
  for (int s = 0; s < nSamples; s++){
    double distBefore = calcDist(ncity, cities);
    
    int i = 1+rand() % (ncity-1);
    int j = 1+rand() % (ncity-1);
    while (i == j) j = 1+rand() % (ncity-1);
    
    twoOptMove(ncity, cities, i, j);
    
    double distAfter = calcDist(ncity, cities);
    double delta = fabs(distAfter - distBefore);
    
    // Revert move
    twoOptMove(ncity, cities, i, j);

    sumDelta += delta;
    count++;
  }
  double avgDelta = sumDelta / count;
  // T0 = 2 * avgDelta gives ~50% acceptance for typical bad moves
  return 2.0 * avgDelta;
}

double simulatedAnnealing(int ncity, COORD *cities, int tempIters, int itersPerTemp, 
                         double T0, double alpha, char* tempLogFile){
  srand(time(NULL));
  
  double currentDist = calcDist(ncity, cities);
  double bestDist = currentDist;
  double T = T0;
  
  // Open temperature log file if specified
  FILE* tempLog = NULL;
  if (tempLogFile != NULL) {
    tempLog = fopen(tempLogFile, "w");
    if (tempLog) {
      fprintf(tempLog, "# Temperature vs Distance log\n");
      fprintf(tempLog, "# Format: temperature distance(km)\n");
    } else {
      printf("Warning: Could not open temperature log file %s\n", tempLogFile);
    }
  }
  
  // Store best solution
  COORD bestCities[2500];
  for (int i = 0; i < ncity; i++) {
    bestCities[i] = cities[i];
  }
  
  int totalIters = tempIters * itersPerTemp;
  int currentTotalIter = 0;
  int accepted = 0;
  int improvements = 0;
  int lastImprovement = 0;
  
  for (int iter = 0; iter < tempIters; iter++) {
    for (int innerIter = 0; innerIter < itersPerTemp; innerIter++){
      currentTotalIter++;
      
      // Generate random move
      int i = 1+rand() % (ncity-1);
      int j = 1+rand() % (ncity-1);
      while (i == j) j = 1+rand() % (ncity-1);

      // Calculate change in distance
      double deltaDist = twoOptDelta(ncity,cities,i,j);
      
      // Acceptance criterion
      bool accept = false;
      if (deltaDist < 0) {
        // Always accept improvements
        accept = true;
        improvements++;
      } else {
        // Accept worse solutions with probability e^(-delta/T)
        double prob = exp(-deltaDist / T);
        if ((double)rand() / RAND_MAX < prob) {
          accept = true;
        }
      }
      
      if (accept) {
        twoOptMove(ncity, cities, i, j);
        accepted++;
        currentDist+=deltaDist;
        
        // Track best solution
        if (currentDist < bestDist) {
          bestDist = currentDist;
          lastImprovement = currentTotalIter;
          for (int k = 0; k < ncity; k++) {
            bestCities[k] = cities[k];
          }
        }
      }
    }
    
    // Log temperature and current distance after thermalizing
    if (tempLog) {
      fprintf(tempLog, "%.6f %.6f\n", T, currentDist);
    }
    
    // Geometric cooling: T = T * alpha
    T = T * alpha;
    
    // Progress report every 10% of temperature iterations
    if ((iter + 1) % (tempIters / 10) == 0 || iter == 0) {
      printf("Temp step %d/%d: Current=%.2f km, Best=%.2f km, T=%.5f, Accept=%.1f%%\n",
             iter + 1, tempIters, currentDist, bestDist, T, 
             100.0 * accepted / currentTotalIter);
    }
  }
  
  // Close temperature log file
  if (tempLog) {
    fclose(tempLog);
    printf("Wrote temperature log to %s\n", tempLogFile);
  }
  
  // Restore best solution
  for (int i = 0; i < ncity; i++) {
    cities[i] = bestCities[i];
  }
  
  printf("\nFinal statistics:\n");
  printf("  Total iterations: %d (%d temps × %d steps)\n", totalIters, tempIters, itersPerTemp);
  printf("  Improvements: %d\n", improvements);
  printf("  Last improvement at iteration: %d\n", lastImprovement);
  printf("  Acceptance rate: %.2f%%\n", 100.0 * accepted / totalIters);
  printf("  Final temperature: %.3f\n", T);
  
  return bestDist;
}

int main(int argc, char *argv[]){
  const int NMAX=2500;
  COORD cities[NMAX];

  if (argc<2){
    printf("Please provide a data file path as argument\n");
    printf("Usage: %s <input_file> [tempIters] [itersPerTemp] [T0] [alpha] [output_file] [temp_log_file]\n", argv[0]);
    return 1;
  }

  char* infile = argv[1];
  int tempIters = (argc > 2) ? atoi(argv[2]) : 1000;
  int itersPerTemp = (argc > 3) ? atoi(argv[3]) : 1000;
  double T0 = (argc > 4) ? atof(argv[4]) : 0.0;
  double alpha = (argc > 5) ? atof(argv[5]) : 0.99;
  char* outfile = (argc > 6) ? argv[6] : (char*)"travelingsalesmansol.dat";
  char* tempLogFile = (argc > 7) ? argv[7] : NULL;
  
  // Start timing
  clock_t startTime = clock();
  
  // Read input data
  int ncity = GetData(infile, cities);
  if (ncity == 0) return 1;
  
  printf("Read %d cities from %s\n", ncity, infile);
  
  double initialDist = calcDist(ncity, cities);
  printf("Initial distance: %.2f km\n", initialDist);
  
  // Auto-estimate T0 if not provided
  if (T0 <= 0) {
    printf("Estimating initial temperature...\n");
    T0 = estimateT0(ncity, cities, 100);
    printf("Estimated T0: %.2f\n", T0);
  }
  
  printf("\nParameters:\n");
  printf("  Temperature steps: %d\n", tempIters);
  printf("  Iterations per temp: %d\n", itersPerTemp);
  printf("  Total iterations: %d\n", tempIters * itersPerTemp);
  printf("  Initial temperature: %.2f\n", T0);
  printf("  Cooling rate (alpha): %.4f\n", alpha);
  if (tempLogFile) {
    printf("  Temperature log file: %s\n", tempLogFile);
  }
  printf("\n");
  
  // Run simulated annealing
  printf("Running simulated annealing...\n");
  double finalDist = simulatedAnnealing(ncity, cities, tempIters, itersPerTemp, T0, alpha, tempLogFile);
  
  // End timing
  clock_t endTime = clock();
  double execTime = (double)(endTime - startTime) / CLOCKS_PER_SEC;
  
  printf("\n=== RESULTS ===\n");
  printf("Initial distance: %.2f km\n", initialDist);
  printf("Final distance:   %.2f km\n", finalDist);
  printf("Improvement:      %.2f%% reduction\n", 
         100.0 * (initialDist - finalDist) / initialDist);
  printf("Execution time:   %.3f seconds\n\n", execTime);
  
  // Write optimized tour to file
  WriteData(outfile, ncity, cities, execTime);
  return 0;
}
