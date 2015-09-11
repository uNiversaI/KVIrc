//=============================================================================
//
//   File : KviThemedLabel.cpp
//   Creation date : Tue Aug 29 2000 21:17:01 by Szymon Stefanek
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2000-2010 Szymon Stefanek (pragma at kvirc dot net)
//
//   This program is FREE software. You can redistribute it and/or
//   modify it under the terms of the GNU General Public License
//   as published by the Free Software Foundation; either version 2
//   of the License, or (at your opinion) any later version.
//
//   This program is distributed in the HOPE that it will be USEFUL,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
//   See the GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program. If not, write to the Free Software Foundation,
//   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
//
//=============================================================================

#include "KviThemedLabel.h"
#include "KviOptions.h"
#include "kvi_settings.h"
#include "KviApplication.h"
#include "KviMainWindow.h"
#include "KviWindow.h"
#include "kvi_out.h"
#include "KviMdiManager.h"

#include <QPainter>
#include <QStyleOptionFrameV2>
#include <QStyleFactory>

#if (QT_VERSION >= 0x050000)
	#include <qdrawutil.h> // qDrawShadePanel
#endif

#ifdef COMPILE_PSEUDO_TRANSPARENCY
	extern QPixmap * g_pShadedChildGlobalDesktopBackground;
#endif

KviThemedLabel::KviThemedLabel(QWidget * par, KviWindow * pWindow,const char * name)
: QLabel(par)
{
	setObjectName(name);
	m_pKviWindow = pWindow;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	QStyle * pWindowsStyle = QStyleFactory::create("plastique");
	if(pWindowsStyle)
		setStyle(pWindowsStyle);
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

#if (QT_VERSION < 0x050000)
	setFrameStyle(QFrame::Sunken | QFrame::StyledPanel);
#else
	setMargin(4);
#endif
	setAutoFillBackground(false);
	applyOptions();
}

KviThemedLabel::~KviThemedLabel()
{
}

void KviThemedLabel::applyOptions()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool bIsTrasparent = (KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing()) || g_pShadedChildGlobalDesktopBackground;
#else
	bool bIsTrasparent = false;
#endif

	QString szStyle = QString("QLabel { background: %1; background-clip: content; color: %2; font-family: %3; font-size: %4pt; font-weight: %5; font-style: %6;}")
	.arg(bIsTrasparent ? "transparent" : KVI_OPTION_COLOR(KviOption_colorLabelBackground).name())
	.arg(bIsTrasparent ? KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()).name() :
				KVI_OPTION_COLOR(KviOption_colorLabelForeground).name())
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).family())
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).pointSize())
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).weight() == QFont::Bold ? "bold" : "normal")
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).style() == QFont::StyleItalic ? "italic" : "normal");
	setStyleSheet(szStyle);
	update();
}

void KviThemedLabel::paintEvent(QPaintEvent *e)
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	QPainter *p = new QPainter(this);

#if (QT_VERSION >= 0x050000)
	// In Qt5 QStyle::drawPrimitive seems to always overwrite the background, no matter what.
	qDrawShadePanel(p,0,0,width(),height(),palette(),true,1,NULL);

	QRect r(1,1,width()-1,height()-1);
#else
	QStyleOptionFrameV2 option;
	option.initFrom(this);

	style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, p, this);

	QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &option, this);

#endif

	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(r, col);
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt = m_pKviWindow->mdiParent() ? mapTo(g_pMainWindow, r.topLeft() + g_pMainWindow->mdiManager()->scrollBarsOffset()) : mapTo(m_pKviWindow, r.topLeft());
		p->drawTiledPixmap(r,*(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	delete p;
#endif
	QLabel::paintEvent(e);
}

void KviThemedLabel::mouseDoubleClickEvent(QMouseEvent *)
{
	emit doubleClicked();
}

