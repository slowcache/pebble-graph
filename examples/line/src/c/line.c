#include <pebble.h>

#include "pebble-graph.h"

static Window* example_window;

static Graph example_graph;

static void exampleWindow_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);

  example_graph.chart_type = LINE;
  example_graph.graph_title = "Heartrate During Run";
  example_graph.num_values = PEBBLE_GRAPH_MAX_VALUES;
  example_graph.values[0] = 0;
  for (int i = 1; i < example_graph.num_values; ++i) {
    example_graph.values[i] = example_graph.values[i-1] + (rand() % 3);
  }
  example_graph.axis_max = PEBBLE_GRAPH_MAX_VALUES;
  example_graph.axis_min = 0;
  example_graph.colors[0].value = 0;
  example_graph.colors[0].color = GColorBlueMoon;
  example_graph.colors[1].value = 30;
  example_graph.colors[1].color = GColorGreen;
  example_graph.colors[2].value = 60;
  example_graph.colors[2].color = GColorYellow;
  example_graph.colors[3].value = 90;
  example_graph.colors[3].color = GColorOrange;
  example_graph.colors[4].value = 120;
  example_graph.colors[4].color = GColorRed;

  pebble_graph_draw_graph(&example_graph, window_layer);
}

static void simpleDataWindow_unload(Window *window) {
  window_destroy(example_window);
  pebble_graph_destroy(&example_graph);

  APP_LOG(APP_LOG_LEVEL_INFO, "Deleted simpleDataWindow");
}

void exampleWindowPushToStack() {
  APP_LOG(APP_LOG_LEVEL_INFO, "Pushing SimpleDataWindow to stack");
  example_window = window_create();
  window_set_click_config_provider(example_window, NULL);
  window_set_window_handlers(example_window, (WindowHandlers) {
    .load = exampleWindow_load,
    .unload = simpleDataWindow_unload,
  });
  const bool animated = true;
  window_stack_push(example_window, animated);
}

int main() {
  exampleWindowPushToStack();
  app_event_loop();
}

