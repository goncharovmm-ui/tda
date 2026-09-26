#include "gpaTurboExpanderV2Info.h"

gpaTurboExpanderV2Info::gpaTurboExpanderV2Info()
    : gpaDeviceInfo(gpaTurboExpanderV2::getDeviceTypeName(), gpaTurboExpanderV2::getDeviceDescription())
{
    setParameters(gpaTurboExpanderV2::getParametersInfo());
    setSensors(gpaTurboExpanderV2::getSensorsInfo());
}
const gpaString* gpaTurboExpanderV2Info::getMixCircuitName(gpaUInt index) const
{
    static const gpaString names[] = {"Компрессор", "Детандер"};
    return index < 2 ? &names[index] : nullptr;
}
const gpaString* gpaTurboExpanderV2Info::getMixPortName(gpaUInt circuit, gpaUInt port) const
{ static const gpaString names[] = {"Вход компрессора", "Выход компрессора", "Вход детандера", "Выход детандера"}; return circuit < 2 && port < 2 ? &names[circuit * 2 + port] : nullptr; }
gpaElement* gpaTurboExpanderV2Info::createDevice(const gpaCoreInterfaces* i, gpaCoreReference r, const gpaExtObjectModel* m) const
{ auto* device = new gpaTurboExpanderV2(i, r); if(device->specifyModel(m)==GPA_RESULT_OK)return device; delete device; return nullptr; }
