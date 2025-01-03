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
        std::shared_ptr<wxBitmap> overlay                  = nullptr;
        std::shared_ptr<wxImage> originalImage             = nullptr;
        std::shared_ptr<wxImage> zoomedImage               = nullptr;
        bool painted                                       = false;
        bool outpainted                                    = false;
        BrushSize inpaintBrushSize                         = BrushSize{1, 20, 5};
        double inpaintZoomFactor                           = 1;
        double inpaintZoomFactorStep                       = 0.1;
        sd_gui_utils::InPaintHelper::BrushStyle brushStyle = sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE;
        const int BITMAP_DEPTH                             = 32;

        // store the original image size
        wxSize originalSize;
        // store if the image is resized to larger than the original
        wxSize outPaintSize;

        wxPoint lastPosition;
        // event checkers
        // if painting is in progress
        bool isDrawing = false;
        // the image is loaded or manually created
        bool imageLoaded = false;

        // border size in px
        int borderSize = 2;
        // event checkers

        void resetPaintArea(int width = 512, int height = 512) {
            if (this->originalImage == nullptr) {
                this->originalImage = std::make_shared<wxImage>(width, height);
                this->imageLoaded   = false;
            }

            if (this->zoomedImage == nullptr) {
                this->zoomedImage = std::make_shared<wxImage>(*this->originalImage);
                width             = this->zoomedImage->GetWidth();
                height            = this->zoomedImage->GetHeight();
            } else {
                width  = this->zoomedImage->GetWidth();
                height = this->zoomedImage->GetHeight();
            }

            this->InitBorder(width, height);
            this->InitOutPaintArea(width, height);
            this->InitOverLay(width, height);
            this->InitPaintArea(width, height);

            this->painted      = false;
            this->originalSize = wxSize(width, height);
            this->outPaintSize = wxSize(width, height);
            this->inpaintBrushSize.ResetSize();
            this->SetZoomFactor(1);
            this->UpdateParent();
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
            // @see https://wiki.wxwidgets.org/Drawing_on_a_panel_with_a_DC
            this->parent->SetDoubleBuffered(true);
            this->parent->SetBackgroundStyle(wxBG_STYLE_PAINT);
        }
        void Reset() {
            this->originalImage = nullptr;
            this->zoomedImage   = nullptr;
            this->painted       = false;
            this->resetPaintArea();
        }

        void CleanMask() {
            if (this->painted == false) {
                return;
            }
            if (this->paintArea == nullptr) {
                return;
            }

            this->InitPaintArea(this->GetOriginalSize());
            this->paintArea->SetScaleFactor(this->inpaintZoomFactor);
            this->painted = false;
            this->UpdateParent();
        }

        void InitBorder(const wxSize& size) { this->InitBorder(size.x, size.y); }
        void InitBorder(int width, int height) {
            this->paintAreaBorder = std::make_shared<wxBitmap>(width + 2, height + 2, BITMAP_DEPTH);
            this->paintAreaBorder->UseAlpha();
            wxMemoryDC dc2(*this->paintAreaBorder);
            dc2.SetBackground(*wxRED_BRUSH);
            dc2.Clear();
            dc2.SelectObject(wxNullBitmap);
        }

        void InitOverLay(const wxSize& size) { this->InitOverLay(size.x, size.y); }
        void InitOverLay(int width, int height) {
            wxBitmap buf(width, height, BITMAP_DEPTH);
            buf.UseAlpha();

            wxMemoryDC memDC(buf);
            wxGraphicsContext* gc = wxGraphicsContext::Create(memDC);
            if (gc) {
                gc->SetBrush(*wxTRANSPARENT_BRUSH);
                gc->SetPen(*wxTRANSPARENT_PEN);
                gc->DrawRectangle(0, 0, width, height);
            }
            delete gc;
            memDC.SelectObject(wxNullBitmap);
            this->overlay = std::make_shared<wxBitmap>(buf);
        }
        void InitOutPaintArea(const wxSize& size) { this->InitOutPaintArea(size.x, size.y); }
        void InitOutPaintArea(int width, int height) {
            wxBitmap buf(width, height, BITMAP_DEPTH);
            buf.UseAlpha();

            wxMemoryDC memDC(buf);
            wxGraphicsContext* gc = wxGraphicsContext::Create(memDC);
            if (gc) {
                gc->SetBrush(*wxTRANSPARENT_BRUSH);
                gc->SetPen(*wxTRANSPARENT_PEN);
                gc->DrawRectangle(0, 0, width, height);
            }
            delete gc;
            memDC.SelectObject(wxNullBitmap);
            this->outPaintArea = std::make_shared<wxBitmap>(buf);
        }

        void InitPaintArea(const wxSize& size) { this->InitPaintArea(size.x, size.y); }
        void InitPaintArea(int width, int height) {
            wxBitmap buf(width, height, BITMAP_DEPTH);
            buf.UseAlpha();

            wxMemoryDC memDC(buf);
            wxGraphicsContext* gc = wxGraphicsContext::Create(memDC);
            if (gc) {
                gc->SetBrush(*wxTRANSPARENT_BRUSH);
                gc->SetPen(*wxTRANSPARENT_PEN);
                gc->DrawRectangle(0, 0, width, height);
            }
            delete gc;
            memDC.SelectObject(wxNullBitmap);
            this->paintArea = std::make_shared<wxBitmap>(buf);
        }
        BrushSize GetBrushSize() { return this->inpaintBrushSize; }
        unsigned int GetCurrentBrushSize() { return this->inpaintBrushSize.current; }
        void SetCurrentBrushSize(unsigned int size) { this->inpaintBrushSize.current = size; }
        void SetBrushSize(BrushSize size) { this->inpaintBrushSize = size; }
        sd_gui_utils::InPaintHelper::BrushStyle GetBrushStyle() { return this->brushStyle; }
        void SetBrushStyle(sd_gui_utils::InPaintHelper::BrushStyle style) { this->brushStyle = style; }
        double GetZoomFactor() { return this->inpaintZoomFactor; }
        void SetZoomFactor(double factor) { this->inpaintZoomFactor = factor; }
        bool isOutPainted() { return this->outpainted; }

        bool SetImage(const wxImage& image) noexcept {
            this->originalImage   = std::make_shared<wxImage>(image);
            this->zoomedImage     = nullptr;
            this->outPaintArea    = nullptr;
            this->paintAreaBorder = nullptr;
            this->originalSize    = image.GetSize();
            this->resetPaintArea(image.GetWidth(), image.GetHeight());

            if (this->originalImage->IsOk()) {
                this->imageLoaded = true;
            }

            return this->imageLoaded;
        }
        // the current used original image is a user loaded image
        bool inPaintImageLoaded() { return this->imageLoaded; }
        // return true if user is painted to the canvas
        bool inPaintCanvasEmpty() { return this->painted == false; }
        // return true if user is currently drawing
        bool GetIsDrawing() { return this->isDrawing; }
        // get the original size of the initial image (return the resized image if resized to sdsizes)
        wxSize GetOriginalSize() { return this->originalSize; }
        // return the size of the image original size + outpaint size
        wxSize GetOutPaintedSize() { return this->outPaintSize; }
        void UpdateParent() {
            this->parent->SetVirtualSize(this->paintAreaBorder->GetSize() * this->inpaintZoomFactor);
            this->parent->SetScrollRate(10, 10);
            this->parent->SetScrollPos(wxBOTH, 0, true);
        }
        void UpdateParent(const wxPoint& scrollPos) {
            this->parent->Scroll(scrollPos);
            this->parent->SetVirtualSize(this->paintAreaBorder->GetSize() * this->inpaintZoomFactor);
            this->parent->Refresh();
        }
        // handle events

        void OnDeleteInitialImage() {
            this->originalImage = nullptr;
            this->zoomedImage   = nullptr;
            this->resetPaintArea();
            this->UpdateParent();
        }

        bool OnInvertMask(wxCommandEvent& event) {
            if (this->imageLoaded) {
                sd_gui_utils::InvertWhiteAndTransparent(this->paintArea);
                this->UpdateParent();
                return true;
            }
            return false;
        }

        // enlarge the image to bigger size, and fill the rest with gray
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
            if (enlargeSize.top == 0 &&
                enlargeSize.left == 0 &&
                enlargeSize.right == 0 &&
                enlargeSize.bottom == 0) {
                this->outpainted = false;
            } else {
                this->outpainted = true;
            }

            this->zoomedImage = std::make_shared<wxImage>(*this->originalImage);

            sd_gui_utils::CropOrFillImage(this->zoomedImage, enlargeSize, wxColour(128, 128, 128));
            this->outPaintSize      = this->zoomedImage->GetSize();
            this->inpaintZoomFactor = 1;
            this->InitBorder(this->zoomedImage->GetWidth(), this->zoomedImage->GetHeight());
            this->InitOverLay(this->zoomedImage->GetWidth(), this->zoomedImage->GetHeight());
            this->UpdateParent();
        }

        wxImage OnSaveMask(wxCommandEvent& event) {
            auto img = this->paintArea->ConvertToImage();
            sd_gui_utils::blendImageOnBlackBackground(img);
            return img;
        }

        wxImage GetMaskImage(bool blend = true) {
            if (blend) {
                wxImage img = this->paintArea->ConvertToImage();
                sd_gui_utils::blendImageOnBlackBackground(img);
                return img;
            }
            return this->paintArea->ConvertToImage();
        }
        wxImage OnResizeOriginalImage(int targetWidth, int targetHeight) {
            sd_gui_utils::ResizeImageToMaxSize(this->originalImage, targetWidth, targetHeight);
            this->originalSize    = this->originalImage->GetSize();
            this->zoomedImage     = nullptr;
            this->paintAreaBorder = nullptr;
            this->outPaintArea    = nullptr;
            this->resetPaintArea();
            return *this->originalImage;
        }

        void OnEraseBackground(wxEraseEvent& event) {}

        void OnDcPaint(wxPaintEvent& event) {
            wxAutoBufferedPaintDC dc(this->parent);
            dc.SetUserScale(this->inpaintZoomFactor, this->inpaintZoomFactor);

            // Panel méretének lekérdezése
            int panelWidth, panelHeight;
            this->parent->GetClientSize(&panelWidth, &panelHeight);

            dc.SetBackground(wxBrush(wxColour(51, 51, 51, 255)));
            dc.Clear();

            wxPoint viewStart = this->parent->GetViewStart();
            int scrollUnitX, scrollUnitY;
            this->parent->GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);
            int scrollOffsetX = viewStart.x * scrollUnitX;
            int scrollOffsetY = viewStart.y * scrollUnitY;

            int offsetX = scrollOffsetX / this->inpaintZoomFactor + this->borderSize;
            int offsetY = scrollOffsetY / this->inpaintZoomFactor + this->borderSize;

            // Border méret figyelembevétele
            if (this->paintAreaBorder != nullptr && this->paintAreaBorder->IsOk()) {
                int offsetX2 = offsetX + (this->paintAreaBorder->GetWidth() - this->zoomedImage->GetWidth()) / 2;
                int offsetY2 = offsetY + (this->paintAreaBorder->GetHeight() - this->zoomedImage->GetHeight()) / 2;
                dc.DrawBitmap(*this->paintAreaBorder, offsetX2, offsetY2, false);
            }

            // Az outPaintArea és a zoomedImage a legnagyobb bitmapot követi
            if (this->outPaintArea != nullptr && this->outPaintArea->IsOk() && this->outpainted) {
                dc.DrawBitmap(*this->outPaintArea, offsetX, offsetY, false);
            }

            // ZoomedImage méretének figyelembevételével
            if (this->zoomedImage != nullptr && this->zoomedImage->IsOk()) {
                dc.DrawBitmap(*this->zoomedImage, offsetX, offsetY, false);
            }

            // A paintArea és az overlay a zoomedImage méretéhez igazodnak
            if (this->paintArea != nullptr && this->paintArea->IsOk()) {
                dc.DrawBitmap(*this->paintArea, offsetX, offsetY, false);
            }

            if (this->overlay != nullptr && this->overlay->IsOk()) {
                dc.DrawBitmap(*this->overlay, offsetX, offsetY, false);
            }
        }

        void OnMouseRightDown(wxMouseEvent& event) {
            this->isDrawing = true;
            // this->lastPosition = this->GetMousePosition(event);
            this->lastPosition = event.GetPosition();
        }
        void OnMouseRightUp(wxMouseEvent& event) { this->isDrawing = false; }
        void OnMouseLeftDown(wxMouseEvent& event) {
            this->isDrawing    = true;
            this->lastPosition = event.GetPosition();  // this->GetMousePosition(event);
        }
        void OnMouseLeftUp(wxMouseEvent& event) { this->isDrawing = false; }
        void OnMouseLeave(wxMouseEvent& event) { this->isDrawing = false; }

        void OnMouseMotion(wxMouseEvent& event) {
            if (!this->isDrawing || (!event.LeftIsDown() && !event.RightIsDown())) {
                return;
            }

            auto currentPos = event.GetPosition();  // this->GetMousePosition(event);
            auto lastPos    = this->lastPosition;
            double dx       = currentPos.x - lastPos.x;
            double dy       = currentPos.y - lastPos.y;

            if (event.ControlDown()) {
                if (fabs(dx) > fabs(dy)) {
                    currentPos.y = lastPos.y;
                } else {
                    currentPos.x = lastPos.x;
                }
                dx = currentPos.x - lastPos.x;
                dy = currentPos.y - lastPos.y;
            }

            double distance = sqrt(dx * dx + dy * dy);
            int steps       = static_cast<int>(distance / (this->inpaintBrushSize.current));
            steps           = (steps == 0) ? 1 : steps;

            wxPoint viewStart = this->parent->GetViewStart();
            int scrollUnitX, scrollUnitY;
            this->parent->GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);
            int scrollOffsetX = viewStart.x * scrollUnitX;
            int scrollOffsetY = viewStart.y * scrollUnitY;

            wxMemoryDC dc(*this->paintArea);
            if (!dc.IsOk()) {
                return;
            }

            dc.SetLogicalFunction(event.LeftIsDown() ? wxCOPY : wxCLEAR);
            dc.SetBrush(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
            dc.SetPen(wxPen(wxColour(255, 255, 255), this->inpaintBrushSize.current, wxPENSTYLE_SOLID));

            for (int i = 1; i <= steps; ++i) {
                double t             = static_cast<double>(i) / steps;
                double interpolatedX = lastPos.x + t * dx - scrollOffsetX;
                double interpolatedY = lastPos.y + t * dy - scrollOffsetY;

                if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE) {
                    dc.DrawCircle(wxPoint(interpolatedX, interpolatedY), this->inpaintBrushSize.current);
                    this->parent->Refresh();
                } else if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_SQUARE) {
                    dc.DrawRectangle(wxPoint(interpolatedX - this->inpaintBrushSize.current / 2, interpolatedY - this->inpaintBrushSize.current / 2),
                                     wxSize(this->inpaintBrushSize.current * 2, this->inpaintBrushSize.current * 2));
                    this->parent->Refresh();
                } else if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_TRIANGLE) {
                    wxPoint points[3];
                    points[0] = wxPoint(interpolatedX - this->inpaintBrushSize.current, interpolatedY - this->inpaintBrushSize.current);
                    points[1] = wxPoint(interpolatedX + this->inpaintBrushSize.current, interpolatedY - this->inpaintBrushSize.current);
                    points[2] = wxPoint(interpolatedX, interpolatedY + this->inpaintBrushSize.current);
                    dc.DrawPolygon(3, points);
                    this->parent->Refresh();
                }
                this->painted = true;
            }

            dc.SelectObject(wxNullBitmap);
            this->lastPosition = currentPos;
        }

        void OnMouseWheel(wxMouseEvent& event, const wxEnlargeImageSizes& enlargeSizes) {
            int rotation = event.GetWheelRotation();

            // Csak akkor, ha az Ctrl lenyomva van és van eredeti kép
            if (event.ControlDown() && this->originalImage->IsOk()) {
                double oldZoomFactor = this->inpaintZoomFactor;

                // Kontrolláljuk a zoom faktor változtatást
                if (rotation > 0) {
                    if (this->inpaintZoomFactor + this->inpaintZoomFactorStep > 2.0)
                        return;
                    this->inpaintZoomFactor += this->inpaintZoomFactorStep;
                } else if (rotation < 0) {
                    if (this->inpaintZoomFactor - this->inpaintZoomFactorStep < 0.1)
                        return;
                    this->inpaintZoomFactor -= this->inpaintZoomFactorStep;
                }

                wxPoint mousePos  = event.GetPosition();
                wxPoint viewStart = this->parent->GetViewStart();
                int scrollUnitX, scrollUnitY;
                this->parent->GetScrollPixelsPerUnit(&scrollUnitX, &scrollUnitY);

                double zoomFactorRatio = oldZoomFactor / this->inpaintZoomFactor;
                wxPoint newViewStart   = viewStart;
                newViewStart.x += (mousePos.x + viewStart.x * scrollUnitX - (this->parent->GetClientSize().GetWidth() / 2)) * (1 - zoomFactorRatio) / scrollUnitX;
                newViewStart.y += (mousePos.y + viewStart.y * scrollUnitY - (this->parent->GetClientSize().GetHeight() / 2)) * (1 - zoomFactorRatio) / scrollUnitY;

                newViewStart.x = std::max(0, std::min(newViewStart.x, (this->paintAreaBorder->GetWidth() - this->parent->GetClientSize().GetWidth()) / scrollUnitX));
                newViewStart.y = std::max(0, std::min(newViewStart.y, (this->paintAreaBorder->GetHeight() - this->parent->GetClientSize().GetHeight()) / scrollUnitY));

                this->UpdateParent(newViewStart);
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