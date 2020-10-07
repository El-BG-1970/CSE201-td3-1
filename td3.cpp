#include <stdlib.h>
#include <math.h>       // sin, cos
#include <iostream>
#include "td3.hpp"
#include "support.hpp"

using namespace std;

using namespace support;

double* extend_array(double* array, int length, int new_size) {
  // IMPLEMENT YOUR FUNCTION HERE

  double* new_array = new double[new_size];

  for (int i = 0; i < new_size; i++) {
    if (i < length)
      new_array[i] = array[i];
    else
      new_array[i] = 0;
  }

  delete[] array;

  return new_array;  // YOU CAN CHANGE THIS
}


double* shrink_array(double* array, int length, int new_size) {
  // IMPLEMENT YOUR FUNCTION HERE

  double* new_array = new double[new_size];

  for (int i = 0; i < new_size; i++) {
    if (i < length)
      new_array[i] = array[i];
    else
      new_array[i] = 0;
  }

  delete[] array;

  return new_array;  // YOU CAN CHANGE THIS
}


double* append_to_array(double element,
                        double* array,
                        int &current_size,
                        int &max_size) {
  // IMPLEMENT YOUR FUNCTION HERE
  if (current_size >= max_size) {
    array = extend_array(array, max_size, max_size+5);
    max_size += 5;
  }
  array[current_size] = element;
  current_size++;

  return array;  // YOU CAN CHANGE THIS
}


double* remove_from_array(double* array,
                          int &current_size,
                          int &max_size) {
  // IMPLEMENT YOUR FUNCTION HERE
  if (current_size > 0) current_size--;

  if (max_size - current_size >= 5) {
    max_size -= 5;
    if (max_size < 0) max_size = 0;
    array = shrink_array(array, current_size, max_size);
  } else
    array = shrink_array(array, current_size, max_size);

  return array;  // YOU CAN CHANGE THIS
}


bool simulate_projectile(const double magnitude, const double angle,
                         const double simulation_interval,
                         double *targets, int &tot_targets,
                         int *obstacles, int tot_obstacles,
                         double* &telemetry,
                         int &telemetry_current_size,
                         int &telemetry_max_size) {
  // YOU CAN MODIFY THIS FUNCTION TO RECORD THE TELEMETRY

  bool hit_target, hit_obstacle;
  double v0_x, v0_y, x, y, t;
  double PI = 3.14159265;
  double g = 9.8;

  v0_x = magnitude * cos(angle * PI / 180);
  v0_y = magnitude * sin(angle * PI / 180);

  t = 0;
  x = 0;
  y = 0;

  hit_target = false;
  hit_obstacle = false;
  while (y >= 0 && (!hit_target) && (!hit_obstacle)) {
    //cout << telemetry_current_size << ' ' << telemetry_max_size << endl;

    telemetry = append_to_array(t, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(x, telemetry, telemetry_current_size, telemetry_max_size);
    telemetry = append_to_array(y, telemetry, telemetry_current_size, telemetry_max_size);

    double * target_coordinates = find_collision(x, y, targets, tot_targets);
    if (target_coordinates != NULL) {
      remove_target(targets, tot_targets, target_coordinates);
      hit_target = true;
    } else if (find_collision(x, y, obstacles, tot_obstacles) != NULL) {
      hit_obstacle = true;
    } else {
      t = t + simulation_interval;
      y = v0_y * t  - 0.5 * g * t * t;
      x = v0_x * t;
    }
  }

  return hit_target;
}


void arrange(double *array, int len_arr){
  int j = len_arr / 3 - 1;
  double tmp;
  
  while ((j > 0) && (array[3 * (j - 1)] > array[3 * j])){
    tmp = array[3 * (j - 1)];
    array[3 * (j - 1)] = array[3 * j];
    array[3 * j] = tmp;

    tmp = array[3 * (j - 1) + 1];
    array[3 * (j - 1) + 1] = array[3 * j + 1];
    array[3 * j + 1] = tmp;

    tmp = array[3 * (j - 1) + 2];
    array[3 * (j - 1) + 2] = array[3 * j + 2];
    array[3 * j + 2] = tmp;

    j--;
  }
}

void merge_telemetry(double **telemetries,
                     int tot_telemetries,
                     int *telemetries_sizes,
                     double* &telemetry,
                     int &telemetry_current_size,
                     int &telemetry_max_size) {
  // IMPLEMENT YOUR FUNCTION HERE

  bool exitp = false;
  int pos = 0;  // counted in timestamps (ie size 3 double)
  while (!exitp){
    exitp = true;
    for (int i = 0; i < tot_telemetries; i++){
      if (3 * pos < telemetries_sizes[i]){
	exitp = false; // delay exit condition as array is nonempty
	
	telemetry = append_to_array(telemetries[i][3*pos], telemetry, telemetry_current_size, telemetry_max_size);
	telemetry = append_to_array(telemetries[i][3*pos+1], telemetry, telemetry_current_size, telemetry_max_size);
	telemetry = append_to_array(telemetries[i][3*pos+2], telemetry, telemetry_current_size, telemetry_max_size);
	
	arrange(telemetry, telemetry_current_size);
      }
    }

    pos++;
  }
}
