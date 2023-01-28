#ifndef __QTABLEWIDGETHEADER_TYPES_H__
#define __QTABLEWIDGETHEADER_TYPES_H__

#include <QTableWidget>
#include <QPainter>

#include "jvx-qt-helpers.h"

// ==========================================================================================
	class myHeaderViewH : public QHeaderView
	{
	private:

		QList<QColor> myColors;
		QStringList myTexts;
		int textSize;
	public:
		myHeaderViewH(int kachelWidthSize, int textSizeArg) : QHeaderView(Qt::Horizontal)
		{
			setDefaultSectionSize(kachelWidthSize);
			setMinimumSectionSize(kachelWidthSize);
			setMaximumSectionSize(kachelWidthSize);
			textSize = textSizeArg;
			setDefaultAlignment(Qt::AlignHCenter);
		}

		void setColors(QList<QColor> myColorsArg)
		{
			myColors = myColorsArg;
		}

		void setTexts(QStringList myTextsArg)
		{
			myTexts = myTextsArg;
		}

		void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
		{
			QString showme = "unspecified";
			painter->save();
			// translate the painter such that rotate will rotate around the correct point
			//std::cout << "X: " << rect.x() << "; Y: " << rect.y() << ";" << rect.width() << ";" << rect.height() << std::endl;
			painter->translate(rect.x() + rect.width(), rect.y());
			painter->rotate(90);
			

			// and have parent code paint at this location
			QRect newrect = QRect(0, 0, rect.height(), rect.width());
			//QHeaderView::paintSection(painter, newrect, logicalIndex);
			//QString data = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
			//QFontMetrics fm = painter->fontMetrics();
			if (logicalIndex < myTexts.size())
			{
				showme = myTexts[logicalIndex];
			}
			
			//painter->fillRect(newrect, QBrush(QColor("gray")));

			QPen myPen = painter->pen();
			QPen myNewPen = myPen;
			if (logicalIndex < myColors.size())
			{
				myNewPen.setColor(myColors[logicalIndex]);
			}
			painter->setPen(myNewPen);

			painter->drawText(newrect, Qt::AlignCenter, showme);
			painter->setPen(myPen);

			painter->drawLine(newrect.topLeft(), newrect.topRight());
			painter->restore();
		}

		QSize sizeHint() const
		{
			// Get the base implementation size.
			QSize baseSize = QHeaderView::sizeHint();

			// Override the height with a custom value.
			baseSize.setHeight(textSize);

			return baseSize;
		}

	};

	class myHeaderViewV : public QHeaderView
	{
	private:

		QList<QColor> myColors;
		QStringList myTexts;
		int textSize;

	public:
		myHeaderViewV(int kachelWidthSize, int textSizeArg) : QHeaderView(Qt::Vertical)
		{
			setDefaultSectionSize(kachelWidthSize);
			setMinimumSectionSize(kachelWidthSize);
			setMaximumSectionSize(kachelWidthSize);
			setDefaultAlignment(Qt::AlignHCenter);
			textSize = textSizeArg;
		}

		void paintSection(QPainter *painter, const QRect &rect, int logicalIndex) const
		{
			QString showme = "unspecified";
			painter->save();
			/*
			QString data = model()->headerData(logicalIndex, orientation(), Qt::DisplayRole).toString();
			QFontMetrics fm = painter->fontMetrics();
			*/
			//painter->fillRect(newrect, QBrush(QColor("gray")));
			if (logicalIndex < myTexts.size())
			{
				showme = myTexts[logicalIndex];
			}

			QPen myPen = painter->pen();
			QPen myNewPen = myPen;
			if (logicalIndex < myColors.size())
			{
				myNewPen.setColor(myColors[logicalIndex]);
			}
			painter->setPen(myNewPen);
			painter->drawText(rect, Qt::AlignCenter, showme);
			painter->setPen(myPen);
			painter->drawLine(rect.topLeft(), rect.topRight());
			painter->restore();
		}
		QSize sizeHint() const
		{
			// Get the base implementation size.
			QSize baseSize = QHeaderView::sizeHint();

			// Override the height with a custom value.
			baseSize.setWidth(textSize);

			return baseSize;
		}

		void setColors(QList<QColor> myColorsArg)
		{
			myColors = myColorsArg;
		}

		void setTexts(QStringList myTextsArg)
		{
			myTexts = myTextsArg;
		}
	};

#endif