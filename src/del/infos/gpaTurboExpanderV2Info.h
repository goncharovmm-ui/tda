#ifndef GPA_TURBOEXPANDER_V2_INFO_H
#define GPA_TURBOEXPANDER_V2_INFO_H

#include "gpaDeviceInfo.h"
#include "gpaTurboExpanderV2.h"

class gpaTurboExpanderV2Info : public gpaDeviceInfo
{
public:
    gpaTurboExpanderV2Info();
    gpaUInt getNumMixCircuits() const override { return 2; }
    gpaUInt getMinNumMixPorts(gpaUInt index) const override { return index < 2 ? 2 : 0; }
    gpaUInt getMaxNumMixPorts(gpaUInt index) const override { return index < 2 ? 2 : 0; }
    const gpaString* getMixCircuitName(gpaUInt index) const override;
    const gpaString* getMixPortName(gpaUInt circuit, gpaUInt port) const override;
    gpaElement* createDevice(const gpaCoreInterfaces* interfaces, gpaCoreReference ref,
                             const gpaExtObjectModel* model) const override;
};

#endif // GPA_TURBOEXPANDER_V2_INFO_H
