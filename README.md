# Manhattan Distance Project for VLSI CAD
Welcome, this is a project of the CENG4120 VLSI CAD course.

The project aim is to calculate the minimum manhattan distance of a given netlist of cells to be placed within a restricted region. 

## How to run this project

Run the following command to compile the C++ program

```
g++ main.cpp -o main.exe
```

Run the executable with the file paths as arguments (e.g., ./testcases/input1.txt)

```
./main.exe <input_file> <output_file>
```

Well done, you're program should be running.

## To run the evaluator
Run this command to check the legality, boundness, overlapping and total displacement of cells
```
python3 evaluator.py <input_file> <output_file>
```
Also, the script is able to visualize the cells in the input or the solution file, by
    python3 evaluator.py <input_file> <solution_file> --plot <dir_saving_visualization>
The displacement vector for each cell can also be plotted by providing the --plot_displacement flag, 
    python3 evaluator.py <input_file> <solution_file> --plot <dir_saving_visualization> --plot_displacement

matplotlib need to be installed for performing visualization. 
It can be quite slow when generating the visualization for large benchmarks.