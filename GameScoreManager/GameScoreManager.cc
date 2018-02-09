#include "GameScoreManager.hh"

GZ_REGISTER_GUI_PLUGIN(GameScoreManager)

GameScoreManager::GameScoreManager() : GUIPlugin(), current_score(INITIAL_SCORE)
{
  this->score_update_topic_name = SCORE_UPDATE_EVENT_TOPIC_NAME;
  this->minute_passed_topic_name = MINUTE_PASSED_EVENT_TOPIC_NAME;
  this->roomba_touched_topic_name = ROOMBA_TOUCHED_EVENT_TOPIC_NAME;
  this->reset_complete_topic_name = RESET_COMPLETE_EVENT_TOPIC_NAME;
  this->roomba_out_of_bounds_topic_name = ROOMBA_OUT_OF_BOUNDS_EVENT_TOPIC_NAME;
  this->Reset();

  this->setStyleSheet("QFrame { background-color : rgba(100, 100, 100, 255); color : white; }");
  QHBoxLayout *main_layout = new QHBoxLayout;
  QFrame *main_frame = new QFrame();
  QHBoxLayout *frame_layout = new QHBoxLayout();
  QLabel *label = new QLabel(tr("Score:"));
  QLabel *score_label = new QLabel(tr(std::to_string(INITIAL_SCORE).c_str()));
  frame_layout->addWidget(label);
  frame_layout->addWidget(score_label);
  main_frame->setLayout(frame_layout);

  main_layout->addWidget(main_frame);
  frame_layout->setContentsMargins(4, 4, 4, 4);
  main_layout->setContentsMargins(0, 0, 0, 0);

  this->setLayout(main_layout);
  this->move(245, 10);
  this->resize(100, 30);

  this->node = transport::NodePtr(new transport::Node());
  this->node->Init("default");
  this->minute_passed_subscriber = this->node->Subscribe(this->minute_passed_topic_name, &GameScoreManager::MinutePassed, this);
  this->roomba_touched_subscriber = this->node->Subscribe(this->roomba_touched_topic_name, &GameScoreManager::RoombaTouched, this);
  this->roomba_out_of_bounds_subscriber = this->node->Subscribe(this->roomba_out_of_bounds_topic_name, &GameScoreManager::RoombaOutOfBounds, this);
  this->reset_complete_subscriber = this->node->Subscribe(this->reset_complete_topic_name, &GameScoreManager::Reset, this);
  this->score_update_publisher = this->node->Advertise<gazebo::msgs::GzString>(this->score_update_topic_name);

  connect(this, SIGNAL(SetScoreLabel(QString)), score_label, SLOT(setText(QString)), Qt::QueuedConnection);
}

void GameScoreManager::Reset(ConstGzStringPtr &msg)
{
  this->Update(INITIAL_SCORE);
  fill(this->has_roomba_been_touched, this->has_roomba_been_touched+NUMBER_OF_ROOMBAS, false);
  fill(this->has_roomba_been_scored, this->has_roomba_been_scored+NUMBER_OF_ROOMBAS, false);
}

void GameScoreManager::Reset()
{
  const boost::shared_ptr<gazebo::msgs::GzString const> ptr;
  this->Reset(ptr);
}

void GameScoreManager::Update(long score)
{
  if (score != this->current_score)
  {
    this->current_score = score;
    const string score_as_string(to_string(this->current_score));
    this->SetScoreLabel(QString::fromStdString(score_as_string));
    
    this->msg.set_data(score_as_string);
    this->score_update_publisher->Publish(this->msg);
  }
}

void GameScoreManager::RoombaTouched(ConstGzStringPtr &msg)
{
  lock_guard<mutex> lock(this->roomba_data_guard);
  const string& data = msg->data();
  int roomba_id = data[data.size()-1] - '0';

  if (roomba_id < NUMBER_OF_ROOMBAS)
  {
    this->has_roomba_been_touched[roomba_id] = true;
  }
}

void GameScoreManager::RoombaOutOfBounds(ConstGzStringPtr &msg)
{
  lock_guard<mutex> lock(this->roomba_data_guard);
  const string& data = msg->data();
  int score_delta = 0;
  int roomba_id = data[data.size()-1] - '0';

  if (roomba_id < NUMBER_OF_ROOMBAS && !this->has_roomba_been_scored[roomba_id])
  {
    this->has_roomba_been_scored[roomba_id] = true;

    if (data.find(ROOMBA_CROSSED_OVER_GREEN_LINE_SUBSTRING) != string::npos && this->has_roomba_been_touched[roomba_id])
    {
      score_delta = POINTS_PER_ROOMBA_CROSSING;
    }
    else
    {
      score_delta = -PENALTY_FOR_ROOMBA_ESCAPING;
    }

    this->Update(this->current_score + score_delta);
  }
}

void GameScoreManager::MinutePassed(ConstGzStringPtr &msg)
{
  this->Update(this->current_score-PER_MINUTE_PENALTY);
}