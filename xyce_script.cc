/***************************************************************************
    copyright            : (C) 2003 by Michael Margraf
                               2018, 2020 Felix Salfelder
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 3 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

// Xyce script task. stub
// BUG: copies in lots of legacy stuff.

#include "qucs_globals.h"
#include "qt_compat.h"
#include "task_element.h"
#include "viewpainter.h"
#include "obsolete_stuff.h"


namespace{

using namespace qucs;

struct Property // : public Object
{
	Property(const QString& _Name="", const QString& _Value="",
			bool _display=false, const QString& Desc="")
		: Name(_Name), Value(_Value), display(_display), Description(Desc) {};
	QString Name, Value;
	bool    display;   // show on schematic or not ?
	QString Description;

	QString const& name() const{ return Name; }
	QString const& value() const{ return Value; }
};

struct Text {
	Text(int _x, int _y, const QString& _s, QColor _Color = QColor(0,0,0),
			float _Size = 10.0, float _mCos=1.0, float _mSin=0.0)
		: x(_x), y(_y), s(_s), Color(_Color), Size(_Size),
		mSin(_mSin), mCos(_mCos) { over = under = false; };
	int	  x, y;
	QString s;
	QColor  Color;
	float	  Size, mSin, mCos; // font size and rotation coefficients
	bool	  over, under;      // text attributes
};

class XyceScript : public TaskElement  {
private:
	XyceScript(XyceScript const& s);
public:
	explicit XyceScript();
	~XyceScript();
	Element* clone() const{return new XyceScript(*this);}
	static Element* info(QString&, char* &, bool getNewOne=false);

	std::string typeName() const{
		return "XYCESCR";
	}
	void paint(ViewPainter *p) const;

	rect_t bounding_rect() const {
		return rect_t(_bb.marginsAdded(QMargins(5,5,10,10)));
	}

public: // legacy stuff.
	bool getPen(const QString&, QPen&, int);
	bool getBrush(const QString&, QBrush&, int);

	int     textSize(int&, int&);
	QList<Line *>     Lines;
	QList<struct Arc *>      Arcs;
	QList<Area *>     Rects;
	QList<Area *>     Ellips;
	QList<Port *>     Ports;
	QList<Text *>     Texts;
	mutable /*BUG*/ Q3PtrList<Property> Props;
private:
  mutable QRect _bb; // HACK
}D;

Dispatcher<TaskElement>::INSTALL p(&element_dispatcher, ".XYCESCR|XYCESCR", &D);
// Module::INSTALL pp("xyce", &D);

XyceScript::XyceScript() : TaskElement()
{
  auto Description = QObject::tr("XYCE SCRIPT");

  QString s = Description;
  int a = s.indexOf(" ");
  int b = s.lastIndexOf(" ");
  if (a != -1 && b != -1) {
    if (a > (int) s.length() - b)  b = a;
  }
  if (a < 8 || s.length() - b < 8) b = -1;
  if (b != -1) s[b] = '\n';

  Texts.append(new Text(0, 0, "XYCE SCRIPT", Qt::darkGreen, QucsSettings.largeFontSize));
  // Texts.append(new Text(0, 0, s.left(b), Qt::darkGreen, QucsSettings.largeFontSize));
  //if (b != -1)
  //  Texts.append(new Text(0, 0, s.mid(b+1), Qt::darkGreen, QucsSettings.largeFontSize));

  x1 = -10; y1 = -9;
  x2 = x1+128; y2 = y1+41;

  tx = 0;
  ty = y2+1;

  // The index of the first 4 properties must not changed. Used in recreate().
  Props.append(new Property("Script", "lin", true,
			QObject::tr("sweep type")+" [lin, log, list, const]"));
//  Props.append(new Property("Start", "1 GHz", true,
//			QObject::tr("start frequency in Hertz")));
//  Props.append(new Property("Stop", "10 GHz", true,
//			QObject::tr("stop frequency in Hertz")));
//  Props.append(new Property("Points", "19", true,
//			QObject::tr("number of simulation steps")));
//  Props.append(new Property("Noise", "no", false,
//			QObject::tr("calculate noise voltages")+
//			" [yes, no]"));
}

XyceScript::~XyceScript()
{
}

XyceScript::XyceScript(XyceScript const& p) : TaskElement(p)
{
	setTypeName("AC");

	assert(!Props.count());
	for(auto i : p.Props){
		Props.append(new Property(*i));
	}
	for(auto i : p.Texts){
		Texts.append(new Text(*i));
	}
	// isActive = COMP_IS_ACTIVE; // bug.
}

void XyceScript::paint(ViewPainter *p) const
{ untested();
	int x2=0; int y2=0; //?
  int cx = center().first;
  int cy = center().second;
  int x, y, a, b, xb, yb;
  QFont f = p->font();   // save current font
  QFont newFont = f;
  {   // is simulation component (dc, ac, ...)
    unsigned size;

#if 1
//    if(!Texts.isEmpty()){
//      size = Texts.first()->Size;
//    }else{
      // possibly incomplete.
      size = 20;
//    }
	 auto scale = 1;
    newFont.setPointSizeF(scale * size);
    newFont.setWeight(QFont::DemiBold);
    p->setFont(newFont);
    p->map(0, 0, x, y);

    p->setPen(QPen(Qt::darkGreen,2));
    a = b = 0;
    QRect t;
    foreach(Text *pt, Texts) {
      t.setRect(x, y+b, 0, 0);
      p->drawText(t, Qt::AlignLeft|Qt::TextDontClip, pt->s, &_bb);
      b += _bb.height();
      if(a < _bb.width())  a = _bb.width();
   }
    xb = a + int(12.0*scale);
    yb = b + int(10.0*scale);
    x2 = x1+25 + int(float(a) / scale);
    y2 = y1+23 + int(float(b) / scale);
    if(ty < y2+1) if(ty > y1-_bb.height())  ty = y2 + 1;

    p->map(-1, 0, x, y);
    p->map(-6, -5, a, b);
    p->drawRect(a, b, xb, yb);
    p->drawLine(x,      y+yb, a,      b+yb);
    p->drawLine(x+xb-1, y+yb, x,      y+yb);
    p->drawLine(x+xb-1, y+yb, a+xb,   b+yb);
    p->drawLine(x+xb-1, y+yb, x+xb-1, y);
    p->drawLine(x+xb-1, y,    a+xb,   b);
#endif
  }

  // restore old font
  p->setFont(f);

  p->setPen(QPen(Qt::black,1));
  p->map(tx, ty, x, y);
  if(showName) {
    p->drawText(x, y, 0, 0, Qt::TextDontClip, "incomplete");
    y += p->LineSpacing;
  }
  // write all properties
#if 1
  for(Property *p4 = Props.first(); p4 != 0; p4 = Props.next()){
    if(p4->display) {
		 QString tmpvalue = "\n.global_param dcsweep=0.0\n.dc lin dcsweep 0.0 0.52 0.01\n.print dc format=std file=dc.txt dcsweep\n+    i(xammeter1:vprobe)\n+   v(xvoltmeter1:pton)";
      p->drawText(x, y, 0, 0, Qt::TextDontClip, p4->Name+"="+tmpvalue);
      y += p->LineSpacing;
    }
  }
#endif

#if 0
  if(isActive == COMP_IS_OPEN)
    p->setPen(QPen(Qt::red,0));
  else if(isActive & COMP_IS_SHORTEN)
    p->setPen(QPen(Qt::darkGreen,0));
  if(isActive != COMP_IS_ACTIVE) {
    p->drawRect(+x1, +y1, x2-x1+1, y2-y1+1);
    p->drawLine(+x1, +y1, +x2, +y2);
    p->drawLine(+x1, +y2, +x2, +y1);
  }
#endif

#if 0
  // draw component bounding box
  if(isSelected()) {
    p->setPen(QPen(Qt::darkGreen,3));
    p->drawRoundRect(cx+x1, cy+y1, x2-x1, y2-y1);
  }
#endif
} // TaskElement::paint

#if 0
void XyceScript::recreate(Schematic*)
{
  Property *pp = Props.first();
  if((pp->Value == "list") || (pp->Value == "const")) {
    // Call them "Symbol" to omit them in the netlist.
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    pp = Props.next();
    pp->Name = "Symbol";
    pp->display = false;
    Props.next()->Name = "Values";
  }
  else {
    Props.next()->Name = "Start";
    Props.next()->Name = "Stop";
    Props.next()->Name = "Points";
  }
}
#endif

}
