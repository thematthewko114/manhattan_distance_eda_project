#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
using namespace std;
using namespace std::chrono;

class Cell { // The class
public:      // Access specifier
  string name;
  float y_coord;
  float x_coord;
  float y_coord_original;
  float x_coord_original;
  int height;
  int width;
  bool assigned_row = false;
};

class Row {
public:
  const int height;
  const float y_coord;
  int total_width;
};

vector<string> splitString(const string inputString) {
  vector<string> outputVector;
  string currentString;
  for (char c : inputString) {
    if (isspace(c)) {
      if (!currentString.empty()) {
        outputVector.push_back(currentString);
        currentString.clear();
      }
    } else {
      currentString += c;
    }
  }
  if (!currentString.empty()) {
    outputVector.push_back(currentString);
  }

  return outputVector;
}

int main(int argc, char* argv[]) {
  auto start = high_resolution_clock::now();
  // init
  ifstream input_stream;
  input_stream.open(argv[1]);
  string line;
  vector<string> input_result;
  getline(input_stream, line);
  input_result = splitString(line);
  const int height = stoi(input_result[0]) * 4;
  const int width = stoi(input_result[1]);
  getline(input_stream, line);
  input_result = splitString(line);
  const float track_height = stof(input_result[0]);
  const float track_width = stof(input_result[1]);
  getline(input_stream, line);
  const float displacement_upper_bound = stof(line);
  getline(input_stream, line);
  const int cell_no = stoi(line);
  cout << "Height: " << height << endl;
  cout << "Width: " << width << endl;
  cout << "Track Height: " << track_height << endl;
  cout << "Track Width: " << track_width << endl;
  cout << "Displacement Upper Bound: " << displacement_upper_bound << endl;
  cout << "Cell Number: " << cell_no << endl;
  cout << "Fetching data within region complete" << endl;
  vector<Cell> cell_list;
  vector<Row> row_list;
  for (int i = 0; i < cell_no; i++) {
    getline(input_stream, line);
    input_result = splitString(line);
    Cell new_cell{input_result[0],       stof(input_result[1]),
                  stof(input_result[2]), stof(input_result[1]),
                  stof(input_result[2]), stoi(input_result[3]),
                  stoi(input_result[4])};
    // cout << "Name: " << new_cell.name << ", Y: " << new_cell.y_coord<< ", X:
    // " << new_cell.x_coord << ", Height: " << new_cell.height<< ", Width: " <<
    // width << endl;
    cell_list.push_back(new_cell);
  }
  cout << "Fetching cell data complete" << endl;
  input_stream.close();
  // row assignment
  float scan_lower_bound = 0;
  while (scan_lower_bound < height) {
    float scan_upper_bound = scan_lower_bound + 12;
    int count8 = 0;
    int count12 = 0;
    for (int i = 0; i < cell_no; i++) {
      if (cell_list[i].y_coord >= scan_lower_bound &&
          cell_list[i].y_coord <= scan_upper_bound &&
          !cell_list[i].assigned_row) {
        if (cell_list[i].height == 8) {
          count8++;
        } else if (cell_list[i].height == 12) {
          count12++;
        }
      }
    }
    int total_width = 0;
    if (count12 >= count8) {
      for (int i = 0; i < cell_no; i++) {
        if (cell_list[i].y_coord >= scan_lower_bound &&
            cell_list[i].y_coord <= scan_upper_bound &&
            !cell_list[i].assigned_row) {
          if (cell_list[i].height == 12 &&
              total_width + cell_list[i].width <= width) {
            cell_list[i].y_coord = scan_lower_bound;
            cell_list[i].assigned_row = true;
            total_width += cell_list[i].width;
          } else {
            cell_list[i].y_coord += 12;
          }
        }
      }
      Row new_row { 12, scan_lower_bound, total_width };
      row_list.push_back(new_row);
      scan_lower_bound += 12;
    } else {
      for (int i = 0; i < cell_no; i++) {
        if (cell_list[i].y_coord >= scan_lower_bound &&
            cell_list[i].y_coord <= scan_upper_bound &&
            !cell_list[i].assigned_row) {
          if (cell_list[i].height == 8 &&
              total_width + cell_list[i].width <= width) {
            cell_list[i].y_coord = scan_lower_bound;
            cell_list[i].assigned_row = true;
            total_width += cell_list[i].width;
          } else {
            cell_list[i].y_coord += 8;
          }
        }
      }
      Row new_row { 8, scan_lower_bound, total_width };
      row_list.push_back(new_row);
      scan_lower_bound += 8;
    }
    // cout << total_width << endl;
  }
  vector<Cell> unassigned_cells;
  for (int i = 0; i < cell_no; i++) {
    if (!cell_list[i].assigned_row) {
      unassigned_cells.push_back(cell_list[i]);
    }
  }
  for(int i = 0; i < unassigned_cells.size(); i++){
    for(int j = i+1; j < unassigned_cells.size(); j++){
      if(unassigned_cells[i].y_coord_original<unassigned_cells[j].y_coord_original){
        Cell tmp = move(unassigned_cells[i]);
        unassigned_cells[i] = move(unassigned_cells[j]);
        unassigned_cells[j] = move(tmp);
      }
    }
  }
  for (int i = 0; i < unassigned_cells.size(); i++) {
    for(int j = row_list.size()-1; j>=0; j--){
      if(unassigned_cells[i].height == row_list[j].height && row_list[j].total_width+unassigned_cells[i].width <=width){
        int cellIndex = stoi(unassigned_cells[i].name.erase(0,1));
        cell_list[cellIndex].y_coord = row_list[j].y_coord;
        break;
      }
    }
  }
  //output
  ofstream output_stream;
  output_stream.open(argv[2]);
  output_stream << row_list.size() << endl;
  for(int i = 0; i<row_list.size(); i++){
    output_stream << row_list[i].y_coord << " " << row_list[i].height << endl;
    for(int j = 0; j<cell_list.size(); j++){
      if(row_list[i].y_coord == cell_list[j].y_coord){
        output_stream << cell_list[j].name << " " << cell_list[j].y_coord << " " << cell_list[j].x_coord << endl;
      }
    }

  }
  output_stream.close();
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Execution time: " << duration.count()/1000 << " ms." << endl;
  return 0;
}
