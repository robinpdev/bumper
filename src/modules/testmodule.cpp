#include "../../olcPixelGameEngine.h"
#include "../bump-module.h"

class testModule : public bump::Module{
protected:
  olc::vf2d ballpos = {0.0f,0.0f};
  olc::vf2d heading = {90.0f,90.0f};
  const int radius = 24;

  bool OnUserCreate() override{
    pos = {0, 0};
    return true;
  }

  bool OnUserUpdate(float totalTime, float fElapsedTime) override{
    ballpos += heading * fElapsedTime;

    if(ballpos.x + radius >= size.x) heading.x = -abs(heading.x);
    else if(ballpos.x - radius  <= 0) heading.x = abs(heading.x);
    if(ballpos.y + radius>= size.y) heading.y = -abs(heading.y);
    else if(ballpos.y - radius <= 0) heading.y = abs(heading.y);

    int intime = (int) (totalTime * 100);
    pge->FillRect(0, 0, size.x, size.y, olc::Pixel((intime + 150) % 180, (intime + 120) % 200, intime % 255));
    
    olc::Pixel ballcolor = olc::Pixel((intime - 150) % 180, (intime - 120) % 200, intime % 180);
    pge->FillCircle(olc::vi2d(ballpos), radius, ballcolor);

    return true;
  }
public:
    testModule(olc::PixelGameEngine* engine) : bump::Module(engine){}
};

extern "C"{
    testModule* allocator(olc::PixelGameEngine* engine){
        //return 0;
        return new testModule(engine);
    }

    void deleter(testModule* ptr){
      delete ptr;
    }
}