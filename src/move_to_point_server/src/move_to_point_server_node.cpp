#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <move_to_point_action/action/move_to_point.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>

class MoveToPointServer: public rclcpp::Node {
public:
    //Для удобства используем локальные сокращения
    using MoveToPoint = move_to_point_action::action::MoveToPoint;
    using GoalHandleMoveToPoint =
        rclcpp_action::ServerGoalHandle<MoveToPoint>;

    MoveToPointServer() : Node("move_to_point_server_node") {
        action_server =
            rclcpp_action::create_server<MoveToPoint>(this,
                                                      "move_to_point",
                                                      std::bind(&MoveToPointServer::handle_goal,
                                                                this,
                                                                std::placeholders::_1,
                                                                std::placeholders::_2),
                                                      std::bind(&MoveToPointServer::handle_cancel,
                                                                this,
                                                                std::placeholders::_1),
                                                      std::bind(&MoveToPointServer::handle_accepted,
                                                                this,
                                                                std::placeholders::_1));

        // **Ключевые моменты:**
        // *   **`std_msgs::msg::String::SharedPtr`** — указатель на сообщение.
        // *   **`std::bind`** используется для связывания метода `listener_callback` с объектом `listener`, чтобы при вызове колбэка у него был доступ к `this`.
        // *   **`rclcpp::spin`** запускает обработку входящих сообщений.

        publisher_turtle_sim = this->create_publisher<geometry_msgs::msg::Twist>(
            "/turtle1/cmd_vel",
            10);


        sub_turtle_sim = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            rclcpp::QoS(rclcpp::KeepLast(1)).best_effort(),
            std::bind(
                &MoveToPointServer::callback_turtle,
                this,
                std::placeholders::_1));

        // subscription_ = this->create_subscription<sensor_msgs::msg::CompressedImage>(
        //     "/camera_image/compressed",
        //     rclcpp::QoS(rclcpp::KeepLast(1)).best_effort(),
        //     std::bind(&LineFollower::image_callback, this, std::placeholders::_1));
    }
private:
    //Объект сервера действия
    rclcpp_action::Server<MoveToPoint>::SharedPtr action_server;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_turtle_sim;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle_sim;
    double   turtle_x;
    double   turtle_y;
    double   turtle_theta;
    double  distans_turtule;
    geometry_msgs::msg::Twist cmd;



    //1. Обработчик цели
    //Сюда поступает цель действия, нужно по её параметрам
    //понять, пригодна ли она для исполнения и если да, то
    //принять её, если нет - то отклонить
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID &uuid, //идентификатор
        std::shared_ptr<const MoveToPoint::Goal> goal) //цель
    {
        //Поскольку цель - это точка, то при её поступлении
        //нужно понять, достижима ли она
        RCLCPP_INFO_STREAM(this->get_logger(),
                           "Получена целевая точка "<<
                               goal->target_point.x<<" "<<
                               goal->target_point.y);
        //Вставить проверку
        if(goal->target_point.x>=0 && goal->target_point.x<=11 && goal->target_point.y>=0 && goal->target_point.y<=11){

            RCLCPP_INFO_STREAM(this->get_logger(),
                               "Точка достижима по Х и Y");


            return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        }

        else {

            RCLCPP_INFO_STREAM(this->get_logger(),
                               "Точка не достижима по Х и Y ");

            return rclcpp_action::GoalResponse::REJECT;
        }



        // //Если точка устраивает
        // return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        // // return rclcpp_action::GoalResponse::REJECT; при недостижимости
    }


    void callback_turtle(const turtlesim::msg::Pose::SharedPtr msg){

     turtle_x=msg->x;
     turtle_y=msg->y;
     turtle_theta=msg->theta;




    }
    //2. Обработка запроса на отмену действия
    //Здесь выполняется запрос на прекращение выполнения операции
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandleMoveToPoint> goal_handle) {
        RCLCPP_WARN_STREAM(this->get_logger(),
                           "Запрос на отмену действия");
        //Если мы принимаем запрос, то вернуть
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    //3. Запуск действия
    //Если цель принята, то необходимо запустить выполнение действия
    void handle_accepted(
        const std::shared_ptr<GoalHandleMoveToPoint> goal_handle) {
        //в отдельном потоке запускаем исполнение действия
        std::thread{std::bind(&MoveToPointServer::execute,
                              this,
                              std::placeholders::_1), goal_handle}.detach();
    }
    //4. Исполнительная функция, которая реализует действие
    //и работает в отдельном потоке
    void execute(
        const std::shared_ptr<GoalHandleMoveToPoint> goal_handle) {
        //Здесь реализуется алгоритм достижения цели
        RCLCPP_INFO_STREAM(this->get_logger(),
                           "Начало движения к цели");
        rclcpp::Rate loop_rate(10); //частота обновления цикла
            //выполнения действия
        //Параметры цели
        auto goal = goal_handle->get_goal();
        auto goal_x= goal->target_point.x;
        auto goal_y= goal->target_point.y;
        auto goal_z= goal->target_point.z;

        //Обратная связь
        auto feedback = std::make_shared<MoveToPoint::Feedback>();
        //Итоговый результат
        auto result = std::make_shared<MoveToPoint::Result>();
        double dx= goal_x-turtle_x;
        double dy= goal_y-turtle_y;

        double distance = sqrt(dx*dx+dy*dy);
        distans_turtule=sqrt(turtle_x*turtle_x+turtle_y*turtle_y);
        //Основной цикл действия
        // int distance = 20;
        while(rclcpp::ok()) {

            if(goal_handle->is_canceling()){

                cmd.linear.x=0;
                cmd.linear.z=0;
                publisher_turtle_sim->publish(cmd);


                result->success=false;
                goal_handle->canceled(result);
                RCLCPP_WARN_STREAM(this->get_logger(),"Действие отменено");

                return;

            }



            //Здесь нужно написать основной алгоритм движения

            //вычислям путь

            double x_process=goal_x-turtle_x;
            double y_process=goal_y-turtle_y;
            double distance_in_process=sqrt(x_process*x_process+y_process*y_process);

            // if (distance_in_process<=0.1){
            //     cmd.linear.x = 0.0;
            //     cmd.angular.z = 0.0;

            //     publisher_turtle_sim->publish(cmd);


            //     result->success = true;
            //     goal_handle->succeed(result);
            //     RCLCPP_INFO_STREAM(this->get_logger(), "Робот завершил движение ");

            //     return;


            // }

            // вычислям угол поворота до заданнйо точки

            double target_angle = atan2(y_process,x_process);

            double angle_eror=target_angle-turtle_theta; // показатель того на сколько черепашка повёрнута не туда




            while (angle_eror > M_PI)
                angle_eror -= 1 * M_PI;

            while (angle_eror < -M_PI)
                angle_eror += 1 * M_PI;


            if(std::abs(angle_eror)>0.1){
                cmd.linear.x=0.0;
                cmd.angular.z=1.0*angle_eror;

                 publisher_turtle_sim->publish(cmd);

            }


            else {

                cmd.linear.x = 1.0;
                cmd.angular.z = 0.0;

             publisher_turtle_sim->publish(cmd);

            }

            if(distance_in_process<0.1){


                RCLCPP_INFO_STREAM(this->get_logger(), "Робот остановился в тчоке !");
                cmd.linear.x = 0.0;
                cmd.angular.z = 0.0;
                angle_eror=0.0;

                publisher_turtle_sim->publish(cmd);

                return ;
                break;


            }

          // RCLCPP_INFO_STREAM(this->get_logger(), "Текущая позиция: " << turtle_x << " "  << turtle_y << " | Цель: " << goal_x << " " << goal_y << " | Расстояние: " << distance_in_process );

 RCLCPP_INFO_STREAM(this->get_logger(), "Значение дистанции в процессе"<<distance_in_process<<" "<< "Значение общего пути"<<distance<<" "<<"Значение ошибки по углу"<<angle_eror<< " "<<" Целевой угол поворота"<<target_angle);
            //Пока его нет, имитируем простым уменьшением расстояния
            // distance--;

            //Формируем обратную связь
            feedback->progress = (distance_in_process / distance)*100;
            goal_handle->publish_feedback(feedback);

            RCLCPP_INFO_STREAM(this->get_logger(),
                               "Прогресс "<<feedback->progress<<"%");

            loop_rate.sleep(); //засыпаем для поддержания делаемой частоты

        }
        //После окончания выполнения действия необходимо отправить результат
        if(rclcpp::ok()) {



            result->success = true;
            goal_handle->succeed(result);
            RCLCPP_INFO_STREAM(this->get_logger(), "Цель достигнута");

            return;

        }
    }

};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveToPointServer>());
    rclcpp::shutdown();

    return 0;
}
