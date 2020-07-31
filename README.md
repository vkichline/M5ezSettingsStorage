# M5ezSettingsStorage

This program runs on the M5Stack, and will copy all M5ez NVS persistent settings to and from a JSON file, named StoredSettings.json, on a mounted SD card.

Required Libraries:
* M5Stack
* M5ez
* ArduinoJson

Required equipment:
* Arduino IDE, computer and serial cable
* M5Stack
* Formatted SD card

Compile the program and upload it to an M5Stack that has previously used the M5ez library.
* Select "Save Settings to File" to write M5ez's NVS settings to StoredSettings.json
* Select "Restore Settings from File" to read StoredSettings.json from SD and set all NVS settings on the M5Stack
* Select "Delete Settings File from SD" to remove StoredSettings.json from the SD card.
* Select "Remove M5Stack NVS Settings" to clear the NVS memory of M5ez's settings
