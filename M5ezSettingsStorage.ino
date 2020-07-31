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
    bool result = write_settings_to_file(json_file);
    json_file.close();
    String msg = "The save operation ";
    msg += result ? "succeeded." : "failed.";
    ez.msgBox("Save Operation", msg);
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
    bool result = read_settings_from_file(json_file);
    json_file.close();
    String msg = "The restore operation ";
    msg += result ? "succeeded." : "failed.";
    ez.msgBox("Restore Operation", msg);
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


// The ez part of the program:
//
void setup() { ez.begin(); }
void loop() {
  ezMenu menu("Settings Storage");
  menu.txtSmall();
  menu.addItem("Save Settings to File", save_settings);
  menu.addItem("Restore Settings from File", restore_settings);
  menu.addItem("Delete Settings File from SD", delete_settings);
  menu.addItem("Remove M5Stack NVS Settings", clear_settings);
  menu.run();
}
