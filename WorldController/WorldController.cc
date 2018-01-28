#include <list>
#include <map>
#include <mutex>
#include <string>
#include <cstdlib>

#include <sdf/sdf.hh>
#include <ignition/transport/Node.hh>

#include <gazebo/transport/Node.hh>
#include <gazebo/transport/Subscriber.hh>
#include <gazebo/msgs/msgs.hh>

#include <gazebo/common/Events.hh>
#include <gazebo/common/Assert.hh>
#include <gazebo/common/Console.hh>

#include <gazebo/physics/World.hh>
#include <gazebo/physics/Model.hh>
#include <gazebo/physics/Joint.hh>

#include "gazebo/physics/physics.hh"
#include "gazebo/common/common.hh"
#include "gazebo/gazebo.hh"

#include <gazebo/math/gzmath.hh>


using namespace gazebo;
using namespace std;

const string TOPIC_NAME = "~/out_of_bounds";
const string RESET_EVENT_TOPIC_NAME = "~/reset";
const string RESET_COMPLETE_EVENT_TOPIC_NAME = "~/reset_complete";
const string DRONE_NAME = "Sentinel";
const string RESET_EVENT_VALUE = "1";
const double MAX_DIST = 10;

namespace gazebo
{
  class WorldController : public WorldPlugin
  {
    physics::WorldPtr world;
    string out_of_bounds_topic_name;
    string reset_topic_name;
    transport::NodePtr message_node;
    transport::NodePtr out_of_bounds_message_node;
    transport::SubscriberPtr out_of_bounds_subscriber;
    transport::SubscriberPtr reset_complete_subscriber;
    transport::PublisherPtr publisher;
    msgs::GzString msg;
    mutex world_update_mutex;
    bool reset_flag;

  public:
    WorldController(const string topic_name = TOPIC_NAME, const string reset_topic_name = RESET_EVENT_TOPIC_NAME) : WorldPlugin()
    {
      this->out_of_bounds_topic_name = topic_name;
      this->reset_topic_name = reset_topic_name;
      this->msg.set_data(RESET_EVENT_VALUE);
      this->reset_flag = false;
    }
    
    void Load(physics::WorldPtr world, sdf::ElementPtr sdf)
    {
      this->world = world;
      this->message_node = transport::NodePtr(new transport::Node());
      this->message_node->Init(world->Name());
      this->out_of_bounds_subscriber = this->message_node->Subscribe(this->out_of_bounds_topic_name, &WorldController::OutOfBoundsEvent, this);
      this->reset_complete_subscriber = this->message_node->Subscribe(RESET_COMPLETE_EVENT_TOPIC_NAME, &WorldController::ResetCompleteEvent, this);
      this->publisher = this->message_node->Advertise<gazebo::msgs::GzString>(this->reset_topic_name);
      
      printf("WorldController plugin is now loaded!\n");
    }

    void ResetCompleteEvent(ConstGzStringPtr &msg)
    {
      if (this->reset_flag)
      {
        lock_guard<mutex> lock(this->world_update_mutex);

        //TODO: reset stats here
        this->world->ResetPhysicsStates();
        this->world->ResetEntities(physics::Base::EntityType::ENTITY);
        this->reset_flag = false;
      }
    }

    void OutOfBoundsEvent(ConstGzStringPtr &msg)
    {
      if (!this->reset_flag)
      {
        lock_guard<mutex> lock(this->world_update_mutex);

        try
        {
          for (auto& model : this->world->Models())
          {
            auto& pos = model->WorldPose().Pos();

            if (abs(pos.X()) >= MAX_DIST || abs(pos.Y()) >= MAX_DIST)
            {
              gzwarn << model->GetName() << " is outside of the arena boundaries!\n";
              if (model->GetName() == DRONE_NAME)
              {
                gzwarn << "Attempting to reset the world...\n";
                this->publisher->Publish(this->msg);
                this->reset_flag = true;
              }
            }
          }
        }
        catch (...)
        {

        }
      }
    }
  };

  GZ_REGISTER_WORLD_PLUGIN(WorldController)
}
