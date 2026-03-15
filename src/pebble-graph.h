#pragma once

#include <pebble.h>

#define PEBBLE_GRAPH_MAX_VALUES (PBL_DISPLAY_WIDTH - 8)
#define PEBBLE_GRAPH_MAX_COLORS 5
#define PEBBLE_GRAPH_LINE_WIDTH 2 // Effects line graph drawing

typedef enum GraphType {
    LINE,
    BAR
} GraphType;

typedef struct DataColor {
    int value;
    GColor8 color;
} DataColor;

typedef struct Graph {
    char* graph_title;
    int values[PEBBLE_GRAPH_MAX_VALUES];
    int num_values;
    int axis_min;
    int axis_max;
    DataColor colors[PEBBLE_GRAPH_MAX_COLORS]; // must be in order: [0].value < [1].value
    GraphType chart_type;
} Graph;

void pebble_graph_draw_graph(Graph* graph, Layer* window_layer);
void pebble_graph_destroy(Graph* graph);
