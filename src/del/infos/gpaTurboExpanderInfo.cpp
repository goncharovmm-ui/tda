#include "gpaTurboExpanderInfo.h"
#include "gpaTurboExpander.h"

gpaTurboExpanderInfo::gpaTurboExpanderInfo() : gpaDeviceInfo(gpaTurboExpander::getDeviceTypeName(),
                                                             gpaTurboExpander::getDeviceDescription())
{
    setParameters(gpaTurboExpander::getParametersInfo());
    setSensors(gpaTurboExpander::getSensorsInfo());
}

gpaUInt gpaTurboExpanderInfo::getNumMixCircuits() const
{
    return gpaTurboExpander::getDeviceNumMixCircuits();
}

gpaUInt gpaTurboExpanderInfo::getMinNumMixPorts(gpaUInt index) const
{
    return gpaTurboExpander::getDeviceMinNumMixPorts(index);
}

gpaUInt gpaTurboExpanderInfo::getMaxNumMixPorts(gpaUInt index) const
{
    return gpaTurboExpander::getDeviceMaxNumMixPorts(index);
}

const gpaString *gpaTurboExpanderInfo::getMixPortName(gpaUInt circIdx, gpaUInt portIdx) const
{
    return gpaTurboExpander::getDeviceMixPortName(circIdx, portIdx);
}

const gpaString *gpaTurboExpanderInfo::getMixCircuitName(gpaUInt index) const
{
    return gpaTurboExpander::getDeviceMixCircuitName(index);
}

gpaElement *gpaTurboExpanderInfo::createDevice(const gpaCoreInterfaces *interfaces, gpaCoreReference pairDevice,
                                               const gpaExtObjectModel *model) const
{
    gpaTurboExpander *cooler = new gpaTurboExpander(interfaces, pairDevice);
    gpaResult res = cooler->specifyModel(model);
    if (res == GPA_RESULT_OK)
        return cooler;
    delete cooler;
    return NULL;
}
