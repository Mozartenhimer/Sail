#pragma once
#ifdef OLC_PGE_APPLICATION
	#define VX_PGE_APPLICATION
#endif


#include "olcPixelGameEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <algorithm>

typedef float Real;


namespace olc {

	struct Mat2d {
		float M[4];
		Mat2d();
		Mat2d(float Angle);
		Mat2d(float, float, float, float);
		Mat2d(vf2d, vf2d);
		inline void		  SetRowVectors(olc::vf2d & row1, olc::vf2d & row2) 
		{
			M[0] = row1.x;  M[1] = row1.y;
			M[2] = row2.x;  M[3] = row2.y;
		};
		inline olc::vf2d  operator *  (const olc::vf2d & rhs) const
			{return olc::vf2d(rhs.x*this->M[0] + rhs.y*this->M[1], rhs.x*this->M[2] + rhs.y*this->M[3]);}
		inline Mat2d      operator *  (const Mat2d & rhs)     const 
		{
			//Mat2d a = *this;
			float * a = (float *)&this->M;
			const float * b = (float *)&rhs.M;

			Mat2d C = { a[0] * b[0] + a[1] * b[2], a[0] * b[1] + a[1] * b[3],
			        	a[2] * b[0] + a[3] * b[2], a[2] * b[1] + a[3] + b[3] };
			return Mat2d();
		}
	
		inline float &    operator[]	  (size_t idx) { return M[idx]; };
		inline float      det()                               const 
		{
			return M[0] * M[3] - M[1] * M[2];
		}
		inline Mat2d      trans()                             const {
			return Mat2d(M[0], M[2], M[1], M[3]);
		};
	};
	inline  std::ostream & operator << (std::ostream & os, olc::vf2d const & V) {
		return os << V.x << "," << V.y;
	}
	typedef v2d_generic<Real> vr2d;
	inline olc::vf2d MirrorX(olc::vf2d V){return olc::vf2d(-V.x, V.y);}
	inline olc::vf2d MirrorY(olc::vf2d V){return olc::vf2d(V.x, -V.y);}
	inline olc::Pixel clampedPixel(int R, int G, int B, int alpha = 255) {
		R = std::min(R, 255);
		G = std::min(G, 255);
		B = std::min(B, 255);
		alpha = std::min(alpha , 255);
		R = std::max(R, 0);
		G = std::max(G, 0);
		B = std::max(B, 0);
		alpha = std::max(alpha, 0);
		return Pixel(R, G, B, alpha);
	}
	inline olc::Pixel clampedPixel(const Pixel & P1, const Pixel & P2) {
		int R = P1.r + P2.r;
		int G = P1.g + P2.g;
		int B = P1.b + P2.b;
		int alpha = P1.a + P2.a;
		R = std::min(R, 255);
		G = std::min(G, 255);
		B = std::min(B, 255);
		alpha = std::min(alpha, 255);
		R = std::max(R, 0);
		G = std::max(G, 0);
		B = std::max(B, 0);
		alpha = std::max(alpha, 0);
		return Pixel(R, G, B, alpha);
	}
	inline olc::vf2d BoxClamp(olc::vf2d in, olc::vf2d TopLeft, olc::vf2d BottomRight) {
		// Assuming X-right Y-up with naming conventions
		if      (in.x < TopLeft.x)     in.x = TopLeft.x;
		else if (in.x > BottomRight.x) in.x = BottomRight.x;

		if      (in.y > TopLeft.y)     in.y = TopLeft.y;
		else if (in.y < BottomRight.y) in.y = BottomRight.y;
		return in;
	}

	class VxOLCPGE : public PixelGameEngine {
	public:
		// World view
		olc::vf2d screenDims; // meters
		olc::vf2d cameraPos; // meters
		olc::vi2d screenOffset;

	private:
		float screenHeightMeters = 10.0f;
		float pixelsPerMeter;

		size_t nFramesRendered = 0;
		size_t debugInfoFrame = 0;
		std::vector<std::string> dbgStrings;
	public:
		inline size_t getCurrentFrameNum() { return nFramesRendered; };
		float getPixelsPerMeter() const { return pixelsPerMeter; };
		inline void DrawDebugLine(std::string const & info) {
			// reset beginning of every frame
			if (nFramesRendered != debugInfoFrame) {
				debugInfoFrame = nFramesRendered;
				dbgStrings.clear();
			}
			dbgStrings.push_back(info);
		}
	public:
		using  PixelGameEngine::OnUserUpdate;
		virtual bool OnEveryFrame(float fElapsedTime); 
		bool OnUserUpdate(float fElaspedTime) override;

		// screen space to world space transforms
		olc::vi2d toScreen(const olc::vf2d & worldCoords);
		float toScreen(float length);
		olc::vf2d toWorld(const olc::vi2d & screenCoords);
		float toWorld(const float length_pixels);
		void setScreenHeightMeters(float screenHeight);
		
		inline olc::vf2d screenTopLeft() {
			return toWorld(olc::vi2d(0,0));
		}
		inline olc::vf2d screenBottomRight(){
			return toWorld(olc::vi2d(ScreenHeight(), ScreenWidth()));
		}

		//! Vector extended PGE for drawing with vectors more convientently
		using PixelGameEngine::Draw;
		bool Draw(vi2d pos, Pixel p = WHITE);

		using PixelGameEngine::DrawCircle;
		void DrawCircle(vi2d pos, int32_t radius, Pixel p = WHITE);

		using PixelGameEngine::FillCircle;
		void FillCircle(vi2d pos, int32_t radius, Pixel p = WHITE);

		using PixelGameEngine::DrawLine;
		void DrawLine(vi2d pos1, vi2d pos2, Pixel p = WHITE, uint32_t pattern = 0xFFFFFFFF);
		
		using PixelGameEngine::FillTriangle;
		void FillTriangle(vi2d p1, vi2d p2, vi2d p3, Pixel p = WHITE);

		using PixelGameEngine::DrawTriangle;
		void DrawTriangle(vi2d p1, vi2d p2, vi2d p3, Pixel p = WHITE);
		
		using PixelGameEngine::DrawRect;
		void DrawRect(vi2d pos, vi2d offset, Pixel p = WHITE);

		using PixelGameEngine::FillRect;
		void FillRect(vi2d start, vi2d offset, Pixel p = WHITE);
		
		using PixelGameEngine::DrawSprite;
		void DrawSprite(vi2d pos, Sprite *sprite, uint32_t scale = 1);
		
		using PixelGameEngine::DrawPartialSprite;
		void DrawPartialSprite(vi2d pos, Sprite *sprite, vi2d o, vi2d wh, uint32_t scale = 1);
		
		using PixelGameEngine::DrawString;
		void DrawString(vi2d pos, std::string sText, Pixel col = WHITE, uint32_t scale = 1);
	};
//#error
//#define VX_PGE_APPLICATION 
#ifdef VX_PGE_APPLICATION
#undef VX_PGE_APPLICATION
	olc::Mat2d::Mat2d() {
		M[0] = 1.0;  M[1] = 0.0;
		M[2] = 0.0;  M[3] = 1.0;
	};
	olc::Mat2d::Mat2d(float Angle) { // in radians
		M[0] = cos(Angle); M[1] = -sin(Angle);
		M[2] = sin(Angle); M[3] = cos(Angle);
	}
	
	olc::Mat2d::Mat2d(float a , float b, float c , float d)
	{
		M[0] = a; M[1] = b;
		M[2] = c; M[3] = d;
	};

	olc::Mat2d::Mat2d(vf2d col1, vf2d col2)
	{
		M[0] = col1.x;  M[1] = col2.x;
		M[2] = col1.y;  M[3] = col2.y;
	}
	bool VxOLCPGE::OnEveryFrame(float fElapsedTime)
	{UNUSED(fElapsedTime);  return false;} // same as pge
	bool VxOLCPGE::OnUserUpdate(float fElapsedTime)
	{
		bool status = OnEveryFrame(fElapsedTime);
		for (size_t i = 0; i < dbgStrings.size(); i++) {
			DrawString(1, 1 + 10 * i, dbgStrings[i],WHITE);
		}
		nFramesRendered++;
		return status; 
	}

	// Screen to world and reverse stuff
	olc::vi2d  VxOLCPGE::toScreen(const olc::vf2d & worldCoords) {
		vi2d S;
		S.x = ((int)((worldCoords.x - cameraPos.x)*pixelsPerMeter) + screenOffset.x);
		S.y = ((int)((worldCoords.y - cameraPos.y) * -pixelsPerMeter) + screenOffset.y);
		return S;
	}
	float  VxOLCPGE::toScreen(float length)
	{return length * pixelsPerMeter;}

	olc::vf2d  VxOLCPGE::toWorld(const olc::vi2d & screenCoords) {
		olc::vf2d W;
		W.x = ((float)(screenCoords.x - screenOffset.x) / pixelsPerMeter + cameraPos.x);
		W.y = ((float)(screenCoords.y - screenOffset.y) / -pixelsPerMeter + cameraPos.y);
		return W;

	}
	float  VxOLCPGE::toWorld(const float length_pixels)
	{return  ((float)(length_pixels) / pixelsPerMeter);}

	void VxOLCPGE::setScreenHeightMeters(float screenHeight){
		screenHeightMeters = screenHeight;
		pixelsPerMeter = (float)ScreenHeight() / screenHeightMeters;
		screenDims.x = (float)ScreenWidth() / pixelsPerMeter;
		screenDims.y = screenHeightMeters;

	}

	// Vector Drawing Overloads

	bool VxOLCPGE::Draw(olc::vi2d pos, olc::Pixel p )
	{return PixelGameEngine::Draw(pos.x, pos.y, p); }

	void  VxOLCPGE::DrawCircle(olc::vi2d pos, int32_t radius, olc::Pixel p )
	{PixelGameEngine::DrawCircle(pos.x, pos.y, radius, p);}

	void  VxOLCPGE::FillCircle(vi2d pos, int32_t radius, Pixel p )
	{PixelGameEngine::FillCircle(pos.x, pos.y, radius, p);}

	void  VxOLCPGE::DrawLine(vi2d pos1, vi2d pos2, Pixel p , uint32_t pattern)
	{PixelGameEngine::DrawLine(pos1.x, pos1.y, pos2.x, pos2.y, p,pattern);}

	void  VxOLCPGE::FillTriangle(vi2d p1, vi2d p2, vi2d p3, Pixel p )
	{PixelGameEngine::FillTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p);}

	void  VxOLCPGE::DrawTriangle(vi2d p1, vi2d p2, vi2d p3, Pixel p )
	{PixelGameEngine::DrawTriangle(p1.x, p1.y, p2.x, p2.y, p3.x, p3.y, p);}

	void  VxOLCPGE::DrawRect(vi2d pos, vi2d offset, Pixel p )
	{PixelGameEngine::DrawRect(pos.x, pos.y, offset.x, offset.y, p);}

	void  VxOLCPGE::FillRect(vi2d start, vi2d offset, Pixel p )
	{PixelGameEngine::FillRect(start.x, start.y, offset.x, offset.y, p);}

	void  VxOLCPGE::DrawSprite(vi2d pos, Sprite *sprite, uint32_t scale )
	{PixelGameEngine::DrawSprite(pos.x, pos.y, sprite, scale);};

	void  VxOLCPGE::DrawPartialSprite(vi2d pos, Sprite *sprite, vi2d o, vi2d wh, uint32_t scale )
	{PixelGameEngine::DrawPartialSprite(pos.x, pos.y, sprite, o.x, o.y, wh.x, wh.y, scale);	}

	void  VxOLCPGE::DrawString(vi2d pos, std::string sText, Pixel col , uint32_t scale )
	{PixelGameEngine::DrawString(pos.x, pos.y, sText, col, scale);}

#endif // VX_PGE_APPLICATION
}
