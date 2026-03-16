#include <pebble.h>

#include "pebble-graph.h"
#include <limits.h>

#define PEBBLE_GRAPH_Y_AXIS_LABELS 5

static int pebbleGraph_margin_x = 4;
static int pebbleGraph_margin_y = 16;

static Graph* pebbleGraph_graph;
static Layer* pebbleGraph_layer;
static TextLayer* pebbleGraph_title_textLayer;
static TextLayer* pebbleGraph_yValue_textLayers[PEBBLE_GRAPH_Y_AXIS_LABELS];

static char value_buffers[PEBBLE_GRAPH_Y_AXIS_LABELS][5];

static GRect pebble_graph_get_graph_bounds(const GRect bounds) {
  GRect graph_bounds;
  graph_bounds.origin.x = bounds.origin.x + pebbleGraph_margin_x;
  graph_bounds.origin.y = bounds.origin.y + pebbleGraph_margin_y;
  graph_bounds.size.w = bounds.size.w - (graph_bounds.origin.x + 20 + pebbleGraph_margin_x); // +20 for Y-value spacing
  graph_bounds.size.h = bounds.size.h - (graph_bounds.origin.y * 1.5);
  return graph_bounds;
}

static void pebble_graph_draw(Layer *layer, GContext *ctx) {
  GRect layer_bounds = layer_get_bounds(layer);
  GRect graph_bounds = pebble_graph_get_graph_bounds(layer_bounds);

  // Draw top and bottom lines of graph
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);
  graphics_draw_line(ctx,
    GPoint(graph_bounds.origin.x, graph_bounds.origin.y),
    GPoint(graph_bounds.origin.x + graph_bounds.size.w, graph_bounds.origin.y));
  graphics_draw_line(ctx,
    GPoint(graph_bounds.origin.x, graph_bounds.origin.y + graph_bounds.size.h),
    GPoint(graph_bounds.origin.x + graph_bounds.size.w, graph_bounds.origin.y + graph_bounds.size.h));

  // Draw lines for Y-values
  graphics_context_set_stroke_color(ctx, GColorLightGray);
  graphics_context_set_fill_color(ctx, GColorLightGray);
  for (int i = 1; i < PEBBLE_GRAPH_Y_AXIS_LABELS - 1; ++i) {
    const int y_value = (graph_bounds.origin.y) + ((graph_bounds.size.h / (PEBBLE_GRAPH_Y_AXIS_LABELS - 1))* i);
    graphics_draw_line(ctx,
      GPoint(graph_bounds.origin.x, y_value),
      GPoint(graph_bounds.origin.x + graph_bounds.size.w, y_value));
  }
  graphics_context_set_stroke_color(ctx, GColorBlack);
  graphics_context_set_fill_color(ctx, GColorBlack);

  const int difference_between_max_and_min = pebbleGraph_graph->axis_max - pebbleGraph_graph->axis_min;
  GPoint previous_point = GPoint(0, 0);
  for (int i = 0; i < pebbleGraph_graph->num_values; ++i) {
    const int value = pebbleGraph_graph->values[i];
    const float y_as_percentage = (float) (pebbleGraph_graph->axis_max - value) / (float) difference_between_max_and_min;
    const float percentage = (float) i / (float) pebbleGraph_graph->num_values;
    const int x = graph_bounds.origin.x + ((percentage * (float) graph_bounds.size.w));
    const int y = graph_bounds.origin.y + (int) (y_as_percentage * (float) graph_bounds.size.h);

    // Find out what color to draw for the point
    int prev_val = INT_MIN;
    for (int j = 0; j < PEBBLE_GRAPH_MAX_COLORS; ++j) {
      if (value >= pebbleGraph_graph->colors[j].value && pebbleGraph_graph->colors[j].value > prev_val) {
        graphics_context_set_stroke_color(ctx, pebbleGraph_graph->colors[j].color);
        graphics_context_set_fill_color(ctx, pebbleGraph_graph->colors[j].color);
        prev_val = pebbleGraph_graph->colors[j].value;
      }
    }

    switch (pebbleGraph_graph->chart_type) {
    case LINE:
      graphics_context_set_stroke_width(ctx, PEBBLE_GRAPH_LINE_WIDTH);
      if (previous_point.x != 0 && previous_point.y != 0) {
        graphics_draw_line(ctx, GPoint(x, y), previous_point);
      }
      previous_point = GPoint(x, y);
      break;
    case BAR:
      graphics_context_set_stroke_width(ctx, 1);
      graphics_fill_rect(ctx, GRect(x, y, 1, (graph_bounds.size.h - y) + (pebbleGraph_margin_y)), 0,  0);
    }
  }
}

void pebble_graph_draw_graph(Graph* graph, Layer* window_layer) {
  if (graph->num_values <= 0) {
    APP_LOG(APP_LOG_LEVEL_ERROR, "ERROR: Pebble-Graph - num_values must be >= 1");
    return;
  }

  const GRect layer_bounds = layer_get_bounds(window_layer);
  const GRect graph_bounds = pebble_graph_get_graph_bounds(layer_bounds);

  pebbleGraph_graph = graph;
  pebbleGraph_layer = layer_create(layer_bounds);

  // Draw title
  pebbleGraph_title_textLayer = text_layer_create(GRect(pebbleGraph_margin_x, 0, graph_bounds.size.w, 14));
  text_layer_set_text_alignment(pebbleGraph_title_textLayer, GTextAlignmentCenter);
  layer_add_child(pebbleGraph_layer, text_layer_get_layer(pebbleGraph_title_textLayer));
  text_layer_set_font(pebbleGraph_title_textLayer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
  text_layer_set_text(pebbleGraph_title_textLayer, pebbleGraph_graph->graph_title);

  // Draw Y-values
  for (int i = 0; i < PEBBLE_GRAPH_Y_AXIS_LABELS; ++i) {
    int value = 0;
    if (i == 0) value = graph->axis_max;
    else if (i == PEBBLE_GRAPH_Y_AXIS_LABELS - 1) value = graph->axis_min;
    else value = graph->axis_max - (((graph->axis_max - graph->axis_min) / (PEBBLE_GRAPH_Y_AXIS_LABELS - 1)) * i);
    pebbleGraph_yValue_textLayers[i] = text_layer_create(GRect(layer_bounds.size.w - 20, (graph_bounds.origin.y - 10) + ((graph_bounds.size.h / (PEBBLE_GRAPH_Y_AXIS_LABELS - 1))* i), 20, 14));
    text_layer_set_text_alignment(pebbleGraph_yValue_textLayers[i], GTextAlignmentCenter);
    layer_add_child(pebbleGraph_layer, text_layer_get_layer(pebbleGraph_yValue_textLayers[i]));
    text_layer_set_font(pebbleGraph_yValue_textLayers[i], fonts_get_system_font(FONT_KEY_GOTHIC_14));
    snprintf(value_buffers[i], 5, "%d", value);
    text_layer_set_text(pebbleGraph_yValue_textLayers[i], value_buffers[i]);
  }

  layer_set_update_proc(pebbleGraph_layer, pebble_graph_draw);
  layer_add_child(window_layer, pebbleGraph_layer);
}

// graph pointer is unused here as memory is not owned by this code
// it is kept as a parameter in case we eventually need to add owned data it can be cleaned up here
void pebble_graph_destroy(Graph* graph) {
  layer_destroy(pebbleGraph_layer);
  pebbleGraph_layer = NULL;
  text_layer_destroy(pebbleGraph_title_textLayer);
  pebbleGraph_title_textLayer = NULL;

  for (int i = 0; i < 4; ++i) {
    text_layer_destroy(pebbleGraph_yValue_textLayers[i]);
    pebbleGraph_yValue_textLayers[i] = NULL;
  }
}
