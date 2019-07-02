
#include "connection.h"

bool createConnection()
{
    QSqlDatabase db;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("sensor.db");
    if (!db.open())
    {
        QMessageBox::critical(0, "Can't open database",
                              "Unable to establish database connection",
                              QMessageBox::Cancel);
        return false;
    }
    return true;
}

void connection(time_t_ time, int temperature, int humidity,
                int pressure, double altitude, double wind_speed)
{
    QSqlQuery query;

    //创建sensor表,包含6个字段:time temp humi pres alti winds
    query.exec(QString("create table sensor(time varchar primary key,"
                       "temp int, humi varchar, pres int, alti double, "
                       "winds double)"));
    char str[100];
    sprintf(str, "insert into sensor values('%02x:%02x:%02x', %d, '%d%%', %d, %.3f,"
                 "%.3f)", time.hour, time.min, time.sec, temperature, humidity,
            pressure, altitude, wind_speed);
    query.exec(QString(str));

}
