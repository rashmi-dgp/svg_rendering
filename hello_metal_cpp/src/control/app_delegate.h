
#pragma once
#include "../config.h"
#include "view_delegate.h"

class AppDelegate : public NS::ApplicationDelegate
{
    public:
        ~AppDelegate();

        virtual void applicationWillFinishLaunching(NS::Notification* notification) override;
        virtual void applicationDidFinishLaunching(NS::Notification* notification) override;
        virtual bool applicationShouldTerminateAfterLastWindowClosed(NS::Application* sender) override;
        //aded to zoom:
        void handleZoom(float delta);
        void handlePan(float deltaX, float deltaY);

    private:
        NS::Window* window;
        MTK::View* mtkView;
        MTL::Device* device;
        ViewDelegate* viewDelegate = nullptr;
        float zoomFactor = 1.0f;
        float panX = 0.0f;
        float panY = 0.0f;
};
