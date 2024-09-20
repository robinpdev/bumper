
#pragma once

#ifndef BUMP_MODULE_H
#define BUMP_MODULE_H

#include "../olcPixelGameEngine.h"

namespace bump {
  class Module{
  public:
    olc::vi2d pos;
    olc::vi2d size;
  protected:
    olc::PixelGameEngine* pge;
    olc::Sprite* target = 0;
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
      if(target) delete target;
      target = new olc::Sprite(width, height);
      size = {width, height};
      return true;
    }

    bool setSize(int width, int height){
      olc::vi2d newsize = {width, height};
      if(newsize != size){
        //printf("resizing...\n");
        Create(newsize.x, newsize.y);
        return true;
      }
      return false;
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