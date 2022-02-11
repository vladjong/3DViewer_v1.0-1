#include "3D_VIEWER.h"

void transformations(matrix_t *matrix, double DX, double DY, double DZ,
                     affine_transformations transform_code, double phi) {
  matrix_t point_coordinates = computing_matrix_create(1, 4);
  matrix_t transformations_matrix = computing_matrix_create(4, 4);
  if (transform_code == transfer) {
    init_transfer_matrix(&transformations_matrix, DX, DY, DZ);
  } else if (transform_code == rotation_around_OX) {
    init_rotation_matrix_OX(&transformations_matrix, phi);
  } else if (transform_code == rotation_around_OY) {
    init_rotation_matrix_OY(&transformations_matrix, phi);
  } else if (transform_code == rotation_around_OZ) {
    init_rotation_matrix_OZ(&transformations_matrix, phi);
  } else if (transform_code == convert_3D_to_2D_parallel) {
    init_convert_matrix_parallel(&transformations_matrix);
  } else {
    init_convert_matrix_central(&transformations_matrix);
  }
  for (int i = 0; i < matrix->rows; i++) {
    get_point_coordinates(matrix, &point_coordinates, i);
    matrix_t temp =
        s21_mult_matrix(&point_coordinates, &transformations_matrix);
    matrix_t mult = s21_mult_number(&temp, 1 / temp.matrix[0][3]);
    result_matrix(matrix, mult, i);
    computing_matrix_remove(&temp);
    computing_matrix_remove(&mult);
  }
  computing_matrix_remove(&point_coordinates);
  computing_matrix_remove(&transformations_matrix);
}

void init_transfer_matrix(matrix_t *transfer_matrix, double DX, double DY,
                          double DZ) {
  for (int i = 0; i < transfer_matrix->rows; i++) {
    for (int j = 0; j < transfer_matrix->columns; j++) {
      if (i == j) {
        transfer_matrix->matrix[i][j] = 1;
      } else if (i == transfer_matrix->rows - 1) {
        transfer_matrix->matrix[i][0] = DX;
        transfer_matrix->matrix[i][1] = DY;
        transfer_matrix->matrix[i][2] = DZ;
        j += 2;
      }
    }
  }
}

void init_convert_matrix_parallel(matrix_t *rot_matrix) {
  rot_matrix->matrix[0][0] = 0.707;
  rot_matrix->matrix[0][1] = -0.408;
  rot_matrix->matrix[1][1] = 0.816;
  rot_matrix->matrix[2][0] = -0.707;
  rot_matrix->matrix[2][1] = -0.408;
  rot_matrix->matrix[3][3] = 1;
}

void init_convert_matrix_central(matrix_t *rot_matrix) {
  rot_matrix->matrix[0][0] = 1;
  rot_matrix->matrix[1][1] = 1;
  rot_matrix->matrix[2][2] = 1;
  rot_matrix->matrix[1][3] = -0.02;
  rot_matrix->matrix[3][3] = 1;
}

void init_rotation_matrix_OX(matrix_t *rot_matrix, double phi) {
  rot_matrix->matrix[0][0] = 1;
  rot_matrix->matrix[1][1] = cos(phi);
  rot_matrix->matrix[1][2] = sin(phi);
  rot_matrix->matrix[2][1] = -1 * sin(phi);
  rot_matrix->matrix[2][2] = cos(phi);
  rot_matrix->matrix[3][3] = 1;
}

void init_rotation_matrix_OY(matrix_t *rot_matrix, double phi) {
  rot_matrix->matrix[0][0] = cos(phi);
  rot_matrix->matrix[0][2] = -1 * sin(phi);
  rot_matrix->matrix[1][1] = 1;
  rot_matrix->matrix[2][0] = sin(phi);
  rot_matrix->matrix[2][2] = cos(phi);
  rot_matrix->matrix[3][3] = 1;
}

void init_rotation_matrix_OZ(matrix_t *rot_matrix, double phi) {
  rot_matrix->matrix[0][0] = cos(phi);
  rot_matrix->matrix[0][1] = sin(phi);
  rot_matrix->matrix[1][0] = -1 * sin(phi);
  rot_matrix->matrix[1][1] = cos(phi);
  rot_matrix->matrix[2][2] = 1;
  rot_matrix->matrix[3][3] = 1;
}

void get_point_coordinates(matrix_t *matrix, matrix_t *point_coordinates,
                           int i) {
  for (int j = 0; j < matrix->columns; j++) {
    point_coordinates->matrix[0][j] = matrix->matrix[i][j];
  }
  point_coordinates->matrix[0][3] = 1;
}

void result_matrix(matrix_t *matrix, matrix_t temp, int i) {
  for (int j = 0; j < temp.columns - 1; j++) {
    matrix->matrix[i][j] = temp.matrix[0][j];
  }
}

matrix_t s21_mult_matrix(matrix_t *A, matrix_t *B) {
  matrix_t Result = computing_matrix_create(A->rows, B->columns);
  for (int i = 0; i < A->rows; i++) {
    for (int j = 0; j < B->columns; j++) {
      Result.matrix[i][j] = 0;
      for (int k = 0; k < B->rows; k++) {
        Result.matrix[i][j] += A->matrix[i][k] * B->matrix[k][j];
      }
    }
  }
  return Result;
}

matrix_t s21_mult_number(matrix_t *A, double number) {
  matrix_t matrixResult = computing_matrix_create(A->rows, A->columns);
  for (int i = 0; i < A->rows; i++)
    for (int j = 0; j < A->columns; j++)
      matrixResult.matrix[i][j] = A->matrix[i][j] * number;
  return matrixResult;
}

matrix_t computing_matrix_create(int rows, int columns) {
  matrix_t matrixStruct;
  if (rows >= 0 && columns >= 0) {
    matrixStruct.rows = rows;
    matrixStruct.columns = columns;
    matrixStruct.matrix = malloc(rows * sizeof(double *));
    if (matrixStruct.matrix != NULL) {
      matrixStruct.matrix[0] =
          (double *)malloc(rows * columns * sizeof(double));
      if (matrixStruct.matrix[0] != NULL) {
        for (int i = 1; i < rows; i++) {
          matrixStruct.matrix[i] = matrixStruct.matrix[0] + i * columns;
        }
        init_matrix(&matrixStruct);
      } else {
        printf("memory error 2");
      }
    } else {
      printf("memory error 1");
    }
  }
  return matrixStruct;
}

void computing_matrix_remove(matrix_t *A) {
  free(A->matrix[0]);
  free(A->matrix);
}
