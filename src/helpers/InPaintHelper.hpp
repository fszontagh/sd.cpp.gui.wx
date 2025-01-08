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

        struct ZoomFactor {
            ZoomFactor(double min, double max, double current)
                : min(min), max(max), current(current) {}

        private:
            double min       = 0.2;
            double max       = 2.0;
            double current   = 1.0;
            double zoomStep  = 0.1;
            double oldFactor = 1.0;

        public:
            void SetMin(double min) { this->min = min; }
            void SetMax(double max) { this->max = max; }
            double GetMin() { return this->min; }
            double GetMax() { return this->max; }
            double GetCurrent() { return this->current; }
            double GetOldFactor() { return this->oldFactor; }
            double GetZoomStep() { return this->zoomStep; }
            void ResetZoom() { this->current = 1; }
            void SetZoomFactor(double factor) {
                this->oldFactor = this->current;
                this->current   = factor;
            }
        };

    private:
        wxScrolledWindow* parent                           = nullptr;
        std::shared_ptr<wxBitmap> paintArea                = nullptr;
        std::shared_ptr<wxBitmap> outPaintArea             = nullptr;
        std::shared_ptr<wxBitmap> overlay                  = nullptr;
        std::shared_ptr<wxImage> originalImage             = nullptr;
        std::shared_ptr<wxImage> zoomedImage               = nullptr;
        std::shared_ptr<wxBitmap> mainBitmap               = nullptr;
        sd_gui_utils::wxEnlargeImageSizes enlargeSize      = sd_gui_utils::wxEnlargeImageSizes();
        bool painted                                       = false;
        bool outpainted                                    = false;
        BrushSize inpaintBrushSize                         = BrushSize{1, 20, 5};
        sd_gui_utils::InPaintHelper::ZoomFactor zoomFactor = ZoomFactor{0.25, 2, 1};
        sd_gui_utils::InPaintHelper::BrushStyle brushStyle = sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE;
        const int BITMAP_DEPTH                             = 32;
        wxPoint currentOffset                              = wxPoint(0, 0);
        wxPoint currentMousePosition                       = wxPoint(0, 0);

        // store the original image size
        wxSize originalSize = wxSize(512, 512);
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

            this->InitOutPaintArea(width, height);
            this->InitOverLay(width, height);
            this->InitPaintArea(width, height);

            this->painted      = false;
            this->originalSize = wxSize(width, height);
            this->outPaintSize = wxSize(width, height);
            this->inpaintBrushSize.ResetSize();
            this->zoomFactor.ResetZoom();
            this->enlargeSize.ResetSizes();
            this->currentOffset = wxPoint(0, 0);
            this->UpdateParent();
        }

        wxBitmap CreateBitmap(const wxSize size, const wxBrush& brush = *wxTRANSPARENT_BRUSH, const wxPen& pen = *wxTRANSPARENT_PEN) {
            return this->CreateBitmap(size.x, size.y, brush, pen);
        }

        wxBitmap CreateBitmap(int width, int height, const wxBrush& brush = *wxTRANSPARENT_BRUSH, const wxPen& pen = *wxTRANSPARENT_PEN) {
            wxBitmap buf(width, height, BITMAP_DEPTH);
            buf.UseAlpha();
            wxGraphicsContext* gc = wxGraphicsContext::Create(buf);
            if (gc) {
                gc->SetAntialiasMode(wxAntialiasMode::wxANTIALIAS_DEFAULT);
                gc->SetBrush(brush);
                gc->SetPen(pen);
                gc->DrawRectangle(0, 0, width, height);
                delete gc;
            }
            return buf;
        }

        wxPoint GetMousePosition(wxMouseEvent& event) {
            wxPoint mousePos = event.GetPosition();
            // Vegyük figyelembe a zoom faktort
            wxPoint zoomAdjustedPos = wxPoint(
                static_cast<int>(mousePos.x / this->zoomFactor.GetCurrent()),
                static_cast<int>(mousePos.y / this->zoomFactor.GetCurrent()));

            wxPoint res                = zoomAdjustedPos - this->currentOffset;
            this->currentMousePosition = res;
            return res;
        }

    public:
        InPaintHelper(wxScrolledWindow* parent)
            : parent(parent) {
            // @see https://wiki.wxwidgets.org/Drawing_on_a_panel_with_a_DC
            this->parent->GetParent()->Layout();
            this->parent->SetDoubleBuffered(true);
            this->parent->SetBackgroundStyle(wxBG_STYLE_PAINT);
            this->parent->SetScrollRate(10, 10);
        }
        void Reset() {
            this->originalImage = nullptr;
            this->zoomedImage   = nullptr;
            this->painted       = false;
            this->resetPaintArea();
        }

        const wxPoint GetMousePosition() {
            return this->currentMousePosition;
        }

        const wxPoint GetCurrentOffest() {
            return this->currentOffset;
        }

        void CleanMask() {
            if (this->painted == false) {
                return;
            }
            if (this->paintArea == nullptr) {
                return;
            }

            this->InitPaintArea(this->originalImage->GetWidth(), this->originalImage->GetHeight());
            this->painted = false;
            this->UpdateParent();
        }

        void InitOverLay(const wxSize& size) { this->InitOverLay(size.x, size.y); }
        void InitOverLay(int width, int height) {
            auto buf      = this->CreateBitmap(width, height);
            this->overlay = std::make_shared<wxBitmap>(buf);
        }
        void InitOutPaintArea(const wxSize& size) { this->InitOutPaintArea(size.x, size.y); }
        void InitOutPaintArea(int width, int height) {
            auto buf           = this->CreateBitmap(width, height);
            this->outPaintArea = std::make_shared<wxBitmap>(buf);
        }

        void InitPaintArea(const wxSize& size) { this->InitPaintArea(size.x, size.y); }
        void InitPaintArea(int width, int height) {
            auto buf        = this->CreateBitmap(width, height);
            this->paintArea = std::make_shared<wxBitmap>(buf);
        }

        BrushSize GetBrushSize() { return this->inpaintBrushSize; }
        unsigned int GetCurrentBrushSize() { return this->inpaintBrushSize.current; }
        void SetCurrentBrushSize(unsigned int size) { this->inpaintBrushSize.current = size; }
        void SetBrushSize(BrushSize size) { this->inpaintBrushSize = size; }
        sd_gui_utils::InPaintHelper::BrushStyle GetBrushStyle() { return this->brushStyle; }
        void SetBrushStyle(sd_gui_utils::InPaintHelper::BrushStyle style) { this->brushStyle = style; }
        double GetZoomFactor() { return this->zoomFactor.GetCurrent(); }
        double GetZoomStep() { return this->zoomFactor.GetZoomStep(); }

        wxSize CalcVirtualSize() {
            if (this->outPaintArea == nullptr) {
                return this->parent->GetVirtualSize();
            }
            wxSize s = {static_cast<int>(this->outPaintArea->GetWidth() * this->zoomFactor.GetCurrent()), static_cast<int>(this->outPaintArea->GetHeight() * this->zoomFactor.GetCurrent())};
            wxSize c = this->parent->GetClientSize();
            auto r   = wxSize(s.x < c.x ? c.x : s.x, s.y < c.y ? c.y : s.y);
            std::cout << "CalcVirtualSize: " << r.x << ", " << r.y << std::endl;
            std::cout << "OutpaintArea: " << this->outPaintArea->GetWidth() << ", " << this->outPaintArea->GetHeight() << std::endl;
            std::cout << "Client size: " << c.x << ", " << c.y << std::endl;
            std::cout << "ZoomFactor: " << this->zoomFactor.GetCurrent() << std::endl;
            return r;
        }
        void SetZoomFactor(double factor) {
            this->zoomFactor.SetZoomFactor(factor);
            this->UpdateParent();
        }

        void ChangeZoomFactor(int direction) {
            if (direction == 0) {
                return;
            }

            auto current = this->GetZoomFactor();
            if (direction > 0) {
                if (current + this->zoomFactor.GetZoomStep() > this->zoomFactor.GetMax()) {
                    if (current != this->zoomFactor.GetMax()) {
                        this->SetZoomFactor(this->zoomFactor.GetMax());
                    }
                    return;
                }
                current += this->zoomFactor.GetZoomStep();
            } else if (direction < 0) {
                if (current - this->zoomFactor.GetZoomStep() < this->zoomFactor.GetMin()) {
                    if (current != this->zoomFactor.GetMin()) {
                        this->SetZoomFactor(this->zoomFactor.GetMin());
                    }
                    return;
                }
                current -= this->zoomFactor.GetZoomStep();
            }

            this->SetZoomFactor(current);
        }
        void SetZoomFactor(sd_gui_utils::InPaintHelper::ZoomFactor factor) { this->zoomFactor = factor; }
        void SetZoomLimits(double min, double max) { this->zoomFactor = sd_gui_utils::InPaintHelper::ZoomFactor(min, max, this->zoomFactor.GetCurrent()); }
        bool isOutPainted() { return this->outpainted; }

        bool SetImage(const wxImage& image) noexcept {
            this->UpdateParent();
            this->originalImage = std::make_shared<wxImage>(image);
            this->zoomedImage   = nullptr;
            this->outPaintArea  = nullptr;
            this->originalSize  = image.GetSize();
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
            this->parent->SetVirtualSize(this->CalcVirtualSize());
            this->parent->Refresh();
            // this->parent->Update();
        }

        void UpdateParent(const wxPoint& scrollPos) {
            this->parent->SetVirtualSize(this->CalcVirtualSize());
            this->parent->SetScrollRate(10, 10);
            this->parent->Scroll(scrollPos);
            this->parent->Refresh();
            // this->parent->Update();
        }
        // handle events

        void OnDeleteInitialImage() {
            this->CleanMask();
            this->originalImage = nullptr;
            this->zoomedImage   = nullptr;
            this->imageLoaded   = false;
            this->resetPaintArea();
            this->UpdateParent();
        }

        bool OnInvertMask(wxCommandEvent& event) {
            if (this->imageLoaded) {
                sd_gui_utils::InvertWhiteAndTransparent(this->paintArea);
                this->UpdateParent();
                // area was empty
                if (this->painted == false) {
                    this->painted = true;
                }
                return true;
            }
            return false;
        }

        void OnMaskFileOpen(wxImage& img) {
            this->CleanMask();
            this->paintArea = std::make_shared<wxBitmap>(img);
            this->painted   = true;
            this->UpdateParent();
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

            wxBitmap outpaintarea = this->CreateBitmap(this->originalImage->GetSize());
            this->outPaintArea    = std::make_shared<wxBitmap>(outpaintarea);
            sd_gui_utils::CropOrFillBitmap(this->outPaintArea, enlargeSize, wxColour(128, 128, 128));
            this->zoomFactor.ResetZoom();
            this->enlargeSize  = enlargeSize;
            this->outPaintSize = this->outPaintArea->GetSize();
            this->UpdateParent();
        }

        wxImage OnSaveMask(wxCommandEvent& event) {
            wxImage img = this->paintArea->ConvertToImage();
            sd_gui_utils::blendImageOnBlackBackground(img);
            return img;
        }

        wxImage GetMaskImage(bool blend = true) {
            wxImage img = this->paintArea->ConvertToImage();
            if (blend) {
                sd_gui_utils::blendImageOnBlackBackground(img);
                return img;
            }
            return img;
        }
        wxImage GetOriginalImage() { return *this->originalImage; }
        wxImage OnResizeOriginalImage(int targetWidth, int targetHeight) {
            sd_gui_utils::ResizeImageToMaxSize(this->originalImage, targetWidth, targetHeight);
            this->originalSize = this->originalImage->GetSize();
            this->zoomedImage  = nullptr;
            this->outPaintArea = nullptr;
            this->resetPaintArea();
            this->UpdateParent();
            return *this->originalImage;
        }

        void OnEraseBackground(wxEraseEvent& event) {}

        void OnDcPaint(wxPaintEvent& event) {
            wxAutoBufferedPaintDC dc(this->parent);
            dc.SetUserScale(this->zoomFactor.GetCurrent(), this->zoomFactor.GetCurrent());
            dc.SetBackground(wxBrush(wxColour(51, 51, 51, 255)));
            dc.Clear();

            if (this->inPaintImageLoaded() == false) {
                return;
            }

            if (this->outPaintArea != nullptr && this->outPaintArea->IsOk() && this->outpainted) {
                auto offset = this->GetOffset(dc, this->outPaintArea->GetSize(), true, true, true);
                dc.DrawBitmap(*this->outPaintArea, offset.x, offset.y, false);
            }

            if (this->zoomedImage != nullptr && this->zoomedImage->IsOk()) {
                auto offset = this->GetOffset(dc, this->zoomedImage->GetSize(), true, true, false);
                dc.DrawBitmap(*this->zoomedImage, offset.x, offset.y, false);
            }

            if (this->paintArea != nullptr && this->paintArea->IsOk()) {
                auto offset         = this->GetOffset(dc, this->paintArea->GetSize(), true, true, false);
                this->currentOffset = offset;
                // this->currentOffset = this->GetOffset(dc, this->paintArea->GetSize(), true, true, true);
                dc.DrawBitmap(*this->paintArea, offset.x, offset.y, false);
            }

            if (this->overlay != nullptr && this->overlay->IsOk()) {
                auto offset         = this->GetOffset(dc, this->overlay->GetSize(), true, true, false);
                this->currentOffset = offset;
                dc.DrawBitmap(*this->overlay, offset.x, offset.y, false);
            }
            dc.SetUserScale(1, 1);
            this->parent->SetVirtualSize(this->CalcVirtualSize());
        }

        void OnSize(wxSizeEvent& event) {
            this->parent->SetVirtualSize(this->CalcVirtualSize());
            if (this->outpainted) {
            }
            event.Skip();
        }

        wxPoint GetOffset(wxAutoBufferedPaintDC& gc, const wxSize& imgSize, bool addScrollPos = true, bool addBorder = true, bool ignoreEnlarge = false) {
            double scaleX, scaleY;
            gc.GetUserScale(&scaleX, &scaleY);

            wxSize size = {static_cast<int>(imgSize.GetWidth() * scaleX), static_cast<int>(imgSize.GetHeight() * scaleY)};
            if (ignoreEnlarge == false) {
                size.x += (this->enlargeSize.left + this->enlargeSize.right) * scaleX;
                size.y += (this->enlargeSize.top + this->enlargeSize.bottom) * scaleY;
            }
            wxSize gcSize = this->parent->GetClientSize();
            std::cout << "Offseting with gc size x: " << gcSize.GetWidth() << ", y: " << gcSize.GetHeight() << std::endl;
            int resultX = addBorder ? this->borderSize : 0;
            int resultY = addBorder ? this->borderSize : 0;

            if (ignoreEnlarge == false) {
                resultX += enlargeSize.left;
                resultY += enlargeSize.top;
            }

            if (gcSize.GetWidth() > size.GetWidth()) {
                resultX = (gcSize.GetWidth() - size.GetWidth()) / 2;
                resultX /= this->zoomFactor.GetCurrent();
                if (ignoreEnlarge == false) {
                    resultX += enlargeSize.left;
                }
                resultX += addBorder ? this->borderSize : 0;
            }
            if (gcSize.GetHeight() > size.GetHeight()) {
                resultY = (gcSize.GetHeight() - size.GetHeight()) / 2;
                resultY /= this->zoomFactor.GetCurrent();
                if (ignoreEnlarge == false) {
                    resultY += enlargeSize.top;
                }

                resultY += addBorder ? this->borderSize : 0;
            }

            if (addScrollPos) {
                wxPoint scrollPos = this->parent->CalcUnscrolledPosition(wxPoint(0, 0));
                resultX -= scrollPos.x;
                resultY -= scrollPos.y;
            }

            return wxPoint(resultX, resultY);
        }

        void OnMouseRightDown(wxMouseEvent& event) {
            auto mPos = this->GetMousePosition(event);
            if (mPos.x >= 0 && mPos.y >= 0 && mPos.x <= this->paintArea->GetWidth() && mPos.y <= this->paintArea->GetHeight()) {
                this->isDrawing    = true;
                this->lastPosition = mPos;
                return;
            }
            this->isDrawing = false;
        }
        void OnMouseRightUp(wxMouseEvent& event) { this->isDrawing = false; }
        void OnMouseLeftDown(wxMouseEvent& event) {
            auto mPos = this->GetMousePosition(event);
            if (mPos.x >= 0 && mPos.y >= 0 && mPos.x <= this->paintArea->GetWidth() && mPos.y <= this->paintArea->GetHeight()) {
                this->isDrawing    = true;
                this->lastPosition = mPos;
                return;
            }
            this->isDrawing = false;
        }
        void OnMouseLeftUp(wxMouseEvent& event) { this->isDrawing = false; }
        void OnMouseLeave(wxMouseEvent& event) { this->isDrawing = false; }
        void OnMouseMotion(wxMouseEvent& event) {
            if (this->isDrawing == false) {
                return;
            }
            if (event.LeftIsDown() == false && event.RightIsDown() == false) {
                return;
            }

            wxGraphicsContext* gc = wxGraphicsContext::Create(*this->paintArea);

            if (!gc) {
                return;
            }

            this->currentMousePosition = this->GetMousePosition(event);
            auto comp                  = gc->GetCompositionMode();
            gc->SetCompositionMode(event.LeftIsDown() ? comp : wxCOMPOSITION_SOURCE);

            if (event.LeftIsDown()) {
                gc->SetBrush(wxBrush(wxColour(255, 255, 255), wxBRUSHSTYLE_SOLID));
                gc->SetPen(wxPen(wxColour(255, 255, 255), this->inpaintBrushSize.current, wxPENSTYLE_SOLID));
            } else {
                gc->SetBrush(wxBrush(wxColour(0, 0, 0, 0), wxBRUSHSTYLE_SOLID));                            // Teljesen átlátszó ecset
                gc->SetPen(wxPen(wxColour(0, 0, 0, 0), this->inpaintBrushSize.current, wxPENSTYLE_SOLID));  // Teljesen átlátszó toll
            }
            int maxBoundaryX = this->paintArea->GetWidth();
            int maxBoundaryY = this->paintArea->GetHeight();

            if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_CIRCLE) {
                int startX = this->currentMousePosition.x - this->inpaintBrushSize.current;
                int startY = this->currentMousePosition.y - this->inpaintBrushSize.current;

                int width  = this->inpaintBrushSize.current * 2;
                int height = this->inpaintBrushSize.current * 2;

                startX = startX < 0 ? 0 : startX;
                startY = startY < 0 ? 0 : startY;

                if (startX + width > maxBoundaryX) {
                    startX = maxBoundaryX - width;
                }
                if (startY + height > maxBoundaryY) {
                    startY = maxBoundaryY - height;
                }
                gc->DrawEllipse(startX, startY, width, height);
                this->parent->Refresh();
            } else if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_SQUARE) {
                int startX = this->currentMousePosition.x - (this->inpaintBrushSize.current / 2);
                int startY = this->currentMousePosition.y - (this->inpaintBrushSize.current / 2);

                int width  = this->inpaintBrushSize.current * 2;
                int height = this->inpaintBrushSize.current * 2;
                startX     = startY < 0 ? 0 : startX;
                startY     = startY < 0 ? 0 : startY;
                if (startX + width > maxBoundaryX) {
                    startX = maxBoundaryX - width;
                }
                if (startY + height > maxBoundaryY) {
                    startY = maxBoundaryY - height;
                }
                gc->DrawRectangle(startX, startY, width, height);
                this->parent->Refresh();
            } else if (this->brushStyle == sd_gui_utils::InPaintHelper::BrushStyle::BRUSH_TRIANGLE) {
                // TODO: fix boundaries here too
                wxPoint2DDouble points[3];
                points[0] = wxPoint2DDouble(this->currentMousePosition.x, this->currentMousePosition.y - (this->inpaintBrushSize.current / 2));
                points[1] = wxPoint2DDouble(this->currentMousePosition.x - (this->inpaintBrushSize.current / 2), this->currentMousePosition.y + (this->inpaintBrushSize.current / 2));
                points[2] = wxPoint2DDouble(this->currentMousePosition.x + (this->inpaintBrushSize.current / 2), this->currentMousePosition.y + (this->inpaintBrushSize.current / 2));

                // check negative boundaries
                if (points[0].m_x < 0) {
                    points[0].m_x = 0;
                }
                if (points[0].m_y < 0) {
                    points[0].m_y = 0;
                }

                if (points[1].m_x < 0) {
                    points[1].m_x = 0;
                }
                if (points[1].m_y < 0) {
                    points[1].m_y = 0;
                }

                gc->DrawLines(3, points);
                this->parent->Refresh();
            }
            this->painted = true;

            delete gc;
            this->lastPosition = this->currentMousePosition;
        }

        void OnMouseWheel(int rotation, bool zoom, const wxEnlargeImageSizes& enlargeSizes) {
            if (this->imageLoaded == false) {
                return;
            }

            if (zoom && this->originalImage->IsOk()) {
                this->ChangeZoomFactor(rotation);
            }

            if (zoom == false) {
                if (rotation > 0) {
                    this->inpaintBrushSize.current = std::min(this->inpaintBrushSize.current + 1, this->inpaintBrushSize.max);
                } else if (rotation < 0) {
                    this->inpaintBrushSize.current = std::max(this->inpaintBrushSize.current - 1, this->inpaintBrushSize.min);
                }
                return;
            }
        }
    };
};

#endif