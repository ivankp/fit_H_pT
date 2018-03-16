#include <iostream>
#include <iomanip>
#include <sstream>
#include <vector>
#include <cmath>

#include <TH1.h>
#include <TF1.h>
#include <TList.h>
// #include <TMath.h>
#include <TText.h>

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;
using std::get;

TF1* get_fit(TH1* h) {
  return static_cast<TF1*>(h->GetListOfFunctions()->At(0));
}

extern "C" void init(const std::string& args) { }

extern "C" void run(std::vector<TObject*>& objs, std::vector<TH1*>& hs) {
  { // fake values below jet pT cut (LO histograms don't have those)
    double xs0 = 0, xs1 = 0;
    for (int i=8; i<15; ++i) {
      xs0 += hs[0]->GetBinContent(i);
      xs1 += hs[1]->GetBinContent(i);
    }
    xs0 /= xs1;
    for (int i=1; i<7; ++i) {
      hs[0]->SetBinContent(i,hs[1]->GetBinContent(i)*xs0);
    }
  }

  unsigned hi = 0;
  for (TH1* h : hs) {
    TF1 *f = get_fit(h);
    f->SetLineWidth(1);
    f->SetLineColorAlpha(1,0.7);
    const unsigned np = f->GetNpar();
    const auto* p = f->GetParameters();

    std::stringstream ss;
    ss << p[0];
    for (unsigned i=1; i<np; ++i)
      ss << "  " << p[i];

    TText *text = new TText(0.13,0.23-0.035*hi,ss.str().c_str());
    // 0.14,0.28-0.03*hi
    text->SetNDC();
    text->SetTextFont(82);
    text->SetTextSize(0.035); // 0.03
    objs.push_back(text);
    ++hi;
  }
  hi = 0;

  { TF1 *f0 = get_fit(hs[1]), *f1 = get_fit(hs[0]);
    const double
      A = f0->GetParameter(0) - f1->GetParameter(0),
      B = f0->GetParameter(1) - f1->GetParameter(1),
      C = f0->GetParameter(2) - f1->GetParameter(2);

    for (const double r : {2., 10.}) {
      const double logr = std::log(r);
      const double E = std::exp((-B+std::sqrt(B*B-4.*C*(A-logr)))/(2.*C));
      std::stringstream ss;
      ss << "EFT/mtop = " << std::setw(2) << r << " at "
         << std::setprecision(3) << E << " GeV";
      TText *text = new TText(0.72,0.47-0.035*hi,ss.str().c_str());
      text->SetNDC();
      text->SetTextFont(82);
      text->SetTextSize(0.035);
      objs.push_back(text);
      ++hi;
    }
  }
}
