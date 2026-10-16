#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <move_to_point_action/action/move_to_point.hpp>
#include <geometry_msgs/msg/twist.hpp>
#include <turtlesim/msg/pose.hpp>
#include <cmath>
#include <thread>

class MoveToPointServer: public rclcpp::Node {
public:
    using MoveToPoint = move_to_point_action::action::MoveToPoint;
    using GoalHandleMoveToPoint =
        rclcpp_action::ServerGoalHandle<MoveToPoint>;

    MoveToPointServer() : Node("move_to_point_server_node") {
        action_server =
            rclcpp_action::create_server<MoveToPoint>(this,
                                                      "move_to_point",
                                                      std::bind(&MoveToPointServer::handle_goal, this,
                                                                std::placeholders::_1, std::placeholders::_2),
                                                      std::bind(&MoveToPointServer::handle_cancel, this,
                                                                std::placeholders::_1),
                                                      std::bind(&MoveToPointServer::handle_accepted, this,
                                                                std::placeholders::_1));

        publisher_turtle_sim =
            this->create_publisher<geometry_msgs::msg::Twist>("/turtle1/cmd_vel", 10);

        sub_turtle_sim = this->create_subscription<turtlesim::msg::Pose>(
            "/turtle1/pose",
            rclcpp::QoS(rclcpp::KeepLast(1)).best_effort(),
            std::bind(&MoveToPointServer::callback_turtle, this,
                      std::placeholders::_1));
    }

private:
    rclcpp_action::Server<MoveToPoint>::SharedPtr action_server;
    rclcpp::Publisher<geometry_msgs::msg::Twist>::SharedPtr publisher_turtle_sim;
    rclcpp::Subscription<turtlesim::msg::Pose>::SharedPtr sub_turtle_sim;

    double turtle_x = 0.0;
    double turtle_y = 0.0;
    double turtle_theta = 0.0;
    double distans_turtule = 0.0;
    geometry_msgs::msg::Twist cmd;

    // 1. Обработчик цели
    rclcpp_action::GoalResponse handle_goal(
        const rclcpp_action::GoalUUID &uuid,
        std::shared_ptr<const MoveToPoint::Goal> goal)
    {
        (void)uuid;
        RCLCPP_INFO_STREAM(this->get_logger(),
                           "Получена целевая точка " << goal->target_point.x
                                                     << " " << goal->target_point.y);

        if (goal->target_point.x >= 0 && goal->target_point.x <= 11 &&
            goal->target_point.y >= 0 && goal->target_point.y <= 11)
        {
            RCLCPP_INFO_STREAM(this->get_logger(), "Точка достижима по X и Y");
            return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        }

        RCLCPP_INFO_STREAM(this->get_logger(), "Точка не достижима по X и Y");
        return rclcpp_action::GoalResponse::REJECT;
    }

    void callback_turtle(const turtlesim::msg::Pose::SharedPtr msg) {
        turtle_x = msg->x;
        turtle_y = msg->y;
        turtle_theta = msg->theta;
    }

    // 2. Обработка запроса на отмену
    rclcpp_action::CancelResponse handle_cancel(
        const std::shared_ptr<GoalHandleMoveToPoint> goal_handle)
    {
        (void)goal_handle;
        RCLCPP_WARN_STREAM(this->get_logger(), "Запрос на отмену действия");
        return rclcpp_action::CancelResponse::ACCEPT;
    }

    // 3. Запуск действия в отдельном потоке
    void handle_accepted(
        const std::shared_ptr<GoalHandleMoveToPoint> goal_handle)
    {
        std::thread{std::bind(&MoveToPointServer::execute, this,
                              std::placeholders::_1), goal_handle}.detach();
    }

    // 4. Исполнительная функция
    void execute(const std::shared_ptr<GoalHandleMoveToPoint> goal_handle)
    {
        RCLCPP_INFO_STREAM(this->get_logger(), "Начало движения к цели");
        rclcpp::Rate loop_rate(10);

        auto goal   = goal_handle->get_goal();
        auto goal_x = goal->target_point.x;
        auto goal_y = goal->target_point.y;

        auto feedback = std::make_shared<MoveToPoint::Feedback>();
        auto result   = std::make_shared<MoveToPoint::Result>();

        // Начальное расстояние для расчёта процента прогресса
        double dx0 = goal_x - turtle_x;
        double dy0 = goal_y - turtle_y;
        double distance_total = std::sqrt(dx0 * dx0 + dy0 * dy0);
        if (distance_total < 0.001) distance_total = 0.001;   // защита от деления на 0

        while (rclcpp::ok()) {

            // --- Проверка отмены ---
            if (goal_handle->is_canceling()) {
                cmd.linear.x  = 0.0;
                cmd.angular.z = 0.0;             // ← было linear.z, стало angular.z
                publisher_turtle_sim->publish(cmd);

                result->success = false;
                goal_handle->canceled(result);
                RCLCPP_WARN_STREAM(this->get_logger(), "Действие отменено");
                return;
            }

            // --- Вычисление пути и угла ---
            double x_process = goal_x - turtle_x;
            double y_process = goal_y - turtle_y;
            double distance_in_process = std::sqrt(x_process * x_process +
                                                   y_process * y_process);

            double target_angle = std::atan2(y_process, x_process);
            double angle_eror   = target_angle - turtle_theta;

            // Нормализация угла
            while (angle_eror >  M_PI) angle_eror -= 2.0 * M_PI;
            while (angle_eror < -M_PI) angle_eror += 2.0 * M_PI;

            // --- Управление ---
            if (std::abs(angle_eror) > 0.1) {
                cmd.linear.x  = 0.0;
                cmd.angular.z = 1.0 * angle_eror;
            } else {
                cmd.linear.x  = 1.0;
                cmd.angular.z = 0.0;
            }
            publisher_turtle_sim->publish(cmd);

            // --- Достигли цели? ---
            if (distance_in_process < 0.1) {
                cmd.linear.x  = 0.0;
                cmd.angular.z = 0.0;
                publisher_turtle_sim->publish(cmd);

                result->success = true;
                goal_handle->succeed(result);            // ← ГЛАВНОЕ ИСПРАВЛЕНИЕ
                RCLCPP_INFO_STREAM(this->get_logger(), "Робот достиг цели");
                return;
            }

            // --- Обратная связь ---
            feedback->progress = (distance_in_process / distance_total) * 100.0;
            goal_handle->publish_feedback(feedback);

            loop_rate.sleep();
        }

        // Если rclcpp::ok() стал false — корректно завершаем
        result->success = false;
        goal_handle->abort(result);
    }
};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveToPointServer>());
    rclcpp::shutdown();
    return 0;
}
