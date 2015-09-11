//=============================================================================
//
//   File : KviThemedLineEdit.cpp
//   Creation date : Sun Jan 10 2010 12:17:00 by Fabio Bas
//
//   This file is part of the KVIrc irc client distribution
//   Copyright (C) 2010 Fabio Bas (ctrlaltca at gmail dot com)
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

#include "KviThemedLineEdit.h"
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

KviThemedLineEdit::KviThemedLineEdit(QWidget * par, KviWindow * pWindow,const char * name)
: QLineEdit(par)
{
	setObjectName(name);
	m_pKviWindow = pWindow;

#if defined(COMPILE_ON_WINDOWS) || defined(COMPILE_ON_MINGW)
	// this should be removed as soon as Qt fixes this bug (>W8)
	QStyle * pWindowsStyle = QStyleFactory::create("plastique");
	if(pWindowsStyle)
		setStyle(pWindowsStyle);
#endif //COMPILE_ON_WINDOWS || COMPILE_ON_MINGW

#if (QT_VERSION >= 0x050000)
	setFrame(false);

	int l,t,r,b;
	getTextMargins(&l,&t,&r,&b);
	if(l < 4)
		l = 4;
	if(r < 4)
		r = 4;
	setTextMargins(l,t,r,b);
#endif


	setAutoFillBackground(false);
	applyOptions();
}

KviThemedLineEdit::~KviThemedLineEdit()
{
}

void KviThemedLineEdit::applyOptions()
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	bool bIsTrasparent = (KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing()) || g_pShadedChildGlobalDesktopBackground;
#else
	bool bIsTrasparent = false;
#endif
	QString szStyle = QString("QLineEdit { background: %1; color: %2; font-family: %3; font-size: %4pt; font-weight: %5; font-style: %6;}")
	.arg(bIsTrasparent ? "transparent" : KVI_OPTION_COLOR(KviOption_colorLabelBackground).name())
	.arg(bIsTrasparent ?
		KVI_OPTION_MIRCCOLOR(KVI_OPTION_MSGTYPE(KVI_OUT_NONE).fore()).name() :
		KVI_OPTION_COLOR(KviOption_colorLabelForeground).name())
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).family())
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).pointSize())
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).weight() == QFont::Bold ? "bold" : "normal")
	.arg(KVI_OPTION_FONT(KviOption_fontLabel).style() == QFont::StyleItalic ? "italic" : "normal");
	setStyleSheet(szStyle);
	update();
}

void KviThemedLineEdit::paintEvent ( QPaintEvent * event )
{
#ifdef COMPILE_PSEUDO_TRANSPARENCY
	QPainter *p = new QPainter(this);
	QPalette pal = palette();

#if (QT_VERSION >= 0x050000)
	// In Qt5 QStyle::drawPrimitive seems to always overwrite the background, no matter what.
	qDrawShadePanel(p,0,0,width(),height(),palette(),true,1,NULL);

	QRect r(1,1,width()-1,height()-1);
#else

	QStyleOptionFrameV2 option;

	option.initFrom(this);
	option.rect = contentsRect();
	option.lineWidth = style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &option, this);
	option.midLineWidth = 0;
	option.state |= QStyle::State_Sunken;
	if(isReadOnly())
		option.state |= QStyle::State_ReadOnly;
	option.features = QStyleOptionFrameV2::None;

	style()->drawPrimitive(QStyle::PE_FrameLineEdit, &option, p, this);

	QRect r = style()->subElementRect(QStyle::SE_LineEditContents, &option, this);

#endif

	if(KVI_OPTION_BOOL(KviOption_boolUseCompositingForTransparency) && g_pApp->supportsCompositing())
	{
		p->setCompositionMode(QPainter::CompositionMode_Source);
		QColor col=KVI_OPTION_COLOR(KviOption_colorGlobalTransparencyFade);
		col.setAlphaF((float)((float)KVI_OPTION_UINT(KviOption_uintGlobalTransparencyChildFadeFactor) / (float)100));
		p->fillRect(r, col);
		p->restore();
	} else if(g_pShadedChildGlobalDesktopBackground)
	{
		QPoint pnt;
		if(m_pKviWindow)
			pnt = m_pKviWindow->mdiParent() ? mapTo(g_pMainWindow, r.topLeft() + g_pMainWindow->mdiManager()->scrollBarsOffset()) : mapTo(m_pKviWindow, r.topLeft());
		else
			pnt = mapToGlobal(event->rect().topLeft());
		p->drawTiledPixmap(r,*(g_pShadedChildGlobalDesktopBackground), pnt);
	}
	delete p;
#endif

	QLineEdit::paintEvent(event);
}

