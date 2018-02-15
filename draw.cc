#include <iostream>
#include <vector>
#include <sstream>

// #include <boost/lexical_cast.hpp>

#include <TH1.h>
#include <TF1.h>
#include <TList.h>
#include <TMath.h>
#include <TText.h>

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;
using std::get;

extern "C" void init(const std::string& args) { }

extern "C" void run(std::vector<TObject*>& objs, std::vector<TH1*>& hs) {
  unsigned hi = 0;

  for (TH1* h : hs) {
    TF1 *f = (TF1*)h->GetListOfFunctions()->At(0);
    f->SetLineWidth(1);
    f->SetLineColorAlpha(1,0.7);
    const unsigned np = f->GetNpar();
    const auto* p = f->GetParameters();

    std::stringstream ss;
    ss << p[0];
    for (unsigned i=1; i<np; ++i)
      ss << "  " << p[i];

    TText *text = new TText(0.14,0.28-0.03*hi,ss.str().c_str());
    text->SetNDC();
    text->SetTextFont(82);
    text->SetTextSize(0.03);
    objs.push_back(text);
    ++hi;
  }
}
