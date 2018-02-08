/*
 * Copyright (C) 2014 Open Source Robotics Foundation
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
*/
#ifndef _GUI_EXAMPLE_TIME_WIDGET_HH_
#define _GUI_EXAMPLE_TIME_WIDGET_HH_

#include <string>
#include <mutex>

#include <gazebo/common/Plugin.hh>
#include <gazebo/gui/GuiPlugin.hh>
#ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
# include <gazebo/transport/transport.hh>
# include <gazebo/gui/gui.hh>
#endif

namespace gazebo
{
  class GAZEBO_VISIBLE GUIExampleTimeWidget : public GUIPlugin
  {
    Q_OBJECT

    /// \brief Constructor
    public: GUIExampleTimeWidget();

    /// \brief Destructor
    public: virtual ~GUIExampleTimeWidget();

    /// \brief A signal used to set the sim time line edit.
    /// \param[in] _string String representation of sim time.
    signals: void SetSimTime(QString _string);

    /// \brief Callback that received world statistics messages.
    /// \param[in] _msg World statistics message that is received.
    protected: void OnStats(ConstWorldStatisticsPtr &_msg);

    /// \brief Callback that receives world reset complete messages.
    /// \param[in] msg Reset complete message that is received.
    protected: void ResetCompleteEvent(ConstGzStringPtr &msg);
    
    /// \brief Helper function to format time string.
    /// \param[in] _msg Time message.
    /// \return Time formatted as a string.
    private: std::string FormatTime(const msgs::Time &_msg);

    /// \brief Node used to establish communication with gzserver.
    private: transport::NodePtr node;

    /// \brief Subscriber to world statistics messages.
    private: transport::SubscriberPtr statsSub;

    /// \brief Topic name for the timeout event
    private: std::string timeout_topic_name;

    /// \brief TOpic name for the minute passed event
    private: std::string minute_passed_topic_name;

    /// \brief Publisher for the timeout event
    private: transport::PublisherPtr timeout_publisher;

    /// \brief Publisher for the minute passed event
    private: transport::PublisherPtr minute_passed_publisher;

    /// \brief Message for the timeout event
    private: msgs::GzString msg;

    /// \brief Subscriber for the reset complete event
    private: transport::SubscriberPtr reset_complete_subscriber;

    /// \brief Timer offset
    private: unsigned int timer_offset;

    /// \brief Number of seconds passed in simulator
    private: unsigned int seconds_passed;

    /// \brief Timer update mutex
    private: std::mutex timer_update_mutex;

    /// \brief Have we already sent a timeout message?
    private: bool reset_flag;

    /// \brief Number of minutes passed in last iteration
    private: long prev_minutes_passed;
  };
}

#endif
