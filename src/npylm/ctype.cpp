#include "ctype.h"

namespace npylm {
    namespace ctype {
        unsigned int get_type(wchar_t c){
            if(0x0000 <= c && c <= 0x007F){
                return CTYPE_BASIC_LATIN;
            }
            if(0x0080 <= c && c <= 0x00FF){
                return CTYPE_LATIN_1_SUPPLEMENT;
            }
            if(0x0100 <= c && c <= 0x017F){
                return CTYPE_LATIN_EXTENDED_A;
            }
            if(0x0180 <= c && c <= 0x024F){
                return CTYPE_LATIN_EXTENDED_B;
            }
            if(0x0250 <= c && c <= 0x02AF){
                return CTYPE_IPA_EXTENSIONS;
            }
            if(0x02B0 <= c && c <= 0x02FF){
                return CTYPE_SPACING_MODIFIER_LETTERS;
            }
            if(0x0300 <= c && c <= 0x036F){
                return CTYPE_COMBINING_DIACRITICAL_MARKS;
            }
            if(0x0370 <= c && c <= 0x03FF){
                return CTYPE_GREEK_AND_COPTIC;
            }
            if(0x0400 <= c && c <= 0x04FF){
                return CTYPE_CYRILLIC;
            }
            if(0x0500 <= c && c <= 0x052F){
                return CTYPE_CYRILLIC_SUPPLEMENT;
            }
            if(0x0530 <= c && c <= 0x058F){
                return CTYPE_ARMENIAN;
            }
            if(0x0590 <= c && c <= 0x05FF){
                return CTYPE_HEBREW;
            }
            if(0x0600 <= c && c <= 0x06FF){
                return CTYPE_ARABIC;
            }
            if(0x0700 <= c && c <= 0x074F){
                return CTYPE_SYRIAC;
            }
            if(0x0750 <= c && c <= 0x077F){
                return CTYPE_ARABIC_SUPPLEMENT;
            }
            if(0x0780 <= c && c <= 0x07BF){
                return CTYPE_THAANA;
            }
            if(0x07C0 <= c && c <= 0x07FF){
                return CTYPE_NKO;
            }
            if(0x0800 <= c && c <= 0x083F){
                return CTYPE_SAMARITAN;
            }
            if(0x0840 <= c && c <= 0x085F){
                return CTYPE_MANDAIC;
            }
            if(0x0860 <= c && c <= 0x086F){
                return CTYPE_SYRIAC_SUPPLEMENT;
            }
            if(0x08A0 <= c && c <= 0x08FF){
                return CTYPE_ARABIC_EXTENDED_A;
            }
            if(0x0900 <= c && c <= 0x097F){
                return CTYPE_DEVANAGARI;
            }
            if(0x0980 <= c && c <= 0x09FF){
                return CTYPE_BENGALI;
            }
            if(0x0A00 <= c && c <= 0x0A7F){
                return CTYPE_GURMUKHI;
            }
            if(0x0A80 <= c && c <= 0x0AFF){
                return CTYPE_GUJARATI;
            }
            if(0x0B00 <= c && c <= 0x0B7F){
                return CTYPE_ORIYA;
            }
            if(0x0B80 <= c && c <= 0x0BFF){
                return CTYPE_TAMIL;
            }
            if(0x0C00 <= c && c <= 0x0C7F){
                return CTYPE_TELUGU;
            }
            if(0x0C80 <= c && c <= 0x0CFF){
                return CTYPE_KANNADA;
            }
            if(0x0D00 <= c && c <= 0x0D7F){
                return CTYPE_MALAYALAM;
            }
            if(0x0D80 <= c && c <= 0x0DFF){
                return CTYPE_SINHALA;
            }
            if(0x0E00 <= c && c <= 0x0E7F){
                return CTYPE_THAI;
            }
            if(0x0E80 <= c && c <= 0x0EFF){
                return CTYPE_LAO;
            }
            if(0x0F00 <= c && c <= 0x0FFF){
                return CTYPE_TIBETAN;
            }
            if(0x1000 <= c && c <= 0x109F){
                return CTYPE_MYANMAR;
            }
            if(0x10A0 <= c && c <= 0x10FF){
                return CTYPE_GEORGIAN;
            }
            if(0x1100 <= c && c <= 0x11FF){
                return CTYPE_HANGUL_JAMO;
            }
            if(0x1200 <= c && c <= 0x137F){
                return CTYPE_ETHIOPIC;
            }
            if(0x1380 <= c && c <= 0x139F){
                return CTYPE_ETHIOPIC_SUPPLEMENT;
            }
            if(0x13A0 <= c && c <= 0x13FF){
                return CTYPE_CHEROKEE;
            }
            if(0x1400 <= c && c <= 0x167F){
                return CTYPE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS;
            }
            if(0x1680 <= c && c <= 0x169F){
                return CTYPE_OGHAM;
            }
            if(0x16A0 <= c && c <= 0x16FF){
                return CTYPE_RUNIC;
            }
            if(0x1700 <= c && c <= 0x171F){
                return CTYPE_TAGALOG;
            }
            if(0x1720 <= c && c <= 0x173F){
                return CTYPE_HANUNOO;
            }
            if(0x1740 <= c && c <= 0x175F){
                return CTYPE_BUHID;
            }
            if(0x1760 <= c && c <= 0x177F){
                return CTYPE_TAGBANWA;
            }
            if(0x1780 <= c && c <= 0x17FF){
                return CTYPE_KHMER;
            }
            if(0x1800 <= c && c <= 0x18AF){
                return CTYPE_MONGOLIAN;
            }
            if(0x18B0 <= c && c <= 0x18FF){
                return CTYPE_UNIFIED_CANADIAN_ABORIGINAL_SYLLABICS_EXTENDED;
            }
            if(0x1900 <= c && c <= 0x194F){
                return CTYPE_LIMBU;
            }
            if(0x1950 <= c && c <= 0x197F){
                return CTYPE_TAI_LE;
            }
            if(0x1980 <= c && c <= 0x19DF){
                return CTYPE_NEW_TAI_LUE;
            }
            if(0x19E0 <= c && c <= 0x19FF){
                return CTYPE_KHMER_SYMBOLS;
            }
            if(0x1A00 <= c && c <= 0x1A1F){
                return CTYPE_BUGINESE;
            }
            if(0x1A20 <= c && c <= 0x1AAF){
                return CTYPE_TAI_THAM;
            }
            if(0x1AB0 <= c && c <= 0x1AFF){
                return CTYPE_COMBINING_DIACRITICAL_MARKS_EXTENDED;
            }
            if(0x1B00 <= c && c <= 0x1B7F){
                return CTYPE_BALINESE;
            }
            if(0x1B80 <= c && c <= 0x1BBF){
                return CTYPE_SUNDANESE;
            }
            if(0x1BC0 <= c && c <= 0x1BFF){
                return CTYPE_BATAK;
            }
            if(0x1C00 <= c && c <= 0x1C4F){
                return CTYPE_LEPCHA;
            }
            if(0x1C50 <= c && c <= 0x1C7F){
                return CTYPE_OL_CHIKI;
            }
            if(0x1C80 <= c && c <= 0x1C8F){
                return CTYPE_CYRILLIC_EXTENDED_C;
            }
            if(0x1CC0 <= c && c <= 0x1CCF){
                return CTYPE_SUNDANESE_SUPPLEMENT;
            }
            if(0x1CD0 <= c && c <= 0x1CFF){
                return CTYPE_VEDIC_EXTENSIONS;
            }
            if(0x1D00 <= c && c <= 0x1D7F){
                return CTYPE_PHONETIC_EXTENSIONS;
            }
            if(0x1D80 <= c && c <= 0x1DBF){
                return CTYPE_PHONETIC_EXTENSIONS_SUPPLEMENT;
            }
            if(0x1DC0 <= c && c <= 0x1DFF){
                return CTYPE_COMBINING_DIACRITICAL_MARKS_SUPPLEMENT;
            }
            if(0x1E00 <= c && c <= 0x1EFF){
                return CTYPE_LATIN_EXTENDED_ADDITIONAL;
            }
            if(0x1F00 <= c && c <= 0x1FFF){
                return CTYPE_GREEK_EXTENDED;
            }
            if(0x2000 <= c && c <= 0x206F){
                return CTYPE_GENERAL_PUNCTUATION;
            }
            if(0x2070 <= c && c <= 0x209F){
                return CTYPE_SUPERSCRIPTS_AND_SUBSCRIPTS;
            }
            if(0x20A0 <= c && c <= 0x20CF){
                return CTYPE_CURRENCY_SYMBOLS;
            }
            if(0x20D0 <= c && c <= 0x20FF){
                return CTYPE_COMBINING_DIACRITICAL_MARKS_FOR_SYMBOLS;
            }
            if(0x2100 <= c && c <= 0x214F){
                return CTYPE_LETTERLIKE_SYMBOLS;
            }
            if(0x2150 <= c && c <= 0x218F){
                return CTYPE_NUMBER_FORMS;
            }
            if(0x2190 <= c && c <= 0x21FF){
                return CTYPE_ARROWS;
            }
            if(0x2200 <= c && c <= 0x22FF){
                return CTYPE_MATHEMATICAL_OPERATORS;
            }
            if(0x2300 <= c && c <= 0x23FF){
                return CTYPE_MISCELLANEOUS_TECHNICAL;
            }
            if(0x2400 <= c && c <= 0x243F){
                return CTYPE_CONTROL_PICTURES;
            }
            if(0x2440 <= c && c <= 0x245F){
                return CTYPE_OPTICAL_CHARACTER_RECOGNITION;
            }
            if(0x2460 <= c && c <= 0x24FF){
                return CTYPE_ENCLOSED_ALPHANUMERICS;
            }
            if(0x2500 <= c && c <= 0x257F){
                return CTYPE_BOX_DRAWING;
            }
            if(0x2580 <= c && c <= 0x259F){
                return CTYPE_BLOCK_ELEMENTS;
            }
            if(0x25A0 <= c && c <= 0x25FF){
                return CTYPE_GEOMETRIC_SHAPES;
            }
            if(0x2600 <= c && c <= 0x26FF){
                return CTYPE_MISCELLANEOUS_SYMBOLS;
            }
            if(0x2700 <= c && c <= 0x27BF){
                return CTYPE_DINGBATS;
            }
            if(0x27C0 <= c && c <= 0x27EF){
                return CTYPE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_A;
            }
            if(0x27F0 <= c && c <= 0x27FF){
                return CTYPE_SUPPLEMENTAL_ARROWS_A;
            }
            if(0x2800 <= c && c <= 0x28FF){
                return CTYPE_BRAILLE_PATTERNS;
            }
            if(0x2900 <= c && c <= 0x297F){
                return CTYPE_SUPPLEMENTAL_ARROWS_B;
            }
            if(0x2980 <= c && c <= 0x29FF){
                return CTYPE_MISCELLANEOUS_MATHEMATICAL_SYMBOLS_B;
            }
            if(0x2A00 <= c && c <= 0x2AFF){
                return CTYPE_SUPPLEMENTAL_MATHEMATICAL_OPERATORS;
            }
            if(0x2B00 <= c && c <= 0x2BFF){
                return CTYPE_MISCELLANEOUS_SYMBOLS_AND_ARROWS;
            }
            if(0x2C00 <= c && c <= 0x2C5F){
                return CTYPE_GLAGOLITIC;
            }
            if(0x2C60 <= c && c <= 0x2C7F){
                return CTYPE_LATIN_EXTENDED_C;
            }
            if(0x2C80 <= c && c <= 0x2CFF){
                return CTYPE_COPTIC;
            }
            if(0x2D00 <= c && c <= 0x2D2F){
                return CTYPE_GEORGIAN_SUPPLEMENT;
            }
            if(0x2D30 <= c && c <= 0x2D7F){
                return CTYPE_TIFINAGH;
            }
            if(0x2D80 <= c && c <= 0x2DDF){
                return CTYPE_ETHIOPIC_EXTENDED;
            }
            if(0x2DE0 <= c && c <= 0x2DFF){
                return CTYPE_CYRILLIC_EXTENDED_A;
            }
            if(0x2E00 <= c && c <= 0x2E7F){
                return CTYPE_SUPPLEMENTAL_PUNCTUATION;
            }
            if(0x2E80 <= c && c <= 0x2EFF){
                return CTYPE_CJK_RADICALS_SUPPLEMENT;
            }
            if(0x2F00 <= c && c <= 0x2FDF){
                return CTYPE_KANGXI_RADICALS;
            }
            if(0x2FF0 <= c && c <= 0x2FFF){
                return CTYPE_IDEOGRAPHIC_DESCRIPTION_CHARACTERS;
            }
            if(0x3000 <= c && c <= 0x303F){
                return CTYPE_CJK_SYMBOLS_AND_PUNCTUATION;
            }
            if(0x3040 <= c && c <= 0x309F){
                return CTYPE_HIRAGANA;
            }
            if(0x30A0 <= c && c <= 0x30FF){
                return CTYPE_KATAKANA;
            }
            if(0x3100 <= c && c <= 0x312F){
                return CTYPE_BOPOMOFO;
            }
            if(0x3130 <= c && c <= 0x318F){
                return CTYPE_HANGUL_COMPATIBILITY_JAMO;
            }
            if(0x3190 <= c && c <= 0x319F){
                return CTYPE_KANBUN;
            }
            if(0x31A0 <= c && c <= 0x31BF){
                return CTYPE_BOPOMOFO_EXTENDED;
            }
            if(0x31C0 <= c && c <= 0x31EF){
                return CTYPE_CJK_STROKES;
            }
            if(0x31F0 <= c && c <= 0x31FF){
                return CTYPE_KATAKANA_PHONETIC_EXTENSIONS;
            }
            if(0x3200 <= c && c <= 0x32FF){
                return CTYPE_ENCLOSED_CJK_LETTERS_AND_MONTHS;
            }
            if(0x3300 <= c && c <= 0x33FF){
                return CTYPE_CJK_COMPATIBILITY;
            }
            if(0x3400 <= c && c <= 0x4DBF){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_A;
            }
            if(0x4DC0 <= c && c <= 0x4DFF){
                return CTYPE_YIJING_HEXAGRAM_SYMBOLS;
            }
            if(0x4E00 <= c && c <= 0x9FFF){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS;
            }
            if(0xA000 <= c && c <= 0xA48F){
                return CTYPE_YI_SYLLABLES;
            }
            if(0xA490 <= c && c <= 0xA4CF){
                return CTYPE_YI_RADICALS;
            }
            if(0xA4D0 <= c && c <= 0xA4FF){
                return CTYPE_LISU;
            }
            if(0xA500 <= c && c <= 0xA63F){
                return CTYPE_VAI;
            }
            if(0xA640 <= c && c <= 0xA69F){
                return CTYPE_CYRILLIC_EXTENDED_B;
            }
            if(0xA6A0 <= c && c <= 0xA6FF){
                return CTYPE_BAMUM;
            }
            if(0xA700 <= c && c <= 0xA71F){
                return CTYPE_MODIFIER_TONE_LETTERS;
            }
            if(0xA720 <= c && c <= 0xA7FF){
                return CTYPE_LATIN_EXTENDED_D;
            }
            if(0xA800 <= c && c <= 0xA82F){
                return CTYPE_SYLOTI_NAGRI;
            }
            if(0xA830 <= c && c <= 0xA83F){
                return CTYPE_COMMON_INDIC_NUMBER_FORMS;
            }
            if(0xA840 <= c && c <= 0xA87F){
                return CTYPE_PHAGS_PA;
            }
            if(0xA880 <= c && c <= 0xA8DF){
                return CTYPE_SAURASHTRA;
            }
            if(0xA8E0 <= c && c <= 0xA8FF){
                return CTYPE_DEVANAGARI_EXTENDED;
            }
            if(0xA900 <= c && c <= 0xA92F){
                return CTYPE_KAYAH_LI;
            }
            if(0xA930 <= c && c <= 0xA95F){
                return CTYPE_REJANG;
            }
            if(0xA960 <= c && c <= 0xA97F){
                return CTYPE_HANGUL_JAMO_EXTENDED_A;
            }
            if(0xA980 <= c && c <= 0xA9DF){
                return CTYPE_JAVANESE;
            }
            if(0xA9E0 <= c && c <= 0xA9FF){
                return CTYPE_MYANMAR_EXTENDED_B;
            }
            if(0xAA00 <= c && c <= 0xAA5F){
                return CTYPE_CHAM;
            }
            if(0xAA60 <= c && c <= 0xAA7F){
                return CTYPE_MYANMAR_EXTENDED_A;
            }
            if(0xAA80 <= c && c <= 0xAADF){
                return CTYPE_TAI_VIET;
            }
            if(0xAAE0 <= c && c <= 0xAAFF){
                return CTYPE_MEETEI_MAYEK_EXTENSIONS;
            }
            if(0xAB00 <= c && c <= 0xAB2F){
                return CTYPE_ETHIOPIC_EXTENDED_A;
            }
            if(0xAB30 <= c && c <= 0xAB6F){
                return CTYPE_LATIN_EXTENDED_E;
            }
            if(0xAB70 <= c && c <= 0xABBF){
                return CTYPE_CHEROKEE_SUPPLEMENT;
            }
            if(0xABC0 <= c && c <= 0xABFF){
                return CTYPE_MEETEI_MAYEK;
            }
            if(0xAC00 <= c && c <= 0xD7AF){
                return CTYPE_HANGUL_SYLLABLES;
            }
            if(0xD7B0 <= c && c <= 0xD7FF){
                return CTYPE_HANGUL_JAMO_EXTENDED_B;
            }
            if(0xD800 <= c && c <= 0xDB7F){
                return CTYPE_HIGH_SURROGATES;
            }
            if(0xDB80 <= c && c <= 0xDBFF){
                return CTYPE_HIGH_PRIVATE_USE_SURROGATES;
            }
            if(0xDC00 <= c && c <= 0xDFFF){
                return CTYPE_LOW_SURROGATES;
            }
            if(0xE000 <= c && c <= 0xF8FF){
                return CTYPE_PRIVATE_USE_AREA;
            }
            if(0xF900 <= c && c <= 0xFAFF){
                return CTYPE_CJK_COMPATIBILITY_IDEOGRAPHS;
            }
            if(0xFB00 <= c && c <= 0xFB4F){
                return CTYPE_ALPHABETIC_PRESENTATION_FORMS;
            }
            if(0xFB50 <= c && c <= 0xFDFF){
                return CTYPE_ARABIC_PRESENTATION_FORMS_A;
            }
            if(0xFE00 <= c && c <= 0xFE0F){
                return CTYPE_VARIATION_SELECTORS;
            }
            if(0xFE10 <= c && c <= 0xFE1F){
                return CTYPE_VERTICAL_FORMS;
            }
            if(0xFE20 <= c && c <= 0xFE2F){
                return CTYPE_COMBINING_HALF_MARKS;
            }
            if(0xFE30 <= c && c <= 0xFE4F){
                return CTYPE_CJK_COMPATIBILITY_FORMS;
            }
            if(0xFE50 <= c && c <= 0xFE6F){
                return CTYPE_SMALL_FORM_VARIANTS;
            }
            if(0xFE70 <= c && c <= 0xFEFF){
                return CTYPE_ARABIC_PRESENTATION_FORMS_B;
            }
            if(0xFF00 <= c && c <= 0xFFEF){
                return CTYPE_HALFWIDTH_AND_FULLWIDTH_FORMS;
            }
            if(0xFFF0 <= c && c <= 0xFFFF){
                return CTYPE_SPECIALS;
            }
            if(0x10000 <= c && c <= 0x1007F){
                return CTYPE_LINEAR_B_SYLLABARY;
            }
            if(0x10080 <= c && c <= 0x100FF){
                return CTYPE_LINEAR_B_IDEOGRAMS;
            }
            if(0x10100 <= c && c <= 0x1013F){
                return CTYPE_AEGEAN_NUMBERS;
            }
            if(0x10140 <= c && c <= 0x1018F){
                return CTYPE_ANCIENT_GREEK_NUMBERS;
            }
            if(0x10190 <= c && c <= 0x101CF){
                return CTYPE_ANCIENT_SYMBOLS;
            }
            if(0x101D0 <= c && c <= 0x101FF){
                return CTYPE_PHAISTOS_DISC;
            }
            if(0x10280 <= c && c <= 0x1029F){
                return CTYPE_LYCIAN;
            }
            if(0x102A0 <= c && c <= 0x102DF){
                return CTYPE_CARIAN;
            }
            if(0x102E0 <= c && c <= 0x102FF){
                return CTYPE_COPTIC_EPACT_NUMBERS;
            }
            if(0x10300 <= c && c <= 0x1032F){
                return CTYPE_OLD_ITALIC;
            }
            if(0x10330 <= c && c <= 0x1034F){
                return CTYPE_GOTHIC;
            }
            if(0x10350 <= c && c <= 0x1037F){
                return CTYPE_OLD_PERMIC;
            }
            if(0x10380 <= c && c <= 0x1039F){
                return CTYPE_UGARITIC;
            }
            if(0x103A0 <= c && c <= 0x103DF){
                return CTYPE_OLD_PERSIAN;
            }
            if(0x10400 <= c && c <= 0x1044F){
                return CTYPE_DESERET;
            }
            if(0x10450 <= c && c <= 0x1047F){
                return CTYPE_SHAVIAN;
            }
            if(0x10480 <= c && c <= 0x104AF){
                return CTYPE_OSMANYA;
            }
            if(0x104B0 <= c && c <= 0x104FF){
                return CTYPE_OSAGE;
            }
            if(0x10500 <= c && c <= 0x1052F){
                return CTYPE_ELBASAN;
            }
            if(0x10530 <= c && c <= 0x1056F){
                return CTYPE_CAUCASIAN_ALBANIAN;
            }
            if(0x10600 <= c && c <= 0x1077F){
                return CTYPE_LINEAR_A;
            }
            if(0x10800 <= c && c <= 0x1083F){
                return CTYPE_CYPRIOT_SYLLABARY;
            }
            if(0x10840 <= c && c <= 0x1085F){
                return CTYPE_IMPERIAL_ARAMAIC;
            }
            if(0x10860 <= c && c <= 0x1087F){
                return CTYPE_PALMYRENE;
            }
            if(0x10880 <= c && c <= 0x108AF){
                return CTYPE_NABATAEAN;
            }
            if(0x108E0 <= c && c <= 0x108FF){
                return CTYPE_HATRAN;
            }
            if(0x10900 <= c && c <= 0x1091F){
                return CTYPE_PHOENICIAN;
            }
            if(0x10920 <= c && c <= 0x1093F){
                return CTYPE_LYDIAN;
            }
            if(0x10980 <= c && c <= 0x1099F){
                return CTYPE_MEROITIC_HIEROGLYPHS;
            }
            if(0x109A0 <= c && c <= 0x109FF){
                return CTYPE_MEROITIC_CURSIVE;
            }
            if(0x10A00 <= c && c <= 0x10A5F){
                return CTYPE_KHAROSHTHI;
            }
            if(0x10A60 <= c && c <= 0x10A7F){
                return CTYPE_OLD_SOUTH_ARABIAN;
            }
            if(0x10A80 <= c && c <= 0x10A9F){
                return CTYPE_OLD_NORTH_ARABIAN;
            }
            if(0x10AC0 <= c && c <= 0x10AFF){
                return CTYPE_MANICHAEAN;
            }
            if(0x10B00 <= c && c <= 0x10B3F){
                return CTYPE_AVESTAN;
            }
            if(0x10B40 <= c && c <= 0x10B5F){
                return CTYPE_INSCRIPTIONAL_PARTHIAN;
            }
            if(0x10B60 <= c && c <= 0x10B7F){
                return CTYPE_INSCRIPTIONAL_PAHLAVI;
            }
            if(0x10B80 <= c && c <= 0x10BAF){
                return CTYPE_PSALTER_PAHLAVI;
            }
            if(0x10C00 <= c && c <= 0x10C4F){
                return CTYPE_OLD_TURKIC;
            }
            if(0x10C80 <= c && c <= 0x10CFF){
                return CTYPE_OLD_HUNGARIAN;
            }
            if(0x10E60 <= c && c <= 0x10E7F){
                return CTYPE_RUMI_NUMERAL_SYMBOLS;
            }
            if(0x11000 <= c && c <= 0x1107F){
                return CTYPE_BRAHMI;
            }
            if(0x11080 <= c && c <= 0x110CF){
                return CTYPE_KAITHI;
            }
            if(0x110D0 <= c && c <= 0x110FF){
                return CTYPE_SORA_SOMPENG;
            }
            if(0x11100 <= c && c <= 0x1114F){
                return CTYPE_CHAKMA;
            }
            if(0x11150 <= c && c <= 0x1117F){
                return CTYPE_MAHAJANI;
            }
            if(0x11180 <= c && c <= 0x111DF){
                return CTYPE_SHARADA;
            }
            if(0x111E0 <= c && c <= 0x111FF){
                return CTYPE_SINHALA_ARCHAIC_NUMBERS;
            }
            if(0x11200 <= c && c <= 0x1124F){
                return CTYPE_KHOJKI;
            }
            if(0x11280 <= c && c <= 0x112AF){
                return CTYPE_MULTANI;
            }
            if(0x112B0 <= c && c <= 0x112FF){
                return CTYPE_KHUDAWADI;
            }
            if(0x11300 <= c && c <= 0x1137F){
                return CTYPE_GRANTHA;
            }
            if(0x11400 <= c && c <= 0x1147F){
                return CTYPE_NEWA;
            }
            if(0x11480 <= c && c <= 0x114DF){
                return CTYPE_TIRHUTA;
            }
            if(0x11580 <= c && c <= 0x115FF){
                return CTYPE_SIDDHAM;
            }
            if(0x11600 <= c && c <= 0x1165F){
                return CTYPE_MODI;
            }
            if(0x11660 <= c && c <= 0x1167F){
                return CTYPE_MONGOLIAN_SUPPLEMENT;
            }
            if(0x11680 <= c && c <= 0x116CF){
                return CTYPE_TAKRI;
            }
            if(0x11700 <= c && c <= 0x1173F){
                return CTYPE_AHOM;
            }
            if(0x118A0 <= c && c <= 0x118FF){
                return CTYPE_WARANG_CITI;
            }
            if(0x11A00 <= c && c <= 0x11A4F){
                return CTYPE_ZANABAZAR_SQUARE;
            }
            if(0x11A50 <= c && c <= 0x11AAF){
                return CTYPE_SOYOMBO;
            }
            if(0x11AC0 <= c && c <= 0x11AFF){
                return CTYPE_PAU_CIN_HAU;
            }
            if(0x11C00 <= c && c <= 0x11C6F){
                return CTYPE_BHAIKSUKI;
            }
            if(0x11C70 <= c && c <= 0x11CBF){
                return CTYPE_MARCHEN;
            }
            if(0x11D00 <= c && c <= 0x11D5F){
                return CTYPE_MASARAM_GONDI;
            }
            if(0x12000 <= c && c <= 0x123FF){
                return CTYPE_CUNEIFORM;
            }
            if(0x12400 <= c && c <= 0x1247F){
                return CTYPE_CUNEIFORM_NUMBERS_AND_PUNCTUATION;
            }
            if(0x12480 <= c && c <= 0x1254F){
                return CTYPE_EARLY_DYNASTIC_CUNEIFORM;
            }
            if(0x13000 <= c && c <= 0x1342F){
                return CTYPE_EGYPTIAN_HIEROGLYPHS;
            }
            if(0x14400 <= c && c <= 0x1467F){
                return CTYPE_ANATOLIAN_HIEROGLYPHS;
            }
            if(0x16800 <= c && c <= 0x16A3F){
                return CTYPE_BAMUM_SUPPLEMENT;
            }
            if(0x16A40 <= c && c <= 0x16A6F){
                return CTYPE_MRO;
            }
            if(0x16AD0 <= c && c <= 0x16AFF){
                return CTYPE_BASSA_VAH;
            }
            if(0x16B00 <= c && c <= 0x16B8F){
                return CTYPE_PAHAWH_HMONG;
            }
            if(0x16F00 <= c && c <= 0x16F9F){
                return CTYPE_MIAO;
            }
            if(0x16FE0 <= c && c <= 0x16FFF){
                return CTYPE_IDEOGRAPHIC_SYMBOLS_AND_PUNCTUATION;
            }
            if(0x17000 <= c && c <= 0x187FF){
                return CTYPE_TANGUT;
            }
            if(0x18800 <= c && c <= 0x18AFF){
                return CTYPE_TANGUT_COMPONENTS;
            }
            if(0x1B000 <= c && c <= 0x1B0FF){
                return CTYPE_KANA_SUPPLEMENT;
            }
            if(0x1B100 <= c && c <= 0x1B12F){
                return CTYPE_KANA_EXTENDED_A;
            }
            if(0x1B170 <= c && c <= 0x1B2FF){
                return CTYPE_NUSHU;
            }
            if(0x1BC00 <= c && c <= 0x1BC9F){
                return CTYPE_DUPLOYAN;
            }
            if(0x1BCA0 <= c && c <= 0x1BCAF){
                return CTYPE_SHORTHAND_FORMAT_CONTROLS;
            }
            if(0x1D000 <= c && c <= 0x1D0FF){
                return CTYPE_BYZANTINE_MUSICAL_SYMBOLS;
            }
            if(0x1D100 <= c && c <= 0x1D1FF){
                return CTYPE_MUSICAL_SYMBOLS;
            }
            if(0x1D200 <= c && c <= 0x1D24F){
                return CTYPE_ANCIENT_GREEK_MUSICAL_NOTATION;
            }
            if(0x1D300 <= c && c <= 0x1D35F){
                return CTYPE_TAI_XUAN_JING_SYMBOLS;
            }
            if(0x1D360 <= c && c <= 0x1D37F){
                return CTYPE_COUNTING_ROD_NUMERALS;
            }
            if(0x1D400 <= c && c <= 0x1D7FF){
                return CTYPE_MATHEMATICAL_ALPHANUMERIC_SYMBOLS;
            }
            if(0x1D800 <= c && c <= 0x1DAAF){
                return CTYPE_SUTTON_SIGNWRITING;
            }
            if(0x1E000 <= c && c <= 0x1E02F){
                return CTYPE_GLAGOLITIC_SUPPLEMENT;
            }
            if(0x1E800 <= c && c <= 0x1E8DF){
                return CTYPE_MENDE_KIKAKUI;
            }
            if(0x1E900 <= c && c <= 0x1E95F){
                return CTYPE_ADLAM;
            }
            if(0x1EE00 <= c && c <= 0x1EEFF){
                return CTYPE_ARABIC_MATHEMATICAL_ALPHABETIC_SYMBOLS;
            }
            if(0x1F000 <= c && c <= 0x1F02F){
                return CTYPE_MAHJONG_TILES;
            }
            if(0x1F030 <= c && c <= 0x1F09F){
                return CTYPE_DOMINO_TILES;
            }
            if(0x1F0A0 <= c && c <= 0x1F0FF){
                return CTYPE_PLAYING_CARDS;
            }
            if(0x1F100 <= c && c <= 0x1F1FF){
                return CTYPE_ENCLOSED_ALPHANUMERIC_SUPPLEMENT;
            }
            if(0x1F200 <= c && c <= 0x1F2FF){
                return CTYPE_ENCLOSED_IDEOGRAPHIC_SUPPLEMENT;
            }
            if(0x1F300 <= c && c <= 0x1F5FF){
                return CTYPE_MISCELLANEOUS_SYMBOLS_AND_PICTOGRAPHS;
            }
            if(0x1F600 <= c && c <= 0x1F64F){
                return CTYPE_EMOTICONS;
            }
            if(0x1F650 <= c && c <= 0x1F67F){
                return CTYPE_ORNAMENTAL_DINGBATS;
            }
            if(0x1F680 <= c && c <= 0x1F6FF){
                return CTYPE_TRANSPORT_AND_MAP_SYMBOLS;
            }
            if(0x1F700 <= c && c <= 0x1F77F){
                return CTYPE_ALCHEMICAL_SYMBOLS;
            }
            if(0x1F780 <= c && c <= 0x1F7FF){
                return CTYPE_GEOMETRIC_SHAPES_EXTENDED;
            }
            if(0x1F800 <= c && c <= 0x1F8FF){
                return CTYPE_SUPPLEMENTAL_ARROWS_C;
            }
            if(0x1F900 <= c && c <= 0x1F9FF){
                return CTYPE_SUPPLEMENTAL_SYMBOLS_AND_PICTOGRAPHS;
            }
            if(0x20000 <= c && c <= 0x2A6DF){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_B;
            }
            if(0x2A700 <= c && c <= 0x2B73F){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_C;
            }
            if(0x2B740 <= c && c <= 0x2B81F){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_D;
            }
            if(0x2B820 <= c && c <= 0x2CEAF){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_E;
            }
            if(0x2CEB0 <= c && c <= 0x2EBEF){
                return CTYPE_CJK_UNIFIED_IDEOGRAPHS_EXTENSION_F;
            }
            if(0x2F800 <= c && c <= 0x2FA1F){
                return CTYPE_CJK_COMPATIBILITY_IDEOGRAPHS_SUPPLEMENT;
            }
            if(0xE0000 <= c && c <= 0xE007F){
                return CTYPE_TAGS;
            }
            if(0xE0100 <= c && c <= 0xE01EF){
                return CTYPE_VARIATION_SELECTORS_SUPPLEMENT;
            }
            if(0xF0000 <= c && c <= 0xFFFFF){
                return CTYPE_SUPPLEMENTARY_PRIVATE_USE_AREA_A;
            }
            if(0x100000 <= c && c <= 0x10FFFF){
                return CTYPE_SUPPLEMENTARY_PRIVATE_USE_AREA_B;
            }
            return CTYPE_UNKNOWN;
        }
        std::string get_name(unsigned int type){
            if(type == 1){
                return "Basic Latin";
            }
            if(type == 2){
                return "Latin 1 Supplement";
            }
            if(type == 3){
                return "Latin Extended A";
            }
            if(type == 4){
                return "Latin Extended B";
            }
            if(type == 5){
                return "Ipa Extensions";
            }
            if(type == 6){
                return "Spacing Modifier Letters";
            }
            if(type == 7){
                return "Combining Diacritical Marks";
            }
            if(type == 8){
                return "Greek And Coptic";
            }
            if(type == 9){
                return "Cyrillic";
            }
            if(type == 10){
                return "Cyrillic Supplement";
            }
            if(type == 11){
                return "Armenian";
            }
            if(type == 12){
                return "Hebrew";
            }
            if(type == 13){
                return "Arabic";
            }
            if(type == 14){
                return "Syriac";
            }
            if(type == 15){
                return "Arabic Supplement";
            }
            if(type == 16){
                return "Thaana";
            }
            if(type == 17){
                return "Nko";
            }
            if(type == 18){
                return "Samaritan";
            }
            if(type == 19){
                return "Mandaic";
            }
            if(type == 20){
                return "Syriac Supplement";
            }
            if(type == 21){
                return "Arabic Extended A";
            }
            if(type == 22){
                return "Devanagari";
            }
            if(type == 23){
                return "Bengali";
            }
            if(type == 24){
                return "Gurmukhi";
            }
            if(type == 25){
                return "Gujarati";
            }
            if(type == 26){
                return "Oriya";
            }
            if(type == 27){
                return "Tamil";
            }
            if(type == 28){
                return "Telugu";
            }
            if(type == 29){
                return "Kannada";
            }
            if(type == 30){
                return "Malayalam";
            }
            if(type == 31){
                return "Sinhala";
            }
            if(type == 32){
                return "Thai";
            }
            if(type == 33){
                return "Lao";
            }
            if(type == 34){
                return "Tibetan";
            }
            if(type == 35){
                return "Myanmar";
            }
            if(type == 36){
                return "Georgian";
            }
            if(type == 37){
                return "Hangul Jamo";
            }
            if(type == 38){
                return "Ethiopic";
            }
            if(type == 39){
                return "Ethiopic Supplement";
            }
            if(type == 40){
                return "Cherokee";
            }
            if(type == 41){
                return "Unified Canadian Aboriginal Syllabics";
            }
            if(type == 42){
                return "Ogham";
            }
            if(type == 43){
                return "Runic";
            }
            if(type == 44){
                return "Tagalog";
            }
            if(type == 45){
                return "Hanunoo";
            }
            if(type == 46){
                return "Buhid";
            }
            if(type == 47){
                return "Tagbanwa";
            }
            if(type == 48){
                return "Khmer";
            }
            if(type == 49){
                return "Mongolian";
            }
            if(type == 50){
                return "Unified Canadian Aboriginal Syllabics Extended";
            }
            if(type == 51){
                return "Limbu";
            }
            if(type == 52){
                return "Tai Le";
            }
            if(type == 53){
                return "New Tai Lue";
            }
            if(type == 54){
                return "Khmer Symbols";
            }
            if(type == 55){
                return "Buginese";
            }
            if(type == 56){
                return "Tai Tham";
            }
            if(type == 57){
                return "Combining Diacritical Marks Extended";
            }
            if(type == 58){
                return "Balinese";
            }
            if(type == 59){
                return "Sundanese";
            }
            if(type == 60){
                return "Batak";
            }
            if(type == 61){
                return "Lepcha";
            }
            if(type == 62){
                return "Ol Chiki";
            }
            if(type == 63){
                return "Cyrillic Extended C";
            }
            if(type == 64){
                return "Sundanese Supplement";
            }
            if(type == 65){
                return "Vedic Extensions";
            }
            if(type == 66){
                return "Phonetic Extensions";
            }
            if(type == 67){
                return "Phonetic Extensions Supplement";
            }
            if(type == 68){
                return "Combining Diacritical Marks Supplement";
            }
            if(type == 69){
                return "Latin Extended Additional";
            }
            if(type == 70){
                return "Greek Extended";
            }
            if(type == 71){
                return "General Punctuation";
            }
            if(type == 72){
                return "Superscripts And Subscripts";
            }
            if(type == 73){
                return "Currency Symbols";
            }
            if(type == 74){
                return "Combining Diacritical Marks For Symbols";
            }
            if(type == 75){
                return "Letterlike Symbols";
            }
            if(type == 76){
                return "Number Forms";
            }
            if(type == 77){
                return "Arrows";
            }
            if(type == 78){
                return "Mathematical Operators";
            }
            if(type == 79){
                return "Miscellaneous Technical";
            }
            if(type == 80){
                return "Control Pictures";
            }
            if(type == 81){
                return "Optical Character Recognition";
            }
            if(type == 82){
                return "Enclosed Alphanumerics";
            }
            if(type == 83){
                return "Box Drawing";
            }
            if(type == 84){
                return "Block Elements";
            }
            if(type == 85){
                return "Geometric Shapes";
            }
            if(type == 86){
                return "Miscellaneous Symbols";
            }
            if(type == 87){
                return "Dingbats";
            }
            if(type == 88){
                return "Miscellaneous Mathematical Symbols A";
            }
            if(type == 89){
                return "Supplemental Arrows A";
            }
            if(type == 90){
                return "Braille Patterns";
            }
            if(type == 91){
                return "Supplemental Arrows B";
            }
            if(type == 92){
                return "Miscellaneous Mathematical Symbols B";
            }
            if(type == 93){
                return "Supplemental Mathematical Operators";
            }
            if(type == 94){
                return "Miscellaneous Symbols And Arrows";
            }
            if(type == 95){
                return "Glagolitic";
            }
            if(type == 96){
                return "Latin Extended C";
            }
            if(type == 97){
                return "Coptic";
            }
            if(type == 98){
                return "Georgian Supplement";
            }
            if(type == 99){
                return "Tifinagh";
            }
            if(type == 100){
                return "Ethiopic Extended";
            }
            if(type == 101){
                return "Cyrillic Extended A";
            }
            if(type == 102){
                return "Supplemental Punctuation";
            }
            if(type == 103){
                return "CJK Radicals Supplement";
            }
            if(type == 104){
                return "Kangxi Radicals";
            }
            if(type == 105){
                return "Ideographic Description Characters";
            }
            if(type == 106){
                return "CJK Symbols And Punctuation";
            }
            if(type == 107){
                return "Hiragana";
            }
            if(type == 108){
                return "Katakana";
            }
            if(type == 109){
                return "Bopomofo";
            }
            if(type == 110){
                return "Hangul Compatibility Jamo";
            }
            if(type == 111){
                return "Kanbun";
            }
            if(type == 112){
                return "Bopomofo Extended";
            }
            if(type == 113){
                return "CJK Strokes";
            }
            if(type == 114){
                return "Katakana Phonetic Extensions";
            }
            if(type == 115){
                return "Enclosed CJK Letters And Months";
            }
            if(type == 116){
                return "CJK Compatibility";
            }
            if(type == 117){
                return "CJK Unified Ideographs Extension A";
            }
            if(type == 118){
                return "Yijing Hexagram Symbols";
            }
            if(type == 119){
                return "CJK Unified Ideographs";
            }
            if(type == 120){
                return "Yi Syllables";
            }
            if(type == 121){
                return "Yi Radicals";
            }
            if(type == 122){
                return "Lisu";
            }
            if(type == 123){
                return "Vai";
            }
            if(type == 124){
                return "Cyrillic Extended B";
            }
            if(type == 125){
                return "Bamum";
            }
            if(type == 126){
                return "Modifier Tone Letters";
            }
            if(type == 127){
                return "Latin Extended D";
            }
            if(type == 128){
                return "Syloti Nagri";
            }
            if(type == 129){
                return "Common Indic Number Forms";
            }
            if(type == 130){
                return "Phags Pa";
            }
            if(type == 131){
                return "Saurashtra";
            }
            if(type == 132){
                return "Devanagari Extended";
            }
            if(type == 133){
                return "Kayah Li";
            }
            if(type == 134){
                return "Rejang";
            }
            if(type == 135){
                return "Hangul Jamo Extended A";
            }
            if(type == 136){
                return "Javanese";
            }
            if(type == 137){
                return "Myanmar Extended B";
            }
            if(type == 138){
                return "Cham";
            }
            if(type == 139){
                return "Myanmar Extended A";
            }
            if(type == 140){
                return "Tai Viet";
            }
            if(type == 141){
                return "Meetei Mayek Extensions";
            }
            if(type == 142){
                return "Ethiopic Extended A";
            }
            if(type == 143){
                return "Latin Extended E";
            }
            if(type == 144){
                return "Cherokee Supplement";
            }
            if(type == 145){
                return "Meetei Mayek";
            }
            if(type == 146){
                return "Hangul Syllables";
            }
            if(type == 147){
                return "Hangul Jamo Extended B";
            }
            if(type == 148){
                return "High Surrogates";
            }
            if(type == 149){
                return "High Private Use Surrogates";
            }
            if(type == 150){
                return "Low Surrogates";
            }
            if(type == 151){
                return "Private Use Area";
            }
            if(type == 152){
                return "CJK Compatibility Ideographs";
            }
            if(type == 153){
                return "Alphabetic Presentation Forms";
            }
            if(type == 154){
                return "Arabic Presentation Forms A";
            }
            if(type == 155){
                return "Variation Selectors";
            }
            if(type == 156){
                return "Vertical Forms";
            }
            if(type == 157){
                return "Combining Half Marks";
            }
            if(type == 158){
                return "CJK Compatibility Forms";
            }
            if(type == 159){
                return "Small Form Variants";
            }
            if(type == 160){
                return "Arabic Presentation Forms B";
            }
            if(type == 161){
                return "Halfwidth And Fullwidth Forms";
            }
            if(type == 162){
                return "Specials";
            }
            if(type == 163){
                return "Linear B Syllabary";
            }
            if(type == 164){
                return "Linear B Ideograms";
            }
            if(type == 165){
                return "Aegean Numbers";
            }
            if(type == 166){
                return "Ancient Greek Numbers";
            }
            if(type == 167){
                return "Ancient Symbols";
            }
            if(type == 168){
                return "Phaistos Disc";
            }
            if(type == 169){
                return "Lycian";
            }
            if(type == 170){
                return "Carian";
            }
            if(type == 171){
                return "Coptic Epact Numbers";
            }
            if(type == 172){
                return "Old Italic";
            }
            if(type == 173){
                return "Gothic";
            }
            if(type == 174){
                return "Old Permic";
            }
            if(type == 175){
                return "Ugaritic";
            }
            if(type == 176){
                return "Old Persian";
            }
            if(type == 177){
                return "Deseret";
            }
            if(type == 178){
                return "Shavian";
            }
            if(type == 179){
                return "Osmanya";
            }
            if(type == 180){
                return "Osage";
            }
            if(type == 181){
                return "Elbasan";
            }
            if(type == 182){
                return "Caucasian Albanian";
            }
            if(type == 183){
                return "Linear A";
            }
            if(type == 184){
                return "Cypriot Syllabary";
            }
            if(type == 185){
                return "Imperial Aramaic";
            }
            if(type == 186){
                return "Palmyrene";
            }
            if(type == 187){
                return "Nabataean";
            }
            if(type == 188){
                return "Hatran";
            }
            if(type == 189){
                return "Phoenician";
            }
            if(type == 190){
                return "Lydian";
            }
            if(type == 191){
                return "Meroitic Hieroglyphs";
            }
            if(type == 192){
                return "Meroitic Cursive";
            }
            if(type == 193){
                return "Kharoshthi";
            }
            if(type == 194){
                return "Old South Arabian";
            }
            if(type == 195){
                return "Old North Arabian";
            }
            if(type == 196){
                return "Manichaean";
            }
            if(type == 197){
                return "Avestan";
            }
            if(type == 198){
                return "Inscriptional Parthian";
            }
            if(type == 199){
                return "Inscriptional Pahlavi";
            }
            if(type == 200){
                return "Psalter Pahlavi";
            }
            if(type == 201){
                return "Old Turkic";
            }
            if(type == 202){
                return "Old Hungarian";
            }
            if(type == 203){
                return "Rumi Numeral Symbols";
            }
            if(type == 204){
                return "Brahmi";
            }
            if(type == 205){
                return "Kaithi";
            }
            if(type == 206){
                return "Sora Sompeng";
            }
            if(type == 207){
                return "Chakma";
            }
            if(type == 208){
                return "Mahajani";
            }
            if(type == 209){
                return "Sharada";
            }
            if(type == 210){
                return "Sinhala Archaic Numbers";
            }
            if(type == 211){
                return "Khojki";
            }
            if(type == 212){
                return "Multani";
            }
            if(type == 213){
                return "Khudawadi";
            }
            if(type == 214){
                return "Grantha";
            }
            if(type == 215){
                return "Newa";
            }
            if(type == 216){
                return "Tirhuta";
            }
            if(type == 217){
                return "Siddham";
            }
            if(type == 218){
                return "Modi";
            }
            if(type == 219){
                return "Mongolian Supplement";
            }
            if(type == 220){
                return "Takri";
            }
            if(type == 221){
                return "Ahom";
            }
            if(type == 222){
                return "Warang Citi";
            }
            if(type == 223){
                return "Zanabazar Square";
            }
            if(type == 224){
                return "Soyombo";
            }
            if(type == 225){
                return "Pau Cin Hau";
            }
            if(type == 226){
                return "Bhaiksuki";
            }
            if(type == 227){
                return "Marchen";
            }
            if(type == 228){
                return "Masaram Gondi";
            }
            if(type == 229){
                return "Cuneiform";
            }
            if(type == 230){
                return "Cuneiform Numbers And Punctuation";
            }
            if(type == 231){
                return "Early Dynastic Cuneiform";
            }
            if(type == 232){
                return "Egyptian Hieroglyphs";
            }
            if(type == 233){
                return "Anatolian Hieroglyphs";
            }
            if(type == 234){
                return "Bamum Supplement";
            }
            if(type == 235){
                return "Mro";
            }
            if(type == 236){
                return "Bassa Vah";
            }
            if(type == 237){
                return "Pahawh Hmong";
            }
            if(type == 238){
                return "Miao";
            }
            if(type == 239){
                return "Ideographic Symbols And Punctuation";
            }
            if(type == 240){
                return "Tangut";
            }
            if(type == 241){
                return "Tangut Components";
            }
            if(type == 242){
                return "Kana Supplement";
            }
            if(type == 243){
                return "Kana Extended A";
            }
            if(type == 244){
                return "Nushu";
            }
            if(type == 245){
                return "Duployan";
            }
            if(type == 246){
                return "Shorthand Format Controls";
            }
            if(type == 247){
                return "Byzantine Musical Symbols";
            }
            if(type == 248){
                return "Musical Symbols";
            }
            if(type == 249){
                return "Ancient Greek Musical Notation";
            }
            if(type == 250){
                return "Tai Xuan Jing Symbols";
            }
            if(type == 251){
                return "Counting Rod Numerals";
            }
            if(type == 252){
                return "Mathematical Alphanumeric Symbols";
            }
            if(type == 253){
                return "Sutton Signwriting";
            }
            if(type == 254){
                return "Glagolitic Supplement";
            }
            if(type == 255){
                return "Mende Kikakui";
            }
            if(type == 256){
                return "Adlam";
            }
            if(type == 257){
                return "Arabic Mathematical Alphabetic Symbols";
            }
            if(type == 258){
                return "Mahjong Tiles";
            }
            if(type == 259){
                return "Domino Tiles";
            }
            if(type == 260){
                return "Playing Cards";
            }
            if(type == 261){
                return "Enclosed Alphanumeric Supplement";
            }
            if(type == 262){
                return "Enclosed Ideographic Supplement";
            }
            if(type == 263){
                return "Miscellaneous Symbols And Pictographs";
            }
            if(type == 264){
                return "Emoticons";
            }
            if(type == 265){
                return "Ornamental Dingbats";
            }
            if(type == 266){
                return "Transport And Map Symbols";
            }
            if(type == 267){
                return "Alchemical Symbols";
            }
            if(type == 268){
                return "Geometric Shapes Extended";
            }
            if(type == 269){
                return "Supplemental Arrows C";
            }
            if(type == 270){
                return "Supplemental Symbols And Pictographs";
            }
            if(type == 271){
                return "CJK Unified Ideographs Extension B";
            }
            if(type == 272){
                return "CJK Unified Ideographs Extension C";
            }
            if(type == 273){
                return "CJK Unified Ideographs Extension D";
            }
            if(type == 274){
                return "CJK Unified Ideographs Extension E";
            }
            if(type == 275){
                return "CJK Unified Ideographs Extension F";
            }
            if(type == 276){
                return "CJK Compatibility Ideographs Supplement";
            }
            if(type == 277){
                return "Tags";
            }
            if(type == 278){
                return "Variation Selectors Supplement";
            }
            if(type == 279){
                return "Supplementary Private Use Area A";
            }
            if(type == 280){
                return "Supplementary Private Use Area B";
            }
            return "Unknown";
        }
    } // namespace chartype
} // namespace npylm