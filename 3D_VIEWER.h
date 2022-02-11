#ifndef SRC_3D_VIEWER_H_
#define SRC_3D_VIEWER_H_

#include <gtk/gtk.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FAILE 0
#define SUCCESS 1

typedef struct matrix_struct {
  double **matrix;
  int rows;
  int columns;
} matrix_t;

typedef struct obj_struct {
  matrix_t vertex;
  matrix_t plane;
  matrix_t convert;
  int check;
} obj_struct;

// парсинг файла
obj_struct obj_parcer(char *nameFile);
void copy_matrix_vertex_to_convert(obj_struct *objStruct);
void open_file_obj(char *nameFile, FILE *file, matrix_t *vertex,
                   matrix_t *plane);
int amount_vertex_and_plane(char *nameFile, matrix_t *vertex, matrix_t *plane);
void count_vertex_and_plane(matrix_t *vertex, matrix_t *plane, FILE *file);
void fill_matrix(char *lineFile, matrix_t *matrix, int row);
void work_file_obg(char *nameFile, matrix_t *vertex, matrix_t *plane);
int count_plane_column(char *lineFile);
void s21_create_matrix(matrix_t *matrix);
void s21_remove_matrix(matrix_t *matrix);
void print_matrix(matrix_t *matrix);
void init_matrix(matrix_t *matrix);
void init_struct(obj_struct *objStruct);
char *s21_get_line(FILE *text);

// аффинные преобразования
typedef enum {
  transfer,
  rotation_around_OX,
  rotation_around_OY,
  rotation_around_OZ,
  convert_3D_to_2D_parallel,
  convert_3D_to_2D_central
} affine_transformations;

void transformations(matrix_t *matrix, double DX, double DY, double DZ,
                     affine_transformations transform_code, double phi);
void init_transfer_matrix(matrix_t *transfer_matrix, double DX, double DY,
                          double DZ);
void init_rotation_matrix_OX(matrix_t *rot_matrix, double phi);
void init_rotation_matrix_OY(matrix_t *rot_matrix, double phi);
void init_rotation_matrix_OZ(matrix_t *rot_matrix, double phi);
void init_convert_matrix_parallel(matrix_t *rot_matrix);
void init_convert_matrix_central(matrix_t *rot_matrix);
void get_point_coordinates(matrix_t *matrix, matrix_t *point_coordinates,
                           int i);
void result_matrix(matrix_t *matrix, matrix_t temp, int i);
matrix_t s21_mult_matrix(matrix_t *A, matrix_t *B);
matrix_t s21_mult_number(matrix_t *A, double number);
matrix_t computing_matrix_create(int rows, int columns);
void computing_matrix_remove(matrix_t *A);

// GUI
static void activate(GtkApplication *app, gpointer user_data);
static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer user_data);
void addCss();
void print_model(GtkWidget *text, gpointer *data);
void create_draw_area();
void moving_model(GtkWidget *text, gpointer *data);
void rotation_model(GtkWidget *text, gpointer *data);
double degree_to_radian(double degree);
void set_information_about_model(char *nameFile);
static void draw_point(cairo_t *cr);
static void draw_edge(cairo_t *cr);

// BONUS PART
// setting

typedef enum { parallel_projection, central_projection } projection_type;

typedef enum { solid_line, dotted_line } line_type;

typedef enum { gold, white, violet, carbonic, wave, dracula } color_type;

typedef enum { nothing, circle, square } displaying_type;

typedef struct setting_struct {
  projection_type projection;
  line_type line;
  color_type color_edge;
  double thickness_edge;
  displaying_type way_of_disp_vertices;
  color_type color_vertices;
  double thickness_vertices;
  color_type background_color;
} setting_struct;

void apply_new_setting();
void init_default_setting();

void init_setting_without_file();
void write_to_file(FILE *file);

void init_projection(GtkBuilder *build);
void init_line_type(GtkBuilder *build);
void init_color_edges(GtkBuilder *build);
void init_thickness_edge(GtkBuilder *build);
void init_display_way(GtkBuilder *build);
void init_color_vertices(GtkBuilder *build);
void init_thickness_vertice(GtkBuilder *build);
void init_background(GtkBuilder *build);

void init_projection_from_file(char *line);
void init_line_type_from_file(char *line);
void init_color_edges_from_file(char *line);
void init_thickness_edges_from_file(char *line);
void init_way_of_disp_from_file(char *line);
void init_color_vertices_from_file(char *line);
void init_thickness_vertices_from_file(char *line);
void init_background_color_from_file(char *line);
void init_setting_from_file(FILE *file);

void choise_line_type(cairo_t *cr);
void choise_color_edge(cairo_t *cr);
void choise_color_vertices(cairo_t *cr);
void choise_color_background(cairo_t *cr);
void choise_projection_type();

void setting_active();
void set_active_projection_type();
void set_active_line_type();
void set_active_edges_color();
void set_active_vertices_color();
void set_active_background_color();
void set_active_disp_way();
void set_active_thickness();

void screenshot_model();

#endif  // SRC_3D_VIEWER_H_
