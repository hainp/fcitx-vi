#ifndef FCITX5_VI_VI_H
#define FCITX5_VI_VI_H

#include "vietnamese.h"
#include <fcitx/addonfactory.h>
#include <fcitx/addoninstance.h>
#include <fcitx/addonmanager.h>
#include <fcitx/inputcontextproperty.h>
#include <fcitx/inputmethodengine.h>
#include <fcitx/instance.h>

namespace fcitx {

class ViState;

class ViEngine : public InputMethodEngineV2 {
public:
    ViEngine(Instance *instance);

    void keyEvent(const InputMethodEntry &entry,
                  KeyEvent &keyEvent) override;
    void activate(const InputMethodEntry &entry,
                  InputContextEvent &event) override;
    void deactivate(const InputMethodEntry &entry,
                    InputContextEvent &event) override;
    void reset(const InputMethodEntry &entry,
               InputContextEvent &event) override;

    Instance *instance() { return instance_; }

private:
    Instance *instance_;
    FactoryFor<ViState> factory_;
};

class ViState : public InputContextProperty {
public:
    ViState(ViEngine *engine, InputContext *ic);

    void keyEvent(const InputMethodEntry &entry, KeyEvent &keyEvent);
    void reset();
    void commitBuffer();
    void updatePreedit();

private:
    ViEngine *engine_;
    InputContext *ic_;
    vi::ViEngine viEngine_;
};

} // namespace fcitx

#endif // FCITX5_VI_VI_H
