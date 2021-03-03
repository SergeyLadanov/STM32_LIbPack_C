/*
 * ws2812b_types.h
 *
 *  Created on: 7 февр. 2021 г.
 *      Author: Acer
 */

#ifndef WS2812B_INC_WS2812B_TYPES_H_
#define WS2812B_INC_WS2812B_TYPES_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

/// Representation of an HSV pixel (hue, saturation, value (aka brightness)).
typedef struct __WS2812_HSV {
		union {
			struct {
				union {
					uint8_t hue;
					uint8_t h; };
				union {
					uint8_t saturation;
					uint8_t sat;
					uint8_t s; };
				union {
					uint8_t value;
					uint8_t val;
					uint8_t v; };
			};
		uint8_t raw[3];
	};
}WS2812_HSV;

/// Pre-defined hue values for HSV objects
typedef enum {
	WS2812_HUE_RED = 0,
	WS2812_HUE_ORANGE = 32,
	WS2812_HUE_YELLOW = 64,
	WS2812_HUE_GREEN = 96,
	WS2812_HUE_AQUA = 128,
	WS2812_HUE_BLUE = 160,
	WS2812_HUE_PURPLE = 192,
	WS2812_HUE_PINK = 224
} WS2812_HSVHue;


typedef struct __WS2812_RGB {
	union {
		struct {
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
        };
		uint8_t raw[3];
	};

}WS2812_RGB;


/// Predefined RGB colors
typedef enum {
	WS2812_Color_AliceBlue=0xF0F8FF,
	WS2812_Color_Amethyst=0x9966CC,
	WS2812_Color_AntiqueWhite=0xFAEBD7,
	WS2812_Color_Aqua=0x00FFFF,
	WS2812_Color_Aquamarine=0x7FFFD4,
	WS2812_Color_Azure=0xF0FFFF,
	WS2812_Color_Beige=0xF5F5DC,
	WS2812_Color_Bisque=0xFFE4C4,
	WS2812_Color_Black=0x000000,
	WS2812_Color_BlanchedAlmond=0xFFEBCD,
	WS2812_Color_Blue=0x0000FF,
	WS2812_Color_BlueViolet=0x8A2BE2,
	WS2812_Color_Brown=0xA52A2A,
	WS2812_Color_BurlyWood=0xDEB887,
	WS2812_Color_CadetBlue=0x5F9EA0,
	WS2812_Color_Chartreuse=0x7FFF00,
	WS2812_Color_Chocolate=0xD2691E,
	WS2812_Color_Coral=0xFF7F50,
	WS2812_Color_CornflowerBlue=0x6495ED,
	WS2812_Color_Cornsilk=0xFFF8DC,
	WS2812_Color_Crimson=0xDC143C,
	WS2812_Color_Cyan=0x00FFFF,
	WS2812_Color_DarkBlue=0x00008B,
	WS2812_Color_DarkCyan=0x008B8B,
	WS2812_Color_DarkGoldenrod=0xB8860B,
	WS2812_Color_DarkGray=0xA9A9A9,
	WS2812_Color_DarkGrey=0xA9A9A9,
	WS2812_Color_DarkGreen=0x006400,
	WS2812_Color_DarkKhaki=0xBDB76B,
	WS2812_Color_DarkMagenta=0x8B008B,
	WS2812_Color_DarkOliveGreen=0x556B2F,
	WS2812_Color_DarkOrange=0xFF8C00,
	WS2812_Color_DarkOrchid=0x9932CC,
	WS2812_Color_DarkRed=0x8B0000,
	WS2812_Color_DarkSalmon=0xE9967A,
	WS2812_Color_DarkSeaGreen=0x8FBC8F,
	WS2812_Color_DarkSlateBlue=0x483D8B,
	WS2812_Color_DarkSlateGray=0x2F4F4F,
	WS2812_Color_DarkSlateGrey=0x2F4F4F,
	WS2812_Color_DarkTurquoise=0x00CED1,
	WS2812_Color_DarkViolet=0x9400D3,
	WS2812_Color_DeepPink=0xFF1493,
	WS2812_Color_DeepSkyBlue=0x00BFFF,
	WS2812_Color_DimGray=0x696969,
	WS2812_Color_DimGrey=0x696969,
	WS2812_Color_DodgerBlue=0x1E90FF,
	WS2812_Color_FireBrick=0xB22222,
	WS2812_Color_FloralWhite=0xFFFAF0,
	WS2812_Color_ForestGreen=0x228B22,
	WS2812_Color_Fuchsia=0xFF00FF,
	WS2812_Color_Gainsboro=0xDCDCDC,
	WS2812_Color_GhostWhite=0xF8F8FF,
	WS2812_Color_Gold=0xFFD700,
	WS2812_Color_Goldenrod=0xDAA520,
	WS2812_Color_Gray=0x808080,
	WS2812_Color_Grey=0x808080,
	WS2812_Color_Green=0x008000,
	WS2812_Color_GreenYellow=0xADFF2F,
	WS2812_Color_Honeydew=0xF0FFF0,
	WS2812_Color_HotPink=0xFF69B4,
	WS2812_Color_IndianRed=0xCD5C5C,
	WS2812_Color_Indigo=0x4B0082,
	WS2812_Color_Ivory=0xFFFFF0,
	WS2812_Color_Khaki=0xF0E68C,
	WS2812_Color_Lavender=0xE6E6FA,
	WS2812_Color_LavenderBlush=0xFFF0F5,
	WS2812_Color_LawnGreen=0x7CFC00,
	WS2812_Color_LemonChiffon=0xFFFACD,
	WS2812_Color_LightBlue=0xADD8E6,
	WS2812_Color_LightCoral=0xF08080,
	WS2812_Color_LightCyan=0xE0FFFF,
	WS2812_Color_LightGoldenrodYellow=0xFAFAD2,
	WS2812_Color_LightGreen=0x90EE90,
	WS2812_Color_LightGrey=0xD3D3D3,
	WS2812_Color_LightPink=0xFFB6C1,
	WS2812_Color_LightSalmon=0xFFA07A,
	WS2812_Color_LightSeaGreen=0x20B2AA,
	WS2812_Color_LightSkyBlue=0x87CEFA,
	WS2812_Color_LightSlateGray=0x778899,
	WS2812_Color_LightSlateGrey=0x778899,
	WS2812_Color_LightSteelBlue=0xB0C4DE,
	WS2812_Color_LightYellow=0xFFFFE0,
	WS2812_Color_Lime=0x00FF00,
	WS2812_Color_LimeGreen=0x32CD32,
	WS2812_Color_Linen=0xFAF0E6,
	WS2812_Color_Magenta=0xFF00FF,
	WS2812_Color_Maroon=0x800000,
	WS2812_Color_MediumAquamarine=0x66CDAA,
	WS2812_Color_MediumBlue=0x0000CD,
	WS2812_Color_MediumOrchid=0xBA55D3,
	WS2812_Color_MediumPurple=0x9370DB,
	WS2812_Color_MediumSeaGreen=0x3CB371,
	WS2812_Color_MediumSlateBlue=0x7B68EE,
	WS2812_Color_MediumSpringGreen=0x00FA9A,
	WS2812_Color_MediumTurquoise=0x48D1CC,
	WS2812_Color_MediumVioletRed=0xC71585,
	WS2812_Color_MidnightBlue=0x191970,
	WS2812_Color_MintCream=0xF5FFFA,
	WS2812_Color_MistyRose=0xFFE4E1,
	WS2812_Color_Moccasin=0xFFE4B5,
	WS2812_Color_NavajoWhite=0xFFDEAD,
	WS2812_Color_Navy=0x000080,
	WS2812_Color_OldLace=0xFDF5E6,
	WS2812_Color_Olive=0x808000,
	WS2812_Color_OliveDrab=0x6B8E23,
	WS2812_Color_Orange=0xFFA500,
	WS2812_Color_OrangeRed=0xFF4500,
	WS2812_Color_Orchid=0xDA70D6,
	WS2812_Color_PaleGoldenrod=0xEEE8AA,
	WS2812_Color_PaleGreen=0x98FB98,
	WS2812_Color_PaleTurquoise=0xAFEEEE,
	WS2812_Color_PaleVioletRed=0xDB7093,
	WS2812_Color_PapayaWhip=0xFFEFD5,
	WS2812_Color_PeachPuff=0xFFDAB9,
	WS2812_Color_Peru=0xCD853F,
	WS2812_Color_Pink=0xFFC0CB,
	WS2812_Color_Plaid=0xCC5533,
	WS2812_Color_Plum=0xDDA0DD,
	WS2812_Color_PowderBlue=0xB0E0E6,
	WS2812_Color_Purple=0x800080,
	WS2812_Color_Red=0xFF0000,
	WS2812_Color_RosyBrown=0xBC8F8F,
	WS2812_Color_RoyalBlue=0x4169E1,
	WS2812_Color_SaddleBrown=0x8B4513,
	WS2812_Color_Salmon=0xFA8072,
	WS2812_Color_SandyBrown=0xF4A460,
	WS2812_Color_SeaGreen=0x2E8B57,
	WS2812_Color_Seashell=0xFFF5EE,
	WS2812_Color_Sienna=0xA0522D,
	WS2812_Color_Silver=0xC0C0C0,
	WS2812_Color_SkyBlue=0x87CEEB,
	WS2812_Color_SlateBlue=0x6A5ACD,
	WS2812_Color_SlateGray=0x708090,
	WS2812_Color_SlateGrey=0x708090,
	WS2812_Color_Snow=0xFFFAFA,
	WS2812_Color_SpringGreen=0x00FF7F,
	WS2812_Color_SteelBlue=0x4682B4,
	WS2812_Color_Tan=0xD2B48C,
	WS2812_Color_Teal=0x008080,
	WS2812_Color_Thistle=0xD8BFD8,
	WS2812_Color_Tomato=0xFF6347,
	WS2812_Color_Turquoise=0x40E0D0,
	WS2812_Color_Violet=0xEE82EE,
	WS2812_Color_Wheat=0xF5DEB3,
	WS2812_Color_White=0xFFFFFF,
	WS2812_Color_WhiteSmoke=0xF5F5F5,
	WS2812_Color_Yellow=0xFFFF00,
	WS2812_Color_YellowGreen=0x9ACD32,

	// LED RGB color that roughly approximates
	// the color of incandescent fairy lights,
	// assuming that you're using FastLED
	// color correction on your LEDs (recommended).
	WS2812_Color_FairyLight=0xFFE42D,
	// If you are using no color correction, use this
	WS2812_Color_FairyLightNCC=0xFF9D2A

} WS2812_HTMLColorCode;


#ifdef __cplusplus
}
#endif

#endif /* WS2812B_INC_WS2812B_TYPES_H_ */
