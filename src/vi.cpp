#include "vi.h"
#include <fcitx-utils/utf8.h>
#include <fcitx/inputcontext.h>
#include <fcitx/inputpanel.h>

namespace fcitx {

ViEngine::ViEngine(Instance *instance)
    : instance_(instance),
      factory_([this](InputContext &ic) { return new ViState(this, &ic); }) {
    instance_->inputContextManager().registerProperty("viState", &factory_);
}

void ViEngine::keyEvent(const InputMethodEntry &entry,
                        KeyEvent &keyEvent) {
    auto *state = keyEvent.inputContext()->propertyFor(&factory_);
    state->keyEvent(entry, keyEvent);
}

void ViEngine::activate(const InputMethodEntry &entry,
                        InputContextEvent &event) {
    auto *state = event.inputContext()->propertyFor(&factory_);
    // Set input method based on entry name
    if (entry.uniqueName() == "vi-vni") {
        state = event.inputContext()->propertyFor(&factory_);
        // VNI mode is handled via entry name check in keyEvent
    }
    state->reset();
}

void ViEngine::deactivate(const InputMethodEntry & /*entry*/,
                          InputContextEvent &event) {
    auto *state = event.inputContext()->propertyFor(&factory_);
    state->commitBuffer();
    state->reset();
}

void ViEngine::reset(const InputMethodEntry & /*entry*/,
                     InputContextEvent &event) {
    auto *state = event.inputContext()->propertyFor(&factory_);
    state->reset();
}

// --- ViState ---

ViState::ViState(ViEngine *engine, InputContext *ic)
    : engine_(engine), ic_(ic), viEngine_(vi::InputMethod::Telex) {}

void ViState::reset() {
    viEngine_.reset();
    ic_->inputPanel().reset();
    ic_->updatePreedit();
    ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
}

void ViState::commitBuffer() {
    if (!viEngine_.empty()) {
        std::string text = viEngine_.commit();
        ic_->commitString(text);
    }
}

void ViState::updatePreedit() {
    auto &inputPanel = ic_->inputPanel();
    inputPanel.reset();

    if (!viEngine_.empty()) {
        std::string preedit = viEngine_.getPreedit();
        Text preeditText;
        preeditText.append(preedit,
                           TextFormatFlag::Underline);
        preeditText.setCursor(preedit.size());

        if (ic_->capabilityFlags().test(CapabilityFlag::Preedit)) {
            inputPanel.setClientPreedit(preeditText);
        } else {
            inputPanel.setPreedit(preeditText);
        }
    }

    ic_->updatePreedit();
    ic_->updateUserInterface(UserInterfaceComponent::InputPanel);
}

void ViState::keyEvent(const InputMethodEntry &entry,
                       KeyEvent &keyEvent) {
    // Ignore key releases
    if (keyEvent.isRelease()) {
        return;
    }

    // Set the input method mode based on entry
    if (entry.uniqueName() == "vi-vni") {
        viEngine_.setMethod(vi::InputMethod::VNI);
    } else {
        viEngine_.setMethod(vi::InputMethod::Telex);
    }

    auto key = keyEvent.key();

    // Handle modifier keys - let them pass through
    if (key.hasModifier()) {
        // But first commit any pending buffer if Ctrl/Alt is pressed
        if (!viEngine_.empty()) {
            commitBuffer();
            reset();
        }
        return;
    }

    // Backspace: delete last character from buffer
    if (key.check(Key(FcitxKey_BackSpace))) {
        if (!viEngine_.empty()) {
            // Re-process: reset and replay all keys except the last
            // For simplicity, just reset for now
            reset();
            keyEvent.filterAndAccept();
            return;
        }
        return; // Let backspace pass through when buffer is empty
    }

    // Enter: commit buffer and pass through
    if (key.check(Key(FcitxKey_Return)) || key.check(Key(FcitxKey_KP_Enter))) {
        if (!viEngine_.empty()) {
            commitBuffer();
            reset();
        }
        return; // Let Enter pass through
    }

    // Escape: discard buffer
    if (key.check(Key(FcitxKey_Escape))) {
        if (!viEngine_.empty()) {
            reset();
            keyEvent.filterAndAccept();
            return;
        }
        return;
    }

    // Space: commit and pass through
    if (key.check(Key(FcitxKey_space))) {
        if (!viEngine_.empty()) {
            commitBuffer();
            reset();
        }
        return; // Let space pass through
    }

    // Tab: commit and pass through
    if (key.check(Key(FcitxKey_Tab))) {
        if (!viEngine_.empty()) {
            commitBuffer();
            reset();
        }
        return;
    }

    // Get the character for this key
    auto sym = key.sym();
    char ch = 0;

    // Map keysym to ASCII character
    if (sym >= FcitxKey_a && sym <= FcitxKey_z) {
        ch = 'a' + (sym - FcitxKey_a);
    } else if (sym >= FcitxKey_A && sym <= FcitxKey_Z) {
        ch = 'A' + (sym - FcitxKey_A);
    } else if (sym >= FcitxKey_0 && sym <= FcitxKey_9) {
        ch = '0' + (sym - FcitxKey_0);
    }

    if (ch == 0) {
        // Non-letter, non-digit key: commit buffer and pass through
        if (!viEngine_.empty()) {
            commitBuffer();
            reset();
        }
        return;
    }

    // Try to process the key through the Vietnamese engine
    if (viEngine_.processKey(ch)) {
        updatePreedit();
        keyEvent.filterAndAccept();
        return;
    }

    // Key was not consumed by Vietnamese engine
    // Commit current buffer, start fresh with this character
    if (!viEngine_.empty()) {
        commitBuffer();
        viEngine_.reset();
    }

    // Try again with empty buffer
    if (viEngine_.processKey(ch)) {
        updatePreedit();
        keyEvent.filterAndAccept();
        return;
    }

    // Not a processable key at all, let it through
    return;
}

} // namespace fcitx

class ViEngineFactory : public fcitx::AddonFactory {
    fcitx::AddonInstance *create(fcitx::AddonManager *manager) override {
        return new fcitx::ViEngine(manager->instance());
    }
};

FCITX_ADDON_FACTORY(ViEngineFactory);
