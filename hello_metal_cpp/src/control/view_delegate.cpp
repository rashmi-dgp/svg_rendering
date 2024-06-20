

#include "view_delegate.h"

ViewDelegate::ViewDelegate(MTL::Device* device)
: MTK::ViewDelegate()
, renderer(new Renderer(device))
{
}

ViewDelegate::~ViewDelegate()
{
    delete renderer;
}

void ViewDelegate::drawInMTKView(MTK::View* view)
{
        renderer->setZoomFactor(zoomFactor);
        renderer->setPanOffset(panX, panY);
        renderer->draw(view);
}
void ViewDelegate::setZoomFactor(float zoom)
{
    zoomFactor = zoom;
}

void ViewDelegate::setPanOffset(float x, float y)
{
    panX = x;
    panY = y;
}
