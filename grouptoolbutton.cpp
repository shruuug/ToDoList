#include "grouptoolbutton.h"

GroupToolButton::GroupToolButton(int index, QString m_groupName, QWidget* parent)
	: QToolButton(parent)
{
	ui.setupUi(this);
	this->setStyleSheet("background: transparent");
	this->setStyleSheet("border-radius: 3px;");
	this->setStyleSheet("border: none;");
	this->setText(m_groupName);
	this->setCursor(QCursor(Qt::PointingHandCursor));
}


void GroupToolButton::mousePressEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton)
	{
		emit myClicked(m_groupName);
	}
}

QString GroupToolButton::getGroupName() const
{
	return m_groupName;
}
int GroupToolButton::getIndex() const
{
	return m_index;
}