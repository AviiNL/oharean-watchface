#include <pebble.h>

#include "oharean.h"

static Window *window;
static TextLayer *time_layer;
static TextLayer *date_layer;
static TextLayer *charge_layer;

bool showNormalTime = false;
int tz = 1;

static void handle_battery_state(BatteryChargeState charge)
{
    static char charge_text[5];

    snprintf(charge_text, sizeof(charge_text), "%d%%", charge.charge_percent);

    text_layer_set_text(charge_layer, charge_text);
}

static void accel_tap_handler(AccelAxisType axis, int32_t direction)
{
    showNormalTime = !showNormalTime;
}

static void handle_second_tick(struct tm *tick_time, TimeUnits units_changed)
{
    static char time_text[24];
    static char date_text[24];

    if (showNormalTime)
    {
        text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
        strftime(time_text, sizeof(time_text), "%H:%M:%S", tick_time);
        strftime(date_text, sizeof(date_text), "%d/%m/%y", tick_time);
    }
    else
    {
        oharean_set_timezone(tz);
        double now = ohare_time();
        struct oharean_tm otm = oharean_make_date(now);
        text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
        snprintf(time_text, sizeof(time_text), "%i.%i.%i", otm.hour, otm.minute, otm.second);
        snprintf(date_text, sizeof(date_text), "%i %s %i:%i", otm.year, oharean_season(otm.season), otm.week, otm.day);
    }

    text_layer_set_text(time_layer, time_text);
    text_layer_set_text(date_layer, date_text);
}

static void init_clock(Window *window)
{
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    time_layer = text_layer_create(GRect(0, (bounds.size.h / 2) - 30, bounds.size.w, bounds.size.h));
    text_layer_set_text_alignment(time_layer, GTextAlignmentCenter);
    text_layer_set_text_color(time_layer, GColorWhite);
    text_layer_set_background_color(time_layer, GColorClear);
    text_layer_set_font(time_layer, fonts_get_system_font(FONT_KEY_LECO_28_LIGHT_NUMBERS));

    date_layer = text_layer_create(GRect(0, (bounds.size.h / 2), bounds.size.w, bounds.size.h));
    text_layer_set_text_alignment(date_layer, GTextAlignmentCenter);
    text_layer_set_text_color(date_layer, GColorWhite);
    text_layer_set_background_color(date_layer, GColorClear);
    text_layer_set_font(date_layer, fonts_get_system_font(FONT_KEY_GOTHIC_28));

    charge_layer = text_layer_create(GRect(0, (bounds.size.h - 17), bounds.size.w - 2, bounds.size.h));
    text_layer_set_text_alignment(charge_layer, GTextAlignmentRight);
    text_layer_set_text_color(charge_layer, GColorWhite);
    text_layer_set_background_color(charge_layer, GColorClear);
    text_layer_set_font(charge_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));

    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    handle_second_tick(current_time, SECOND_UNIT);
    tick_timer_service_subscribe(SECOND_UNIT, &handle_second_tick);

    handle_battery_state(battery_state_service_peek());
    battery_state_service_subscribe(&handle_battery_state);

    accel_tap_service_subscribe(accel_tap_handler);

    layer_add_child(window_get_root_layer(window), text_layer_get_layer(time_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(date_layer));
    layer_add_child(window_get_root_layer(window), text_layer_get_layer(charge_layer));
}

static void window_load(Window *window)
{
    init_clock(window);
}

static void window_unload(Window *window)
{
    text_layer_destroy(time_layer);
    text_layer_destroy(date_layer);
    text_layer_destroy(charge_layer);
}

static void inbox_received_handler(DictionaryIterator *iter, void *context)
{
    Tuple *timezone = dict_find(iter, 0);
    if (timezone)
    {
        tz = timezone->value->int32;
    }
}

static void init(void)
{
    app_message_register_inbox_received(inbox_received_handler);

    window = window_create();
    window_set_window_handlers(
        window,
        (WindowHandlers){
            .load = window_load,
            .unload = window_unload,
        });

    app_message_open(128, 128);

    window_stack_push(window, false);
    window_set_background_color(window, GColorBlack);
}

static void deinit(void)
{
    window_destroy(window);
}

int main(void)
{
    init();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

    app_event_loop();
    deinit();
}
