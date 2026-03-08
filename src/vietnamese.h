#ifndef FCITX5_VI_VIETNAMESE_H
#define FCITX5_VI_VIETNAMESE_H

#include <string>
#include <vector>

namespace vi {

enum class InputMethod { Telex, VNI };

// Represents a Vietnamese character: base vowel/consonant + diacritic + tone
struct VChar {
    char32_t base;     // ASCII base letter
    int diacritic;     // 0=none, 1=circumflex(â), 2=breve(ă), 3=horn(ơ,ư), 4=stroke(đ)
    int tone;          // 0=none, 1=sắc, 2=huyền, 3=hỏi, 4=ngã, 5=nặng

    char32_t toUnicode() const;
    bool isVowel() const;
};

class ViEngine {
public:
    explicit ViEngine(InputMethod method = InputMethod::Telex);

    // Process a key press. Returns true if the key was consumed.
    bool processKey(char key);

    // Get the current composed string (UTF-8)
    std::string getPreedit() const;

    // Commit: returns the final string and resets
    std::string commit();

    // Reset without committing
    void reset();

    // Check if buffer is empty
    bool empty() const { return chars_.empty(); }

    void setMethod(InputMethod method) { method_ = method; }

private:
    InputMethod method_;
    std::vector<VChar> chars_;
    std::string rawKeys_;

    bool processTelexKey(char key);
    bool processVNIKey(char key);

    // Apply a diacritic to the appropriate character
    bool applyDiacritic(int diacritic);

    // Apply a tone mark to the appropriate vowel
    bool applyTone(int tone);

    // Find the vowel position where tone should be placed
    int findToneTarget() const;

    // Find the last vowel that can take a diacritic
    int findDiacriticTarget(int diacritic) const;

    // Check if the current buffer forms a valid Vietnamese syllable start
    bool hasVietnameseVowel() const;

    // Convert chars_ to UTF-8 string
    std::string toUtf8() const;
};

// Utility: encode a single Unicode codepoint to UTF-8
std::string utf8Encode(char32_t cp);

} // namespace vi

#endif // FCITX5_VI_VIETNAMESE_H
