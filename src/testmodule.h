#pragma once

#include "bump-module.h"

class testModule : public bump::Module{
protected:
  bool OnUserCreate() override{
    pos = {0, 0};
    return true;
  }

  bool OnUserUpdate(float fElapsedTime) override{
    pge->FillRect(0, 0, 100, 150, olc::RED);
    return true;
  }
public:
    testModule(olc::PixelGameEngine* engine) : bump::Module(engine){}
};