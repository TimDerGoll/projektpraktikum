#include "errors.h"

namespace io {
    const char error_1000[][DISPLAY_SIZE_HEIGHT][DISPLAY_SIZE_WIDTH +1] = {
        {
            "     ERROR 1000     ",
            "                    ",
            "  Max Stringroesse  ",
            "   ueberschritten   "
        },
        {
            "     ERROR 1001     ",
            "                    ",
            "      Falscher      ",
            "    Zeilenbeginn    "
        },
        {
            "     ERROR 1002     ",
            "                    ",
            "      Falsches      ",
            "     Zeilenende     "
        },
        {
            "     ERROR 1003     ",
            "                    ",
            "    Serial Read     ",
            "      Timeout       "
        },
        {
            "     ERROR 1004     ",
            "                    ",
            "   Falschen Anzahl  ",
            "   von Argumenten   "
        }
    };

    const char error_5000[][DISPLAY_SIZE_HEIGHT][DISPLAY_SIZE_WIDTH +1] = {
        {
            "     ERROR 5000     ",
            "                    ",
            "     Zugriff auf    ",
            "   undefinierte ID  "
        }
    };
}
