//------------------------------------------------------------------------------
//
// 1D cellular automata
//
//------------------------------------------------------------------------------
//
// This file is licensed after LGPL v3
// Look at: https://www.gnu.org/licenses/lgpl-3.0.en.html for details
//
//------------------------------------------------------------------------------

#include <memory>

#include "drawutil.hpp"
#include "opts.hpp"

constexpr int DEF_QUIET = 0;

// default rule is famous rule 110
constexpr int DEF_CODE = 110;

constexpr int DEF_XSZ = 1024;
constexpr int DEF_YSZ = 768;

struct Config {
  int Code, XSz, YSz;
  bool Quiet = DEF_QUIET;
};

static Config parse_cfg(int argc, char **argv) {
  Config Cfg;
  options::Parser OptParser;
  OptParser.template add<int>("code", DEF_CODE, "code of automata");
  OptParser.template add<int>("xsize", DEF_XSZ, "X size");
  OptParser.template add<int>("ysize", DEF_YSZ, "Y size");
  OptParser.template add<int>("quiet", DEF_QUIET, "no debug output");
  OptParser.parse(argc, argv);

  Cfg.Code = OptParser.template get<int>("code");
  Cfg.XSz = OptParser.template get<int>("xsize");
  Cfg.YSz = OptParser.template get<int>("ysize");
  Cfg.Quiet = OptParser.exists("quiet");
  return Cfg;
}

constexpr unsigned Black = DrawUtil::buildcolor(0, 0, 0, 0xAA);
constexpr unsigned White = DrawUtil::buildcolor(0, 0xFF, 0xFF, 0xFF);

static char calc_cell(char Left, char Center, char Right, int Code) {
  // Wolfram code
  // L C R: 111 110 101 100 011 010 ....
  //         1   0   0   1   0   1  ....
  assert(Left < 2 && Center < 2 && Right < 2);
  assert(Left >= 0 && Center >= 0 && Right >= 0);
  char CIdx = (Left << 2) + (Center << 1) + Right;
  return ((Code & (1 << CIdx)) == 0) ? 0 : 1;
}

static void draw_cellular(DrawUtil::ISurface *S, Config Cfg) {
  int XSize = S->w();
  int YSize = S->h();
  std::vector<char> Row(XSize, 0);

  S->fillwith(White);

  Row[XSize / 2] = 1;

  for (int Step = 0; Step < YSize; ++Step) {
    // draw the row
    for (int X = 0; X < XSize; ++X) {
      unsigned Color = Row[X] ? Black : White;
      S->putpixel(X, Step, Color);
    }
    // recalc the row
    std::vector<char> SwapRow(XSize);
    for (int X = 0; X < XSize; ++X) {
      auto Left = (X > 0) ? Row[X - 1] : 0;
      auto Right = (X < XSize - 1) ? Row[X + 1] : 0;
      SwapRow[X] = calc_cell(Left, Row[X], Right, Cfg.Code);
    }
    // update the row
    std::copy(SwapRow.begin(), SwapRow.end(), Row.begin());
  }
}

int main(int argc, char **argv) {
  auto Cfg = parse_cfg(argc, argv);

  auto draw_external = [&Cfg](DrawUtil::ISurface *S) { draw_cellular(S, Cfg); };

  auto *ViewPort = DrawUtil::QueryViewPort("Cellular Automata", Cfg.XSz,
                                           Cfg.YSz, draw_external, true);
  auto freeViewPort = [](DrawUtil::IViewPort *V) { delete V; };
  using VpTy = std::unique_ptr<DrawUtil::IViewPort, decltype(freeViewPort)>;

  VpTy V(ViewPort, freeViewPort);
  while (V->poll() == DrawUtil::pollres::PROCEED) {
  }
}
