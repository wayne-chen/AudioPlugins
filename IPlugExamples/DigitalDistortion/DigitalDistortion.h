#ifndef __DIGITALDISTORTION__
#define __DIGITALDISTORTION__

#include "IPlug_include_in_plug_hdr.h"

class DigitalDistortion : public IPlug
{
public:
  DigitalDistortion(IPlugInstanceInfo instanceInfo);
  ~DigitalDistortion();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mDistortionAmount;
  void CreatePresets();
};

#endif
