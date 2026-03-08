#include "vietnamese.h"
#include <algorithm>
#include <cctype>

namespace vi {

// Vietnamese Unicode character table
// Organized by: base vowel -> diacritic -> tone
// Tones: 0=none, 1=sắc, 2=huyền, 3=hỏi, 4=ngã, 5=nặng

// clang-format off
struct CharMapping {
    char32_t base;
    int diacritic;
    char32_t tones[6]; // indexed by tone (0-5)
};

static const CharMapping kCharMap[] = {
    // a: no diacritic
    {'a', 0, {U'a', U'\u00e1', U'\u00e0', U'\u1ea3', U'\u00e3', U'\u1ea1'}},
    // â: circumflex
    {'a', 1, {U'\u00e2', U'\u1ea5', U'\u1ea7', U'\u1ea9', U'\u1eab', U'\u1ead'}},
    // ă: breve
    {'a', 2, {U'\u0103', U'\u1eaf', U'\u1eb1', U'\u1eb3', U'\u1eb5', U'\u1eb7'}},

    // e: no diacritic
    {'e', 0, {U'e', U'\u00e9', U'\u00e8', U'\u1ebb', U'\u1ebd', U'\u1eb9'}},
    // ê: circumflex
    {'e', 1, {U'\u00ea', U'\u1ebf', U'\u1ec1', U'\u1ec3', U'\u1ec5', U'\u1ec7'}},

    // i: no diacritic
    {'i', 0, {U'i', U'\u00ed', U'\u00ec', U'\u1ec9', U'\u0129', U'\u1ecb'}},

    // o: no diacritic
    {'o', 0, {U'o', U'\u00f3', U'\u00f2', U'\u1ecf', U'\u00f5', U'\u1ecd'}},
    // ô: circumflex
    {'o', 1, {U'\u00f4', U'\u1ed1', U'\u1ed3', U'\u1ed5', U'\u1ed7', U'\u1ed9'}},
    // ơ: horn
    {'o', 3, {U'\u01a1', U'\u1edb', U'\u1edd', U'\u1edf', U'\u1ee1', U'\u1ee3'}},

    // u: no diacritic
    {'u', 0, {U'u', U'\u00fa', U'\u00f9', U'\u1ee7', U'\u0169', U'\u1ee5'}},
    // ư: horn
    {'u', 3, {U'\u01b0', U'\u1ee9', U'\u1eeb', U'\u1eed', U'\u1eef', U'\u1ef1'}},

    // y: no diacritic
    {'y', 0, {U'y', U'\u00fd', U'\u1ef3', U'\u1ef7', U'\u1ef9', U'\u1ef5'}},

    // d: stroke
    {'d', 4, {U'\u0111', U'\u0111', U'\u0111', U'\u0111', U'\u0111', U'\u0111'}},
};

// Uppercase versions
static const CharMapping kCharMapUpper[] = {
    {'A', 0, {U'A', U'\u00c1', U'\u00c0', U'\u1ea2', U'\u00c3', U'\u1ea0'}},
    {'A', 1, {U'\u00c2', U'\u1ea4', U'\u1ea6', U'\u1ea8', U'\u1eaa', U'\u1eac'}},
    {'A', 2, {U'\u0102', U'\u1eae', U'\u1eb0', U'\u1eb2', U'\u1eb4', U'\u1eb6'}},

    {'E', 0, {U'E', U'\u00c9', U'\u00c8', U'\u1eba', U'\u1ebc', U'\u1eb8'}},
    {'E', 1, {U'\u00ca', U'\u1ebe', U'\u1ec0', U'\u1ec2', U'\u1ec4', U'\u1ec6'}},

    {'I', 0, {U'I', U'\u00cd', U'\u00cc', U'\u1ec8', U'\u0128', U'\u1eca'}},

    {'O', 0, {U'O', U'\u00d3', U'\u00d2', U'\u1ece', U'\u00d5', U'\u1ecc'}},
    {'O', 1, {U'\u00d4', U'\u1ed0', U'\u1ed2', U'\u1ed4', U'\u1ed6', U'\u1ed8'}},
    {'O', 3, {U'\u01a0', U'\u1eda', U'\u1edc', U'\u1ede', U'\u1ee0', U'\u1ee2'}},

    {'U', 0, {U'U', U'\u00da', U'\u00d9', U'\u1ee6', U'\u0168', U'\u1ee4'}},
    {'U', 3, {U'\u01af', U'\u1ee8', U'\u1eea', U'\u1eec', U'\u1eee', U'\u1ef0'}},

    {'Y', 0, {U'Y', U'\u00dd', U'\u1ef2', U'\u1ef6', U'\u1ef8', U'\u1ef4'}},

    {'D', 4, {U'\u0110', U'\u0110', U'\u0110', U'\u0110', U'\u0110', U'\u0110'}},
};
// clang-format on

static const CharMapping *findMapping(char32_t base, int diacritic, bool upper) {
    const auto *map = upper ? kCharMapUpper : kCharMap;
    int size = upper ? (int)(sizeof(kCharMapUpper) / sizeof(kCharMapUpper[0]))
                     : (int)(sizeof(kCharMap) / sizeof(kCharMap[0]));
    for (int i = 0; i < size; i++) {
        if (map[i].base == base && map[i].diacritic == diacritic) {
            return &map[i];
        }
    }
    return nullptr;
}

char32_t VChar::toUnicode() const {
    bool upper = std::isupper(static_cast<unsigned char>(base));
    char32_t lookupBase = upper ? base : base;
    const auto *m = findMapping(lookupBase, diacritic, upper);
    if (m && tone >= 0 && tone <= 5) {
        return m->tones[tone];
    }
    // Fallback: return base character
    return base;
}

bool VChar::isVowel() const {
    char lower = std::tolower(static_cast<unsigned char>(base));
    return lower == 'a' || lower == 'e' || lower == 'i' || lower == 'o' ||
           lower == 'u' || lower == 'y';
}

std::string utf8Encode(char32_t cp) {
    std::string result;
    if (cp < 0x80) {
        result += static_cast<char>(cp);
    } else if (cp < 0x800) {
        result += static_cast<char>(0xC0 | (cp >> 6));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else if (cp < 0x10000) {
        result += static_cast<char>(0xE0 | (cp >> 12));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    } else {
        result += static_cast<char>(0xF0 | (cp >> 18));
        result += static_cast<char>(0x80 | ((cp >> 12) & 0x3F));
        result += static_cast<char>(0x80 | ((cp >> 6) & 0x3F));
        result += static_cast<char>(0x80 | (cp & 0x3F));
    }
    return result;
}

ViEngine::ViEngine(InputMethod method) : method_(method) {}

bool ViEngine::processKey(char key) {
    if (method_ == InputMethod::Telex) {
        return processTelexKey(key);
    }
    return processVNIKey(key);
}

std::string ViEngine::getPreedit() const { return toUtf8(); }

std::string ViEngine::commit() {
    std::string result = toUtf8();
    reset();
    return result;
}

void ViEngine::reset() {
    chars_.clear();
    rawKeys_.clear();
}

std::string ViEngine::toUtf8() const {
    std::string result;
    for (const auto &ch : chars_) {
        result += utf8Encode(ch.toUnicode());
    }
    return result;
}

bool ViEngine::hasVietnameseVowel() const {
    for (const auto &ch : chars_) {
        if (ch.isVowel()) {
            return true;
        }
    }
    return false;
}

int ViEngine::findToneTarget() const {
    // Vietnamese tone placement rules:
    // 1. If there's a vowel with diacritic (circumflex, breve, horn), place tone there
    // 2. If single vowel, place it there
    // 3. For vowel clusters: find the right vowel based on rules

    std::vector<int> vowelPositions;
    int specialVowel = -1;

    for (int i = 0; i < (int)chars_.size(); i++) {
        if (chars_[i].isVowel()) {
            vowelPositions.push_back(i);
            if (chars_[i].diacritic != 0) {
                specialVowel = i;
            }
        }
    }

    if (vowelPositions.empty()) {
        return -1;
    }

    // If there's a vowel with a diacritic mark, tone goes on it
    if (specialVowel >= 0) {
        return specialVowel;
    }

    // Single vowel
    if (vowelPositions.size() == 1) {
        return vowelPositions[0];
    }

    // Check if there's a final consonant after the vowel cluster
    bool hasFinalConsonant = false;
    if (!vowelPositions.empty()) {
        int lastVowelPos = vowelPositions.back();
        for (int i = lastVowelPos + 1; i < (int)chars_.size(); i++) {
            if (!chars_[i].isVowel()) {
                hasFinalConsonant = true;
                break;
            }
        }
    }

    // Handle 'qu' - u after q is part of consonant
    if (vowelPositions.size() >= 2) {
        int first = vowelPositions[0];
        if (first > 0) {
            char prevLower =
                std::tolower(static_cast<unsigned char>(chars_[first - 1].base));
            char vowelLower =
                std::tolower(static_cast<unsigned char>(chars_[first].base));
            if (prevLower == 'q' && vowelLower == 'u') {
                // Remove 'u' from vowel positions, it's part of 'qu' consonant
                std::vector<int> adjusted(vowelPositions.begin() + 1,
                                          vowelPositions.end());
                if (adjusted.size() == 1) {
                    return adjusted[0];
                }
                if (adjusted.size() >= 2) {
                    // Apply diphthong rules to remaining vowels
                    return hasFinalConsonant ? adjusted[adjusted.size() - 1]
                                            : adjusted[0];
                }
            }
        }
    }

    // For diphthongs/triphthongs:
    // With final consonant -> tone on last vowel in the vowel cluster
    // Without final consonant -> tone on second-to-last vowel
    if (vowelPositions.size() >= 2) {
        if (hasFinalConsonant) {
            // Tone on the second vowel (penultimate in the cluster before consonant)
            if (vowelPositions.size() >= 3) {
                return vowelPositions[1]; // triphthong: middle vowel
            }
            return vowelPositions[1]; // diphthong with final consonant
        }
        // No final consonant: tone on first vowel of the cluster
        return vowelPositions[0];
    }

    return vowelPositions[0];
}

int ViEngine::findDiacriticTarget(int diacritic) const {
    // Find the last vowel that can accept this diacritic
    for (int i = (int)chars_.size() - 1; i >= 0; i--) {
        if (!chars_[i].isVowel()) {
            continue;
        }
        char lower = std::tolower(static_cast<unsigned char>(chars_[i].base));

        switch (diacritic) {
        case 1: // circumflex: a, e, o
            if (lower == 'a' || lower == 'e' || lower == 'o') {
                return i;
            }
            break;
        case 2: // breve: a only
            if (lower == 'a') {
                return i;
            }
            break;
        case 3: // horn: o, u
            if (lower == 'o' || lower == 'u') {
                return i;
            }
            break;
        }
    }
    return -1;
}

bool ViEngine::applyDiacritic(int diacritic) {
    int pos = findDiacriticTarget(diacritic);
    if (pos < 0) {
        return false;
    }

    // Toggle: if same diacritic already applied, remove it
    if (chars_[pos].diacritic == diacritic) {
        chars_[pos].diacritic = 0;
    } else {
        chars_[pos].diacritic = diacritic;
    }
    return true;
}

bool ViEngine::applyTone(int tone) {
    int pos = findToneTarget();
    if (pos < 0) {
        return false;
    }

    // Toggle: if same tone already applied, remove it
    if (chars_[pos].tone == tone) {
        chars_[pos].tone = 0;
    } else {
        chars_[pos].tone = tone;
    }
    return true;
}

bool ViEngine::processTelexKey(char key) {
    char lower = std::tolower(static_cast<unsigned char>(key));

    // Tone marks
    if (hasVietnameseVowel()) {
        int tone = -1;
        switch (lower) {
        case 's':
            tone = 1;
            break; // sắc
        case 'f':
            tone = 2;
            break; // huyền
        case 'r':
            tone = 3;
            break; // hỏi
        case 'x':
            tone = 4;
            break; // ngã
        case 'j':
            tone = 5;
            break; // nặng
        case 'z':
            tone = 0;
            break; // remove tone
        }
        if (tone >= 0) {
            if (applyTone(tone)) {
                rawKeys_ += key;
                return true;
            }
        }
    }

    // Diacritics via doubling: aa->â, ee->ê, oo->ô
    if ((lower == 'a' || lower == 'e' || lower == 'o') && !chars_.empty()) {
        auto &last = chars_.back();
        char lastLower = std::tolower(static_cast<unsigned char>(last.base));
        if (lastLower == lower && last.diacritic == 0) {
            last.diacritic = 1; // circumflex
            rawKeys_ += key;
            return true;
        }
        // Undo circumflex: typing the letter a third time
        if (lastLower == lower && last.diacritic == 1) {
            last.diacritic = 0;
            // Add the letter as a new character
            bool isUpper = std::isupper(static_cast<unsigned char>(key));
            chars_.push_back(
                VChar{static_cast<char32_t>(isUpper ? std::toupper(lower) : lower),
                      0, 0});
            rawKeys_ += key;
            return true;
        }
    }

    // w -> breve for 'a', horn for 'o'/'u'
    if (lower == 'w' && !chars_.empty()) {
        // Try horn first (o, u), then breve (a)
        // Look for the last applicable vowel
        for (int i = (int)chars_.size() - 1; i >= 0; i--) {
            char baseLower = std::tolower(static_cast<unsigned char>(chars_[i].base));
            if (baseLower == 'u' || baseLower == 'o') {
                if (chars_[i].diacritic == 3) {
                    chars_[i].diacritic = 0; // toggle off
                } else {
                    chars_[i].diacritic = 3; // horn
                }
                rawKeys_ += key;
                return true;
            }
            if (baseLower == 'a') {
                if (chars_[i].diacritic == 2) {
                    chars_[i].diacritic = 0; // toggle off
                } else {
                    chars_[i].diacritic = 2; // breve
                }
                rawKeys_ += key;
                return true;
            }
            // Stop at consonants
            if (!chars_[i].isVowel()) {
                break;
            }
        }
        // 'w' with no applicable vowel - could be start of 'ư'
        // Standalone w = ư
        if (chars_.empty() || !chars_.back().isVowel()) {
            bool isUpper = std::isupper(static_cast<unsigned char>(key));
            chars_.push_back(
                VChar{static_cast<char32_t>(isUpper ? 'U' : 'u'), 3, 0});
            rawKeys_ += key;
            return true;
        }
    }

    // dd -> đ
    if (lower == 'd' && !chars_.empty()) {
        auto &last = chars_.back();
        char lastLower = std::tolower(static_cast<unsigned char>(last.base));
        if (lastLower == 'd' && last.diacritic == 0) {
            last.diacritic = 4; // stroke
            rawKeys_ += key;
            return true;
        }
        if (lastLower == 'd' && last.diacritic == 4) {
            // Undo stroke, add new 'd'
            last.diacritic = 0;
            bool isUpper = std::isupper(static_cast<unsigned char>(key));
            chars_.push_back(
                VChar{static_cast<char32_t>(isUpper ? 'D' : 'd'), 0, 0});
            rawKeys_ += key;
            return true;
        }
    }

    // Regular letter: add to buffer
    if (std::isalpha(static_cast<unsigned char>(key))) {
        chars_.push_back(VChar{static_cast<char32_t>(key), 0, 0});
        rawKeys_ += key;
        return true;
    }

    return false;
}

bool ViEngine::processVNIKey(char key) {
    // VNI uses number keys for tones and diacritics
    if (key >= '0' && key <= '9' && !chars_.empty()) {
        switch (key) {
        case '1': // sắc
            if (hasVietnameseVowel() && applyTone(1)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '2': // huyền
            if (hasVietnameseVowel() && applyTone(2)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '3': // hỏi
            if (hasVietnameseVowel() && applyTone(3)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '4': // ngã
            if (hasVietnameseVowel() && applyTone(4)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '5': // nặng
            if (hasVietnameseVowel() && applyTone(5)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '6': // circumflex (â, ê, ô)
            if (applyDiacritic(1)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '7': // horn (ơ, ư)
            if (applyDiacritic(3)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '8': // breve (ă)
            if (applyDiacritic(2)) {
                rawKeys_ += key;
                return true;
            }
            break;
        case '9': { // stroke (đ)
            // Find last 'd'
            for (int i = (int)chars_.size() - 1; i >= 0; i--) {
                char baseLower =
                    std::tolower(static_cast<unsigned char>(chars_[i].base));
                if (baseLower == 'd') {
                    if (chars_[i].diacritic == 4) {
                        chars_[i].diacritic = 0; // toggle off
                    } else {
                        chars_[i].diacritic = 4; // stroke
                    }
                    rawKeys_ += key;
                    return true;
                }
            }
            break;
        }
        case '0': // remove tone
            if (hasVietnameseVowel() && applyTone(0)) {
                rawKeys_ += key;
                return true;
            }
            break;
        }
    }

    // Regular letter
    if (std::isalpha(static_cast<unsigned char>(key))) {
        chars_.push_back(VChar{static_cast<char32_t>(key), 0, 0});
        rawKeys_ += key;
        return true;
    }

    return false;
}

} // namespace vi
