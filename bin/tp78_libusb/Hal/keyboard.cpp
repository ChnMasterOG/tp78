
#include "../Hal/keyboard.h"
#include <fstream>
#include <string>

using namespace std;

const unordered_map<string, unsigned char> KEY_hash{

	{ "KEY_None",   0x00 },
	{ "KEY_Fn",		0xFF },

	{ "KEY_A",		0x04 },
	{ "KEY_B",		0x05 },
	{ "KEY_C",		0x06 },
	{ "KEY_D",		0x07 },
	{ "KEY_E",		0x08 },
	{ "KEY_F",		0x09 },
	{ "KEY_G",		0x0A },
	{ "KEY_H",		0x0B },
	{ "KEY_I",		0x0C },
	{ "KEY_J",		0x0D },
	{ "KEY_K",		0x0E },
	{ "KEY_L",		0x0F },
	{ "KEY_M",		0x10 },
	{ "KEY_N",		0x11 },
	{ "KEY_O",		0x12 },
	{ "KEY_P",		0x13 },
	{ "KEY_Q",		0x14 },
	{ "KEY_R",		0x15 },
	{ "KEY_S",		0x16 },
	{ "KEY_T",		0x17 },
	{ "KEY_U",		0x18 },
	{ "KEY_V",		0x19 },
	{ "KEY_W",		0x1A },
	{ "KEY_X",		0x1B },
	{ "KEY_Y",		0x1C },
	{ "KEY_Z",		0x1D },
	{ "KEY_1",		0x1E },    //!
	{ "KEY_2",		0x1F },    //@
	{ "KEY_3",		0x20 },    //#
	{ "KEY_4",		0x21 },    //$
	{ "KEY_5",		0x22 },    //%
	{ "KEY_6",		0x23 },    //^
	{ "KEY_7",		0x24 },    //&
	{ "KEY_8",		0x25 },    //*
	{ "KEY_9",		0x26 },    //(
	{ "KEY_0",		0x27 },    //)

	//others
	{ "KEY_ENTER",			0x28 },
	{ "KEY_ESCAPE",			0x29 },
	{ "KEY_BACKSPACE",		0x2A },
	{ "KEY_TAB",			0x2B },
	{ "KEY_SPACEBAR",		0x2C },
	{ "KEY_Subtraction",	0x2D },    //- or _
	{ "KEY_Equal",			0x2E },    //= or +
	{ "KEY_LSbrackets",		0x2F },    //[ or {
	{ "KEY_RSbrackets",		0x30 },    //] or }
	{ "KEY_Backslash",		0x31 },
	{ "KEY_NonUS_WS",		0x32 },    //\ or |
	{ "KEY_Semicolon",		0x33 },    //; or :
	{ "KEY_Quotation",		0x34 },    //' or "
	{ "KEY_GraveAccent",	0x35 },    //` or ~
	{ "KEY_Comma",			0x36 },    //, or <
	{ "KEY_FullStop",		0x37 },    //. or >
	{ "KEY_Slash",			0x38 },    /// or ?
	{ "KEY_CapsLock",		0x39 },
	{ "KEY_F1",				0x3A },
	{ "KEY_F2",				0x3B },
	{ "KEY_F3",				0x3C },
	{ "KEY_F4",				0x3D },
	{ "KEY_F5",				0x3E },
	{ "KEY_F6",				0x3F },
	{ "KEY_F7",				0x40 },
	{ "KEY_F8",				0x41 },
	{ "KEY_F9",				0x42 },
	{ "KEY_F10",			0x43 },
	{ "KEY_F11",			0x44 },
	{ "KEY_F12",			0x45 },

	{ "KEY_PrintScreen",	0x46 },
	{ "KEY_ScrollLock",		0x47 },
	{ "KEY_Pause",			0x48 },
	{ "KEY_Insert",			0x49 },
	{ "KEY_Home",			0x4A },
	{ "KEY_PageUp",			0x4B },
	{ "KEY_Delete",			0x4C },
	{ "KEY_End",			0x4D },
	{ "KEY_PageDown",		0x4E },
	{ "KEY_RightArrow",		0x4F },    //→
	{ "KEY_LeftArrow",		0x50 },    //←
	{ "KEY_DownArrow",		0x51 },    //↓
	{ "KEY_UpArrow",		0x52 },    //↑

	//特殊
	{ "KEY_Undo",			0x7A },
	{ "KEY_Cut",			0x7B },
	{ "KEY_Copy",			0x7C },
	{ "KEY_Paste",			0x7D },

	//功能键
	{ "KEY_LeftCTRL",		0xE0 },
	{ "KEY_LeftShift",		0xE1 },
	{ "KEY_LeftAlt",		0xE2 },
	{ "KEY_LeftGUI",		0xE3 },
	{ "KEY_RightCTRL",		0xE4 },
	{ "KEY_RightShift",		0xE5 },
	{ "KEY_RightAlt",		0xE6 },
	{ "KEY_RightGUI",		0xE7 },

	//矮轴
	{ "KEY_MouseL",			0xF0 },
	{ "KEY_MouseR",			0xF1 },
	{ "KEY_MouseM",			0xF2 },

};

const unordered_map<unsigned char, string> KEY_r_hash{

	{ 0x00, "KEY_None"	},
	{ 0xFF,	"KEY_Fn"	},

	{ 0x04,	"KEY_A"		},
	{ 0x05,	"KEY_B"		},
	{ 0x06,	"KEY_C"		},
	{ 0x07, "KEY_D"		},
	{ 0x08,	"KEY_E"		},
	{ 0x09,	"KEY_F"		},
	{ 0x0A,	"KEY_G"		},
	{ 0x0B,	"KEY_H"		},
	{ 0x0C,	"KEY_I"		},
	{ 0x0D,	"KEY_J"		},
	{ 0x0E,	"KEY_K"		},
	{ 0x0F,	"KEY_L"		},
	{ 0x10,	"KEY_M"		},
	{ 0x11,	"KEY_N"		},
	{ 0x12,	"KEY_O"		},
	{ 0x13,	"KEY_P"		},
	{ 0x14,	"KEY_Q"		},
	{ 0x15,	"KEY_R"		},
	{ 0x16,	"KEY_S"		},
	{ 0x17,	"KEY_T"		},
	{ 0x18,	"KEY_U"		},
	{ 0x19,	"KEY_V"		},
	{ 0x1A,	"KEY_W"		},
	{ 0x1B,	"KEY_X"		},
	{ 0x1C,	"KEY_Y"		},
	{ 0x1D,	"KEY_Z"		},
	{ 0x1E,	"KEY_1"		},    //!
	{ 0x1F,	"KEY_2"		},    //@
	{ 0x20,	"KEY_3"		},    //#
	{ 0x21,	"KEY_4"		},    //$
	{ 0x22,	"KEY_5"		},    //%
	{ 0x23,	"KEY_6"		},    //^
	{ 0x24,	"KEY_7"		},    //&
	{ 0x25,	"KEY_8"		},    //*
	{ 0x26,	"KEY_9"		},    //(
	{ 0x27,	"KEY_0"		},    //)

	//others
	{ 0x28,	"KEY_ENTER"			},
	{ 0x29,	"KEY_ESCAPE"		},
	{ 0x2A,	"KEY_BACKSPACE"		},
	{ 0x2B,	"KEY_TAB"			},
	{ 0x2C,	"KEY_SPACEBAR"		},
	{ 0x2D,	"KEY_Subtraction"	},    //- or _
	{ 0x2E,	"KEY_Equal"			},    //= or +
	{ 0x2F,	"KEY_LSbrackets"	},    //[ or {
	{ 0x30,	"KEY_RSbrackets"	},    //] or }
	{ 0x31,	"KEY_Backslash"		},
	{ 0x32,	"KEY_NonUS_WS"		},    //\ or |
	{ 0x33,	"KEY_Semicolon"		},    //; or :
	{ 0x34,	"KEY_Quotation"		},    //' or "
	{ 0x35,	"KEY_GraveAccent"	},    //` or ~
	{ 0x36,	"KEY_Comma"			},    //, or <
	{ 0x37,	"KEY_FullStop"		},    //. or >
	{ 0x38,	"KEY_Slash"			},    /// or ?
	{ 0x39,	"KEY_CapsLock"		},
	{ 0x3A,	"KEY_F1"			},
	{ 0x3B,	"KEY_F2"			},
	{ 0x3C,	"KEY_F3"			},
	{ 0x3D,	"KEY_F4"			},
	{ 0x3E,	"KEY_F5"			},
	{ 0x3F,	"KEY_F6"			},
	{ 0x40,	"KEY_F7"			},
	{ 0x41,	"KEY_F8"			},
	{ 0x42, "KEY_F9"			},
	{ 0x43,	"KEY_F10"			},
	{ 0x44,	"KEY_F11"			},
	{ 0x45,	"KEY_F12"			},

	{ 0x46,	"KEY_PrintScreen"	},
	{ 0x47,	"KEY_ScrollLock"	},
	{ 0x48,	"KEY_Pause"			},
	{ 0x49,	"KEY_Insert"		},
	{ 0x4A,	"KEY_Home"			},
	{ 0x4B,	"KEY_PageUp"		},
	{ 0x4C,	"KEY_Delete"		},
	{ 0x4D,	"KEY_End"			},
	{ 0x4E,	"KEY_PageDown"		},
	{ 0x4F,	"KEY_RightArrow"	},    //→
	{ 0x50,	"KEY_LeftArrow"		},    //←
	{ 0x51,	"KEY_DownArrow"		},    //↓
	{ 0x52,	"KEY_UpArrow"		},    //↑

	//特殊
	{ 0x7A,	"KEY_Undo"			},
	{ 0x7B,	"KEY_Cut"			},
	{ 0x7C,	"KEY_Copy"			},
	{ 0x7D,	"KEY_Paste"			},

	//功能键
	{ 0xE0,	"KEY_LeftCTRL"		},
	{ 0xE1,	"KEY_LeftShift"		},
	{ 0xE2,	"KEY_LeftAlt"		},
	{ 0xE3,	"KEY_LeftGUI"		},
	{ 0xE4,	"KEY_RightCTRL"		},
	{ 0xE5,	"KEY_RightShift"	},
	{ 0xE6,	"KEY_RightAlt"		},
	{ 0xE7,	"KEY_RightGUI"		},

	//矮轴
	{ 0xF0,	"KEY_MouseL"		},
	{ 0xF1, "KEY_MouseR"		},
	{ 0xF2,	"KEY_MouseM"		},

};

unsigned char Row = 6;
unsigned char Col = 14;
unsigned char LED_Style = 0;
unsigned char BLE_Device = 0;
#define PRECNT		6
vector<vector<string>> Custom_Layer;
vector<vector<string>> EX_Layer;

KEY_ERRCODE keyboard_read_configuration(const string & fp)
{
	ifstream infile;
	string c;
	int cnt = 0;

	Custom_Layer.clear();
	EX_Layer.clear();

	infile.open(fp);

	while (1) {
		infile >> c;
		if (infile.eof()) break;
		++cnt;
		if (cnt == 1 && strcmp(c.c_str(), "TP78Configuration") != 0) {
			infile.close();
			return -1;
		}
		else if (memcmp(c.c_str(), "Row", 3) == 0) Row = stoi(&c.c_str()[3]);
		else if (memcmp(c.c_str(), "Col", 3) == 0) Col = stoi(&c.c_str()[3]);
		else if (memcmp(c.c_str(), "LED_Style", 9) == 0) LED_Style = stoi(&c.c_str()[9]);
		else if (memcmp(c.c_str(), "BLE_Device", 10) == 0) BLE_Device = stoi(&c.c_str()[10]);
		else if (cnt >= PRECNT && cnt < PRECNT + Row * Col) {
			if ((cnt - PRECNT) % Row == 0) {
				Custom_Layer.push_back(vector<string>());
			}
			Custom_Layer[(cnt - PRECNT) / Row].push_back(c);
		}
		else if (cnt >= PRECNT + Row * Col && cnt < PRECNT + 2 * Row * Col) {
			if ((cnt - PRECNT - Row * Col) % Row == 0) {
				EX_Layer.push_back(vector<string>());
			}
			EX_Layer[(cnt - PRECNT - Row * Col) / Row].push_back(c);
		}
	}

	infile.close();
	return 0;
}

KEY_ERRCODE keyboard_write_configuration(const string& fp)
{
	ofstream outfile;
	string c;
	int cnt = 0;

	outfile.open(fp);

	outfile << "TP78Configuration\n";
	outfile << "Row" << to_string(Row) << '\t';
	outfile << "Col" << to_string(Col) << '\n';
	outfile << "LED_Style" << to_string(LED_Style) << '\t';
	outfile << "BLE_Device" << to_string(BLE_Device) << '\n';

	for (unsigned char i = 0; i < Col; i++) {
		for (unsigned char j = 0; j < Row; j++) {
			if (Custom_Layer.size() > i && Custom_Layer[i].size() > j) {
				outfile << Custom_Layer[i][j];
			}
			else {
				outfile << "KEY_None";
			}
			if (j == Row - 1) {
				outfile << "\n";
			}
			else {
				outfile << "\t";
			}
		}
	}

	for (unsigned char i = 0; i < Col; i++) {
		for (unsigned char j = 0; j < Row; j++) {
			if (EX_Layer.size() > i && EX_Layer[i].size() > j) {
				outfile << EX_Layer[i][j];
			}
			else {
				outfile << "KEY_None";
			}
			if (j == Row - 1) {
				outfile << "\n";
			}
			else {
				outfile << "\t";
			}
		}
	}

	outfile.close();
	return 0;
}

