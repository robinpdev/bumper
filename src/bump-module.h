
#pragma once

#ifndef BUMP_MODULE_H
#define BUMP_MODULE_H

#include "../olcPixelGameEngine.h"

namespace bump {
  class Module{
  protected:
    olc::PixelGameEngine* pge;
    olc::Sprite* target;
    olc::vi2d pos;
    virtual bool OnUserCreate() = 0;
    virtual bool OnUserUpdate(float totalTime, float fElapsedTime) = 0;
    virtual ~Module() = default;
  public:
    Module(olc::PixelGameEngine* engine){
      pge = engine;
      target = nullptr;
      pos = {0, 0};
      printf("module engine: %p\n", pge);
    }

    bool Create(int width, int height){
      target = new olc::Sprite(width, height);
      return true;
    }

    bool Update(float totalTime, float fElapsedTime){
      pge->SetDrawTarget(target);
      
      bool res = OnUserUpdate(totalTime, fElapsedTime);
      
      pge->SetDrawTarget(nullptr);
      return res;
    }

    void draw(){
      //pge->Clear(olc::BLACK);
      
      pge->DrawSprite(pos.x, pos.y, target);
    }

    void updateSprite(olc::Sprite* target){
      this->target = target;
    }
  };
};

#endif