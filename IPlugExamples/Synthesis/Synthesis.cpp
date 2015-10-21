#include "Synthesis.h"
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wint-to-pointer-cast"
#include "IPlug_include_in_plug_src.h"
#pragma clang diagnostic pop
#include "IControl.h"
#include "resource.h"

const int kNumPrograms = 1;

enum EParams
{
  kFrequency = 0,
  kNumParams
};

enum ELayout
{
  kWidth = GUI_WIDTH,
  kHeight = GUI_HEIGHT,

  kFrequencyX = 100,
  kFrequencyY = 100,
  kKnobFrames = 60
};

Synthesis::Synthesis(IPlugInstanceInfo instanceInfo)
  :	IPLUG_CTOR(kNumParams, kNumPrograms, instanceInfo), mFrequency(1.)
{
  TRACE;

  //arguments are: name, defaultVal, minVal, maxVal, step, label
  GetParam(kFrequency)->InitDouble("Frequency", 440., 50., 2000.0, 0.01, "Hz");
  //can give nonlinear behavior to the knob (offsets from center)
  GetParam(kFrequency)->SetShape(2.0);

  //loads a red background to the window with height/width parameters
  IGraphics* pGraphics = MakeGraphics(this, kWidth, kHeight);
  pGraphics->AttachPanelBackground(&COLOR_RED);

  //creates knob with 60 frames
  IBitmap knob = pGraphics->LoadIBitmap(KNOB_ID, KNOB_FN, kKnobFrames);

  pGraphics->AttachControl(new IKnobMultiControl(this, kFrequencyX, kFrequencyY, kFrequency, &knob));

  AttachGraphics(pGraphics);

  //MakePreset("preset 1", ... );
  MakeDefaultPreset((char *) "-", kNumPrograms);
}

Synthesis::~Synthesis() {}

//Audio processing
void Synthesis::ProcessDoubleReplacing(double** inputs, double** outputs, int nFrames)
{
  // Mutex is already locked for us.

  double *leftOutput = outputs[0];
  double *rightOutput = outputs[1];

  mOscillator.generate(leftOutput, nFrames);
  
  for (int s = 0; s < nFrames; ++s)
  {
    rightOutput[s] = leftOutput[s];
  }
}

void Synthesis::Reset()
{
  TRACE;
  IMutexLock lock(this);
  mOscillator.setSampleRate(GetSampleRate());
}

void Synthesis::OnParamChange(int paramIdx)
{
  //enables thread safety using mutex
  IMutexLock lock(this);

  switch (paramIdx)
  {
    //depends on what param was changed, kFrequency=[0, 100]
    //mFrequency = [0, 1]
    case kFrequency:
      mOscillator.setFrequency(GetParam(kFrequency)->Value());
      break;

    default:
      break;
  }
}

void Synthesis::CreatePresets()
{
  MakePreset("clean", 440.0);
}
