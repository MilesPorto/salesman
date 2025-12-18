import sys
import numpy as np
import matplotlib.pyplot as plt

def read_temp_dist_data(filename):
    """Read temperature and distance data from file, skipping comment lines."""
    temperatures = []
    distances = []
    
    with open(filename, 'r') as f:
        for line in f:
            # Skip comment lines
            if line.strip().startswith('#') or not line.strip():
                continue
            
            # Parse temperature and distance
            parts = line.strip().split()
            if len(parts) >= 2:
                try:
                    temp = float(parts[0])
                    dist = float(parts[1])
                    temperatures.append(temp)
                    distances.append(dist)
                except ValueError:
                    # Skip lines that can't be parsed as floats
                    continue
    
    return np.array(temperatures), np.array(distances)

def plot_temp_vs_distance(temperatures, distances, output_file):
    """Create and save a temperature vs distance plot."""
    
    # Create figure with good size
    plt.figure(figsize=(10, 6))
    
    # Plot the data
    plt.plot(temperatures, distances, 'b-', linewidth=1.5, alpha=0.8)
    
    # Add labels and title
    plt.xlabel('Temperature', fontsize=12)
    plt.ylabel('Distance (km)', fontsize=12)
    plt.title('Distance vs Temperature During Simulated Annealing', fontsize=14, fontweight='bold')
    
    # Add grid for better readability
    plt.grid(True, alpha=0.3, linestyle='--')
    
    # Add some statistics as text
    min_dist = np.min(distances)
    max_dist = np.max(distances)
    final_dist = distances[-1]
    initial_dist = distances[0]
    improvement = 100 * (initial_dist - final_dist) / initial_dist
    
    stats_text = f'Initial: {initial_dist:.2f} km\nFinal: {final_dist:.2f} km\nImprovement: {improvement:.2f}%'
    plt.text(0.98, 0.97, stats_text, transform=plt.gca().transAxes,
             fontsize=10, verticalalignment='top', horizontalalignment='right',
             bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))
    
    # Tight layout to prevent label cutoff
    plt.tight_layout()
    
    # Save the figure
    plt.savefig(output_file, dpi=300, bbox_inches='tight')
    print(f"Plot saved to {output_file}")
    
    # Show some statistics
    print(f"\nStatistics:")
    print(f"  Temperature range: {temperatures[-1]:.2f} to {temperatures[0]:.2f}")
    print(f"  Distance range: {min_dist:.2f} to {max_dist:.2f} km")
    print(f"  Initial distance: {initial_dist:.2f} km")
    print(f"  Final distance: {final_dist:.2f} km")
    print(f"  Improvement: {improvement:.2f}%")
    print(f"  Number of temperature steps: {len(temperatures)}")

def main():
    # Check command line arguments
    if len(sys.argv) != 3:
        print("Usage: python plot_temp_dist.py <input_file> <output_png>")
        print("\nExample:")
        print("  python plot_temp_dist.py temp_log.dat annealing_plot.png")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    # Ensure output file has .png extension
    if not output_file.lower().endswith('.png'):
        output_file += '.png'
        print(f"Note: Added .png extension to output filename")
    
    try:
        # Read data
        print(f"Reading data from {input_file}...")
        temperatures, distances = read_temp_dist_data(input_file)
        
        if len(temperatures) == 0:
            print("Error: No valid data found in input file")
            sys.exit(1)
        
        print(f"Successfully read {len(temperatures)} data points")
        
        # Create plot
        print("Creating plot...")
        plot_temp_vs_distance(temperatures, distances, output_file)
        
    except FileNotFoundError:
        print(f"Error: Could not find input file '{input_file}'")
        sys.exit(1)
    except Exception as e:
        print(f"Error: {str(e)}")
        sys.exit(1)

if __name__ == "__main__":
    main()
