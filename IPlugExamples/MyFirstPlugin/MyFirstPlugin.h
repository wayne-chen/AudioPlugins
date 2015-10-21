#ifndef __MYFIRSTPLUGIN__
#define __MYFIRSTPLUGIN__

#include "IPlug_include_in_plug_hdr.h"

class MyFirstPlugin : public IPlug
{
public:
  MyFirstPlugin(IPlugInstanceInfo instanceInfo);
  ~MyFirstPlugin();

  void Reset();
  void OnParamChange(int paramIdx);
  void ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames);

private:
  double mGain;
};

#endif
