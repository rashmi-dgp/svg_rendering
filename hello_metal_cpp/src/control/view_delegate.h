
#pragma once
#include "config.h"
#include "../view/renderer.h"
class ViewDelegate : public MTK::ViewDelegate
{
    public:
        ViewDelegate(MTL::Device* device);
        virtual ~ViewDelegate() override;
        virtual void drawInMTKView(MTK::View* view) override;
    
        void setZoomFactor(float zoom);
        void setPanOffset(float x, float y);

    private:
        Renderer* renderer;
        float zoomFactor = 1.0f;
        float panX = 0.0f;
        float panY = 0.0f;
};
