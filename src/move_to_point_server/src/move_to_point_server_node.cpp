#include <rclcpp/rclcpp.hpp>
#include <rclcpp_action/rclcpp_action.hpp>
#include <move_to_point_action/action/move_to_point.hpp>


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
    }
private:
    //Объект сервера действия
    rclcpp_action::Server<MoveToPoint>::SharedPtr action_server;

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

        //Если точка устраивает
        return rclcpp_action::GoalResponse::ACCEPT_AND_EXECUTE;
        // return rclcpp_action::GoalResponse::REJECT; при недостижимости
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
        rclcpp::Rate loop_rate(1); //частота обновления цикла
            //выполнения действия
        //Параметры цели
        auto goal = goal_handle->get_goal();
        //Обратная связь
        auto feedback = std::make_shared<MoveToPoint::Feedback>();
        //Итоговый результат
        auto result = std::make_shared<MoveToPoint::Result>();

        //Основной цикл действия
        int distance = 20;
        while((distance > 0) && rclcpp::ok()) {
            //Проверяем, не отменена ли цель
            if(goal_handle->is_canceling()) {
                //Тут нужно сделать что-то для остановки движения

                //Формируем результат
                result->success = false;
                goal_handle->canceled(result);
                RCLCPP_INFO_STREAM(this->get_logger(),
                                   "Цель отменена");
                return;
            }
            //Здесь нужно написать основной алгоритм движения

            //Пока его нет, имитируем простым уменьшением расстояния
            distance--;

            //Формируем обратную связь
            feedback->progress = distance / 20.0 * 100;
            goal_handle->publish_feedback(feedback);

            RCLCPP_INFO_STREAM(this->get_logger(),
                               "Прогресс "<<feedback->progress);

            loop_rate.sleep(); //засыпаем для поддержания делаемой частоты

        }
        //После окончания выполнения действия необходимо отправить результат
        if(rclcpp::ok()) {
            result->success = true;
            goal_handle->succeed(result);
            RCLCPP_INFO_STREAM(this->get_logger(), "Цель достигнута");
        }
    }

};

int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<MoveToPointServer>());
    rclcpp::shutdown();

    return 0;
}
