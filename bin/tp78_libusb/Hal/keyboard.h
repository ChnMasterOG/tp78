#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include <iostream>
#include <unordered_map>

typedef char KEY_ERRCODE;

extern const std::unordered_map<std::string, unsigned char> KEY_hash;
extern const std::unordered_map<unsigned char, std::string> KEY_r_hash;
extern unsigned char Row;
extern unsigned char Col;
extern unsigned char LED_Style;
extern unsigned char BLE_Device;
extern std::vector<std::vector<std::string>> Custom_Layer;
extern std::vector<std::vector<std::string>> EX_Layer;

KEY_ERRCODE keyboard_read_configuration(const std::string& fp);
KEY_ERRCODE keyboard_write_configuration(const std::string& fp);

#endif
