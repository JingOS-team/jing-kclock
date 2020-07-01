/*
 * Copyright 2020 Devin Lin <espidev@gmail.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License or (at your option) version 3 or any later version
 * accepted by the membership of KDE e.V. (or its successor approved
 * by the membership of KDE e.V.), which shall act as a proxy
 * defined in Section 14 of version 3 of the license.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "timermodel.h"

#include <KLocalizedString>
#include <KNotification>

#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QtGlobal>

/* ~ Timer ~ */

Timer::Timer(QObject *parent, qint64 length, qint64 elapsed, QString label, bool running)
{
    length_ = length;
    elapsed_ = elapsed;
    label_ = label;
    running_ = running;
    finished_ = false;
}

Timer::Timer(QString json)
{
    // TODO
}

QString Timer::serialize()
{
    // TODO
    return "";
}

void Timer::updateTimer(qint64 duration)
{
    if (running_) {
        elapsed_ += duration;
        
        // if the timer has finished
        if (elapsed_ >= length_) {
            elapsed_ = length_;
            running_ = false;
            finished_ = true;
            
            qDebug("Timer finished, sending notification...");

            KNotification *notif = new KNotification("timerFinished");
            notif->setIconName("kclock");
            notif->setTitle(i18n("Timer complete"));
            notif->setText(i18n("Your timer has finished!"));
            notif->setDefaultAction(i18n("View"));
            notif->setUrgency(KNotification::HighUrgency);
            notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);
            notif->sendEvent();
        }
        
        emit propertyChanged();
    }
}

void Timer::toggleRunning()
{
    if (finished_) {
        elapsed_ = 0;
        running_ = true;
        finished_ = false;
    } else {
        running_ = !running_;
    }
    emit propertyChanged();
}

void Timer::reset()
{
    finished_ = false;
    running_ = false;
    elapsed_ = 0;
    emit propertyChanged();
}


/* ~ TimerModel ~ */
const int TIMER_CHECK_LENGTH = 16; // milliseconds

TimerModel::TimerModel(QObject *parent)
{
    load();
    
    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, QOverload<>::of(&TimerModel::updateTimerLoop));
    timer->start(TIMER_CHECK_LENGTH);
}

void TimerModel::load()
{
    // TODO
}

void TimerModel::save()
{
    // TODO
}

void TimerModel::updateTimerLoop()
{
    for (auto *timer : timerList) 
        timer->updateTimer(TIMER_CHECK_LENGTH); 
}

int TimerModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return timerList.size();
}

int TimerModel::count()
{
    return timerList.size();
}

QVariant TimerModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

void TimerModel::addNew()
{
    insert(0, new Timer());
}

void TimerModel::insert(int index, Timer *timer)
{
    if ((index < 0) || (index > timerList.count())) 
        return;
    
    QQmlEngine::setObjectOwnership(timer, QQmlEngine::CppOwnership);
    emit beginInsertRows(QModelIndex(), index, index);
    timerList.insert(index, timer);
    emit endInsertRows();
    
    save();
}

void TimerModel::remove(int index)
{
    if ((index < 0) || (index >= timerList.count()))
        return;
    
    emit beginRemoveRows(QModelIndex(), index, index);
    auto timer = timerList.at(index);
    timerList.removeAt(index);
    delete timer;
    emit endRemoveRows();
    
    save();
}

Timer *TimerModel::get(int index)
{
    if ((index < 0) || (index >= timerList.count()))
        return {};
    
    return timerList.at(index);
}

void TimerModel::move(int oldIndex, int newIndex)
{
    if (oldIndex < 0 || oldIndex >= timerList.count() || newIndex < 0 || newIndex >= timerList.count())
        return;
    
    timerList.move(oldIndex, newIndex);
}

void TimerModel::timerFinished()
{
    qDebug("Timer finished, sending notification...");

    KNotification *notif = new KNotification("timerFinished");
    notif->setIconName("kronometer");
    notif->setTitle(i18n("Timer complete"));
    notif->setText(i18n("Your timer has finished!"));
    notif->setDefaultAction(i18n("View"));
    notif->setUrgency(KNotification::HighUrgency);
    notif->setFlags(KNotification::NotificationFlag::LoopSound | KNotification::NotificationFlag::Persistent);
    notif->sendEvent();
}
