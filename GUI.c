#include "3D_VIEWER.h"
setting_struct Setting;
obj_struct objStruct;
GtkBuilder *builder;
GtkBuilder *build;
int cairoScreeenshot = FAILE;

int main(int argc, char **argv) {
  GtkApplication *app;
  int status;
  app = gtk_application_new("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);
  return status;
}

double degree_to_radian(double degree) { return degree * M_PI / 180; }

static void show_dialog() {
  GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWin"));
  GtkWidget *dialog = gtk_message_dialog_new(
      GTK_WINDOW(window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR,
      GTK_BUTTONS_CLOSE, "Incorrect data entered!");
  g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
  gtk_widget_show(dialog);
}

void print_model(GtkWidget *text, gpointer *data) {
  GtkWidget *fileBox = GTK_WIDGET(gtk_builder_get_object(builder, "fileBox"));
  char *nameFile =
      (char *)gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(fileBox));
  char *fullNammeFile = calloc(100, sizeof(char));
  strcat(fullNammeFile, "./models/");
  strcat(fullNammeFile, nameFile);
  objStruct = obj_parcer(fullNammeFile);
  if (objStruct.check == SUCCESS) {
    init_default_setting();
    copy_matrix_vertex_to_convert(&objStruct);
    choise_projection_type();
    create_draw_area();
    set_information_about_model(nameFile);
  } else {
    show_dialog();
  }
  free(fullNammeFile);
}

void choise_projection_type() {
  if (Setting.projection == 0)
    transformations(&objStruct.convert, 0, 0, 0, convert_3D_to_2D_parallel, 0);
  else
    transformations(&objStruct.convert, 0, 0, 0, convert_3D_to_2D_central, 0);
}

void set_information_about_model(char *nameFile) {
  GtkWidget *textNameLabel =
      GTK_WIDGET(gtk_builder_get_object(builder, "textNameLabel"));
  gtk_entry_buffer_set_text(gtk_text_get_buffer(GTK_TEXT(textNameLabel)),
                            nameFile, strlen(nameFile));
  GtkWidget *textVerticesLabel =
      GTK_WIDGET(gtk_builder_get_object(builder, "textVerticesLabel"));
  char *verticesLabel = calloc(20, sizeof(char));
  snprintf(verticesLabel, sizeof(verticesLabel), "%d", objStruct.vertex.rows);
  gtk_entry_buffer_set_text(gtk_text_get_buffer(GTK_TEXT(textVerticesLabel)),
                            verticesLabel, strlen(verticesLabel));
  GtkWidget *textEdgesLabel =
      GTK_WIDGET(gtk_builder_get_object(builder, "textEdgesLabel"));
  char *edgesLabel = calloc(20, sizeof(char));
  snprintf(edgesLabel, sizeof(edgesLabel), "%d",
           objStruct.vertex.rows + objStruct.plane.rows - 2);
  gtk_entry_buffer_set_text(gtk_text_get_buffer(GTK_TEXT(textEdgesLabel)),
                            edgesLabel, strlen(edgesLabel));
  free(verticesLabel);
  free(edgesLabel);
}

void moving_model(GtkWidget *text, gpointer *data) {
  const char *label = gtk_button_get_label(GTK_BUTTON(data));
  char *numberMove =
      (char *)gtk_entry_buffer_get_text(gtk_text_get_buffer(GTK_TEXT(text)));
  double moving_number = strtod(numberMove, &numberMove) / 50;
  if (strcmp(label, "Y") == 0) {
    transformations(&objStruct.vertex, 0, moving_number, 0, transfer, 0);
  } else if (strcmp(label, "X") == 0) {
    transformations(&objStruct.vertex, moving_number, 0, 0, transfer, 0);
  } else if (strcmp(label, "Z") == 0) {
    transformations(&objStruct.vertex, 0, 0, moving_number, transfer, 0);
  }
  copy_matrix_vertex_to_convert(&objStruct);
  choise_projection_type();
  create_draw_area();
}

void scale_model(GtkWidget *text, gpointer *data) {
  const char *label = gtk_button_get_label(GTK_BUTTON(data));
  char *numberMove =
      (char *)gtk_entry_buffer_get_text(gtk_text_get_buffer(GTK_TEXT(text)));
  double scale = strtod(numberMove, &numberMove);
  if (scale < 0)
    scale = 1;
  else if (scale == 0 || scale == 1)
    scale = 1.1;
  if (strcmp(label, "+") == 0)
    objStruct.vertex = s21_mult_number(&objStruct.vertex, scale);
  else if (strcmp(label, "-") == 0)
    objStruct.vertex = s21_mult_number(&objStruct.vertex, 1.0 / scale);
  copy_matrix_vertex_to_convert(&objStruct);
  choise_projection_type();
  create_draw_area();
}

void rotation_model(GtkWidget *text, gpointer *data) {
  const char *label = gtk_button_get_label(GTK_BUTTON(data));
  char *numberRotation =
      (char *)gtk_entry_buffer_get_text(gtk_text_get_buffer(GTK_TEXT(text)));
  double rotationDegree = strtod(numberRotation, &numberRotation);
  double rotationRadian = degree_to_radian(rotationDegree);
  if (strcmp(label, "Y") == 0) {
    transformations(&objStruct.vertex, 0, 0, 0, rotation_around_OY,
                    rotationRadian);
  } else if (strcmp(label, "X") == 0) {
    transformations(&objStruct.vertex, 0, 0, 0, rotation_around_OX,
                    rotationRadian);
  } else if (strcmp(label, "Z") == 0) {
    transformations(&objStruct.vertex, 0, 0, 0, rotation_around_OZ,
                    rotationRadian);
  }
  copy_matrix_vertex_to_convert(&objStruct);
  choise_projection_type();
  create_draw_area();
}

void screenshot_model() {
  cairoScreeenshot = SUCCESS;
  create_draw_area();
}

void create_draw_area() {
  GtkWidget *draw_area =
      GTK_WIDGET(gtk_builder_get_object(builder, "draw_area"));
  gtk_drawing_area_set_draw_func(GTK_DRAWING_AREA(draw_area), draw_function,
                                 NULL, NULL);
}

void setting_model(GtkWidget *text, gpointer *data) {
  build = gtk_builder_new_from_file("./gui/SETTING.xml");
  GtkWidget *windowSetting =
      GTK_WIDGET(gtk_builder_get_object(build, "windowSetting"));
  init_default_setting();
  setting_active();
  gtk_widget_show(windowSetting);
}

static void draw_function(GtkDrawingArea *area, cairo_t *cr, int width,
                          int height, gpointer user_data) {
  gdouble scaleX = 50, scaleY = 50;
  cairo_surface_t *surface;
  if (cairoScreeenshot == SUCCESS) {
    cairo_save(cr);
    surface = cairo_image_surface_create(CAIRO_FORMAT_RGB24, 600, 530);
    cr = cairo_create(surface);
  }
  choise_color_background(cr);
  cairo_paint(cr);
  cairo_translate(cr, 600 / 2, 530 / 2);
  cairo_scale(cr, scaleX, -scaleY);
  choise_line_type(cr);
  cairo_set_line_width(cr, Setting.thickness_edge / 1000.0);
  choise_color_edge(cr);
  draw_edge(cr);
  cairo_stroke(cr);
  if (Setting.way_of_disp_vertices != 0 && !(isnan(Setting.thickness_vertices)))
    draw_point(cr);
  if (cairoScreeenshot == SUCCESS) {
    cairo_surface_write_to_png(surface, "screenshot.bmp");
    cairo_surface_write_to_png(surface, "screenshot.jpeg");
    cairo_surface_destroy(surface);
    cairoScreeenshot = FAILE;
  }
}

static void draw_edge(cairo_t *cr) {
  int number = 0;
  double x = 0, y = 0;
  for (int i = 0; i < objStruct.plane.rows; i++) {
    for (int j = 0; j <= objStruct.plane.columns; j++) {
      number = (int)objStruct.plane.matrix[i][j] - 1;
      if (j != objStruct.plane.columns && number != -1) {
        x = objStruct.convert.matrix[number][0];
        y = objStruct.convert.matrix[number][1];
        if (j == 0) {
          cairo_move_to(cr, x, y);
        } else {
          cairo_line_to(cr, x, y);
        }
      } else {
        number = (int)objStruct.plane.matrix[i][0] - 1;
        x = objStruct.convert.matrix[number][0];
        y = objStruct.convert.matrix[number][1];
        cairo_line_to(cr, x, y);
        break;
      }
    }
  }
}

static void draw_point(cairo_t *cr) {
  int number = 0;
  double x = 0, y = 0;
  choise_color_vertices(cr);
  cairo_set_line_width(cr, Setting.thickness_vertices / 1000.0);
  for (int i = 0; i < objStruct.plane.rows; i++) {
    for (int j = 0; j <= objStruct.plane.columns; j++) {
      number = (int)objStruct.plane.matrix[i][j] - 1;
      if (j != objStruct.plane.columns && number != -1) {
        x = objStruct.convert.matrix[number][0];
        y = objStruct.convert.matrix[number][1];
        if (Setting.way_of_disp_vertices == 2) {
          cairo_rectangle(cr, x - 0.05, y - 0.05, 0.1, 0.1);
        } else {
          cairo_arc(cr, x, y, 0.05, 0, 2 * M_PI);
          cairo_new_sub_path(cr);
        }
      } else {
        number = (int)objStruct.plane.matrix[i][0] - 1;
        x = objStruct.convert.matrix[number][0];
        y = objStruct.convert.matrix[number][1];
        if (Setting.way_of_disp_vertices == 2) {
          cairo_rectangle(cr, x - 0.05, y - 0.05, 0.1, 0.1);
        } else {
          cairo_arc(cr, x, y, 0.05, 0, 2 * M_PI);
          cairo_new_sub_path(cr);
        }
        cairo_fill(cr);
        break;
      }
    }
  }
  cairo_stroke(cr);
}

void choise_line_type(cairo_t *cr) {
  if (Setting.line != 0) {
    static const double dash[] = {0.2, 0.4};
    static int lenDash = sizeof(dash) / sizeof(dash[0]);
    cairo_set_dash(cr, dash, lenDash, 0);
  }
}

void choise_color_background(cairo_t *cr) {
  if (Setting.background_color == carbonic)
    cairo_set_source_rgba(cr, 0.05, 0.06, 0.07, 1);
  else if (Setting.background_color == wave)
    cairo_set_source_rgba(cr, 0.13, 0.19, 0.25, 1);
  else if (Setting.background_color == dracula)
    cairo_set_source_rgba(cr, 0.16, 0.16, 0.21, 1);
}

void choise_color_edge(cairo_t *cr) {
  if (Setting.color_edge == gold)
    cairo_set_source_rgba(cr, 1, 0.92, 0.8, 1);
  else if (Setting.color_edge == white)
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
  else if (Setting.color_edge == violet)
    cairo_set_source_rgba(cr, 0.4, 0.4, 0.67, 1);
}

void choise_color_vertices(cairo_t *cr) {
  if (Setting.color_vertices == 0)
    cairo_set_source_rgba(cr, 1, 0.92, 0.8, 1);
  else if (Setting.color_vertices == 1)
    cairo_set_source_rgba(cr, 1, 1, 1, 1);
  else if (Setting.color_vertices == 2)
    cairo_set_source_rgba(cr, 0.4, 0.4, 0.67, 1);
}

static void activate(GtkApplication *app, gpointer user_data) {
  builder = gtk_builder_new_from_file("./gui/GUI.xml");
  GtkWidget *window = GTK_WIDGET(gtk_builder_get_object(builder, "mainWin"));
  GtkWidget *main_box = GTK_WIDGET(gtk_builder_get_object(builder, "main_box"));
  addCss();
  gtk_application_add_window(app, GTK_WINDOW(window));
  gtk_window_present(GTK_WINDOW(window));
}

void addCss() {
  GdkDisplay *disp = gdk_display_get_default();
  GtkCssProvider *prov = gtk_css_provider_new();
  gtk_css_provider_load_from_file(prov, g_file_new_for_path("./gui/style.css"));
  gtk_style_context_add_provider_for_display(
      disp, GTK_STYLE_PROVIDER(prov), GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
}

void init_default_setting() {
  FILE *file = fopen("setting.txt", "r");
  init_setting_from_file(file);
  fclose(file);
}

void apply_new_setting() {
  FILE *file;
  init_setting_without_file();
  write_to_file(file);
  GtkWidget *windowSetting =
      GTK_WIDGET(gtk_builder_get_object(build, "windowSetting"));
  gtk_window_destroy(GTK_WINDOW(windowSetting));
}

void setting_active() {
  set_active_projection_type();
  set_active_line_type();
  set_active_edges_color();
  set_active_thickness();
  set_active_disp_way();
  set_active_vertices_color();
  set_active_background_color();
}

void set_active_projection_type() {
  if (Setting.projection == parallel_projection)
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(gtk_builder_get_object(build, "parallelProjection")),
        true);
  else if (Setting.projection == central_projection)
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(gtk_builder_get_object(build, "centralProjection")),
        true);
}

void set_active_line_type() {
  if (Setting.line == solid_line)
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(gtk_builder_get_object(build, "solidLine")), true);
  else if (Setting.line == dotted_line)
    gtk_check_button_set_active(
        GTK_CHECK_BUTTON(gtk_builder_get_object(build, "dottedLine")), true);
}

void set_active_edges_color() {
  if (Setting.color_edge == gold)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorEdgeComboBox"))),
        "Gold");
  else if (Setting.color_edge == white)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorEdgeComboBox"))),
        "White");
  else if (Setting.color_edge == violet)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorEdgeComboBox"))),
        "Violet");
}

void set_active_vertices_color() {
  if (Setting.color_vertices == gold)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorVerticesComboBox"))),
        "Gold");
  else if (Setting.color_vertices == white)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorVerticesComboBox"))),
        "White");
  else if (Setting.color_vertices == violet)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorVerticesComboBox"))),
        "Violet");
}

void set_active_background_color() {
  if (Setting.background_color == carbonic)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorBackgroundComboBox"))),
        "Carbonic");
  else if (Setting.background_color == wave)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorBackgroundComboBox"))),
        "Wave");
  else if (Setting.background_color == dracula)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "colorBackgroundComboBox"))),
        "Dracula");
}

void set_active_disp_way() {
  if (Setting.way_of_disp_vertices == nothing)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "displayingVerticesComboBox"))),
        "Nothing");
  else if (Setting.color_vertices == circle)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "displayingVerticesComboBox"))),
        "Circle");
  else if (Setting.color_vertices == square)
    gtk_combo_box_set_active_id(
        GTK_COMBO_BOX(GTK_COMBO_BOX_TEXT(
            gtk_builder_get_object(build, "displayingVerticesComboBox"))),
        "Square");
}

void set_active_thickness() {
  char thinkess[4] = "";
  snprintf(thinkess, 4, "%0.f", Setting.thickness_edge);
  GtkEntryBuffer *buf =
      GTK_ENTRY_BUFFER(gtk_builder_get_object(build, "thicknessEdgeBuf"));
  gtk_entry_buffer_set_text(buf, thinkess, 4);

  snprintf(thinkess, 4, "%0.f", Setting.thickness_vertices);
  GtkEntryBuffer *buff =
      GTK_ENTRY_BUFFER(gtk_builder_get_object(build, "thicknessVertBuf"));
  gtk_entry_buffer_set_text(buff, thinkess, 4);
}

void init_setting_from_file(FILE *file) {
  char *lineFile = NULL;
  int i;
  while ((lineFile = s21_get_line(file))) {
    if (i == 0)
      init_projection_from_file(lineFile);
    else if (i == 1)
      init_line_type_from_file(lineFile);
    else if (i == 2)
      init_color_edges_from_file(lineFile);
    else if (i == 3)
      init_thickness_edges_from_file(lineFile);
    else if (i == 4)
      init_way_of_disp_from_file(lineFile);
    else if (i == 5)
      init_color_vertices_from_file(lineFile);
    else if (i == 6)
      init_thickness_vertices_from_file(lineFile);
    else if (i == 7)
      init_background_color_from_file(lineFile);
    free(lineFile);
    i += 1;
  }
}

void init_projection_from_file(char *line) {
  if (atoi(line) == parallel_projection)
    Setting.projection = parallel_projection;
  else if (atoi(line) == central_projection)
    Setting.projection = central_projection;
}

void init_line_type_from_file(char *line) {
  if (atoi(line) == solid_line)
    Setting.line = solid_line;
  else if (atoi(line) == dotted_line)
    Setting.line = dotted_line;
}

void init_color_edges_from_file(char *line) {
  if (atoi(line) == gold)
    Setting.color_edge = gold;
  else if (atoi(line) == white)
    Setting.color_edge = white;
  else if (atoi(line) == violet)
    Setting.color_edge = violet;
}

void init_thickness_edges_from_file(char *line) {
  if (!isnan(atof(line)))
    Setting.thickness_edge = atof(line);
  else
    Setting.thickness_edge = NAN;
}

void init_way_of_disp_from_file(char *line) {
  if (atoi(line) == nothing)
    Setting.way_of_disp_vertices = nothing;
  else if (atoi(line) == circle)
    Setting.way_of_disp_vertices = circle;
  else if (atoi(line) == square)
    Setting.way_of_disp_vertices = square;
}

void init_color_vertices_from_file(char *line) {
  if (atoi(line) == gold)
    Setting.color_vertices = gold;
  else if (atoi(line) == white)
    Setting.color_vertices = white;
  else if (atoi(line) == violet)
    Setting.color_vertices = violet;
}

void init_thickness_vertices_from_file(char *line) {
  if (!isnan(atof(line)))
    Setting.thickness_vertices = atof(line);
  else
    Setting.thickness_vertices = NAN;
}

void init_background_color_from_file(char *line) {
  if (atoi(line) == carbonic)
    Setting.background_color = carbonic;
  else if (atoi(line) == wave)
    Setting.background_color = wave;
  else if (atoi(line) == dracula)
    Setting.background_color = dracula;
}

void write_to_file(FILE *file) {
  file = fopen("setting.txt", "w+");
  fprintf(file, "%d\n", Setting.projection);
  fprintf(file, "%d\n", Setting.line);
  fprintf(file, "%d\n", Setting.color_edge);
  fprintf(file, "%f\n", Setting.thickness_edge);
  fprintf(file, "%d\n", Setting.way_of_disp_vertices);
  fprintf(file, "%d\n", Setting.color_vertices);
  fprintf(file, "%f\n", Setting.thickness_vertices);
  fprintf(file, "%d", Setting.background_color);
  fclose(file);
}

void init_setting_without_file() {
  init_projection(build);
  init_line_type(build);
  init_color_edges(build);
  init_thickness_edge(build);
  init_display_way(build);
  init_color_vertices(build);
  init_thickness_vertice(build);
  init_background(build);
}

void init_color_edges(GtkBuilder *build) {
  GtkWidget *color_box_edges =
      GTK_WIDGET(gtk_builder_get_object(build, "colorEdgeComboBox"));
  char *color = (char *)gtk_combo_box_text_get_active_text(
      GTK_COMBO_BOX_TEXT(color_box_edges));
  if (strcmp(color, "Gold") == 0) {
    Setting.color_edge = gold;
  } else if (strcmp(color, "White") == 0) {
    Setting.color_edge = white;
  } else if (strcmp(color, "Violet") == 0) {
    Setting.color_edge = violet;
  }
}

void init_color_vertices(GtkBuilder *build) {
  GtkWidget *color_box_vertices =
      GTK_WIDGET(gtk_builder_get_object(build, "colorVerticesComboBox"));
  char *color = (char *)gtk_combo_box_text_get_active_text(
      GTK_COMBO_BOX_TEXT(color_box_vertices));
  if (strcmp(color, "Gold") == 0) {
    Setting.color_vertices = gold;
  } else if (strcmp(color, "White") == 0) {
    Setting.color_vertices = white;
  } else if (strcmp(color, "Violet") == 0) {
    Setting.color_vertices = violet;
  }
}

void init_projection(GtkBuilder *build) {
  GtkCheckButton *parallel =
      GTK_CHECK_BUTTON(gtk_builder_get_object(build, "parallelProjection"));
  if (gtk_check_button_get_active(parallel)) {
    Setting.projection = parallel_projection;
  } else {
    Setting.projection = central_projection;
  }
}

void init_line_type(GtkBuilder *build) {
  GtkCheckButton *solid =
      GTK_CHECK_BUTTON(gtk_builder_get_object(build, "solidLine"));
  if (gtk_check_button_get_active(solid))
    Setting.line = solid_line;
  else
    Setting.line = dotted_line;
}

void init_thickness_edge(GtkBuilder *build) {
  GtkWidget *thickness =
      GTK_WIDGET(gtk_builder_get_object(build, "edgeThicknessText"));
  const char *tmp = gtk_entry_buffer_get_text(
      GTK_ENTRY_BUFFER(gtk_text_get_buffer(GTK_TEXT(thickness))));
  double thickness_edges_value = atof(tmp);
  if (thickness_edges_value != 0)
    Setting.thickness_edge = thickness_edges_value;
  else
    Setting.thickness_edge = NAN;
}

void init_thickness_vertice(GtkBuilder *build) {
  GtkWidget *thickness =
      GTK_WIDGET(gtk_builder_get_object(build, "verticesThicknessText"));
  const char *tmp = gtk_entry_buffer_get_text(
      GTK_ENTRY_BUFFER(gtk_text_get_buffer(GTK_TEXT(thickness))));
  double thickness_vertices_value = atof(tmp);
  if (thickness_vertices_value != 0)
    Setting.thickness_vertices = thickness_vertices_value;
  else
    Setting.thickness_vertices = NAN;
}

void init_display_way(GtkBuilder *build) {
  GtkWidget *display_combo_box =
      GTK_WIDGET(gtk_builder_get_object(build, "displayingVerticesComboBox"));
  char *disp = (char *)gtk_combo_box_text_get_active_text(
      GTK_COMBO_BOX_TEXT(display_combo_box));
  if (strcmp(disp, "Nothing") == 0) {
    Setting.way_of_disp_vertices = nothing;
  } else if (strcmp(disp, "Circle") == 0) {
    Setting.way_of_disp_vertices = circle;
  } else if (strcmp(disp, "Square") == 0) {
    Setting.way_of_disp_vertices = square;
  }
}

void init_background(GtkBuilder *build) {
  GtkWidget *color_box_backgroung =
      GTK_WIDGET(gtk_builder_get_object(build, "colorBackgroundComboBox"));
  char *backgr = (char *)gtk_combo_box_text_get_active_text(
      GTK_COMBO_BOX_TEXT(color_box_backgroung));
  if (strcmp(backgr, "Сarbonic") == 0) {
    Setting.background_color = carbonic;
  } else if (strcmp(backgr, "Wave") == 0) {
    Setting.background_color = wave;
  } else if (strcmp(backgr, "Dracula") == 0) {
    Setting.background_color = dracula;
  }
}

void copy_matrix_vertex_to_convert(obj_struct *objStruct) {
  objStruct->convert.rows = objStruct->vertex.rows;
  objStruct->convert.columns = objStruct->vertex.columns;
  s21_create_matrix(&objStruct->convert);
  for (int i = 0; i < objStruct->convert.rows; i++)
    for (int j = 0; j < objStruct->convert.columns; j++)
      objStruct->convert.matrix[i][j] = objStruct->vertex.matrix[i][j];
}
