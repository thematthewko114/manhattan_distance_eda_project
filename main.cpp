#include <fstream>
#include <iostream>
#include <string>
#include <cstring>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>
#include <sstream>

using namespace std;
using namespace std::chrono;

//#define k_x -0.99
//#define k_w 0.01


class Cell { // The class
public:      // Access specifier
  string name;
  float y_coord;
  float x_coord;
  float y_coord_original;
  float x_coord_original;
  //Mike
  float x_l;
  float x_r;
  int height;
  int width;

  vector<Cell> MergeCell;//initial as the cell of itself
  bool assigned_row = false;
  bool Merge = false;
};

class Row {
public:
  const int height;
  const float y_coord;
  int total_width;
  //Mike
  vector<Cell> cells_in_row;
  vector<Cell> list;
};
/*
bool cmp (const Cell& a, const Cell& b){
  string s1 = a.name.erase(a.name.begin());
  string s2 = b.name.erase(b.name.begin());

  return 0;

}
*/
int change_to_digit(const string s){
    int sum,i=0;
    while(i<s.length())
    {
        sum=0;
        while(isdigit(s[i]))
        {
          
            sum=sum*10+s[i++]-'0';
        }
       
        while(!isdigit(s[i]))i++;
    }
    return sum;

}
bool cmp( Cell a,  Cell b){
int A = change_to_digit(a.name);
int B = change_to_digit(b.name);
return A < B;
}
bool x_sort(Cell a, Cell b){
  return a.x_coord < b.x_coord;
}
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
//Sort the cell in row with x coordinate
/*
vector<Cell> quickSort(vector<Cell>& arr) {
    if (arr.size() <= 1) {
        return arr;
    }
    Cell pivot = arr[arr.size() / 2];
    vector<Cell> left, middle, right;
    for (int i = 0; i < arr.size(); i++) {
        if (arr[i].x_coord < pivot.x_coord) {
            left.push_back(arr[i]);
        } else if (arr[i].x_coord == pivot.x_coord) {
            middle.push_back(arr[i]);
        } else {
            right.push_back(arr[i]);
        }
    }
    left = quickSort(left);
    right = quickSort(right);
    vector<Cell> sortedArr;
    sortedArr.reserve(left.size() + middle.size() + right.size());
    sortedArr.insert(sortedArr.end(), left.begin(), left.end());
    sortedArr.insert(sortedArr.end(), middle.begin(), middle.end());
    sortedArr.insert(sortedArr.end(), right.begin(), right.end());
    return sortedArr;
}
*/
vector<Cell> Merge(vector<Cell>& list, int pos){
  //Updated the merge cells' x coord and width
  list[pos-1].Merge = true;//Assign the pos-1 cell is now a merge cells
  list[pos-1].x_coord = list[pos].x_coord - list[pos-1].width;
  list[pos-1].width = list[pos-1].width + list[pos].width;

  list[pos-1].x_l = list[pos-1].x_coord;
  list[pos-1].x_r = list[pos-1].x_coord+list[pos-1].width;
  for(int i = 0; i < list[pos].MergeCell.size(); i++){
  list[pos-1].MergeCell.push_back(list[pos].MergeCell[i]);//Can check how many merge cell in this block by size fn.
  }
  list.erase(list.begin() + pos);

return list;
}
//Optimal Row Placement
vector<Cell> OptRowPlace(vector<Cell>& list, int n, int width){
  //Assumen all cells place on the site
int i = 0;
while(i < n){
if(i == 0){
  list[i].x_coord = list[i].x_l; 
  i++;
}
else if(list[i-1].x_coord + list[i-1].width <= list[i].x_r){
      if(list[i-1].x_coord + list[i-1].width +  list[i].width <= width){ //No Overrange
       list[i].x_coord = max(list[i-1].x_coord + list[i-1].width , list[i].x_l);//Place to optimal place
       i++;
     }
     else{
       list = Merge(list, i);
       i--;
       n--;
   
     }    
}
else {//Merge the previous cells
  list = Merge(list, i);
  i--;
  n--;
}
}
return list;
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
                  stof(input_result[2]), stof(input_result[2]), 
                  stof(input_result[2]) + stoi(input_result[4]), 
                  stoi(input_result[3]),
                  stoi(input_result[4])};
    // cout << "Name: " << new_cell.name << ", Y: " << new_cell.y_coord<< ", X:
    // " << new_cell.x_coord << ", Height: " << new_cell.height<< ", Width: " <<
    // width << endl;
    new_cell.MergeCell.push_back(new_cell);//initial to cell's itself
    cell_list.push_back(new_cell);
  }
  cout << "Fetching cell data complete" << endl;
  input_stream.close();
  // row assignment

 /*count the number of 8T and 12T cell for each row*/
  float scan_lower_bound = 0;
  while (scan_lower_bound+8 < height) {
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

/*Determine 8T/12T dominant for each row, assign the right cell and move the recesive cell upward*/
    int total_width = 0;
    if (count12 >= count8) {
      for (int i = 0; i < cell_no; i++) {
        if (cell_list[i].y_coord >= scan_lower_bound &&
            cell_list[i].y_coord <= scan_upper_bound &&
            !cell_list[i].assigned_row) {
          if (cell_list[i].height == 12 &&
              total_width + cell_list[i].width <= width) {
            cell_list[i].y_coord = scan_lower_bound;
            cell_list[i].assigned_row = true; //Assign the right height cell for this row
            total_width += cell_list[i].width;//updated the current width of this row
          } else {
            cell_list[i].y_coord += 12;       //Move the cell to the upward
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

  //Simple sorting the unassigned array, highest coordinate cells place first
  for(int i = 0; i < unassigned_cells.size(); i++){
    for(int j = i+1; j < unassigned_cells.size(); j++){
      if(unassigned_cells[i].y_coord_original<unassigned_cells[j].y_coord_original){
        Cell tmp = move(unassigned_cells[i]);
        unassigned_cells[i] = move(unassigned_cells[j]);
        unassigned_cells[j] = move(tmp);
      }
    }
  }

//scan downward the row from the top to bottom to sign the unsigned cells
  for (int i = 0; i < unassigned_cells.size(); i++) {
    for(int j = row_list.size()-1; j>=0; j--){
      if(unassigned_cells[i].height == row_list[j].height && row_list[j].total_width+unassigned_cells[i].width <=width){
        int cellIndex = stoi(unassigned_cells[i].name.erase(0,1));
        cell_list[cellIndex].y_coord = row_list[j].y_coord;
        break;
      }
    }
  }
//////////////////////////////////////////////////////////////////////
//optimal single row ordered placement (Edited by Mike 2023/5/6)
//cout << row_list.size()<<endl;
//scan from bottom of row
int n = 0;
 for(int i = 0; i < row_list.size(); i++){
//check if the cell is in this row. Assign the cell to the corresponding rows
  for(int j = 0; j < cell_list.size(); j++){
    if(cell_list[j].y_coord == row_list[i].y_coord){
       row_list[i].cells_in_row.push_back(cell_list[j]);     
    }
  }
 }

 //Test out of region
 for(int i = 0; i < row_list.size(); i++){
  int width_sum = 0;
  for (int j = 0; j < row_list[i].cells_in_row.size(); j++){
    width_sum += row_list[i].cells_in_row[j].width;
  }
  if(width_sum > width){
    cout << "\nRow " << i <<" Out of Region! Width is: "<< width_sum << " while the constraint is "<< width <<endl;
  }
}
//cout << row_list[0].cells_in_row.size() << endl;

//Sorting based on x_coordinate, create cell list for each row for further legalization
for(int i = 0; i < row_list.size(); i++){


sort(row_list[i].cells_in_row.begin(), row_list[i].cells_in_row.end(), x_sort);

 for(int j = 0; j < row_list[i].cells_in_row.size(); j++){
 row_list[i].cells_in_row[j].x_coord = round(row_list[i].cells_in_row[j].x_coord);
 row_list[i].cells_in_row[j].x_l = round(row_list[i].cells_in_row[j].x_l);
 row_list[i].cells_in_row[j].x_r = round(row_list[i].cells_in_row[j].x_r);
 }
 row_list[i].list = row_list[i].cells_in_row;
 
 //Legalization
 //each row should be legalize with some merge cells
 
 OptRowPlace(row_list[i].list, row_list[i].list.size(), width);

int k = 0, n = 0;//assign the cells in row from pos 0
while(n < row_list[i].list.size()){//list size after merge

  if(row_list[i].list[n].Merge != true){//No merge
    row_list[i].cells_in_row[k].x_coord = row_list[i].list[n].x_coord;
    k++;
  }
  else{ // Merge == true
    int h = 0;
    while(h <  row_list[i].list[n].MergeCell.size()){
      if(h ==0){
        row_list[i].cells_in_row[k].x_coord = row_list[i].list[n].x_coord;
        k++;
      }
      else{      
        row_list[i].cells_in_row[k].x_coord = row_list[i].cells_in_row[k-1].x_coord + row_list[i].cells_in_row[k-1].width;
        k++;
      }
    h++;  
    }
  }
  n++;
}

}

 //Test priority sorting (Now, a priority based sorted cell list for each row can be obtained)
/*
 int row = 0;
 cout<<"Row height is: "<< row_list[row].height<<endl;
 cout<<"\nCells_in_row_Legalization:  "<< row <<":" << endl;
 for(int k = 0; k < row_list[row].cells_in_row.size(); k++){
  cout << row_list[row].cells_in_row[k].name<<" x coor: "<<row_list[row].cells_in_row[k].x_coord<<" width: " <<row_list[row].cells_in_row[k].width
  << " x_l: "<< row_list[row].cells_in_row[k].x_l<< " x_r: "<< row_list[row].cells_in_row[k].x_r<< endl;
 }
 */

 //Test output List
/*
 cout<<"\nList_in_row "<< row <<":" << endl;
 for(int k = 0; k < row_list[row].list.size(); k++){
  cout << row_list[row].list[k].name<<" x coor: "<<row_list[row].list[k].x_coord<<" width: " <<row_list[row].list[k].width
  << " x_l: "<< row_list[row].list[k].x_l<< " x_r: "<< row_list[row].list[k].x_r<< " Bool of Merge is "<< row_list[row].list[k].Merge
  << " Size of Merge Cells: " << row_list[row].list[k].MergeCell.size()<<endl;
 }
*/
 vector<Cell> Cell_list_for_sort;
//Assign back to the cells_list
for(int i = 0; i < row_list.size(); i++){
  for(int j = 0; j < row_list[i].cells_in_row.size(); j++){
    Cell_list_for_sort.push_back(row_list[i].cells_in_row[j]);
}
}

sort(Cell_list_for_sort.begin(),Cell_list_for_sort.end(),cmp);
cell_list = Cell_list_for_sort;
/*
cout << "\nLength of Cell_list_for_sort: "<< Cell_list_for_sort.size() << endl;
for(int k = 0; k < Cell_list_for_sort.size(); k++){
  cout << Cell_list_for_sort[k].name<<" y coor: "<<Cell_list_for_sort[k].y_coord<<" x coor: "<<Cell_list_for_sort[k].x_coord<<" width: " <<Cell_list_for_sort[k].width<<endl;
 }
cout << "\nLength of Cell_list_for_output: "<< cell_list.size() << endl;
 for(int k = 0; k < cell_list.size(); k++){
  cout << cell_list[k].name<<" y coor: "<<cell_list[k].y_coord<<" x coor: "<<cell_list[k].x_coord<<" width: " <<cell_list[k].width<<endl;
 }
*/


///////////////////////////////////////////////////////////
  //output
  ofstream output_stream;
  output_stream.open(argv[2]);
  output_stream << row_list.size() << endl;
  for(int i = 0; i<row_list.size(); i++){
    output_stream << row_list[i].y_coord << " " << row_list[i].height << endl;
  }
  for(int i = 0; i<cell_list.size(); i++){
    output_stream << cell_list[i].name << " " << cell_list[i].y_coord << " " << cell_list[i].x_coord << endl;
  }
  output_stream.close();
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  cout << "Execution time: " << duration.count()/1000 << " ms." << endl;
  return 0;
}
