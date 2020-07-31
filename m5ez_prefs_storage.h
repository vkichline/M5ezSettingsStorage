#ifndef __m5ez_prefs_storage_h__
#define __m5ez_prefs_storage_h__

#include <FS.h>

bool read_settings_from_file(File json_file);
bool write_settings_to_file(File json_file);
void clear_settings();

#endif // #define __m5ez_prefs_storage_h__
