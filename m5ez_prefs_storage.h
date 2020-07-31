#ifndef __m5ez_prefs_storage_h__
#define __m5ez_prefs_storage_h__

#include <FS.h>

struct SsidPswd {
  String SSID;
  String password;
};

struct M5Settings {
  std::vector<SsidPswd> networks;
  bool                  autoconnect_on;
  bool                  battery_icon_on;
  bool                  clock_on;
  String                timezone;
  bool                  clock12;
  bool                  ampm;
  uint8_t               brightness;
  uint8_t               inactivity;
  bool                  faces_on;
  String                theme;
};

bool fetch_settings_from_nvs(M5Settings* settings);
bool load_settings_from_file(File json_file);
bool save_settings_to_file(File json_file);
bool clear_m5ez_nvs_settings();

#endif // #define __m5ez_prefs_storage_h__
