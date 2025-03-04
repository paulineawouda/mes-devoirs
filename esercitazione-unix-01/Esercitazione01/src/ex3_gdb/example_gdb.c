#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define N 10

typedef struct {
  int buffer[N];
  int read_idx;
  int write_idx;
  int size;
} Data;


int read_buffer(Data* data, int* val);

int write_buffer(Data* data, int val);

int main() {
  Data data;
  int i, j, k, val; 
  
  data.read_idx = 0;
  data.write_idx = 0;
  data.size = 0;
  
  srand(time(NULL));
  for (i = 0; i < 1000; ++i) {
    k = rand() % 7;
    for (j = 0; j < k; ++j) {
      if (read_buffer(&data, &val)) {
        printf("reader: read %d from buffer, buffer size %d, read_idx %d\n", val, data.size, data.read_idx);
      }
      else {
        printf("reader: NOT read: buffer size %d\n", data.size);
      }
    }
    k = rand() % 7;
    for (j = 0; j < k; ++j) {
      val = (2 * i + 5 * j) % 13;
      if (write_buffer(&data, val)) {
        printf("writer: written %d to buffer, buffer size %d, write_idx %d\n", val, data.size, data.write_idx);
      }
      else {
        printf("writer: NOT written: buffer size %d\n", data.size);
      }
    }
  }
  return 0;
}


int read_buffer(Data* data, int* val) {
  if (data->size > 0) {
    *val = data->buffer[data->read_idx];
    data->read_idx = (data->read_idx + 1) % N;
    data->size--; 
    return 1;
  }
  else {
    return 0;
  }
}

int write_buffer(Data* data, int val) {
  if (data->size < N) {
    data->buffer[data->write_idx] = val;
    data->write_idx++;
    data->size++; 
    return 1;
  }
  else {
    return 0;
  }
}

