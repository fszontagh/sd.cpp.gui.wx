#ifndef REFRESH_PNG_H
#define REFRESH_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char refresh_png[] =
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 
	0xF3, 0xFF, 0x61, 0x00, 0x00, 0x00, 0x09, 0x70, 0x48, 0x59, 
	0x73, 0x00, 0x00, 0x00, 0x76, 0x00, 0x00, 0x00, 0x76, 0x01, 
	0x4E, 0x7B, 0x26, 0x08, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 
	0x58, 0x74, 0x53, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 
	0x00, 0x77, 0x77, 0x77, 0x2E, 0x69, 0x6E, 0x6B, 0x73, 0x63, 
	0x61, 0x70, 0x65, 0x2E, 0x6F, 0x72, 0x67, 0x9B, 0xEE, 0x3C, 
	0x1A, 0x00, 0x00, 0x01, 0x16, 0x49, 0x44, 0x41, 0x54, 0x38, 
	0x8D, 0x9D, 0xD3, 0xBB, 0x4A, 0x43, 0x41, 0x10, 0x06, 0xE0, 
	0x8F, 0xE0, 0x35, 0x88, 0x22, 0x3E, 0x85, 0x78, 0x97, 0x80, 
	0x85, 0x20, 0x36, 0x16, 0xDA, 0xA4, 0xB3, 0xB7, 0xF1, 0x39, 
	0xF2, 0x06, 0x82, 0xC1, 0xEB, 0x9B, 0xD8, 0x44, 0xF0, 0x15, 
	0x44, 0xB1, 0xB1, 0xD0, 0xCE, 0x36, 0x48, 0x42, 0x88, 0x31, 
	0x16, 0x3B, 0x27, 0x1C, 0x35, 0x17, 0xC8, 0x0F, 0x87, 0xD9, 
	0x5D, 0xE6, 0xFF, 0x77, 0xE6, 0xEC, 0x3F, 0xFC, 0xC7, 0x06, 
	0x2E, 0xF0, 0x82, 0x36, 0x5A, 0x78, 0x46, 0x15, 0xAB, 0x7F, 
	0x72, 0xE7, 0xF2, 0x9B, 0x22, 0x6E, 0xF1, 0x8D, 0xEE, 0x80, 
	0xAF, 0x13, 0x42, 0x53, 0xA8, 0xA0, 0x9C, 0x91, 0x67, 0xF1, 
	0x10, 0x49, 0x4D, 0x9C, 0xA1, 0x14, 0xA2, 0x45, 0xEC, 0x04, 
	0xB1, 0x15, 0x39, 0xAF, 0x11, 0x7B, 0x02, 0x37, 0x71, 0xF0, 
	0x8E, 0xB5, 0x3E, 0x6D, 0x65, 0xD8, 0x42, 0x3D, 0x57, 0x51, 
	0x19, 0x36, 0xA3, 0xB4, 0xE6, 0x08, 0xB2, 0x28, 0x3B, 0xDF, 
	0xD2, 0xC1, 0x04, 0x4E, 0x51, 0xC0, 0x35, 0x1E, 0x87, 0x90, 
	0xE7, 0xF1, 0x84, 0xE3, 0xD8, 0x37, 0x70, 0x4F, 0xFA, 0xDB, 
	0xDD, 0xE8, 0x79, 0x2C, 0x34, 0x43, 0xA0, 0x38, 0xAE, 0xC0, 
	0x38, 0x58, 0x8C, 0x4B, 0x3F, 0x0B, 0x63, 0x0A, 0x2C, 0x47, 
	0x7C, 0x9B, 0xC0, 0x34, 0x0E, 0x31, 0x99, 0x4B, 0xB8, 0x93, 
	0x9E, 0x6B, 0x10, 0x4E, 0x22, 0xD6, 0xE0, 0xC8, 0xEF, 0xA7, 
	0xA9, 0x8C, 0xB8, 0xBD, 0x24, 0x19, 0xAA, 0x83, 0x75, 0x92, 
	0x19, 0x32, 0x72, 0x1D, 0xBB, 0x43, 0xC8, 0xDB, 0xF8, 0x88, 
	0xDC, 0xAB, 0xEC, 0x30, 0x13, 0xC8, 0xEC, 0xD9, 0x96, 0x9C, 
	0xB9, 0x87, 0x25, 0x2C, 0x60, 0x5F, 0x1A, 0xB0, 0xAF, 0xC8, 
	0xA9, 0x61, 0x26, 0x2F, 0x50, 0x91, 0x06, 0xE4, 0x3C, 0x4A, 
	0x1B, 0x36, 0x4C, 0x97, 0xD2, 0xEC, 0xF4, 0xF0, 0x6B, 0x24, 
	0xB1, 0x12, 0x42, 0x4F, 0x92, 0x47, 0x1A, 0xB1, 0xAE, 0xEA, 
	0x63, 0xF5, 0x1F, 0x67, 0xAD, 0x56, 0xFD, 0xBC, 0x3E, 0x67, 
	0x79, 0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 
	0x42, 0x60, 0x82, 
};

wxBitmap& refresh_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( refresh_png, sizeof( refresh_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
}


#endif //REFRESH_PNG_H
