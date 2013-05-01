#pragma once

#include "resource.h"

class LayoutCalc
{
public:
	LayoutCalc(int w, int h) {m_w = w; m_h = h; };
	~LayoutCalc() {};

	static const int bw = 90; // button width
	static const int bh = 25; // button height
	static const int mw = 10; // margin width

	int x1() { return mw; };
	int y1() { return bh*4; };
	int w1() { return m_w - mw - mw; };
	int h1() { return m_h - bh*4 - mw - bh - mw; };

	int x2() { return m_w - mw - bw - mw - bw; };
	int y2() { return m_h - mw - bh; };
	int w2() { return bw; };
	int h2() { return bh; };

	int x3() { return m_w - mw - bw; };
	int y3() { return m_h - mw - bh; };
	int w3() { return bw; };
	int h3() { return bh; };


	int w7() { return mw + m_w + mw + bw + mw + 2*mw; };
	int h7() { return mw + m_h + mw + mw + mw + 2*mw; };

	int x8() { return mw + m_w + mw; };
	int y8() { return mw; };
	int w8() { return bw; };
	int h8() { return bh; };

	int x9() { return mw + m_w + mw; };
	int y9() { return mw + bh + mw; };
	int w9() { return bw; };
	int h9() { return bh; };

protected:
	int m_w;
	int m_h;
};
