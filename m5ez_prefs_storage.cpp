// This file handles reading and writing the M5ez NVS settings to and from JSON
// M5ez Prefs currently include (as of 2.1.2)
// Setting Name    Type    Category   Default
// --------------- ------- ---------- --------
// autoconnect_on  bool    WiFi       true
// SSID1 .. SSIDn  String  WiFi       Optional
// key1  .. keyn   String  WiFi       Optional
// battery_icon_on bool    Battery    false
// clock_on        bool    Clock      true
// timezone        String  Clock      GeoIP
// clock12         bool    Clock      false
// ampm            bool    Clock      false
// brightness      uint8_t Backlight  128
// inactivity      uint8_t Backlight  0 (NEVER)
// faces_on        bool    FACES      false
// theme           String  Theme      Optional

#include "m5ez_prefs_storage.h"
#include <ArduinoJson.h>
#include <Preferences.h>

#define M5EZ_NAMESPACE  "M5ez"

const String ssid("SSID");
const String key("key");


// Read all the settings from NVS and return them in a structure.
// Return true for success, false for failure.
//
bool fetch_settings_from_nvs(M5Settings* settings) {
  Preferences prefs;
  String      str;
  uint8_t     index = 1;

  if(nullptr == settings) return false;
  prefs.begin(M5EZ_NAMESPACE, true); // read-only

  // Fetch WiFi settings
  settings->autoconnect_on = prefs.getBool("autoconnect_on", true);
  while(true) {
    String sindex = String(index++);
    String ssidn  = ssid + sindex;
    String keyn   = key  + sindex;
    str           = prefs.getString(ssidn.c_str(), "");
    if(0 == str.length()) break;  // We've run out of WiFi networks
    SsidPswd pair;
    pair.SSID     = str;
    pair.password = prefs.getString(keyn.c_str(),  "");
    settings->networks.push_back(pair);
  }

  // Fetch battery settings
  settings->battery_icon_on = prefs.getBool("battery_icon_on", false);

  // Fetch the clock settings
  settings->clock_on = prefs.getBool("clock_on", true);
  settings->timezone = prefs.getString("timezone", "GeoIP");
  settings->clock12  = prefs.getBool("clock12", false);
  settings->ampm     =   prefs.getBool("ampm", false);

  // Fetch the Backlight settings
  settings->brightness  = prefs.getUChar("brightness", 128);
  settings->inactivity  = prefs.getUChar("inactivity", 0);

  // Fetch the FACES setting
  settings->faces_on = prefs.getBool("faces_on", false);

  // Save the theme setting
  settings->theme = prefs.getString("theme", "");

  prefs.end();
  return true;
}


// Using ArduinoJSON, read all the settings from the JSON file provided and replace the NVS settings with them.
// Return true for success, false for failure.
//
bool load_settings_from_file(File json_file) {
  Preferences prefs;
  uint8_t     index = 1;
  const int   capacity = JSON_OBJECT_SIZE(74) + 1024; // Room for 32 networks, 1KB of strings
  StaticJsonDocument<capacity> doc;

  DeserializationError err = deserializeJson(doc, json_file);
  if (err) {
    Serial.print("deserializeJson() returned ");
    Serial.println(err.c_str());
    return false;
  }
  prefs.begin(M5EZ_NAMESPACE, false); // read-write

  // erase all existing WiFi SSIDs and keys
  while (true) {
    String ssidn = ssid + index;
    if(!prefs.remove(ssidn.c_str())) break;
    String keyn = key + index;
    prefs.remove(keyn.c_str());
    index++;
  }
  index = 1;

  // write all values from the JSON file to NVS
  while(true) {
    String network = doc[(ssid + index).c_str()].as<String>();
    if(nullptr == network || 0 == network.length() || 0 == network.compareTo("null")) break;
    String password = doc[(key + index).c_str()];
    prefs.putString((ssid + index).c_str(), network.c_str());
    prefs.putString((key  + index).c_str(), password.c_str());
    index++;
  }
  prefs.putBool("autoconnect_on",   doc["autoconnect_on"]  | true);
  prefs.putBool("battery_icon_on",  doc["battery_icon_on"] | false);
  prefs.putBool("clock_on",         doc["clock_on"]        | true);
  prefs.putString("timezone",       doc["timezone"]        | "GeoIP");
  prefs.putBool("clock12",          doc["clock12"]         | false);
  prefs.putUChar("brightness",      doc["brightness"]      | 128);
  prefs.putUChar("inactivity",      doc["inactivity"]      | 0);
  prefs.putBool("faces_on",         doc["faces_on"]        | false);
  prefs.putString("theme",          doc["theme"].as<String>());
  prefs.end();
  return true;
}


// Write all the settings from M5ez's NVS NameSpace to the JSON file provided.
// Return true for success, false for failure.
//
bool save_settings_to_file(File json_file) {
  Preferences prefs;
  String      str;
  uint8_t     index = 1;

  if(0 == json_file.print("{\n")) return false;  // If writing fails, return an error
  prefs.begin(M5EZ_NAMESPACE, true); // read-only

  // Save WiFi settings
  json_file.printf("  \"autoconnect_on\": %s,\n", prefs.getBool("autoconnect_on", true) ? "true" : "false");
  while(true) {
    String sindex = String(index++);
    String ssidn  = ssid + sindex;
    String keyn   = key  + sindex;
    str           = prefs.getString(ssidn.c_str(), "");
    if(0 == str.length()) break;  // We've run out of WiFi networks
    json_file.printf("  \"%s\": \"%s\",\n", ssidn.c_str(), str.c_str());
    json_file.printf("  \"%s\": \"%s\",\n", keyn.c_str(),  prefs.getString(keyn.c_str(),  "").c_str());
  }

  // Save battery settings
  json_file.printf("  \"battery_icon_on\": %s,\n", prefs.getBool("battery_icon_on", false) ? "true" : "false");

  // Save the clock settings
  json_file.printf("  \"clock_on\": %s,\n",     prefs.getBool("clock_on", true) ? "true" : "false");
  json_file.printf("  \"timezone\": \"%s\",\n", prefs.getString("timezone", "GeoIP").c_str());
  json_file.printf("  \"clock12\": %s,\n",      prefs.getBool("clock12", false) ? "true" : "false");
  json_file.printf("  \"ampm\": %s,\n",         prefs.getBool("ampm", false) ? "true" : "false");

  // Save the Backlight settings
  json_file.printf("  \"brightness\": %d,\n", prefs.getUChar("brightness", 128));
  json_file.printf("  \"inactivity\": %d,\n", prefs.getUChar("inactivity", 0));

  // Save the FACES setting
  json_file.printf("  \"faces_on\": %s,\n", prefs.getBool("faces_on", false) ? "true" : "false");

  // Save the theme setting
  json_file.printf("  \"theme\": \"%s\"\n", prefs.getString("theme", "").c_str());

  prefs.end();
  json_file.print("}\n");
  return true;
}


// Clear all the M5ez-specific settings from NVS memeory on the M5Stack.
//
bool clear_m5ez_nvs_settings() {
  bool result;
  Preferences prefs;
  result = prefs.begin(M5EZ_NAMESPACE, false); // read-write
  prefs.clear();
  prefs.end();
  return result;
}
