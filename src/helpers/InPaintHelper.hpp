#ifndef __HELPERS_INPAINT__
#define __HELPERS_INPAINT__

namespace sd_gui_utils {
    class InPaintHelper {
    public:
        enum BrushStyle {
            BRUSH_CIRCLE,
            BRUSH_SQUARE,
            BRUSH_TRIANGLE
        };
        struct BrushSize {
            BrushSize(unsigned int min, unsigned int max, unsigned int current)
                : min(min), max(max), current(current), original(current) {}
            unsigned int min;
            unsigned int max;
            unsigned int current;
            unsigned int original;

            void ResetSize() { this->current = this->original; }
        };

    private:
        wxScrolledWindow* parent                           = nullptr;
        std::shared_ptr<wxBitmap> paintArea                = nullptr;
        std::shared_ptr<wxBitmap> paintAreaBorder          = nullptr;
        std::shared_ptr<wxBitmap> outPaintArea             = nullptr;
        std::shared_ptr<wxImage> originalImage             = nullptr;
        std::shared_ptr<wxImage> zoomedImage               = nullptr;
        bool painted                                       = false;
        BrushSize inpaintBrushSize                         = BrushSize{1, 20, 5};
        double inpaintZoomFactor                           = 1;
        double inpaintZoomFactorStep                       = 0.1;
        sd_gui_utils::InPaintHelper::BrushStyle brushStyle = sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE;

        // store the original image size
        wxSize originalSize;
        // store if the image is resized to larger than the original
        wxSize outPaintSize;

        wxPosition lastPosition;
        // event checkers
        // if painting is in progress
        bool isDrawing = false;
        // event checkers

        void resetPaintArea(int width = 512, int height = 512) {
            if (this->zoomedImage != nullptr) {
                width  = this->zoomedImage->GetWidth();
                height = this->zoomedImage->GetHeight();
            }
            wxImage inpaintImage(width, height);
            inpaintImage.InitAlpha();

            unsigned char* alpha = inpaintImage.GetAlpha();
            unsigned char* data  = inpaintImage.GetData();
            for (int i = 0; i < inpaintImage.GetWidth() * inpaintImage.GetHeight(); ++i) {
                alpha[i]        = 0;    // Fully transparent
                data[i * 3]     = 255;  // R White
                data[i * 3 + 1] = 255;  // G
                data[i * 3 + 2] = 255;  // B
            }
            inpaintImage.SetMask(false);
            this->paintArea = std::make_shared<wxBitmap>(inpaintImage);
            wxMemoryDC dc(*this->paintArea.get());
            dc.SetBackground(*wxTRANSPARENT_BRUSH);
            dc.Clear();
            dc.SelectObject(wxNullBitmap);

            this->InitBorder(width, height);

            this->painted      = false;
            this->originalSize = wxSize(width, height);
            this->outPaintSize = wxSize(width, height);
            this->inpaintBrushSize.ResetSize();
            this->SetZoomFactor(1);
        }

        wxPosition GetMousePosition(wxMouseEvent& event) {
            wxPoint pos    = event.GetPosition();
            wxPoint offset = this->parent->GetViewStart();

            int scrollUnitX, scrollUnitY;
            this->parent->GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);
            offset.x *= scrollUnitX;
            offset.y *= scrollUnitY;

            double x = (pos.x + offset.x);
            double y = (pos.y + offset.y);

            int panelWidth, panelHeight;
            this->parent->GetClientSize(&panelWidth, &panelHeight);

            int imageWidth  = this->paintArea->IsOk() ? this->paintArea->GetScaledWidth() : 0;
            int imageHeight = this->paintArea->IsOk() ? this->paintArea->GetScaledHeight() : 0;

            // centered image pos
            int offsetX = 0;
            int offsetY = 0;
            if (panelWidth > imageWidth) {
                offsetX = (panelWidth - imageWidth) / 2;
            }
            if (panelHeight > imageHeight) {
                offsetY = (panelHeight - imageHeight) / 2;
            }

            x -= offsetX;
            y -= offsetY;

            return {{x, y}, {offset.x, offset.y}, event.GetPosition()};
        }

    public:
        InPaintHelper(wxScrolledWindow* parent)
            : parent(parent) {
            resetPaintArea();
        }
        void Reset() {
            this->originalImage = nullptr;
            this->zoomedImage   = nullptr;
            this->painted       = false;
            this->resetPaintArea();
        }

        void InitBorder(const wxSize& size) { this->InitBorder(size.x, size.y); }
        void InitBorder(int width, int height) {
            this->paintAreaBorder = std::make_shared<wxBitmap>(width + 2, height + 2);
            wxMemoryDC dc2(*this->paintAreaBorder.get());
            dc2.SetBackground(*wxRED_BRUSH);
            dc2.Clear();
            dc2.SelectObject(wxNullBitmap);
        }
        BrushSize GetBrushSize() { return this->inpaintBrushSize; }
        unsigned int GetCurrentBrushSize() { return this->inpaintBrushSize.current; }
        void SetCurrentBrushSize(unsigned int size) { this->inpaintBrushSize.current = size; }
        void SetBrushSize(BrushSize size) { this->inpaintBrushSize = size; }
        sd_gui_utils::InPaintHelper::BrushStyle GetBrushStyle() { return this->brushStyle; }
        void SetBrushStyle(sd_gui_utils::InPaintHelper::BrushStyle style) { this->brushStyle = style; }
        double GetZoomFactor() { return this->inpaintZoomFactor; }
        void SetZoomFactor(double factor) { this->inpaintZoomFactor = factor; }

        void SetImage(const wxImage& image) {
            this->originalImage = std::make_shared<wxImage>(image);
            this->resetPaintArea(image.GetWidth(), image.GetHeight());
            this->originalSize = image.GetSize();
        }
        bool inPaintImageLoaded() { return this->originalImage != nullptr; }
        bool inPaintCnvasEmpty() { return this->painted == false; }
        bool GetIsDrawing() { return this->isDrawing; }
        wxSize GetOriginalSize() { return this->originalSize; }
        wxSize GetOutPaintedSize() { return this->outPaintSize; }
        // handle events

        void OnDeleteInitialImage() {
            this->originalImage = nullptr;
            this->zoomedImage   = nullptr;
            this->resetPaintArea();

            // set the size to the border. This will cause the parent window to resize
            this->parent->SetVirtualSize(this->paintAreaBorder->GetWidth(), this->paintAreaBorder->GetHeight());
            this->parent->SetScrollRate(10, 10);
            this->parent->SetScrollPos(wxBOTH, 0, true);
        }

        void OnOutPaintResize(sd_gui_utils::wxEnlargeImageSizes& enlargeSize) {
            if (enlargeSize.top < 0 && this->originalImage->GetHeight() + enlargeSize.top < 0) {
                enlargeSize.top = 0;
            }
            if (enlargeSize.left < 0 && this->originalImage->GetWidth() + enlargeSize.left < 0) {
                enlargeSize.left = 0;
            }
            if (enlargeSize.right < 0 && this->originalImage->GetWidth() + enlargeSize.right < 0) {
                enlargeSize.right = 0;
            }
            if (enlargeSize.bottom < 0 && this->originalImage->GetHeight() + enlargeSize.bottom < 0) {
                enlargeSize.bottom = 0;
            }
            this->zoomedImage = std::make_shared<wxImage>(*this->originalImage);

            sd_gui_utils::CropOrFillImage(this->zoomedImage, enlargeSize, wxColour(128, 128, 128));
            this->outPaintSize      = this->zoomedImage->GetSize();
            this->inpaintZoomFactor = 1;
            // this->m_inpaintZoom->SetLabel(_("Zoom: 1x"));

            // set generated image size to the target image size
            // this->m_width->SetValue(img.GetWidth());
            // this->m_height->SetValue(img.GetHeight());

            // update the border
            this->InitBorder(this->zoomedImage->GetWidth(), this->zoomedImage->GetHeight());

            // refresh the display
            if (this->originalImage.IsOk()) {
                this->parent->SetVirtualSize(this->paintAreaBorder->GetSize());
            }

            this->parent->SetScrollRate(10, 10);
            this->parent->SetScrollPos(wxBOTH, 0, true);
            this->parent->Refresh();
        }

        void OnDcPaint(wxPaintEvent& event) {
            wxAutoBufferedPaintDC dc(this->parent);

            int panelWidth, panelHeight;
            this->parent->GetClientSize(&panelWidth, &panelHeight);

            int imageWidth  = this->zoomedImage->IsOk() ? this->zoomedImage->GetWidth() : 0;
            int imageHeight = this->zoomedImage->IsOk() ? this->zoomedImage->GetHeight() : 0;

            // int canvasWidth  = this->inpaintCanvas.IsOk() ? this->inpaintCanvas.GetScaledWidth() : 0;
            // int canvasHeight = this->inpaintCanvas.IsOk() ? this->inpaintCanvas.GetScaledHeight() : 0;

            int borderWidth  = this->paintAreaBorder->IsOk() ? this->paintAreaBorder->GetScaledWidth() : 0;
            int borderHeight = this->paintAreaBorder->IsOk() ? this->paintAreaBorder->GetScaledHeight() : 0;

            int inpaintBitMapWidth  = this->paintArea->IsOk() ? this->paintArea->GetScaledWidth() : 0;
            int inpaintBitMapHeight = this->paintArea->IsOk() ? this->paintArea->GetScaledHeight() : 0;

            wxPoint viewStart = this->parent->GetViewStart();
            int scrollUnitX, scrollUnitY;
            this->parent->GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);
            int scrollOffsetX = viewStart.x * scrollUnitX;
            int scrollOffsetY = viewStart.y * scrollUnitY;

            int offsetX = (panelWidth > borderWidth ? (panelWidth - borderWidth) / 2 : 0) - scrollOffsetX;
            int offsetY = (panelHeight > borderHeight ? (panelHeight - borderHeight) / 2 : 0) - scrollOffsetY;

            wxBrush background(wxColour(33, 33, 33));
            dc.SetBackground(background);
            dc.Clear();

            if (this->paintAreaBorder->IsOk()) {
                dc.DrawBitmap(*this->paintAreaBorder, offsetX, offsetY, false);
            }

            if (this->outPaintArea->IsOk()) {
                int canvasOffsetX = offsetX + (borderWidth - imageWidth) / 2;
                int canvasOffsetY = offsetY + (borderHeight - imageHeight) / 2;
                dc.DrawBitmap(*this->outPaintArea, canvasOffsetX, canvasOffsetY, false);
            }

            if (this->zoomedImage->IsOk()) {
                int imageOffsetX = offsetX + (borderWidth - imageWidth) / 2;
                int imageOffsetY = offsetY + (borderHeight - imageHeight) / 2;
                dc.DrawBitmap(*this->zoomedImage, imageOffsetX, imageOffsetY, false);
            }

            if (this->paintArea->IsOk()) {
                int bitmapOffsetX = offsetX + (borderWidth - inpaintBitMapWidth) / 2;
                int bitmapOffsetY = offsetY + (borderHeight - inpaintBitMapHeight) / 2;
                dc.DrawBitmap(*this->paintArea, bitmapOffsetX, bitmapOffsetY, true);
            }
        }

        void OnMouseRightDown(wxMouseEvent& event) {
            this->isDrawing    = true;
            this->lastPosition = this->GetMousePosition(event);
        }
        void OnMouseRightUp(wxMouseEvent& event) { this->isDrawing = false; }
        void OnMouseLeftDown(wxMouseEvent& event) {
            this->isDrawing    = true;
            this->lastPosition = this->GetMousePosition(event);
        }
        void OnMouseLeftUp(wxMouseEvent& event) { this->isDrawing = false; }

        void OnMouseMotion(wxMouseEvent& event) {
            if (this->isDrawing && event.Dragging() && (event.LeftIsDown() || event.RightIsDown())) {
                auto currentPos = this->GetMousePosition(event);
                auto lastPos    = this->lastPosition;
                double dx       = currentPos.pos.x - lastPos.pos.x;
                double dy       = currentPos.pos.y - lastPos.pos.y;

                if (event.ControlDown()) {
                    if (fabs(dx) > fabs(dy)) {
                        currentPos.pos.y = lastPos.pos.y;
                    } else {
                        currentPos.pos.x = lastPos.pos.x;
                    }
                    dx = currentPos.pos.x - lastPos.pos.x;
                    dy = currentPos.pos.y - lastPos.pos.y;
                }

                // interpolate

                double distance = sqrt(dx * dx + dy * dy);

                int steps = static_cast<int>(distance / (this->inpaintBrushSize.current));
                if (steps == 0) {
                    steps = 1;
                }

                wxMemoryDC dc(*this->paintArea);
                if (event.LeftIsDown()) {
                    dc.SetBrush(*wxWHITE_BRUSH);
                    dc.SetPen(*wxWHITE_PEN);
                    if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE) {
                        for (int i = 1; i <= steps; ++i) {
                            double t             = static_cast<double>(i) / steps;
                            double interpolatedX = lastPos.pos.x + t * dx;
                            double interpolatedY = lastPos.pos.y + t * dy;
                            dc.DrawCircle(wxPoint(interpolatedX, interpolatedY), this->inpaintBrushSize.current);
                            this->painted = true;
                        }
                    } else if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_SQUARE) {
                        for (int i = 1; i <= steps; ++i) {
                            double t             = static_cast<double>(i) / steps;
                            double interpolatedX = lastPos.pos.x + t * dx;
                            double interpolatedY = lastPos.pos.y + t * dy;
                            dc.DrawRectangle(wxPoint(interpolatedX - this->inpaintBrushSize.current / 2, interpolatedY - this->inpaintBrushSize.current / 2), wxSize(this->inpaintBrushSize.current * 2, this->inpaintBrushSize.current * 2));
                            this->painted = true;
                        }

                    } else if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_TRIANGLE) {
                        for (int i = 1; i <= steps; ++i) {
                            double t             = static_cast<double>(i) / steps;
                            double interpolatedX = lastPos.pos.x + t * dx;
                            double interpolatedY = lastPos.pos.y + t * dy;

                            wxPoint points[3];
                            points[0] = wxPoint(interpolatedX - this->inpaintBrushSize.current / 2, interpolatedY - this->inpaintBrushSize.current / 2);
                            points[1] = wxPoint(interpolatedX + this->inpaintBrushSize.current / 2, interpolatedY - this->inpaintBrushSize.current / 2);
                            points[2] = wxPoint(interpolatedX, interpolatedY + this->inpaintBrushSize.current / 2);
                            dc.DrawPolygon(3, points);
                            this->painted = true;
                        }
                    }
                } else if (event.RightIsDown()) {
                    wxImage img = this->paintArea->ConvertToImage().Scale(this->paintArea->GetScaledWidth(), this->paintArea->GetScaledHeight(), wxIMAGE_QUALITY_HIGH);

                    if (!img.HasAlpha()) {
                        img.InitAlpha();
                    }

                    for (int i = 1; i <= steps; ++i) {
                        double t             = static_cast<double>(i) / steps;
                        double interpolatedX = lastPos.pos.x + t * dx;
                        double interpolatedY = lastPos.pos.y + t * dy;

                        int startX = std::max(0, static_cast<int>(interpolatedX - this->inpaintBrushSize.current));
                        int startY = std::max(0, static_cast<int>(interpolatedY - this->inpaintBrushSize.current));
                        int endX   = std::min(img.GetWidth(), static_cast<int>(interpolatedX + this->inpaintBrushSize.current));
                        int endY   = std::min(img.GetHeight(), static_cast<int>(interpolatedY + this->inpaintBrushSize.current));

                        for (int y = startY; y < endY; ++y) {
                            for (int x = startX; x < endX; ++x) {
                                double distance = sqrt(pow(x - interpolatedX, 2) + pow(y - interpolatedY, 2));
                                if (distance <= this->inpaintBrushSize.current) {
                                    img.SetAlpha(x, y, 0);
                                }
                            }
                        }
                    }
                    this->paintArea = std::make_shared<wxBitmap>(img);
                }

                dc.SelectObject(wxNullBitmap);

                this->parent->Refresh();
                this->lastPosition = currentPos;
            }
        }

        void OnMouseWheel(wxMouseEvent& event, const wxEnlargeImageSizes& enlargeSizes) {
            int rotation = event.GetWheelRotation();

            if (event.ControlDown() && this->originalImage->IsOk()) {
                if (this->inpaintZoomFactor + this->inpaintZoomFactorStep > 2.0 && rotation > 0) {
                    return;
                }

                if (this->inpaintZoomFactor - this->inpaintZoomFactorStep < 0.1 && rotation < 0) {
                    return;
                }

                double oldZoomFactor = this->inpaintZoomFactor;

                if (rotation > 0) {
                    this->inpaintZoomFactor += this->inpaintZoomFactorStep;
                } else if (rotation < 0) {
                    this->inpaintZoomFactor -= this->inpaintZoomFactorStep;
                }

                auto tmpImg       = this->originalImage->Scale(this->originalImage->GetWidth() * this->inpaintZoomFactor, this->originalImage->GetHeight() * this->inpaintZoomFactor, wxIMAGE_QUALITY_HIGH);
                this->zoomedImage = std::make_shared<wxImage>(tmpImg);

                if (this->originalSize != this->outPaintSize) {
                    sd_gui_utils::CropOrFillImage(this->zoomedImage, enlargeSizes, wxColour(128, 128, 128));
                }

                this->paintArea->SetScaleFactor(1.0 / this->inpaintZoomFactor);
                this->outPaintArea->SetScaleFactor(1.0 / this->inpaintZoomFactor);
                this->paintAreaBorder->SetScaleFactor(1.0 / this->inpaintZoomFactor);

                // this->InitBorder(this->outPaintArea->GetScaledSize());

                wxPoint mousePos  = event.GetPosition();
                wxPoint viewStart = this->parent->GetViewStart();
                int scrollUnitX, scrollUnitY;
                this->parent->GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);

                double zoomFactorRatio = oldZoomFactor / this->inpaintZoomFactor;
                wxPoint newViewStart   = viewStart;
                newViewStart.x += (mousePos.x + viewStart.x * scrollUnitX -
                                   (this->parent->GetClientSize().GetWidth() / 2)) *
                                  (1 - zoomFactorRatio) / scrollUnitX;
                newViewStart.y += (mousePos.y + viewStart.y * scrollUnitY -
                                   (this->parent->GetClientSize().GetHeight() / 2)) *
                                  (1 - zoomFactorRatio) / scrollUnitY;

                newViewStart.x = std::max(0, std::min(newViewStart.x,
                                                      (this->paintAreaBorder->GetWidth() - this->parent->GetClientSize().GetWidth()) / scrollUnitX));
                newViewStart.y = std::max(0, std::min(newViewStart.y,
                                                      (this->paintAreaBorder->GetHeight() - this->parent->GetClientSize().GetHeight()) / scrollUnitY));

                this->parent->Scroll(newViewStart);

                this->parent->SetVirtualSize(this->paintAreaBorder->GetScaledSize());

                this->parent->Refresh();
            }

            if (event.ShiftDown()) {
                if (rotation > 0) {
                    this->inpaintBrushSize.current = std::min(this->inpaintBrushSize.current + 1, this->inpaintBrushSize.max);
                } else if (rotation < 0) {
                    this->inpaintBrushSize.current = std::max(this->inpaintBrushSize.current - 1, this->inpaintBrushSize.min);
                }
                return;
            }

            event.Skip();
        }
    };
};

#endif