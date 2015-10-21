#include "DigitalDistortion.h"
#include "math.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
#include "IPlug_include_in_plug_src.h"
#pragma clang diagnostic pop
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 5;

enum EParams
{
  kDistortionAmount = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kDistortionAmountX = 79,
  kDistortionAmountY = 62,
  kKnobFrames = 128
};

DigitalDistortion::DigitalDistortion(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mDistortionAmount(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kDistortionAmount)->InitDouble("Distortion Amount", 0.0, 0.0, 99.0, 0.01, "%"); //modified so there is no division by zero
  //can give nonlinear behavior to the knob (offsets from center)
  GetParam(kDistortionAmount)->SetShape(2.);

  //loads a red background to the window with height/width parameters
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  //pGraphics->AttachPanelBackground(&COLOR_RED);
  pGraphics->AttachBackground(BACKGROUND_ID, BACKGROUND_FN);
  //creates knob with 60 frames
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kDistortionAmountX, kDistortionAmountY, kDistortionAmount, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  CreatePresets();
}

DigitalDistortion::~DigitalDistortion() {}

//Audio processing
void DigitalDistortion::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  int const channelCount = 2;
  
  for(int i = 0; i < channelCount; i++)
  {
    double* input = inputs[i];
    double* output = outputs[i];
    
    //attenuates the sound at a certain mDistortionAmount (= 2)
    for (int s = 0; s < nFrames; ++s, ++input, ++output)
    {
      if(*input >= 0)
      {
        *output = fmin(*input, mDistortionAmount);
      }
      else
      {
        *output = fmax(*input, -mDistortionAmount);
      }
      *output /= mDistortionAmount; //this helps by dividing by a constant DistortionAmount
    }
  }
}

void DigitalDistortion::Reset()
{
  TRACE;
  IMutexLock lock(this);
}

void DigitalDistortion::OnParamChange(int paramIdx)
{
  //enables thread safety using mutex
  IMutexLock lock(this);

  switch (paramIdx)
  {
    //depends on what param was changed, kDistortionAmount=[0, 100]
    //mDistortionAmount = [0, 1]
    case kDistortionAmount:
      mDistortionAmount = 1 - (GetParam(kDistortionAmount)->Value() / 100.);
      break;

    default:
      break;
  }
}

void DigitalDistortion::CreatePresets()
{
  MakePreset("clean", 0.0);
  MakePreset("slightly distorted", 20.0);
  MakePreset("woooo", 40.0);
  MakePreset("waaaa", 80.0);
  MakePreset("buzz!!!!", 100.0);
}
