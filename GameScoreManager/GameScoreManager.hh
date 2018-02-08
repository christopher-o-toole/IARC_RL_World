#ifndef GAME_SCORE_MANAGER_HEADER
#define GAME_SCORE_MANAGER_HEADER
    #include <string>
    #include <mutex>
    #include <sstream>
    #include <algorithm>
    #include <string>

    #include <gazebo/common/Plugin.hh>
    #include <gazebo/gui/GuiPlugin.hh>


    #ifndef Q_MOC_RUN  // See: https://bugreports.qt-project.org/browse/QTBUG-22829
    #include <gazebo/transport/transport.hh>
    #include <gazebo/gui/gui.hh>
    #endif

    #include <gazebo/msgs/msgs.hh>

    using namespace std;
    using namespace gazebo;

    const int INITIAL_SCORE = 12000;
    const int POINTS_PER_ROOMBA_CROSSING = 2000;
    const int PENALTY_FOR_ROOMBA_ESCAPING = 1000;
    const int PER_MINUTE_PENALTY = 100;
    const int NUMBER_OF_ROOMBAS = 10;

    const string SCORE_UPDATE_EVENT_TOPIC_NAME = "~/score_update";
    const string MINUTE_PASSED_EVENT_TOPIC_NAME = "~/minute_passed";
    const string ROOMBA_TOUCHED_EVENT_TOPIC_NAME = "~/roomba_touched";
    const string ROOMBA_OUT_OF_BOUNDS_EVENT_TOPIC_NAME = "~/roomba_out_of_bounds";
    const string RESET_COMPLETE_EVENT_TOPIC_NAME = "~/reset_complete";

    const string ROOMBA_CROSSED_OVER_GREEN_LINE_SUBSTRING = "front";

    namespace gazebo
    {
        class GameScoreManager : public GUIPlugin
        {
            Q_OBJECT

            signals: void SetScoreLabel(QString _string);

            private:
                long current_score;
                transport::SubscriberPtr minute_passed_subscriber;
                transport::SubscriberPtr roomba_touched_subscriber;
                transport::SubscriberPtr roomba_out_of_bounds_subscriber;
                transport::SubscriberPtr reset_complete_subscriber;
                transport::PublisherPtr score_update_publisher;
                transport::NodePtr node;
                string score_update_topic_name;
                string minute_passed_topic_name;
                string roomba_touched_topic_name;
                string roomba_out_of_bounds_topic_name;
                string reset_complete_topic_name;
                msgs::GzString msg;
                bool has_roomba_been_touched[NUMBER_OF_ROOMBAS];
                bool has_roomba_been_scored[NUMBER_OF_ROOMBAS];
                mutex roomba_data_guard;

                void Update(long score);

            public:
                GameScoreManager();
                void Reset(ConstGzStringPtr &msg);
                void Reset();
                void MinutePassed(ConstGzStringPtr &msg);
                void RoombaTouched(ConstGzStringPtr &msg);
                void RoombaOutOfBounds(ConstGzStringPtr &msg);

        };
    }
#endif