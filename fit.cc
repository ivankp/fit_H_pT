#include <iostream>
#include <vector>
#include <cmath>

#include <boost/lexical_cast.hpp>

#include <TH1.h>
#include <TF1.h>
#include <TList.h>
#include <TMinuit.h>
#include <TMath.h>

#define TEST(var) \
  std::cout << "\033[36m" #var "\033[0m = " << var << std::endl;

using std::cout;
using std::endl;
using std::get;

double range[2];

extern "C" void init(const std::string& args) {
  const auto d = args.find(':');
  range[0] = boost::lexical_cast<double>(args.c_str(),d);
  range[1] = boost::lexical_cast<double>(args.c_str()+d+1,args.size()-d-1);
}

struct _xce { double x, c, e; };
std::vector<_xce> xce;

double ffit(double x, double* p) noexcept {
  return p[0] + p[1]*x + p[2]*x*x;
}
double fdraw(double* x, double* p) noexcept {
  return std::exp(ffit(std::log(*x),p));
}

double fchi2(double* p) noexcept {
  double chi2 = 0;
  for (const auto& xce : xce) {
    const double diff = xce.c - ffit(xce.x,p);
    chi2 += diff*diff / xce.e;
  }
  return chi2;
}
void fchi2_wrap(
  Int_t& npar, Double_t* grad, Double_t& fval, Double_t* par, Int_t flag
) noexcept {
  fval = fchi2(par);
}

extern "C" void run(TH1* h) {
  cout << "Fitting " << h->GetName()
       << " on [" << range[0] <<','<< range[1] <<']' << endl;

  unsigned a, b;
  if (range[0]==0 && range[1]==0) {
    a = 1, b = h->GetNbinsX();
  } else {
    a = h->FindBin(range[0]);
    b = h->FindBin(range[1]);
    if (h->GetBinLowEdge(b) == range[1]) --b;
  }
  xce.clear();
  xce.reserve(b-a+1);

  if (b <= a) throw std::runtime_error("bad fit range");

  for (unsigned i=a; i<=b; ++i) {
    const auto c = h->GetBinContent(i);
    if (c < 0) continue;
    const auto x = h->GetBinCenter(i);
    const auto e = h->GetBinError(i);
    xce.push_back({ std::log(x), std::log(c), e/c });
  }

  const auto& _a = xce.front();
  const auto& _b = xce.back();

  constexpr unsigned npar = 3;

  double p[npar], e[npar];
  p[1] = (_b.c-_a.c)/(_b.x-_a.x);
  p[0] = _a.c - p[1]*_a.x;
  p[2] = 0;

  TF1 *f = new TF1("",fdraw,h->GetBinLowEdge(a),h->GetBinLowEdge(b+1),npar);
  for (unsigned i=0; i<npar; ++i)
    f->SetParameter(i,p[i]);

  h->GetListOfFunctions()->Add(f);

  TMinuit m(npar);
  m.SetFCN(fchi2_wrap);

  // i, name, start, step, min, max
  for (unsigned i=0; i<npar; ++i)
    m.DefineParameter(i, ("p"+std::to_string(i)).c_str(), p[i], 0.01, 0, 0);

  if (auto flag = m.Migrad()) {
    std::cerr << "\033[31mFit did not converge ("<<flag<<")\033[0m" << endl;
  } else { // if fit converges
    for (unsigned i=0; i<npar; ++i) {
      m.GetParameter(i,p[i],e[i]);
      f->SetParameter(i,p[i]);
      f->SetParError (i,e[i]);
    }
  }

  const double chi2_min = fchi2(p);

  TEST( chi2_min )
  TEST( TMath::Prob(chi2_min,npar) )
}
