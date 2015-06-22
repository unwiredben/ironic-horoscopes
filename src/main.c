#include <pebble.h>

#define SCREEN_WIDTH (144)
#define SCREEN_HEIGHT (168)

#define ZODIAC_SIGN_KEY (0)

static Window *s_mainWindow, *s_pickerWindow;
static TextLayer *s_helpTextLayer;
static MenuLayer *s_picker;

typedef struct ZodiacSignInfo {
    const char *sign;
    const char *range;
} ZodiacSignInfo;

const ZodiacSignInfo s_signs[12] = {
    { "Aries", "Mar 21 - Apr 19" },
    { "Taurus", "Apr 20 - May 20" },
    { "Gemini", "May 21 - Jun 20" },
    { "Cancer", "Jun 21 - Jul 22" },
    { "Leo", "Jul 23 - Aug 22" },
    { "Virgo", "Aug 23 - Sep 22" },
    { "Libra", "Sep 23 - Oct 22" },
    { "Scorpio", "Oct 23 - Nov 21" },
    { "Sagittarius", "Nov 22 - Dec 21" },
    { "Capricorn", "Dec 22 - Jan 19" },
    { "Aquarius", "Jan 20 - Feb 18" },
    { "Pisces", "Feb 19 - Mar 20" },
};

// -----------------------------------------------------------------------------------------------

uint16_t pickerNumSections(struct MenuLayer *menu_layer, void *callback_context) {
    return 1;
}

uint16_t pickerNumRows(struct MenuLayer *menu_layer, uint16_t section_index, void *callback_context) {
    return 12;
}

void pickerDrawRow(GContext* ctx, const Layer *cell_layer, MenuIndex *cell_index, void *callback_context) {
    int8_t i = cell_index->row;
    menu_cell_basic_draw(ctx, cell_layer, s_signs[i].sign, s_signs[i].range, NULL);
}

void pickerSelect(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
    int8_t i = cell_index->row;
    persist_write_int(ZODIAC_SIGN_KEY, i);
    vibes_short_pulse();
    window_stack_pop(true);
}

void setup_picker(void) {
    s_pickerWindow = window_create();
    s_picker = menu_layer_create(GRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT));
    menu_layer_set_callbacks(
        s_picker,
        NULL,
        (MenuLayerCallbacks){
            .get_num_sections = pickerNumSections,
            .get_num_rows = pickerNumRows,
            .draw_row = pickerDrawRow,
            .select_click = pickerSelect
        });

    int8_t row = 0;
    if (persist_exists(ZODIAC_SIGN_KEY)) {
        row = persist_read_int(ZODIAC_SIGN_KEY);
    }
    menu_layer_set_selected_index(
        s_picker,
        (MenuIndex){ 0, row },
        MenuRowAlignCenter,
        false);
    
    Layer *root = window_get_root_layer(s_pickerWindow);
    layer_add_child(root, menu_layer_get_layer(s_picker));
    menu_layer_set_click_config_onto_window(s_picker, s_pickerWindow);
    window_stack_push(s_pickerWindow, true);
}
    
// -----------------------------------------------------------------------------------------------

void help_click_handler(ClickRecognizerRef recognizer, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "main - select");
    setup_picker();
}

void help_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, help_click_handler);
}
    
void setup_main(void) {
    s_mainWindow = window_create();
    s_helpTextLayer = text_layer_create(GRect(0, 4, SCREEN_WIDTH, SCREEN_HEIGHT - 8));
    text_layer_set_font(s_helpTextLayer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
    text_layer_set_text_alignment(s_helpTextLayer, GTextAlignmentCenter);
    text_layer_set_overflow_mode(s_helpTextLayer, GTextOverflowModeWordWrap);
    text_layer_set_text(
        s_helpTextLayer,
        "Your daily \"horoscope\" will appear on your timeline.\n\n"
        "Press select to pick your sign.");
    Layer *root = window_get_root_layer(s_mainWindow);
    layer_add_child(root, text_layer_get_layer(s_helpTextLayer));
    window_set_click_config_provider(s_mainWindow, help_click_config_provider);
    window_stack_push(s_mainWindow, true);
}

// -----------------------------------------------------------------------------------------------

int main(void) {
    setup_main();
    app_event_loop();
}
