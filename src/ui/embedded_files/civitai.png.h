#ifndef CIVITAI_PNG_H
#define CIVITAI_PNG_H

#include <wx/mstream.h>
#include <wx/image.h>
#include <wx/bitmap.h>

static const unsigned char civitai_png[] =
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 
	0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 0x00, 0x00, 0x00, 0x10, 
	0x00, 0x00, 0x00, 0x10, 0x08, 0x06, 0x00, 0x00, 0x00, 0x1F, 
	0xF3, 0xFF, 0x61, 0x00, 0x00, 0x00, 0xED, 0x7A, 0x54, 0x58, 
	0x74, 0x52, 0x61, 0x77, 0x20, 0x70, 0x72, 0x6F, 0x66, 0x69, 
	0x6C, 0x65, 0x20, 0x74, 0x79, 0x70, 0x65, 0x20, 0x65, 0x78, 
	0x69, 0x66, 0x00, 0x00, 0x78, 0xDA, 0x8D, 0x51, 0x5B, 0x6E, 
	0xC4, 0x30, 0x08, 0xFC, 0xF7, 0x29, 0x7A, 0x04, 0x0C, 0x18, 
	0x9B, 0xE3, 0x38, 0x8F, 0x95, 0x7A, 0x83, 0x3D, 0xFE, 0x4E, 
	0x62, 0x9C, 0x34, 0x95, 0x2A, 0x15, 0xC9, 0x18, 0x06, 0x02, 
	0xE3, 0x49, 0xDA, 0xDF, 0xDF, 0xAF, 0xF4, 0x75, 0x18, 0x37, 
	0x4E, 0x5A, 0x6A, 0x33, 0x37, 0x23, 0x98, 0xBA, 0x3A, 0x77, 
	0x04, 0x8D, 0x86, 0x2D, 0xA7, 0xCF, 0xA4, 0xA7, 0x8F, 0x84, 
	0x66, 0xF0, 0xC0, 0xD3, 0x55, 0x60, 0x40, 0x82, 0x5B, 0x46, 
	0xEA, 0x1C, 0xF8, 0x0E, 0x1C, 0x71, 0x8E, 0xDC, 0x63, 0x49, 
	0x9E, 0xFD, 0x73, 0xD0, 0xB5, 0xA9, 0x23, 0x2A, 0x77, 0xA1, 
	0xF7, 0xC0, 0x97, 0x27, 0xBE, 0xC4, 0x40, 0x6E, 0xBF, 0x07, 
	0x05, 0x03, 0xC9, 0x63, 0x33, 0x6D, 0xF1, 0x41, 0x0C, 0x12, 
	0x0E, 0x46, 0x3A, 0xF2, 0x35, 0x18, 0x99, 0xB7, 0xFA, 0x78, 
	0xDA, 0xB6, 0xD2, 0xD3, 0xDA, 0x7D, 0x54, 0x2A, 0x5B, 0xB1, 
	0x5C, 0x15, 0x5E, 0x99, 0x6A, 0x35, 0x47, 0xDC, 0x98, 0xB4, 
	0x42, 0xCF, 0xED, 0x20, 0xFA, 0x5A, 0xD9, 0xCF, 0x41, 0x65, 
	0x08, 0x7A, 0x01, 0x33, 0x9F, 0xAD, 0x0C, 0x4E, 0xBC, 0x4B, 
	0x16, 0x82, 0x17, 0xB1, 0xC1, 0x52, 0x8E, 0xA3, 0xD2, 0x71, 
	0x0B, 0x7C, 0x96, 0x96, 0xD0, 0x98, 0xC5, 0x90, 0x8C, 0x42, 
	0x91, 0x21, 0x3C, 0xE1, 0x57, 0x82, 0x02, 0x98, 0x7B, 0x2C, 
	0xEA, 0x74, 0x89, 0xF9, 0x53, 0x9B, 0x5B, 0xA3, 0x3F, 0xEC, 
	0x3F, 0xCF, 0x4A, 0x1F, 0x18, 0xF4, 0x76, 0xB0, 0x04, 0x3D, 
	0x3A, 0x1F, 0x00, 0x00, 0x01, 0x84, 0x69, 0x43, 0x43, 0x50, 
	0x49, 0x43, 0x43, 0x20, 0x70, 0x72, 0x6F, 0x66, 0x69, 0x6C, 
	0x65, 0x00, 0x00, 0x78, 0x9C, 0x7D, 0x91, 0x3D, 0x48, 0xC3, 
	0x40, 0x1C, 0xC5, 0x5F, 0x53, 0x45, 0x91, 0x4A, 0x41, 0x3B, 
	0x14, 0x11, 0xC9, 0x50, 0x9D, 0xEC, 0xA2, 0x22, 0x8E, 0xA5, 
	0x8A, 0x45, 0xB0, 0x50, 0xDA, 0x0A, 0xAD, 0x3A, 0x98, 0x5C, 
	0xFA, 0x05, 0x4D, 0x1A, 0x92, 0x14, 0x17, 0x47, 0xC1, 0xB5, 
	0xE0, 0xE0, 0xC7, 0x62, 0xD5, 0xC1, 0xC5, 0x59, 0x57, 0x07, 
	0x57, 0x41, 0x10, 0xFC, 0x00, 0x71, 0x76, 0x70, 0x52, 0x74, 
	0x91, 0x12, 0xFF, 0x97, 0x14, 0x5A, 0xC4, 0x78, 0x70, 0xDC, 
	0x8F, 0x77, 0xF7, 0x1E, 0x77, 0xEF, 0x00, 0xA1, 0x59, 0x65, 
	0xAA, 0xD9, 0x13, 0x03, 0x54, 0xCD, 0x32, 0xD2, 0x89, 0xB8, 
	0x98, 0xCB, 0xAF, 0x8A, 0x7D, 0xAF, 0xF0, 0x63, 0x08, 0x41, 
	0x84, 0x31, 0x26, 0x31, 0x53, 0x4F, 0x66, 0x16, 0xB3, 0xF0, 
	0x1C, 0x5F, 0xF7, 0xF0, 0xF1, 0xF5, 0x2E, 0xCA, 0xB3, 0xBC, 
	0xCF, 0xFD, 0x39, 0x06, 0x95, 0x82, 0xC9, 0x00, 0x9F, 0x48, 
	0x1C, 0x63, 0xBA, 0x61, 0x11, 0x6F, 0x10, 0xCF, 0x6E, 0x5A, 
	0x3A, 0xE7, 0x7D, 0xE2, 0x10, 0x2B, 0x4B, 0x0A, 0xF1, 0x39, 
	0xF1, 0xA4, 0x41, 0x17, 0x24, 0x7E, 0xE4, 0xBA, 0xEC, 0xF2, 
	0x1B, 0xE7, 0x92, 0xC3, 0x02, 0xCF, 0x0C, 0x19, 0xD9, 0xF4, 
	0x3C, 0x71, 0x88, 0x58, 0x2C, 0x75, 0xB1, 0xDC, 0xC5, 0xAC, 
	0x6C, 0xA8, 0xC4, 0x33, 0xC4, 0x11, 0x45, 0xD5, 0x28, 0x5F, 
	0xC8, 0xB9, 0xAC, 0x70, 0xDE, 0xE2, 0xAC, 0x56, 0xEB, 0xAC, 
	0x7D, 0x4F, 0xFE, 0xC2, 0x40, 0x41, 0x5B, 0xC9, 0x70, 0x9D, 
	0xE6, 0x28, 0x12, 0x58, 0x42, 0x12, 0x29, 0x88, 0x90, 0x51, 
	0x47, 0x05, 0x55, 0x58, 0x88, 0xD2, 0xAA, 0x91, 0x62, 0x22, 
	0x4D, 0xFB, 0x71, 0x0F, 0xFF, 0x88, 0xE3, 0x4F, 0x91, 0x4B, 
	0x26, 0x57, 0x05, 0x8C, 0x1C, 0x0B, 0xA8, 0x41, 0x85, 0xE4, 
	0xF8, 0xC1, 0xFF, 0xE0, 0x77, 0xB7, 0x66, 0x71, 0x7A, 0xCA, 
	0x4D, 0x0A, 0xC4, 0x81, 0xDE, 0x17, 0xDB, 0xFE, 0x18, 0x07, 
	0xFA, 0x76, 0x81, 0x56, 0xC3, 0xB6, 0xBF, 0x8F, 0x6D, 0xBB, 
	0x75, 0x02, 0xF8, 0x9F, 0x81, 0x2B, 0xAD, 0xE3, 0xAF, 0x35, 
	0x81, 0xB9, 0x4F, 0xD2, 0x1B, 0x1D, 0x2D, 0x72, 0x04, 0x04, 
	0xB7, 0x81, 0x8B, 0xEB, 0x8E, 0x26, 0xEF, 0x01, 0x97, 0x3B, 
	0x40, 0xF8, 0x49, 0x97, 0x0C, 0xC9, 0x91, 0xFC, 0x34, 0x85, 
	0x62, 0x11, 0x78, 0x3F, 0xA3, 0x6F, 0xCA, 0x03, 0xC3, 0xB7, 
	0xC0, 0xC0, 0x9A, 0xDB, 0x5B, 0x7B, 0x1F, 0xA7, 0x0F, 0x40, 
	0x96, 0xBA, 0x5A, 0xBE, 0x01, 0x0E, 0x0E, 0x81, 0x89, 0x12, 
	0x65, 0xAF, 0x7B, 0xBC, 0xBB, 0xBF, 0xBB, 0xB7, 0x7F, 0xCF, 
	0xB4, 0xFB, 0xFB, 0x01, 0x9E, 0x9E, 0x72, 0xB8, 0x22, 0x2D, 
	0xF7, 0xC0, 0x00, 0x00, 0x0D, 0x76, 0x69, 0x54, 0x58, 0x74, 
	0x58, 0x4D, 0x4C, 0x3A, 0x63, 0x6F, 0x6D, 0x2E, 0x61, 0x64, 
	0x6F, 0x62, 0x65, 0x2E, 0x78, 0x6D, 0x70, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3C, 0x3F, 0x78, 0x70, 0x61, 0x63, 0x6B, 0x65, 
	0x74, 0x20, 0x62, 0x65, 0x67, 0x69, 0x6E, 0x3D, 0x22, 0xEF, 
	0xBB, 0xBF, 0x22, 0x20, 0x69, 0x64, 0x3D, 0x22, 0x57, 0x35, 
	0x4D, 0x30, 0x4D, 0x70, 0x43, 0x65, 0x68, 0x69, 0x48, 0x7A, 
	0x72, 0x65, 0x53, 0x7A, 0x4E, 0x54, 0x63, 0x7A, 0x6B, 0x63, 
	0x39, 0x64, 0x22, 0x3F, 0x3E, 0x0A, 0x3C, 0x78, 0x3A, 0x78, 
	0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x20, 0x78, 0x6D, 0x6C, 
	0x6E, 0x73, 0x3A, 0x78, 0x3D, 0x22, 0x61, 0x64, 0x6F, 0x62, 
	0x65, 0x3A, 0x6E, 0x73, 0x3A, 0x6D, 0x65, 0x74, 0x61, 0x2F, 
	0x22, 0x20, 0x78, 0x3A, 0x78, 0x6D, 0x70, 0x74, 0x6B, 0x3D, 
	0x22, 0x58, 0x4D, 0x50, 0x20, 0x43, 0x6F, 0x72, 0x65, 0x20, 
	0x34, 0x2E, 0x34, 0x2E, 0x30, 0x2D, 0x45, 0x78, 0x69, 0x76, 
	0x32, 0x22, 0x3E, 0x0A, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 
	0x52, 0x44, 0x46, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 
	0x72, 0x64, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 
	0x2F, 0x2F, 0x77, 0x77, 0x77, 0x2E, 0x77, 0x33, 0x2E, 0x6F, 
	0x72, 0x67, 0x2F, 0x31, 0x39, 0x39, 0x39, 0x2F, 0x30, 0x32, 
	0x2F, 0x32, 0x32, 0x2D, 0x72, 0x64, 0x66, 0x2D, 0x73, 0x79, 
	0x6E, 0x74, 0x61, 0x78, 0x2D, 0x6E, 0x73, 0x23, 0x22, 0x3E, 
	0x0A, 0x20, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 0x44, 0x65, 
	0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 0x6F, 0x6E, 0x20, 
	0x72, 0x64, 0x66, 0x3A, 0x61, 0x62, 0x6F, 0x75, 0x74, 0x3D, 
	0x22, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x6C, 
	0x6E, 0x73, 0x3A, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3D, 0x22, 
	0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 
	0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 
	0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x6D, 0x6D, 
	0x2F, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x6C, 
	0x6E, 0x73, 0x3A, 0x73, 0x74, 0x45, 0x76, 0x74, 0x3D, 0x22, 
	0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 
	0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 
	0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x73, 0x54, 
	0x79, 0x70, 0x65, 0x2F, 0x52, 0x65, 0x73, 0x6F, 0x75, 0x72, 
	0x63, 0x65, 0x45, 0x76, 0x65, 0x6E, 0x74, 0x23, 0x22, 0x0A, 
	0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 
	0x64, 0x63, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 
	0x2F, 0x70, 0x75, 0x72, 0x6C, 0x2E, 0x6F, 0x72, 0x67, 0x2F, 
	0x64, 0x63, 0x2F, 0x65, 0x6C, 0x65, 0x6D, 0x65, 0x6E, 0x74, 
	0x73, 0x2F, 0x31, 0x2E, 0x31, 0x2F, 0x22, 0x0A, 0x20, 0x20, 
	0x20, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x47, 0x49, 
	0x4D, 0x50, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 
	0x2F, 0x77, 0x77, 0x77, 0x2E, 0x67, 0x69, 0x6D, 0x70, 0x2E, 
	0x6F, 0x72, 0x67, 0x2F, 0x78, 0x6D, 0x70, 0x2F, 0x22, 0x0A, 
	0x20, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 
	0x74, 0x69, 0x66, 0x66, 0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 
	0x3A, 0x2F, 0x2F, 0x6E, 0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 
	0x65, 0x2E, 0x63, 0x6F, 0x6D, 0x2F, 0x74, 0x69, 0x66, 0x66, 
	0x2F, 0x31, 0x2E, 0x30, 0x2F, 0x22, 0x0A, 0x20, 0x20, 0x20, 
	0x20, 0x78, 0x6D, 0x6C, 0x6E, 0x73, 0x3A, 0x78, 0x6D, 0x70, 
	0x3D, 0x22, 0x68, 0x74, 0x74, 0x70, 0x3A, 0x2F, 0x2F, 0x6E, 
	0x73, 0x2E, 0x61, 0x64, 0x6F, 0x62, 0x65, 0x2E, 0x63, 0x6F, 
	0x6D, 0x2F, 0x78, 0x61, 0x70, 0x2F, 0x31, 0x2E, 0x30, 0x2F, 
	0x22, 0x0A, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x70, 0x4D, 0x4D, 
	0x3A, 0x44, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x49, 
	0x44, 0x3D, 0x22, 0x67, 0x69, 0x6D, 0x70, 0x3A, 0x64, 0x6F, 
	0x63, 0x69, 0x64, 0x3A, 0x67, 0x69, 0x6D, 0x70, 0x3A, 0x61, 
	0x65, 0x34, 0x33, 0x31, 0x33, 0x37, 0x61, 0x2D, 0x37, 0x31, 
	0x64, 0x39, 0x2D, 0x34, 0x30, 0x61, 0x33, 0x2D, 0x39, 0x64, 
	0x63, 0x65, 0x2D, 0x34, 0x63, 0x30, 0x66, 0x65, 0x36, 0x36, 
	0x38, 0x33, 0x37, 0x33, 0x34, 0x22, 0x0A, 0x20, 0x20, 0x20, 
	0x78, 0x6D, 0x70, 0x4D, 0x4D, 0x3A, 0x49, 0x6E, 0x73, 0x74, 
	0x61, 0x6E, 0x63, 0x65, 0x49, 0x44, 0x3D, 0x22, 0x78, 0x6D, 
	0x70, 0x2E, 0x69, 0x69, 0x64, 0x3A, 0x32, 0x62, 0x32, 0x66, 
	0x39, 0x34, 0x63, 0x61, 0x2D, 0x31, 0x38, 0x63, 0x61, 0x2D, 
	0x34, 0x30, 0x36, 0x65, 0x2D, 0x61, 0x33, 0x31, 0x33, 0x2D, 
	0x65, 0x63, 0x35, 0x66, 0x64, 0x63, 0x65, 0x35, 0x39, 0x32, 
	0x62, 0x62, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x78, 0x6D, 0x70, 
	0x4D, 0x4D, 0x3A, 0x4F, 0x72, 0x69, 0x67, 0x69, 0x6E, 0x61, 
	0x6C, 0x44, 0x6F, 0x63, 0x75, 0x6D, 0x65, 0x6E, 0x74, 0x49, 
	0x44, 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x64, 0x69, 0x64, 
	0x3A, 0x36, 0x37, 0x31, 0x34, 0x34, 0x62, 0x65, 0x36, 0x2D, 
	0x34, 0x34, 0x66, 0x64, 0x2D, 0x34, 0x64, 0x35, 0x34, 0x2D, 
	0x39, 0x33, 0x62, 0x63, 0x2D, 0x39, 0x61, 0x61, 0x38, 0x30, 
	0x38, 0x35, 0x38, 0x36, 0x31, 0x39, 0x39, 0x22, 0x0A, 0x20, 
	0x20, 0x20, 0x64, 0x63, 0x3A, 0x46, 0x6F, 0x72, 0x6D, 0x61, 
	0x74, 0x3D, 0x22, 0x69, 0x6D, 0x61, 0x67, 0x65, 0x2F, 0x70, 
	0x6E, 0x67, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x47, 0x49, 0x4D, 
	0x50, 0x3A, 0x41, 0x50, 0x49, 0x3D, 0x22, 0x32, 0x2E, 0x30, 
	0x22, 0x0A, 0x20, 0x20, 0x20, 0x47, 0x49, 0x4D, 0x50, 0x3A, 
	0x50, 0x6C, 0x61, 0x74, 0x66, 0x6F, 0x72, 0x6D, 0x3D, 0x22, 
	0x57, 0x69, 0x6E, 0x64, 0x6F, 0x77, 0x73, 0x22, 0x0A, 0x20, 
	0x20, 0x20, 0x47, 0x49, 0x4D, 0x50, 0x3A, 0x54, 0x69, 0x6D, 
	0x65, 0x53, 0x74, 0x61, 0x6D, 0x70, 0x3D, 0x22, 0x31, 0x37, 
	0x31, 0x30, 0x37, 0x37, 0x35, 0x34, 0x39, 0x33, 0x33, 0x34, 
	0x38, 0x38, 0x34, 0x33, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x47, 
	0x49, 0x4D, 0x50, 0x3A, 0x56, 0x65, 0x72, 0x73, 0x69, 0x6F, 
	0x6E, 0x3D, 0x22, 0x32, 0x2E, 0x31, 0x30, 0x2E, 0x33, 0x36, 
	0x22, 0x0A, 0x20, 0x20, 0x20, 0x74, 0x69, 0x66, 0x66, 0x3A, 
	0x4F, 0x72, 0x69, 0x65, 0x6E, 0x74, 0x61, 0x74, 0x69, 0x6F, 
	0x6E, 0x3D, 0x22, 0x31, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x78, 
	0x6D, 0x70, 0x3A, 0x43, 0x72, 0x65, 0x61, 0x74, 0x6F, 0x72, 
	0x54, 0x6F, 0x6F, 0x6C, 0x3D, 0x22, 0x47, 0x49, 0x4D, 0x50, 
	0x20, 0x32, 0x2E, 0x31, 0x30, 0x22, 0x0A, 0x20, 0x20, 0x20, 
	0x78, 0x6D, 0x70, 0x3A, 0x4D, 0x65, 0x74, 0x61, 0x64, 0x61, 
	0x74, 0x61, 0x44, 0x61, 0x74, 0x65, 0x3D, 0x22, 0x32, 0x30, 
	0x32, 0x34, 0x3A, 0x30, 0x33, 0x3A, 0x31, 0x38, 0x54, 0x31, 
	0x36, 0x3A, 0x32, 0x34, 0x3A, 0x35, 0x30, 0x2B, 0x30, 0x31, 
	0x3A, 0x30, 0x30, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x78, 0x6D, 
	0x70, 0x3A, 0x4D, 0x6F, 0x64, 0x69, 0x66, 0x79, 0x44, 0x61, 
	0x74, 0x65, 0x3D, 0x22, 0x32, 0x30, 0x32, 0x34, 0x3A, 0x30, 
	0x33, 0x3A, 0x31, 0x38, 0x54, 0x31, 0x36, 0x3A, 0x32, 0x34, 
	0x3A, 0x35, 0x30, 0x2B, 0x30, 0x31, 0x3A, 0x30, 0x30, 0x22, 
	0x3E, 0x0A, 0x20, 0x20, 0x20, 0x3C, 0x78, 0x6D, 0x70, 0x4D, 
	0x4D, 0x3A, 0x48, 0x69, 0x73, 0x74, 0x6F, 0x72, 0x79, 0x3E, 
	0x0A, 0x20, 0x20, 0x20, 0x20, 0x3C, 0x72, 0x64, 0x66, 0x3A, 
	0x53, 0x65, 0x71, 0x3E, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x3C, 0x72, 0x64, 0x66, 0x3A, 0x6C, 0x69, 0x0A, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x73, 0x74, 0x45, 0x76, 0x74, 0x3A, 
	0x61, 0x63, 0x74, 0x69, 0x6F, 0x6E, 0x3D, 0x22, 0x73, 0x61, 
	0x76, 0x65, 0x64, 0x22, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x73, 0x74, 0x45, 0x76, 0x74, 0x3A, 0x63, 0x68, 0x61, 
	0x6E, 0x67, 0x65, 0x64, 0x3D, 0x22, 0x2F, 0x22, 0x0A, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74, 0x45, 0x76, 0x74, 
	0x3A, 0x69, 0x6E, 0x73, 0x74, 0x61, 0x6E, 0x63, 0x65, 0x49, 
	0x44, 0x3D, 0x22, 0x78, 0x6D, 0x70, 0x2E, 0x69, 0x69, 0x64, 
	0x3A, 0x31, 0x63, 0x66, 0x38, 0x36, 0x64, 0x63, 0x30, 0x2D, 
	0x37, 0x39, 0x30, 0x39, 0x2D, 0x34, 0x38, 0x32, 0x33, 0x2D, 
	0x38, 0x64, 0x31, 0x38, 0x2D, 0x38, 0x36, 0x36, 0x34, 0x33, 
	0x35, 0x61, 0x62, 0x31, 0x61, 0x64, 0x64, 0x22, 0x0A, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x73, 0x74, 0x45, 0x76, 0x74, 
	0x3A, 0x73, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 0x65, 0x41, 
	0x67, 0x65, 0x6E, 0x74, 0x3D, 0x22, 0x47, 0x69, 0x6D, 0x70, 
	0x20, 0x32, 0x2E, 0x31, 0x30, 0x20, 0x28, 0x57, 0x69, 0x6E, 
	0x64, 0x6F, 0x77, 0x73, 0x29, 0x22, 0x0A, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x73, 0x74, 0x45, 0x76, 0x74, 0x3A, 0x77, 
	0x68, 0x65, 0x6E, 0x3D, 0x22, 0x32, 0x30, 0x32, 0x34, 0x2D, 
	0x30, 0x33, 0x2D, 0x31, 0x38, 0x54, 0x31, 0x36, 0x3A, 0x32, 
	0x34, 0x3A, 0x35, 0x33, 0x22, 0x2F, 0x3E, 0x0A, 0x20, 0x20, 
	0x20, 0x20, 0x3C, 0x2F, 0x72, 0x64, 0x66, 0x3A, 0x53, 0x65, 
	0x71, 0x3E, 0x0A, 0x20, 0x20, 0x20, 0x3C, 0x2F, 0x78, 0x6D, 
	0x70, 0x4D, 0x4D, 0x3A, 0x48, 0x69, 0x73, 0x74, 0x6F, 0x72, 
	0x79, 0x3E, 0x0A, 0x20, 0x20, 0x3C, 0x2F, 0x72, 0x64, 0x66, 
	0x3A, 0x44, 0x65, 0x73, 0x63, 0x72, 0x69, 0x70, 0x74, 0x69, 
	0x6F, 0x6E, 0x3E, 0x0A, 0x20, 0x3C, 0x2F, 0x72, 0x64, 0x66, 
	0x3A, 0x52, 0x44, 0x46, 0x3E, 0x0A, 0x3C, 0x2F, 0x78, 0x3A, 
	0x78, 0x6D, 0x70, 0x6D, 0x65, 0x74, 0x61, 0x3E, 0x0A, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0A, 0x3C, 0x3F, 0x78, 
	0x70, 0x61, 0x63, 0x6B, 0x65, 0x74, 0x20, 0x65, 0x6E, 0x64, 
	0x3D, 0x22, 0x77, 0x22, 0x3F, 0x3E, 0xC7, 0x2B, 0xD9, 0xB7, 
	0x00, 0x00, 0x00, 0x06, 0x62, 0x4B, 0x47, 0x44, 0x00, 0x24, 
	0x00, 0x1D, 0x00, 0x1D, 0xBA, 0xFA, 0x6E, 0x41, 0x00, 0x00, 
	0x00, 0x09, 0x70, 0x48, 0x59, 0x73, 0x00, 0x00, 0x2E, 0x23, 
	0x00, 0x00, 0x2E, 0x23, 0x01, 0x78, 0xA5, 0x3F, 0x76, 0x00, 
	0x00, 0x00, 0x07, 0x74, 0x49, 0x4D, 0x45, 0x07, 0xE8, 0x03, 
	0x12, 0x0F, 0x18, 0x35, 0x54, 0x87, 0xCD, 0xCC, 0x00, 0x00, 
	0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x43, 0x6F, 0x6D, 0x6D, 
	0x65, 0x6E, 0x74, 0x00, 0x43, 0x72, 0x65, 0x61, 0x74, 0x65, 
	0x64, 0x20, 0x77, 0x69, 0x74, 0x68, 0x20, 0x47, 0x49, 0x4D, 
	0x50, 0x57, 0x81, 0x0E, 0x17, 0x00, 0x00, 0x03, 0x2A, 0x49, 
	0x44, 0x41, 0x54, 0x38, 0xCB, 0x7D, 0x93, 0x4D, 0x6C, 0x54, 
	0x65, 0x14, 0x86, 0xDF, 0x73, 0xEF, 0x37, 0x77, 0xE8, 0xCC, 
	0x94, 0xCA, 0x1D, 0x3A, 0x8E, 0x2D, 0x74, 0x2A, 0xCD, 0x50, 
	0x7E, 0x5A, 0x4C, 0x15, 0x94, 0xD2, 0x40, 0x8B, 0x06, 0x63, 
	0x08, 0x62, 0xEB, 0x4F, 0x02, 0x8A, 0xC6, 0x44, 0xC1, 0x98, 
	0xB8, 0x20, 0x71, 0xA3, 0x5B, 0x0D, 0x26, 0xAE, 0x88, 0x21, 
	0x26, 0x92, 0x4C, 0xDC, 0x18, 0x11, 0x1A, 0x93, 0xAA, 0x95, 
	0x50, 0x45, 0x03, 0x63, 0x53, 0xDA, 0x14, 0x89, 0xB1, 0x4D, 
	0xC7, 0xD6, 0x68, 0x41, 0x6A, 0xC5, 0x4E, 0x5B, 0xFA, 0x37, 
	0xED, 0x70, 0xEF, 0xFD, 0xBE, 0x73, 0xDC, 0x90, 0x2E, 0x10, 
	0x7D, 0x97, 0x27, 0x6F, 0x9E, 0x9C, 0xC5, 0xFB, 0x10, 0xFE, 
	0x23, 0xED, 0xFD, 0xDE, 0xBA, 0xEC, 0x08, 0x1F, 0x28, 0x6A, 
	0xA1, 0xA6, 0x1A, 0xFB, 0xF4, 0x9E, 0x3A, 0x35, 0x5A, 0xE5, 
	0xDA, 0x72, 0x67, 0x8F, 0xEE, 0x3C, 0x5C, 0xFA, 0xD5, 0x73, 
	0x33, 0xDD, 0xC1, 0x13, 0x23, 0x13, 0x72, 0x28, 0x57, 0x40, 
	0x35, 0x00, 0xAA, 0x8D, 0x62, 0x6C, 0x67, 0xB5, 0xD5, 0xB1, 
	0x6B, 0x93, 0x7D, 0xE6, 0xC9, 0x86, 0x15, 0x37, 0xEF, 0x0A, 
	0xE8, 0xBC, 0x52, 0x54, 0x3F, 0xFE, 0xA6, 0x5B, 0x86, 0xC7, 
	0xF9, 0xC8, 0x77, 0x37, 0x64, 0x03, 0x8B, 0x24, 0xA5, 0x58, 
	0x88, 0x42, 0x04, 0x14, 0x29, 0x5D, 0x22, 0xA2, 0x7C, 0x73, 
	0x82, 0x86, 0x9B, 0xD6, 0xDB, 0xA7, 0x13, 0xAE, 0xD5, 0xF1, 
	0xD2, 0xCE, 0x88, 0x5E, 0x06, 0x7C, 0x7C, 0xBE, 0x50, 0xD7, 
	0x9D, 0x0B, 0x5E, 0xE9, 0xBA, 0xCA, 0x3B, 0x8A, 0x8C, 0x35, 
	0xD0, 0x7E, 0x19, 0x8F, 0xFD, 0xE2, 0xC8, 0xC0, 0x65, 0x1B, 
	0x86, 0x41, 0x0F, 0x6C, 0x65, 0xAB, 0x6A, 0xB3, 0x8F, 0x50, 
	0x78, 0x3E, 0x4C, 0x18, 0x7F, 0xB4, 0xCA, 0xEA, 0x79, 0x24, 
	0xAD, 0x32, 0x47, 0xF7, 0x97, 0x0E, 0x52, 0x7B, 0xEF, 0xA2, 
	0xFD, 0xE5, 0x0F, 0xB7, 0x8E, 0x9F, 0xBD, 0x6A, 0x1E, 0x83, 
	0x70, 0x05, 0x26, 0xAF, 0x45, 0x38, 0xD7, 0xA7, 0xEC, 0xA9, 
	0x49, 0x7A, 0xF6, 0xE9, 0x14, 0x39, 0x8E, 0xC2, 0x67, 0x67, 
	0x7E, 0x17, 0xBD, 0xCA, 0x15, 0xDA, 0xB4, 0x5D, 0x53, 0xA2, 
	0xA6, 0x08, 0xCB, 0xFA, 0x6B, 0x6B, 0xC2, 0xCA, 0x36, 0xD6, 
	0x87, 0xDE, 0x50, 0x5A, 0x0B, 0x71, 0x60, 0x12, 0xE2, 0x19, 
	0x97, 0xAE, 0x5D, 0x8E, 0xE0, 0xE7, 0x6E, 0xD5, 0xB8, 0xCD, 
	0xA5, 0xA3, 0xC7, 0xDB, 0x68, 0xF7, 0xEE, 0x2D, 0x20, 0x22, 
	0xB4, 0xB5, 0x0E, 0xD1, 0x89, 0x13, 0xDF, 0x22, 0x9B, 0xFD, 
	0x42, 0xA1, 0x7E, 0x7B, 0x44, 0x6A, 0x76, 0xC4, 0xD9, 0x97, 
	0x7B, 0xB5, 0x6F, 0x93, 0x02, 0x0B, 0x48, 0xB3, 0x82, 0x6F, 
	0x14, 0x0A, 0x73, 0x56, 0xB9, 0xCB, 0x94, 0xC9, 0x1C, 0xA6, 
	0x8A, 0x0A, 0x17, 0xB9, 0xDC, 0x1F, 0x30, 0x86, 0xD1, 0xD2, 
	0x52, 0x8F, 0x86, 0x86, 0x75, 0xD4, 0xBC, 0xEB, 0x5D, 0xFC, 
	0x59, 0x98, 0x23, 0xF8, 0xC6, 0x46, 0x80, 0x10, 0x0C, 0x43, 
	0x81, 0x05, 0xD0, 0x86, 0xC4, 0x33, 0xB0, 0xB4, 0x81, 0x13, 
	0xD2, 0x28, 0x8D, 0xAD, 0xC0, 0xE0, 0xC0, 0x28, 0x5A, 0xF7, 
	0x7F, 0x00, 0x66, 0xC1, 0xDE, 0x7D, 0x69, 0xC4, 0x62, 0x61, 
	0xCC, 0x4C, 0x2F, 0x81, 0x12, 0x9A, 0xD8, 0x33, 0x24, 0x01, 
	0x48, 0xF4, 0x32, 0x80, 0x01, 0xDF, 0x00, 0x3A, 0x00, 0x21, 
	0x00, 0x00, 0xD8, 0x36, 0x21, 0xEC, 0x18, 0x18, 0xC3, 0x88, 
	0xC7, 0x4B, 0x10, 0x8B, 0x39, 0x50, 0x56, 0x00, 0x32, 0xFA, 
	0x76, 0x17, 0x80, 0x11, 0x28, 0x18, 0x86, 0x68, 0x23, 0xE2, 
	0x1B, 0x90, 0xD1, 0xF0, 0x6F, 0x79, 0x98, 0x9D, 0x5D, 0xC0, 
	0xE6, 0xBA, 0xFB, 0xD1, 0x79, 0xEE, 0x2D, 0x30, 0x0B, 0xD2, 
	0xE9, 0xB5, 0x98, 0x9E, 0x9E, 0xC5, 0xA9, 0x4F, 0x7A, 0xB0, 
	0xA0, 0xB5, 0x88, 0x6F, 0x04, 0x01, 0x04, 0xC6, 0xC0, 0xBA, 
	0xFD, 0x81, 0x86, 0x6F, 0x34, 0xC4, 0xE1, 0xD9, 0x7C, 0x51, 
	0x5E, 0x3E, 0x74, 0x4C, 0xBE, 0xFE, 0x2A, 0x8B, 0xCA, 0xCA, 
	0x72, 0xA4, 0x52, 0x49, 0x7C, 0xD3, 0xD5, 0x83, 0x17, 0x0F, 
	0xBC, 0x83, 0x89, 0xEB, 0x8B, 0x20, 0x38, 0x02, 0xDF, 0x18, 
	0x04, 0x1C, 0x40, 0x0B, 0x14, 0x44, 0x44, 0x0C, 0xE7, 0xC5, 
	0xE7, 0x9B, 0xFE, 0xAA, 0x87, 0x4B, 0xC4, 0x2A, 0x2B, 0xC9, 
	0x0D, 0x5F, 0x50, 0xAF, 0xBD, 0xF0, 0x21, 0x1E, 0x6F, 0xFD, 
	0x9E, 0x42, 0x21, 0x85, 0xB3, 0x9F, 0x0F, 0x89, 0x71, 0xE2, 
	0x12, 0xA4, 0x9E, 0xD2, 0xC1, 0x3D, 0x1B, 0x8B, 0xF0, 0x79, 
	0x1A, 0x9A, 0x27, 0x88, 0x45, 0x08, 0x00, 0xDE, 0x3F, 0x39, 
	0x5E, 0xD7, 0x7F, 0x65, 0xF1, 0xF0, 0xC5, 0x01, 0xAF, 0xC9, 
	0x33, 0xA8, 0x24, 0x53, 0x5C, 0xE9, 0xCC, 0xFC, 0xE4, 0x38, 
	0xF9, 0x8B, 0x36, 0xC4, 0x20, 0x48, 0x34, 0xB3, 0xE7, 0x3E, 
	0xE4, 0x89, 0x1D, 0x9D, 0x0F, 0x59, 0x18, 0x6F, 0xDC, 0x10, 
	0xEA, 0x69, 0xD8, 0x12, 0xCD, 0x1C, 0x7B, 0x73, 0xED, 0xE0, 
	0xF2, 0x94, 0x4F, 0x9E, 0xFA, 0x5B, 0x4D, 0x4D, 0xFA, 0x6D, 
	0xBD, 0xFD, 0x0B, 0x07, 0xB3, 0x43, 0x41, 0x2D, 0x8B, 0x24, 
	0x6D, 0x7F, 0x26, 0x02, 0x61, 0x32, 0xE1, 0xF8, 0x12, 0x81, 
	0xF2, 0x8D, 0xB5, 0x6A, 0x78, 0xDB, 0x83, 0xB1, 0x4F, 0x4B, 
	0xCB, 0x54, 0xC7, 0xDB, 0xAF, 0xAF, 0xD1, 0x77, 0x95, 0xA9, 
	0xBD, 0x33, 0x1F, 0xEF, 0xED, 0x9B, 0x3B, 0x98, 0xED, 0x5D, 
	0x7C, 0x6E, 0xE4, 0x86, 0x49, 0x01, 0xA0, 0xEA, 0xD5, 0xD6, 
	0xD8, 0x33, 0x7B, 0x57, 0x76, 0x55, 0xA5, 0x22, 0x1F, 0xBD, 
	0xFA, 0xFC, 0x7D, 0x53, 0xFF, 0x6B, 0x23, 0x00, 0x8C, 0x5E, 
	0x2F, 0xD2, 0xB9, 0xF3, 0x53, 0xEB, 0x2F, 0xF5, 0xCD, 0x1F, 
	0x59, 0xED, 0xDA, 0xE5, 0xC9, 0x64, 0xF8, 0xBD, 0xD6, 0x7D, 
	0x89, 0x91, 0x8D, 0xE9, 0xE8, 0xBF, 0x74, 0xFE, 0x07, 0x64, 
	0xD4, 0x96, 0x54, 0x32, 0xAE, 0x27, 0xC0, 0x00, 0x00, 0x00, 
	0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82, };

wxBitmap& civitai_png_to_wx_bitmap()
{
	static wxMemoryInputStream memIStream( civitai_png, sizeof( civitai_png ) );
	static wxImage image( memIStream, wxBITMAP_TYPE_PNG );
	static wxBitmap bmp( image );
	return bmp;
}


#endif //CIVITAI_PNG_H
