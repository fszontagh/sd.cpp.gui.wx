#ifndef SETTINGS_PNG_H
#define SETTINGS_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char settings_png[] =
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
	0x1A, 0x00, 0x00, 0x01, 0x24, 0x49, 0x44, 0x41, 0x54, 0x38, 
	0x8D, 0x85, 0xD3, 0xBB, 0x2E, 0x44, 0x61, 0x14, 0x05, 0xE0, 
	0x6F, 0x06, 0x41, 0xE7, 0x01, 0x28, 0x48, 0x68, 0x54, 0x74, 
	0x1E, 0xC4, 0x2B, 0x30, 0x6E, 0x8D, 0x56, 0xA9, 0x17, 0x0D, 
	0x71, 0x49, 0x46, 0x46, 0x3C, 0x83, 0x67, 0x50, 0x8E, 0xA1, 
	0xF7, 0x10, 0x6E, 0x89, 0x88, 0x51, 0x9C, 0x35, 0xE3, 0x0C, 
	0x67, 0xC6, 0x4E, 0x76, 0xF2, 0x9F, 0xB5, 0xD7, 0xDA, 0xE7, 
	0xDF, 0x97, 0xBF, 0x66, 0xB8, 0x75, 0x7F, 0x7D, 0xD7, 0xAA, 
	0x48, 0xF5, 0x11, 0x09, 0xE0, 0x28, 0x3E, 0xD4, 0x7A, 0x09, 
	0x66, 0x71, 0x80, 0x69, 0x4C, 0x62, 0x2F, 0xF8, 0x5D, 0x5C, 
	0xB0, 0xC9, 0x70, 0x0E, 0xA2, 0xE9, 0xDB, 0x4D, 0xAE, 0xFC, 
	0x84, 0x07, 0xBC, 0xA2, 0x89, 0xF9, 0xF8, 0x55, 0xB0, 0x4E, 
	0x38, 0xDD, 0x68, 0xC0, 0x1A, 0xBE, 0xB0, 0x8D, 0x6B, 0x3C, 
	0x62, 0xB1, 0xE2, 0xB6, 0x8B, 0x89, 0xB5, 0xB0, 0x15, 0xCD, 
	0x1A, 0xDC, 0xE2, 0x13, 0x0D, 0x8C, 0xC5, 0x61, 0x05, 0x17, 
	0x38, 0xCB, 0x59, 0x29, 0xDE, 0x88, 0xE6, 0x16, 0x66, 0x14, 
	0x8D, 0xFA, 0xC4, 0x61, 0x49, 0xFC, 0x9E, 0x72, 0xDA, 0x39, 
	0xAF, 0x26, 0x76, 0x18, 0xEE, 0x51, 0xB4, 0x7D, 0x7B, 0xC1, 
	0x46, 0xCE, 0x67, 0xB9, 0xEE, 0xB8, 0xA2, 0xD1, 0x6D, 0x9C, 
	0x26, 0xB6, 0x89, 0xE7, 0x9E, 0x68, 0xD4, 0x18, 0x6B, 0xA9, 
	0xB3, 0x8E, 0x09, 0x7C, 0x54, 0x91, 0xEA, 0xA5, 0x12, 0xA6, 
	0x30, 0x17, 0xFC, 0x1C, 0x0B, 0x29, 0xE1, 0x5E, 0x31, 0x89, 
	0xCB, 0xC4, 0x66, 0x15, 0xA3, 0xEC, 0x97, 0x30, 0xAA, 0x89, 
	0x27, 0x38, 0xC6, 0x72, 0xB0, 0x5E, 0x7C, 0x53, 0xA9, 0x89, 
	0xE5, 0x31, 0xB6, 0xF2, 0xD7, 0xAA, 0x31, 0x2E, 0x29, 0xFA, 
	0x72, 0x8D, 0x1D, 0xA5, 0x31, 0x32, 0xB8, 0x48, 0x1D, 0xBC, 
	0x19, 0x5C, 0xA4, 0x66, 0xB0, 0xCA, 0x45, 0xEA, 0xD5, 0x55, 
	0x5E, 0xE5, 0xDD, 0x90, 0xD6, 0xE3, 0xDD, 0x60, 0x7F, 0x56, 
	0xB9, 0xF2, 0x85, 0xC5, 0xBA, 0x7E, 0x1E, 0xD2, 0xFE, 0x30, 
	0xEE, 0x7F, 0x09, 0xFE, 0xE5, 0x7E, 0x03, 0x52, 0x38, 0x4E, 
	0xAF, 0x8A, 0x7A, 0x49, 0x82, 0x00, 0x00, 0x00, 0x00, 0x49, 
	0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, 
};

wxBitmap& settings_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( settings_png, sizeof( settings_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
}


#endif //SETTINGS_PNG_H