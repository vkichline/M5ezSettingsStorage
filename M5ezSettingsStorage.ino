#include <M5ez.h>
#include "m5ez_prefs_storage.h"


#define SETTINGS_FILE_NAME  "/StoredSettings.json"


// Test to see if the SD card is found, and notify user if it isn't.
// This isn't necessary, but allows us to give better error messages.
//
bool test_sd_card_mounted()  {
  File root = SD.open("/");
  if(root) {
    root.close();
    return true;
  }
  else {
    ez.msgBox("SD Error", "SD card not found.");
    return false;
  }
}


// Test to see if the preferences file exists, and notify user if it isn't.
//
bool test_prefs_file_found() {
  if(!test_sd_card_mounted()) return false;
  bool found = SD.exists(SETTINGS_FILE_NAME);
  if(!found) ez.msgBox("File Error", "Error: Cannot find the file '" + String(SETTINGS_FILE_NAME) + "' on the SD card.");
  return found;
}


// Read all settngs and display them in a menu
//
void display_nvs_settings() {
  M5Settings  settings;
  if(!fetch_settings_from_nvs(&settings)) {
    ez.msgBox("Error", "Faild to load M5ez NVS settings.");
    return;
  }
  ezMenu menu("Current M5ez Settings");
  menu.txtSmall();
  menu.buttons("up # back # down");
  menu.addItem(String("autoconnect_on\t ") + (settings.autoconnect_on  ? "true" : "false"));
  for (uint8_t n = 0; n < settings.networks.size(); n++) {
    menu.addItem(String("SSID") + n + "\t" + settings.networks[n].SSID);
    menu.addItem(String("key")  + n + "\t" + settings.networks[n].password);
  }
  menu.addItem(String("battery_icon_on\t") + (settings.battery_icon_on ? "true" : "false"));
  menu.addItem(String("clock_on\t")        + (settings.clock_on        ? "true" : "false"));
  menu.addItem(String("timezone\t")        + settings.timezone);
  menu.addItem(String("clock12\t")         + (settings.clock12         ? "true" : "false"));
  menu.addItem(String("ampm\t")            + (settings.ampm            ? "true" : "false"));
  menu.addItem(String("brightness\t")      + settings.brightness);
  menu.addItem(String("inactivity\t")      + settings.inactivity);
  menu.addItem(String("faces_on\t")        + (settings.faces_on        ? "true" : "false"));
  menu.addItem(String("theme\t")           + settings.theme);
  menu.run();
}


// Determine if the SD card is mounted, and tell the user if it isn't.
// Then determine if the settings file indeed exists, and tell the user if it doesn't.
// Finally, write the M5ez settings to the JSON file.
//
void save_settings() {
  if(test_sd_card_mounted()) {
    File json_file = SD.open(SETTINGS_FILE_NAME, "w");
    if(!json_file) {
      ez.msgBox("File Operation", "Error: Failed to open file '" + String(SETTINGS_FILE_NAME) + "'.");
      return;
    }
    bool result = save_settings_to_file(json_file);
    json_file.close();
    ez.msgBox("Save Operation", String("The save operation ") + (result ? "succeeded." : "failed."));
  }
}


// Determine if the SD card is mounted, and tell the user if it isn't.
// Then determine if the settings file indeed exists, and tell the user if it doesn't.
// Finally, load the settings from JSON and write them to M5ez's settings.
//
void restore_settings() {
  if(test_prefs_file_found()) {
    File json_file = SD.open(SETTINGS_FILE_NAME, "r");
    if(!json_file) {
      ez.msgBox("File Operation", "Error: Failed to open file '" + String(SETTINGS_FILE_NAME) + "'.");
      return;
    }
    bool result = load_settings_from_file(json_file);
    json_file.close();
    ez.msgBox("Restore Operation", String("The restore operation ") + (result ? "succeeded." : "failed."));
  }
}


// Load and display the stored settings file if present.
//
void view_file() {
  if(test_prefs_file_found()) {
    File json_file = SD.open(SETTINGS_FILE_NAME, "r");
    if(!json_file) {
      ez.msgBox("File Operation", "Error: Failed to open file '" + String(SETTINGS_FILE_NAME) + "'.");
      return;
    }
    unsigned long fsize = json_file.size();
    uint8_t* buffer = (uint8_t*)malloc(fsize + 1);
    json_file.read(buffer, fsize);
    buffer[fsize] = '\0';
    json_file.close();
    ez.textBox(SETTINGS_FILE_NAME, (char*)buffer, true);
    free(buffer);
  }
}


// Determine if the SD card is mounted, and tell the user if it isn't.
// Then determine if the settings file indeed exists, and tell the user if it doesn't.
// Finally, delete the file and tell the user if the operation was successful or not.
//
void delete_settings() {
  if(test_prefs_file_found()) {
    if(SD.remove(SETTINGS_FILE_NAME))
      ez.msgBox("File Operation", "File '" + String(SETTINGS_FILE_NAME) + "' successfully deleted.");
    else
      ez.msgBox("File Operation", "Error: Failed to delete file '" + String(SETTINGS_FILE_NAME) + "'.");
  }
}


// Verify that the user really wants to clar the M5ez namespace, then delete all settings.
//
void clear_settings() {
  String result = ez.msgBox("Warning", "This will delete all M5ez presistent settings from NVS. Be sure to Save M5ez Settings to File first.", "Delete ## Cancel");
  if(result == "Delete") {
    bool deleted = clear_m5ez_nvs_settings();
    ez.msgBox("NVS Operation", String("Clearing M5ez settings from NVS ") + (deleted ? "succeeded." : "failed."));
  }
}


// The ez part of the program:
//
void setup() { ez.begin(); }
void loop() {
  ezMenu menu("Settings Storage");
  menu.txtSmall();
  menu.addItem("Display M5ez Settings", display_nvs_settings);
  menu.addItem("Save M5ez Settings to File", save_settings);
  menu.addItem("Restore M5ez Settings from File", restore_settings);
  menu.addItem("View Settings File", view_file);
  menu.addItem("Delete Settings File from SD", delete_settings);
  menu.addItem("Delete M5ez NVS Settings", clear_settings);
  menu.run();
}
