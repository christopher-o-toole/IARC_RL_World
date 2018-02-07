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
#include <sstream>
#include <algorithm>
#include <string>
using namespace std;

#include <gazebo/msgs/msgs.hh>
#include "GUIExampleTimeWidget.hh"

using namespace gazebo;

// # of seconds per minute
#define NUM_SECONDS_PER_MINUTE 60
// # of minutes per round
#define MINUTES_PER_ROUND 2
// round time in seconds
#define ROUND_TIME MINUTES_PER_ROUND*NUM_SECONDS_PER_MINUTE

const string TIMEOUT_EVENT_NAME = "~/timeout";
const string RESET_COMPLETE_EVENT_TOPIC_NAME = "~/reset_complete";

// Register this plugin with the simulator
GZ_REGISTER_GUI_PLUGIN(GUIExampleTimeWidget)

/////////////////////////////////////////////////
GUIExampleTimeWidget::GUIExampleTimeWidget()
  : GUIPlugin()
{
  this->timeout_topic_name = TIMEOUT_EVENT_NAME;
  this->msg.set_data(TIMEOUT_EVENT_NAME);
  this->timer_offset = 0;
  this->reset_flag = false;
        
  // Set the frame background and foreground colors
  this->setStyleSheet(
      "QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");

  // Create the main layout
  QHBoxLayout *mainLayout = new QHBoxLayout;

  // Create the frame to hold all the widgets
  QFrame *mainFrame = new QFrame();

  // Create the layout that sits inside the frame
  QHBoxLayout *frameLayout = new QHBoxLayout();

  QLabel *label = new QLabel(tr("Round Time Remaining:"));

  // Create a time label
  QLabel *timeLabel = new QLabel(tr("00:00"));

  // Add the label to the frame's layout
  frameLayout->addWidget(label);
  frameLayout->addWidget(timeLabel);
  connect(this, SIGNAL(SetSimTime(QString)),
      timeLabel, SLOT(setText(QString)), Qt::QueuedConnection);

  // Add frameLayout to the frame
  mainFrame->setLayout(frameLayout);

  // Add the frame to the main layout
  mainLayout->addWidget(mainFrame);

  // Remove margins to reduce space
  frameLayout->setContentsMargins(4, 4, 4, 4);
  mainLayout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(mainLayout);

  // Position and resize this widget
  this->move(10, 10);
  this->resize(225, 30);

  // Create a node for transportation
  this->node = transport::NodePtr(new transport::Node());
  this->node->Init("default");
  this->statsSub = this->node->Subscribe("~/world_stats",
      &GUIExampleTimeWidget::OnStats, this);
  this->timeout_publisher = this->node->Advertise<gazebo::msgs::GzString>(this->timeout_topic_name);
  this->reset_complete_subscriber = this->node->Subscribe(RESET_COMPLETE_EVENT_TOPIC_NAME, &GUIExampleTimeWidget::ResetCompleteEvent, this);
}

void GUIExampleTimeWidget::ResetCompleteEvent(ConstGzStringPtr &msg)
{
  if (this->reset_flag)
  {
    lock_guard<mutex> lock(this->timer_update_mutex);
    this->timer_offset = this->seconds_passed;
    this->reset_flag = false;
  }
}

/////////////////////////////////////////////////
GUIExampleTimeWidget::~GUIExampleTimeWidget()
{
}

/////////////////////////////////////////////////
void GUIExampleTimeWidget::OnStats(ConstWorldStatisticsPtr &_msg)
{
  this->SetSimTime(QString::fromStdString(
        this->FormatTime(_msg->sim_time())));
}

/////////////////////////////////////////////////
std::string GUIExampleTimeWidget::FormatTime(const msgs::Time &_msg)
{
  lock_guard<mutex> lock(this->timer_update_mutex);
  this->seconds_passed = _msg.sec();

  std::ostringstream stream;
  int day, hour, min, sec, msec;

  stream.str("");

  int delta = ROUND_TIME + this->timer_offset - _msg.sec();
  sec = std::max(0, delta);

  if (delta <= 0 && !this->reset_flag)
  {
    this->timeout_publisher->Publish(this->msg);
    this->reset_flag = true;
  }

  stream << std::setw(2) << std::setfill('0') << (sec/60) << ":";
  stream << std::setw(2) << std::setfill('0') << (sec % 60);

  return stream.str();
}
